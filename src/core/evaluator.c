#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "evaluator.h"

#define PI  3.14159265358979323846
#define E_  2.71828182845904523536

/* Limites conformes au manuel Casio (section 5.1) */
#define CASIO_MAX       9.999999999e99
#define SINH_MAX        230.2585092
#define EXP_MAX         230.2585092
#define LOG10_MAX_EXP   99.99999999

static double to_rad  (double x, int deg) { return deg ? x * PI / 180.0 : x; }
static double from_rad(double x, int deg) { return deg ? x * 180.0 / PI : x; }
static int    is_invalid(double x)        { return isinf(x) || isnan(x); }

/* F-CO-09 : profondeur maximale d'imbrication */
#define MAX_EVAL_DEPTH 200
static CalcError eval_depth(const ASTNode *node, CalcMemory *mem,
                             double *result, int depth);

/* =========================================================
 *  Initialisation de la memoire
 * ========================================================= */

void eval_memory_init(CalcMemory *mem) {
    memset(mem->vars, 0, sizeof(mem->vars));
    mem->mem_M     = 0.0;
    mem->ans       = 0.0;
    mem->angle_deg = 1;   /* mode degres par defaut */
}

/* =========================================================
 *  Evaluation des fonctions mathematiques
 * ========================================================= */

static CalcError eval_function(const char *name, double *args, int argc,
                               CalcMemory *mem, double *result) {
    /* ---- Trigonometrie (F-FN-01) ---- */
    if (strcmp(name, "sin") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = sin(to_rad(args[0], mem->angle_deg));
        return ERR_NONE;
    }
    if (strcmp(name, "cos") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cos(to_rad(args[0], mem->angle_deg));
        return ERR_NONE;
    }
    if (strcmp(name, "tan") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        /* tan(90 deg) indefini (F-FN-05) */
        if (mem->angle_deg) {
            double mod = fmod(fabs(args[0]), 180.0);
            if (fabs(mod - 90.0) < 1e-10) return ERR_DOMAIN;
        }
        *result = tan(to_rad(args[0], mem->angle_deg));
        return ERR_NONE;
    }

    /* ---- Trigonometrie inverse (F-FN-02) ---- */
    if (strcmp(name, "asin") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < -1.0 || args[0] > 1.0) return ERR_DOMAIN;
        *result = from_rad(asin(args[0]), mem->angle_deg);
        return ERR_NONE;
    }
    if (strcmp(name, "acos") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < -1.0 || args[0] > 1.0) return ERR_DOMAIN;
        *result = from_rad(acos(args[0]), mem->angle_deg);
        return ERR_NONE;
    }
    if (strcmp(name, "atan") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = from_rad(atan(args[0]), mem->angle_deg);
        return ERR_NONE;
    }

    /* ---- Hyperboliques (F-FN-06) ---- */
    if (strcmp(name, "sinh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) > SINH_MAX) return ERR_OVERFLOW;
        *result = sinh(args[0]);
        return ERR_NONE;
    }
    if (strcmp(name, "cosh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) > SINH_MAX) return ERR_OVERFLOW;
        *result = cosh(args[0]);
        return ERR_NONE;
    }
    if (strcmp(name, "tanh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = tanh(args[0]);
        return ERR_NONE;
    }

    /* ---- Hyperboliques inverses (F-FN-07, F-FN-08) ---- */
    if (strcmp(name, "asinh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = asinh(args[0]);
        return ERR_NONE;
    }
    if (strcmp(name, "acosh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < 1.0) return ERR_DOMAIN;  /* cosh^-1 exige x >= 1 */
        *result = acosh(args[0]);
        return ERR_NONE;
    }
    if (strcmp(name, "atanh") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (fabs(args[0]) >= 1.0) return ERR_DOMAIN;  /* tanh^-1 exige |x| < 1 */
        *result = atanh(args[0]);
        return ERR_NONE;
    }

    /* ---- Exponentielles (F-FN-09) ---- */
    if (strcmp(name, "exp") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] > EXP_MAX) return ERR_OVERFLOW;
        *result = exp(args[0]);
        return ERR_NONE;
    }

    /* ---- Logarithmes (F-FN-10, F-FN-11, F-FN-12) ---- */
    if (strcmp(name, "ln") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] <= 0.0) return ERR_DOMAIN;
        *result = log(args[0]);
        return ERR_NONE;
    }
    if (strcmp(name, "log") == 0) {
        if (argc == 1) {
            /* log(x) = log base 10 */
            if (args[0] <= 0.0) return ERR_DOMAIN;
            *result = log10(args[0]);
            return ERR_NONE;
        }
        if (argc == 2) {
            /* log(base, x) = log en base arbitraire (F-FN-11) */
            if (args[0] <= 0.0 || args[0] == 1.0) return ERR_ARGUMENT;
            if (args[1] <= 0.0) return ERR_DOMAIN;
            *result = log(args[1]) / log(args[0]);
            return ERR_NONE;
        }
        return ERR_ARGUMENT;
    }

    /* ---- Puissances et racines (F-FN-13, F-FN-14) ---- */
    if (strcmp(name, "sqrt") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        if (args[0] < 0.0) return ERR_DOMAIN;
        *result = sqrt(args[0]);
        return ERR_NONE;
    }
    if (strcmp(name, "cbrt") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = cbrt(args[0]);
        return ERR_NONE;
    }

    /* ---- Valeur absolue (F-FN-15) ---- */
    if (strcmp(name, "abs") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = fabs(args[0]);
        return ERR_NONE;
    }

    /* ---- Arrondi ---- */
    if (strcmp(name, "floor") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = floor(args[0]);
        return ERR_NONE;
    }
    if (strcmp(name, "ceil") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = ceil(args[0]);
        return ERR_NONE;
    }
    if (strcmp(name, "round") == 0) {
        if (argc != 1) return ERR_ARGUMENT;
        *result = round(args[0]);
        return ERR_NONE;
    }

    /* ---- Coordonnees polaires / rectangulaires (F-FN-19, F-FN-20) ---- */
    if (strcmp(name, "Pol") == 0) {
        /* Pol(x, y) -> affiche r et theta, retourne r */
        double x, y, r, theta;
        if (argc != 2) return ERR_ARGUMENT;
        x = args[0]; y = args[1];
        r     = sqrt(x * x + y * y);
        theta = from_rad(atan2(y, x), mem->angle_deg);
        printf("  r = %g, theta = %g\n", r, theta);
        *result = r;
        return ERR_NONE;
    }
    if (strcmp(name, "Rec") == 0) {
        /* Rec(r, theta) -> affiche x et y, retourne x */
        double r, theta_rad, xv, yv;
        if (argc != 2) return ERR_ARGUMENT;
        r         = args[0];
        theta_rad = to_rad(args[1], mem->angle_deg);
        xv = r * cos(theta_rad);
        yv = r * sin(theta_rad);
        printf("  x = %g, y = %g\n", xv, yv);
        *result = xv;
        return ERR_NONE;
    }

    return ERR_ARGUMENT;  /* fonction inconnue */
}

