#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "tokenizer.h"

#define PI 3.14159265358979323846
#define E  2.71828182845904523536

/* Liste des fonctions mathematiques reconnues */
static const char *KNOWN_FUNCTIONS[] = {
    "sin", "cos", "tan",
    "asin", "acos", "atan",
    "sinh", "cosh", "tanh",
    "asinh", "acosh", "atanh",
    "log", "ln", "exp",
    "sqrt", "cbrt", "abs",
    "floor", "ceil", "round",
    "Pol", "Rec",
    NULL
};

/* Variables autorisees : A-F, M, X, Y, Ans */
static const char *KNOWN_VARIABLES[] = {
    "Ans", "A", "B", "C", "D", "E", "F", "M", "X", "Y",
    NULL
};

static int is_function(const char *name) {
    int i;
    for (i = 0; KNOWN_FUNCTIONS[i]; i++)
        if (strcmp(name, KNOWN_FUNCTIONS[i]) == 0) return 1;
    return 0;
}

static int is_variable(const char *name) {
    int i;
    for (i = 0; KNOWN_VARIABLES[i]; i++)
        if (strcmp(name, KNOWN_VARIABLES[i]) == 0) return 1;
    return 0;
}

/* Insere un token TOK_MUL pour la multiplication implicite */
static int insert_implicit_mul(Token *out, int *n, int pos) {
    if (*n >= MAX_TOKENS - 1) return 0;
    memset(&out[*n], 0, sizeof(Token));
    out[*n].type     = TOK_MUL;
    out[*n].position = pos;
    (*n)++;
    return 1;
}

/* Verifie si une multiplication implicite est necessaire apres le token courant */
static int needs_implicit_mul(Token *out, int n) {
    if (n == 0) return 0;
    TokenType prev = out[n - 1].type;
    return (prev == TOK_NUMBER   ||
            prev == TOK_RPAREN   ||
            prev == TOK_VARIABLE);
}

CalcError tokenize(const char *expr, Token *out, int *count, int *err_pos) {
    int i = 0, n = 0;
    int len = (int)strlen(expr);
    *err_pos = -1;
    *count   = 0;

    /* Limite F-CO-05 : 99 octets */
    if (len > 99) { *err_pos = 99; return ERR_SYNTAX; }

    while (i < len) {
        if (n >= MAX_TOKENS - 1) { *err_pos = i; return ERR_SYNTAX; }

        Token t;
        memset(&t, 0, sizeof(t));
        t.position = i;

        /* Espaces ignores */
        if (isspace((unsigned char)expr[i])) { i++; continue; }

        /* ---- Constante pi ---- */
        if (expr[i] == 'p' && i + 1 < len && expr[i+1] == 'i') {
            if (needs_implicit_mul(out, n))
                if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
            t.type  = TOK_NUMBER;
            t.value = PI;
            i += 2;
            out[n++] = t;
            continue;
        }

        /* ---- Nombre : entier ou decimal ---- */
        if (isdigit((unsigned char)expr[i]) || expr[i] == '.') {
            char *end;
            t.type  = TOK_NUMBER;
            t.value = strtod(&expr[i], &end);
            i = (int)(end - expr);
            out[n++] = t;

            /* Multiplication implicite : 4sin( / 4( / 4A */
            if (i < len) {
                int is_alpha = isalpha((unsigned char)expr[i]);
                int is_lpar  = expr[i] == '(';
                if ((is_alpha || is_lpar) && n < MAX_TOKENS - 1)
                    if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
            }
            continue;
        }

        /* ---- Identifiants : fonctions et variables ---- */
        if (isalpha((unsigned char)expr[i])) {
            int j = 0;
            while (i < len && (isalnum((unsigned char)expr[i])))
                t.name[j++] = expr[i++];
            t.name[j] = '\0';

            if (is_function(t.name)) {
                if (needs_implicit_mul(out, n))
                    if (!insert_implicit_mul(out, &n, t.position)) { *err_pos = t.position; return ERR_SYNTAX; }
                t.type = TOK_FUNCTION;
            } else if (is_variable(t.name)) {
                if (needs_implicit_mul(out, n))
                    if (!insert_implicit_mul(out, &n, t.position)) { *err_pos = t.position; return ERR_SYNTAX; }
                t.type = TOK_VARIABLE;
                /* Multiplication implicite apres variable : A( */
                if (i < len && expr[i] == '(')
                    if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
                    /* Note : on laisse la variable etre inseree apres */
            } else if (strcmp(t.name, "and")  == 0) { t.type = TOK_AND;
            } else if (strcmp(t.name, "or")   == 0) { t.type = TOK_OR;
            } else if (strcmp(t.name, "xor")  == 0) { t.type = TOK_XOR;
            } else if (strcmp(t.name, "xnor") == 0) { t.type = TOK_XNOR;
            } else {
                *err_pos = t.position;
                return ERR_SYNTAX;
            }
            out[n++] = t;
            continue;
        }

        /* ---- Operateurs et symboles ---- */
        switch (expr[i]) {
            case '+': t.type = TOK_PLUS;   break;
            case '*': t.type = TOK_MUL;    break;
            case '/': t.type = TOK_DIV;    break;
            case '^': t.type = TOK_POW;    break;
            case '%': t.type = TOK_MOD;    break;
            case ',': t.type = TOK_COMMA;  break;
            case '=': t.type = TOK_ASSIGN; break;

            case '-':
                /* Signe unaire : debut ou apres operateur/parenthese ouvrante */
                if (n == 0                    ||
                    out[n-1].type == TOK_PLUS  ||
                    out[n-1].type == TOK_MINUS ||
                    out[n-1].type == TOK_MUL   ||
                    out[n-1].type == TOK_DIV   ||
                    out[n-1].type == TOK_POW   ||
                    out[n-1].type == TOK_MOD   ||
                    out[n-1].type == TOK_LPAREN) {
                    /* Insere 0 pour transformer -x en (0-x) */
                    Token zero;
                    memset(&zero, 0, sizeof(zero));
                    zero.type = TOK_NUMBER; zero.value = 0.0; zero.position = i;
                    out[n++] = zero;
                }
                t.type = TOK_MINUS;
                break;

            case '(':
                if (needs_implicit_mul(out, n))
                    if (!insert_implicit_mul(out, &n, i)) { *err_pos = i; return ERR_SYNTAX; }
                t.type = TOK_LPAREN;
                break;

            case ')': t.type = TOK_RPAREN; break;

            default:
                *err_pos = i;
                return ERR_SYNTAX;
        }
        i++;
        out[n++] = t;
    }

    /* Token de fin obligatoire */
    memset(&out[n], 0, sizeof(Token));
    out[n].type     = TOK_END;
    out[n].position = len;
    n++;
    *count = n;
    return ERR_NONE;
}
