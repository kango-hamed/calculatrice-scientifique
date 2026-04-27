#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "core.h"
#include <math.h>
#include "stat.h"

#define MAX_EXPR  512
#define HIST_SIZE 20

/* CalcMode est défini dans core.h */
/* mode_name() est défini dans core.c */

/* =========================================================
 *  Historique des expressions (F-ME-07)
 * ========================================================= */

static char history[HIST_SIZE][MAX_EXPR];
static int  hist_count = 0;

static void hist_add(const char *expr) {
    if (hist_count < HIST_SIZE) {
        strncpy(history[hist_count], expr, MAX_EXPR - 1);
        hist_count++;
    } else {
        /* Decalage : supprime la plus ancienne */
        int i;
        for (i = 0; i < HIST_SIZE - 1; i++)
            strcpy(history[i], history[i + 1]);
        strncpy(history[HIST_SIZE - 1], expr, MAX_EXPR - 1);
    }
}

static void hist_print(void) {
    int i;
    if (hist_count == 0) { printf("  (historique vide)\n"); return; }
    for (i = 0; i < hist_count; i++)
        printf("  [%d] %s\n", i + 1, history[i]);
}

/* =========================================================
 *  Aide par mode -- liste des fonctions disponibles
 * ========================================================= */

static void print_functions_for_mode(CalcMode mode) {
    printf("\n  Fonctions disponibles en mode %s :\n", mode_name(mode));
    printf("  ----------------------------------------\n");
    
    /* Fonctions de base (toujours disponibles) */
    printf("  Fonctions mathematiques de base :\n");
    printf("    sin, cos, tan, asin, acos, atan\n");
    printf("    sinh, cosh, tanh, asinh, acosh, atanh\n");
    printf("    log, ln, exp, sqrt, cbrt, abs\n");
    printf("    floor, ceil, round\n");
    printf("    nPr, nCr, sqr, cub, fact\n");
    
    switch (mode) {
        case MODE_CMPLX:
            printf("\n  Fonctions specifiques aux nombres complexes :\n");
            printf("    arg(z)    - Argument (angle) de z\n");
            printf("    conj(z)   - Conjugue de z\n");
            printf("    re(z)     - Partie reelle de z\n");
            printf("    im(z)     - Partie imaginaire de z\n");
            printf("    mod(z)    - Module (norme) de z\n");
            printf("\n  Constante : i (unite imaginaire)\n");
            break;
        case MODE_STAT:
            printf("\n  Fonctions statistiques (utilisables sans parentheses comme des variables) :\n");
            printf("    mean, std, samp_std, var, n, sum, min, max\n");
            printf("    regA, regB, regC (Coefficients de la regression choisie)\n");
            printf("    P(t), Q(t), R(t), norm(x)\n");
            printf("  Astuce : Tapez 'stat res' pour afficher tous les resultats d'un coup.\n");
            break;
        case MODE_MATRIX:
            printf("\n  Fonctions matricielles :\n");
            printf("    det, tr, inv, dim, trans\n");
            break;
        case MODE_BASE_N:
            printf("\n  Operateurs et fonctions Base-N :\n");
            printf("    and, or, xor, not, shl, shr\n");
            break;
        case MODE_COMP:
        case MODE_TABLE:
        case MODE_EQN:
        default:
            printf("\n  (Pas de fonctions speciales en mode %s)\n", mode_name(mode));
            break;
    }
    printf("\n");
}

static void cmd_help_general(void) {
    printf("\n");
    printf("  Commandes disponibles :\n");
    printf("  -------------------------\n");
    printf("  exit / quit       Quitter la calculatrice\n");
    printf("  help              Afficher cette aide generale\n");
    printf("  help <mode>       Afficher les fonctions d'un mode (comp, cmplx, stat, ...)\n");
    printf("  hist              Afficher l'historique\n");
    printf("  vars              Afficher les variables\n");
    printf("  clr               Effacer toutes les variables\n");
    printf("  deg               Passer en mode degres\n");
    printf("  rad               Passer en mode radians\n");
    printf("  mode              Afficher le mode actuel\n");
    printf("  mode <nom>        Changer de mode\n");
    printf("                      (comp, cmplx, stat, mat, table, basen, eqn)\n");
    printf("  M+                Ajouter Ans a M\n");
    printf("  M-                Soustraire Ans de M\n");
    printf("  MR                Rappeler M\n");
    printf("  MC                Effacer M\n");
    printf("  stat res          Afficher le resume des statistiques\n");
    printf("\n");
    printf("  Exemples d'expressions :\n");
    printf("  -------------------------\n");
    printf("  3 + 4 * 2         = 11\n");
    printf("  sin(30)           = 0.5  (mode deg)\n");
    printf("  log(2, 16)        = 4    (log base 2)\n");
    printf("  2^10              = 1024\n");
    printf("  A = 5             affectation variable\n");
    printf("  4sin(30)          multiplication implicite\n");
    printf("  sqrt(2) * sqrt(2) = 2\n");
    printf("\n");
}

