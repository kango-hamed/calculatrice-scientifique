#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "core.h"
#include <math.h>

#define MAX_EXPR  512
#define HIST_SIZE 20

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
 *  Commandes speciales
 * ========================================================= */

static void cmd_help(void) {
    printf("\n");
    printf("  Commandes disponibles :\n");
    printf("  -------------------------\n");
    printf("  exit / quit       Quitter la calculatrice\n");
    printf("  help              Afficher cette aide\n");
    printf("  hist              Afficher l'historique\n");
    printf("  vars              Afficher les variables\n");
    printf("  clr               Effacer toutes les variables\n");
    printf("  deg               Passer en mode degres\n");
    printf("  rad               Passer en mode radians\n");
    printf("  M+                Ajouter Ans a M\n");
    printf("  M-                Soustraire Ans de M\n");
    printf("  MR                Rappeler M\n");
    printf("  MC                Effacer M\n");
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

static void cmd_vars(CalcMemory *mem) {
    int i;
    printf("\n  Variables :\n");
    for (i = 0; i < 26; i++) {
        if (mem->vars[i] != 0.0)
            printf("  %c = %g\n", 'A' + i, mem->vars[i]);
    }
    printf("  M   = %g\n", mem->mem_M);
    printf("  Ans = %g\n", mem->ans);
    printf("  Mode : %s\n\n", mem->angle_deg ? "Degres" : "Radians");
}

/* =========================================================
 *  Pipeline : tokenize -> parse -> eval
 * ========================================================= */

static void run_expr(const char *expr, CalcMemory *mem) {
    Token     tokens[MAX_TOKENS];
    int       count   = 0;
    int       err_pos = -1;
    double    result  = 0.0;
    CalcError err;

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
    eval_print_result(result);
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
    if (strcmp(line, "help") == 0) { cmd_help();       return 1; }
    if (strcmp(line, "hist") == 0) { hist_print();     return 1; }
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
    if (strcmp(line, "M+") == 0) {
        mem->mem_M += mem->ans;
        printf("  M = %g\n", mem->mem_M);
        return 1;
    }
    if (strcmp(line, "M-") == 0) {
        mem->mem_M -= mem->ans;
        printf("  M = %g\n", mem->mem_M);
        return 1;
    }
    if (strcmp(line, "MR") == 0) {
        printf("  M = %g\n", mem->mem_M);
        return 1;
    }
    if (strcmp(line, "MC") == 0) {
        mem->mem_M = 0.0;
        printf("  M efface.\n");
        return 1;
    }
    return 0;
}

/* =========================================================
 *  Gestion des instructions multiples separees par ':' (F-CO-06)
 * ========================================================= */

static void run_line(const char *line, CalcMemory *mem) {
    char  buf[MAX_EXPR];
    char *tok;
    char  copy[MAX_EXPR];

    strncpy(copy, line, MAX_EXPR - 1);
    tok = strtok(copy, ":");
    while (tok) {
        /* Supprime les espaces en debut */
        while (*tok == ' ') tok++;
        /* Supprime les espaces en fin */
        int len = (int)strlen(tok);
        while (len > 0 && tok[len-1] == ' ') tok[--len] = '\0';

        if (strlen(tok) > 0) {
            strncpy(buf, tok, MAX_EXPR - 1);
            if (!handle_command(buf, mem))
                run_expr(buf, mem);
        }
        tok = strtok(NULL, ":");
    }
}

/* =========================================================
 *  Batterie de tests automatiques
 * ========================================================= */

static int tests_passed = 0;
static int tests_total  = 0;

static void run_test(const char *expr, double expected, CalcMemory *mem) {
    Token     tokens[MAX_TOKENS];
    int       count = 0, err_pos = -1;
    double    result = 0.0;
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
    if (fabs(result - expected) < 1e-9 || fabs(result - expected) < 1e-8 * fabs(expected)) {
        printf("  [OK]   %-35s = %g\n", expr, result);
        tests_passed++;
    } else {
        printf("  [FAIL] %-35s = %g  (attendu %g)\n", expr, result, expected);
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
        Parser p2; ASTNode *tr2; double r2;
        eval_memory_init(&m2);
        tokenize("A = 5", t2, &c2, &e2);
        parser_init(&p2, t2, c2, "A = 5");
        tr2 = parser_parse(&p2);
        eval(tr2, &m2, &r2); ast_free(tr2);
        tests_total++;
        if (m2.vars['A' - 'A'] == 5.0) {
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
    printf("  Mode : %s  |  tape 'help' pour l'aide\n",
           mem.angle_deg ? "Degres" : "Radians");
    printf("  ============================================\n\n");

    while (1) {
        /* Affichage de l'invite (EG-07) */
        printf("[COMP-%s] > ", mem.angle_deg ? "DEG" : "RAD");
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