/* =========================================================
 *  Evaluation recursive de l'AST
 *  F-CO-09 : Stack ERROR si imbrication > MAX_EVAL_DEPTH
 * ========================================================= */

CalcError eval(const ASTNode *node, CalcMemory *mem, double *result) {
    return eval_depth(node, mem, result, 0);
}

static CalcError eval_depth(const ASTNode *node, CalcMemory *mem,
                             double *result, int depth) {
    CalcError err;
    double    lv, rv;
    double    args[16];
    int       i;

    if (depth > MAX_EVAL_DEPTH) return ERR_STACK;
    if (!node) return ERR_SYNTAX;

    switch (node->type) {

        /* ---- Nombre literal ---- */
        case NODE_NUMBER:
            *result = node->value;
            return ERR_NONE;

        /* ---- Variable (F-ME-01, F-ME-02) ---- */
        case NODE_VARIABLE:
            if (strcmp(node->name, "Ans") == 0) {
                *result = mem->ans;
            } else if (strcmp(node->name, "M") == 0) {
                *result = mem->mem_M;
            } else if (strlen(node->name) == 1 &&
                       node->name[0] >= 'A' && node->name[0] <= 'Z') {
                *result = mem->vars[node->name[0] - 'A'];
            } else {
                return ERR_ARGUMENT;
            }
            return ERR_NONE;

        /* ---- Affectation : A = expr (F-ME-02) ---- */
        case NODE_ASSIGN:
            err = eval_depth(node->left, mem, result, depth + 1);
            if (err != ERR_NONE) return err;
            if (strcmp(node->name, "M") == 0) {
                mem->mem_M = *result;
            } else if (strlen(node->name) == 1 &&
                       node->name[0] >= 'A' && node->name[0] <= 'Z') {
                mem->vars[node->name[0] - 'A'] = *result;
            } else {
                return ERR_ARGUMENT;
            }
            return ERR_NONE;

        /* ---- Operateur unaire (negation) ---- */
        case NODE_UNARYOP:
            err = eval_depth(node->left, mem, &lv, depth + 1);
            if (err != ERR_NONE) return err;
            if (node->name[0] == '-') *result = -lv;
            else return ERR_SYNTAX;
            return ERR_NONE;

        /* ---- Operateur binaire ---- */
        case NODE_BINOP:
            err = eval_depth(node->left, mem, &lv, depth + 1);
            if (err != ERR_NONE) return err;
            err = eval_depth(node->right, mem, &rv, depth + 1);
            if (err != ERR_NONE) return err;

            /* Operateurs logiques bit-a-bit (F-CO-02) */
            if (strcmp(node->name, "and")  == 0) {
                *result = (double)((long long)lv & (long long)rv);
                return ERR_NONE;
            }
            if (strcmp(node->name, "or")   == 0) {
                *result = (double)((long long)lv | (long long)rv);
                return ERR_NONE;
            }
            if (strcmp(node->name, "xor")  == 0) {
                *result = (double)((long long)lv ^ (long long)rv);
                return ERR_NONE;
            }
            if (strcmp(node->name, "xnor") == 0) {
                *result = (double)(~((long long)lv ^ (long long)rv));
                return ERR_NONE;
            }

            switch (node->name[0]) {
                case '+': *result = lv + rv; break;
                case '-': *result = lv - rv; break;
                case '*': *result = lv * rv; break;
                case '/':
                    if (rv == 0.0) return ERR_DIV_ZERO;
                    *result = lv / rv;
                    break;
                case '^':
                    if (lv < 0.0 && rv != (long long)rv) return ERR_DOMAIN;
                    *result = pow(lv, rv);
                    break;
                case '%':
                    if (rv == 0.0) return ERR_DIV_ZERO;
                    *result = fmod(lv, rv);
                    break;
                default:
                    return ERR_SYNTAX;
            }

            /* Verification depassement de plage (F-CO-07) */
            if (is_invalid(*result)) return ERR_OVERFLOW;
            return ERR_NONE;

        /* ---- Appel de fonction ---- */
        case NODE_FUNCTION:
            if (node->argc > 16) return ERR_ARGUMENT;
            for (i = 0; i < node->argc; i++) {
                err = eval_depth(node->args[i], mem, &args[i], depth + 1);
                if (err != ERR_NONE) return err;
            }
            err = eval_function(node->name, args, node->argc, mem, result);
            if (err != ERR_NONE) return err;
            if (is_invalid(*result)) return ERR_MATH;
            return ERR_NONE;

        default:
            return ERR_SYNTAX;
    }
}

/* =========================================================
 *  Affichage du resultat
 * ========================================================= */

void eval_print_result(double result) {
    if (fabs(result) < 1e-10)
        result = 0.0;
    /* Entier si la valeur est entiere et dans la plage */
    if (result == (long long)result &&
        result >= -1e15 && result <= 1e15) {
        printf("  = %.0f\n", result);
    } else {
        printf("  = %g\n", result);
    }
}