static void cmd_show_mode(CalcMemory *mem) {
    printf("\n  Mode actuel : %s\n", mode_name(mem->current_mode));
    printf("  Modes disponibles :\n");
    printf("    comp   - Calcul standard (COMP)\n");
    printf("    cmplx  - Nombres complexes (CMPLX)\n");
    printf("    stat   - Statistiques (STAT)\n");
    printf("    mat    - Matrices (MAT)\n");
    printf("    table  - Table de valeurs (TABLE)\n");
    printf("    basen  - Base-N (BASE-N)\n");
    printf("    eqn    - Equations (EQN)\n\n");
}

static void interactive_stat_editor(void) {
    char line[256];
    int mode = 0;

    printf("  Choisissez le type d'analyse STAT :\n");
    printf("  1) 1-VAR (Une seule variable X)\n");
    printf("  2) A+BX  (Regression lineaire)\n");
    printf("  3) _+CX^2(Regression quadratique)\n");
    printf("  4) ln X  (Regression logarithmique)\n");
    printf("  5) e^X   (Regression exponentielle e)\n");
    printf("  6) A*B^X (Regression exponentielle ab)\n");
    printf("  7) A*X^B (Regression puissance)\n");
    printf("  8) 1/X   (Regression inverse)\n");
    printf("  > ");
    fflush(stdout);

    if (!fgets(line, sizeof(line), stdin)) return;
    int choice = atoi(line);
    if (choice == 1) mode = 1;
    else if (choice >= 2 && choice <= 8) {
        mode = 2;
        stat_set_regression_type(choice - 1);
    } else {
        printf("  Choix invalide. Mode STAT actif mais tableau vide.\n");
        return;
    }

    stat_clear();
    int count = 1;
    printf("  Saisissez vos donnees (tapez 'fin' ou laissez vide pour terminer) :\n");
    while (1) {
        if (mode == 1) {
            printf("  X[%d] = ", count);
        } else {
            printf("  X, Y [%d] (separe par espace/virgule) = ", count);
        }
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) break;
        
        int len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[--len] = '\0';
        }

        if (len == 0 || strcmp(line, "fin") == 0 || strcmp(line, "quit") == 0) break;

        if (mode == 1) {
            double x;
            if (sscanf(line, "%lf", &x) == 1) {
                stat_push(x);
                count++;
            } else {
                printf("  Valeur invalide, reessayez.\n");
            }
        } else {
            double x, y;
            char *comma = strchr(line, ',');
            if (comma) *comma = ' ';
            
            if (sscanf(line, "%lf %lf", &x, &y) == 2) {
                stat_push2(x, y);
                count++;
            } else {
                printf("  Paire invalide, attente de 2 nombres. Reessayez.\n");
            }
        }
    }
    printf("  %d ligne(s) enregistree(s).\n", count - 1);
}

static int cmd_set_mode(const char *modename, CalcMemory *mem) {
    if (strcmp(modename, "comp") == 0) {
        mem->current_mode = MODE_COMP;
        mem->complex_mode = 0;
        printf("  Mode : COMP (Calcul standard)\n");
        return 1;
    }
    if (strcmp(modename, "cmplx") == 0) {
        mem->current_mode = MODE_CMPLX;
        mem->complex_mode = 1;
        printf("  Mode : CMPLX (Nombres complexes)\n");
        return 1;
    }
    if (strcmp(modename, "stat") == 0) {
        mem->current_mode = MODE_STAT;
        printf("  Mode : STAT (Statistiques)\n");
        interactive_stat_editor();
        return 1;
    }
    if (strcmp(modename, "mat") == 0) {
        mem->current_mode = MODE_MATRIX;
        printf("  Mode : MAT (Matrices)\n");
        return 1;
    }
    if (strcmp(modename, "table") == 0) {
        mem->current_mode = MODE_TABLE;
        printf("  Mode : TABLE (Table de valeurs)\n");
        return 1;
    }
    if (strcmp(modename, "basen") == 0 || strcmp(modename, "base-n") == 0) {
        mem->current_mode = MODE_BASE_N;
        printf("  Mode : BASE-N (Calcul en base N)\n");
        return 1;
    }
    if (strcmp(modename, "eqn") == 0) {
        mem->current_mode = MODE_EQN;
        printf("  Mode : EQN (Equations)\n");
        return 1;
    }
    printf("  Mode inconnu : '%s'\n", modename);
    return 0;
}

static void cmd_vars(CalcMemory *mem) {
    int i;
    printf("\n  Variables :\n");
    for (i = 0; i < 26; i++) {
        if (mem->vars[i].re != 0.0 || mem->vars[i].im != 0.0) {
            if (cx_is_real(mem->vars[i]))
                printf("  %c = %g\n", 'A' + i, mem->vars[i].re);
            else
                printf("  %c = %g%+gi\n", 'A' + i, mem->vars[i].re, mem->vars[i].im);
        }
    }
    if (cx_is_real(mem->mem_M))
        printf("  M   = %g\n", mem->mem_M.re);
    else
        printf("  M   = %g%+gi\n", mem->mem_M.re, mem->mem_M.im);
    
    if (cx_is_real(mem->ans))
        printf("  Ans = %g\n", mem->ans.re);
    else
        printf("  Ans = %g%+gi\n", mem->ans.re, mem->ans.im);
    
    printf("  Mode : %s, Complexe: %s\n\n", 
           mem->angle_deg ? "Degres" : "Radians",
           mem->complex_mode ? "Oui" : "Non");
}

/* =========================================================
 *  Pipeline : tokenize -> parse -> eval
 * ========================================================= */

static void run_expr(const char *expr, CalcMemory *mem) {
    Token     tokens[MAX_TOKENS];
    int       count   = 0;
    int       err_pos = -1;
    ComplexValue    result  = cx_make(0.0, 0.0);
    CalcError err;
    
    /* Detection automatique du mode necessaire */
    CalcMode required_mode = detect_mode_from_expr(expr);
    if (required_mode == MODE_CMPLX) {
        mem->current_mode = MODE_CMPLX;
        mem->complex_mode = 1;
    }

    /* Etape 1 : Tokenization */
    err = tokenize(expr, tokens, &count, &err_pos);
    if (err != ERR_NONE) {
        error_print(err, expr, err_pos);
        return;
    }

    /* Etape 2 : Parsing -> AST */
    Parser  p;
    parser_init(&p, tokens, count, expr);
    ASTNode *tree = parser_parse(&p);
    if (!tree) {
        parser_print_error(&p);
        return;
    }

    /* Etape 3 : Evaluation */
    err = eval(tree, mem, &result);
    ast_free(tree);

    if (err != ERR_NONE) {
        error_print(err, expr, -1);
        return;
    }

    /* Mise a jour de Ans et affichage */
    mem->ans = result;
    /* Activer le mode complexe si la partie imaginaire est non nulle */
    if (!cx_is_real(result)) mem->complex_mode = 1;
    eval_print_result(result, mem->complex_mode);
    hist_add(expr);
}

/* =========================================================
 *  Traitement des commandes speciales (M+, M-, MR, MC...)
 *  Retourne 1 si c'etait une commande, 0 sinon
 * ========================================================= */

static int handle_command(const char *line, CalcMemory *mem) {
    if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
        printf("\n  Au revoir !\n\n");
        exit(0);
    }
    if (strcmp(line, "help") == 0) { cmd_help_general(); return 1; }
    if (strncmp(line, "help ", 5) == 0) {
        const char *modename = line + 5;
        CalcMode mode = MODE_COMP;
        if (strcmp(modename, "comp") == 0) mode = MODE_COMP;
        else if (strcmp(modename, "cmplx") == 0) mode = MODE_CMPLX;
        else if (strcmp(modename, "stat") == 0) mode = MODE_STAT;
        else if (strcmp(modename, "mat") == 0 || strcmp(modename, "matrix") == 0) mode = MODE_MATRIX;
        else if (strcmp(modename, "table") == 0) mode = MODE_TABLE;
        else if (strcmp(modename, "basen") == 0 || strcmp(modename, "base-n") == 0) mode = MODE_BASE_N;
        else if (strcmp(modename, "eqn") == 0) mode = MODE_EQN;
        else {
            printf("  Mode inconnu : '%s'\n", modename);
            return 1;
        }
        print_functions_for_mode(mode);
        return 1;
    }
    if (strcmp(line, "hist") == 0) { hist_print();     return 1; }
    if (strcmp(line, "stat res") == 0) {
        if (mem->current_mode != MODE_STAT) {
            printf("  [ERREUR] Vous n'etes pas en mode STAT.\n");
        } else {
            printf("\n  === RESULTATS STATISTIQUES ===\n");
            printf("  n         = %d\n", stat_count());
            printf("  Sum       = %g\n", stat_sum());
            printf("  Mean      = %g\n", stat_mean());
            printf("  Std (pop) = %g\n", stat_stddev_pop());
            printf("  Std (smp) = %g\n", stat_stddev_samp());
            printf("  Var       = %g\n", stat_var());
            printf("  Min       = %g\n", stat_min());
            printf("  Max       = %g\n", stat_max());
            if (stat_get_regression_type() >= 1) {
                printf("  --- Regression (Type %d) ---\n", stat_get_regression_type());
                printf("  A = %g\n", stat_reg_A(stat_get_regression_type()));
                printf("  B = %g\n", stat_reg_B(stat_get_regression_type()));
                if (stat_get_regression_type() == 2) {
                    printf("  C = %g\n", stat_reg_C(stat_get_regression_type()));
                }
            }
            printf("  ==============================\n\n");
        }
        return 1;
    }
    if (strcmp(line, "vars") == 0) { cmd_vars(mem);    return 1; }
    if (strcmp(line, "clr")  == 0) {
        eval_memory_init(mem);
        printf("  Memoire effacee.\n");
        return 1;
    }
    if (strcmp(line, "deg") == 0) {
        mem->angle_deg = 1;
        printf("  Mode : Degres\n");
        return 1;
    }
    if (strcmp(line, "rad") == 0) {
        mem->angle_deg = 0;
        printf("  Mode : Radians\n");
        return 1;
    }
    if (strcmp(line, "mode") == 0) {
        cmd_show_mode(mem);
        return 1;
    }
    if (strncmp(line, "mode ", 5) == 0) {
        cmd_set_mode(line + 5, mem);
        return 1;
    }
    if (strcmp(line, "M+") == 0) {
        mem->mem_M = cx_add(mem->mem_M, mem->ans);
        eval_print_result(mem->mem_M, mem->complex_mode);
        return 1;
    }
    if (strcmp(line, "M-") == 0) {
        mem->mem_M = cx_sub(mem->mem_M, mem->ans);
        eval_print_result(mem->mem_M, mem->complex_mode);
        return 1;
    }
    if (strcmp(line, "MR") == 0) {
        eval_print_result(mem->mem_M, mem->complex_mode);
        return 1;
    }
    if (strcmp(line, "MC") == 0) {
        mem->mem_M = cx_make(0.0, 0.0);
        printf("  M efface.\n");
        return 1;
    }
    return 0;
}

/* =========================================================
 *  Batterie de tests automatiques
 * ========================================================= */

static int tests_passed = 0;
static int tests_total  = 0;

static void run_test(const char *expr, double expected, CalcMemory *mem) {
    Token     tokens[MAX_TOKENS];
    int       count = 0, err_pos = -1;
    ComplexValue    result = cx_make(0.0, 0.0);
    CalcError err;

    tests_total++;
    err = tokenize(expr, tokens, &count, &err_pos);
    if (err != ERR_NONE) {
        printf("  [FAIL] %s  -> erreur tokenizer\n", expr);
        return;
    }
    Parser p;
    parser_init(&p, tokens, count, expr);
    ASTNode *tree = parser_parse(&p);
    if (!tree) {
        printf("  [FAIL] %s  -> erreur parser\n", expr);
        return;
    }
    err = eval(tree, mem, &result);
    ast_free(tree);
    if (err != ERR_NONE) {
        printf("  [FAIL] %s  -> %s\n", expr, error_message(err));
        return;
    }
    if (cx_is_real(result) && fabs(result.re - expected) < 1e-9) {
        printf("  [OK]   %-35s = %g\n", expr, result.re);
        tests_passed++;
    } else {
        printf("  [FAIL] %-35s = %g%+gi  (attendu %g)\n", expr, result.re, result.im, expected);
    }
}

static void run_all_tests(void) {
    CalcMemory mem;
    eval_memory_init(&mem);

    printf("\n");
    printf("  ============================================\n");
    printf("  TESTS AUTOMATIQUES -- Core Calculatrice\n");
    printf("  ============================================\n\n");

    /* Operations de base */
    run_test("3 + 4",           7.0,   &mem);
    run_test("10 - 3",          7.0,   &mem);
    run_test("3 * 4",           12.0,  &mem);
    run_test("10 / 4",          2.5,   &mem);
    run_test("2 ^ 10",          1024.0,&mem);
    run_test("10 % 3",          1.0,   &mem);

    /* Priorites (F-CO-02) */
    run_test("3 + 4 * 2",       11.0,  &mem);
    run_test("(3 + 4) * 2",     14.0,  &mem);
    run_test("2 ^ 3 ^ 2",       512.0, &mem);  /* 2^(3^2)=512 assoc droite */
    run_test("-3 + 5",          2.0,   &mem);
    run_test("-(2 + 1)",        -3.0,  &mem);

    /* Multiplication implicite (F-CO-03) */
    run_test("4sin(30)",        2.0,   &mem);   /* 4 * 0.5 = 2 */
    run_test("2sqrt(9)",        6.0,   &mem);   /* 2 * 3 = 6   */

    /* Trigonometrie (F-FN-01) mode degres */
    run_test("sin(30)",         0.5,          &mem);
    run_test("cos(60)",         0.5,          &mem);
    run_test("sin(90)",         1.0,          &mem);
    run_test("asin(1)",         90.0,         &mem);
    run_test("acos(0.5)",       60.0,         &mem);

    /* Hyperboliques (F-FN-06) */
    run_test("sinh(1)",         1.1752011936, &mem);
    run_test("cosh(0)",         1.0,          &mem);
    run_test("tanh(0)",         0.0,          &mem);

    /* Logarithmes (F-FN-10, F-FN-11) */
    run_test("log(100)",        2.0,   &mem);
    run_test("log(2, 16)",      4.0,   &mem);
    run_test("ln(1)",           0.0,   &mem);

    /* Racines et puissances (F-FN-13, F-FN-14) */
    run_test("sqrt(9)",         3.0,   &mem);
    run_test("cbrt(27)",        3.0,   &mem);
    run_test("abs(-5)",         5.0,   &mem);

    /* Cas du cahier des charges (section 6.3) */
    run_test("4 * sin(30) * (30 + 10 * 3)", 120.0, &mem);

    /* Variables (F-ME-02) */
    {
        CalcMemory m2;
        Token t2[MAX_TOKENS]; int c2, e2;
        Parser p2; ASTNode *tr2; ComplexValue r2;
        eval_memory_init(&m2);
        tokenize("A = 5", t2, &c2, &e2);
        parser_init(&p2, t2, c2, "A = 5");
        tr2 = parser_parse(&p2);
        eval(tr2, &m2, &r2); ast_free(tr2);
        tests_total++;
        if (m2.vars['A' - 'A'].re == 5.0 && m2.vars['A' - 'A'].im == 0.0) {
            printf("  [OK]   %-35s = 5\n", "A = 5 (variable)");
            tests_passed++;
        } else {
            printf("  [FAIL] A = 5 (variable)\n");
        }
    }

    printf("\n");
    printf("  ============================================\n");
    printf("  Resultat : %d / %d tests passes\n", tests_passed, tests_total);
    printf("  ============================================\n\n");
}

static void run_line(const char *line, CalcMemory *mem) {
    if (!handle_command(line, mem))
        run_expr(line, mem);
}

/* =========================================================
 *  Point d'entree principal
 * ========================================================= */

int main(int argc, char *argv[]) {
    CalcMemory mem;
    char       line[MAX_EXPR];
    eval_memory_init(&mem);

    /* Mode test : ./calc --test */
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        run_all_tests();
        return (tests_passed == tests_total) ? 0 : 1;
    }

    /* Mode script : ./calc -f fichier.txt (EG-11) */
    if (argc > 2 && strcmp(argv[1], "-f") == 0) {
        FILE *f = fopen(argv[2], "r");
        if (!f) { fprintf(stderr, "Impossible d'ouvrir : %s\n", argv[2]); return 1; }
        while (fgets(line, MAX_EXPR, f)) {
            int len = (int)strlen(line);
            while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
                line[--len] = '\0';
            if (len == 0 || line[0] == '#') continue;
            printf("  > %s\n", line);
            run_line(line, &mem);
        }
        fclose(f);
        return 0;
    }

    /* Mode interactif (EG-06, EG-07) */
    printf("\n");
    printf("  ============================================\n");
    printf("  Calculatrice Scientifique -- Core v1.0\n");
    printf("  Reference : Casio fx-570ES PLUS\n");
    printf("  Mode : %s-%s  |  tape 'help' pour l'aide\n", mode_name(mem.current_mode), mem.angle_deg ? "DEG" : "RAD");
    printf("  ============================================\n\n");

    while (1) {
        /* Affichage de l'invite avec le mode actif */
        printf("[%s-%s] > ", mode_name(mem.current_mode), mem.angle_deg ? "DEG" : "RAD");
        fflush(stdout);

        if (!fgets(line, MAX_EXPR, stdin)) break;

        /* Suppression du saut de ligne */
        int len = (int)strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        if (len == 0) continue;

        /* Traitement de la ligne (multi-instructions ou expression) */
        run_line(line, &mem);
    }

    return 0;
}