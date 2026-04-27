#include "../../src/core/core.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/*
 * Tests unitaires — module core
 * Couverture : F-CO-01 à F-CO-11, priorités, multiplication implicite, erreurs
 */

static int tests_passed = 0;
static int tests_total = 0;

#define EPSILON 1e-9

#define TEST_ASSERT(expr, msg) do { \
    tests_total++; \
    if (expr) { \
        tests_passed++; \
        printf("  [OK] %s\n", msg); \
    } else { \
        printf("  [FAIL] %s (ligne %d)\n", msg, __LINE__); \
    } \
} while(0)

/* Helper : évalue une expression et retourne le résultat */
static double eval_expr(const char *expr, CalcMemory *mem, CalcError *err_out) {
    Token tokens[256];
    int count = 0, err_pos = -1;
    ComplexValue result;
    CalcError err;

    err = tokenize(expr, tokens, &count, &err_pos);
    if (err != ERR_NONE) {
        *err_out = err;
        return 0.0;
    }

    Parser p;
    parser_init(&p, tokens, count, expr);
    ASTNode *tree = parser_parse(&p);
    if (!tree) {
        *err_out = p.error;
        return 0.0;
    }

    err = eval(tree, mem, &result);
    ast_free(tree);
    *err_out = err;
    return result.re;
}

/* ==================== F-CO-01 : Expressions de base ==================== */

void test_basic_operations(void) {
    printf("\n--- Test : Opérations de base (F-CO-01) ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* Addition */
    result = eval_expr("3 + 4", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 7.0) < EPSILON, "3 + 4 = 7");

    /* Soustraction */
    result = eval_expr("10 - 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 7.0) < EPSILON, "10 - 3 = 7");

    /* Multiplication */
    result = eval_expr("3 * 4", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 12.0) < EPSILON, "3 * 4 = 12");

    /* Division */
    result = eval_expr("10 / 4", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 2.5) < EPSILON, "10 / 4 = 2.5");

    /* Puissance */
    result = eval_expr("2 ^ 10", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1024.0) < EPSILON, "2 ^ 10 = 1024");

    /* Modulo */
    result = eval_expr("10 % 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1.0) < EPSILON, "10 % 3 = 1");
}

/* ==================== F-CO-02 : Priorités d'opérateurs ==================== */

void test_operator_precedence(void) {
    printf("\n--- Test : Priorités d'opérateurs (F-CO-02) ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* Priorité * sur + */
    result = eval_expr("3 + 4 * 2", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 11.0) < EPSILON, "3 + 4 * 2 = 11 (prio * > +)");

    /* Parenthèses changent la priorité */
    result = eval_expr("(3 + 4) * 2", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 14.0) < EPSILON, "(3 + 4) * 2 = 14");

    /* Puissance associative à droite */
    result = eval_expr("2 ^ 3 ^ 2", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 512.0) < EPSILON, "2 ^ 3 ^ 2 = 512 (2^(3^2))");

    /* Priorité puissance sur multiplication */
    result = eval_expr("2 * 3 ^ 2", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 18.0) < EPSILON, "2 * 3 ^ 2 = 18");
}

/* ==================== F-CO-03 : Multiplication implicite ==================== */

void test_implicit_multiplication(void) {
    printf("\n--- Test : Multiplication implicite (F-CO-03) ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* Nombre avant fonction */
    mem.angle_deg = 1;
    result = eval_expr("4sin(30)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 2.0) < EPSILON, "4sin(30) = 2 (4 * 0.5)");

    /* Nombre avant sqrt */
    result = eval_expr("2sqrt(9)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 6.0) < EPSILON, "2sqrt(9) = 6 (2 * 3)");

    /* Nombre avant parenthèse - DEVRAIT MARCHER mais actuellement NON */
    result = eval_expr("4(3+2)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 20.0) < EPSILON, "4(3+2) = 20 (MULT IMPLICITE)");

    /* Variable après nombre - DEVRAIT MARCHER mais actuellement NON */
    eval_expr("A = 5", &mem, &err);
    result = eval_expr("4A", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 20.0) < EPSILON, "4A = 20 (MULT IMPLICITE)");

    /* Parenthèses adjacentes - DEVRAIT MARCHER mais actuellement NON */
    result = eval_expr("(2)(3)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 6.0) < EPSILON, "(2)(3) = 6 (MULT IMPLICITE)");
}

/* ==================== F-CO-04 : Parenthèses omises ==================== */

void test_missing_closing_paren(void) {
    printf("\n--- Test : Parenthèses fermantes omises (F-CO-04) ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* Parenthèse fermante manquante - DEVRAIT marcher avec auto-complétion */
    result = eval_expr("(3 + 4", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 7.0) < EPSILON, "(3 + 4 = 7 (parenthèse auto-complétée)");

    /* Fonction sans parenthèse fermante */
    mem.angle_deg = 1;
    result = eval_expr("sin(30", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 0.5) < EPSILON, "sin(30 = 0.5 (parenthèse auto-complétée)");
}

/* ==================== F-CO-05 : Longueur maximale ==================== */

void test_max_length(void) {
    printf("\n--- Test : Longueur maximale 99 octets (F-CO-05) ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    int err_pos;
    Token tokens[256];
    int count = 0;

    /* Expression de 50 caractères - OK */
    char expr50[51];
    memset(expr50, '1', 50);
    expr50[50] = '\0';
    err = tokenize(expr50, tokens, &count, &err_pos);
    TEST_ASSERT(err == ERR_NONE, "Expression 50 caractères acceptée");

    /* Expression de 100 caractères - TROP LONGUE */
    char expr100[101];
    memset(expr100, '1', 100);
    expr100[100] = '\0';
    err = tokenize(expr100, tokens, &count, &err_pos);
    TEST_ASSERT(err == ERR_SYNTAX, "Expression 100 caractères rejetée (ERR_SYNTAX)");
}

/* ==================== F-CO-07 à F-CO-11 : Gestion des erreurs ==================== */

void test_error_handling(void) {
    printf("\n--- Test : Gestion des erreurs (F-CO-07 à F-CO-11) ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* F-CO-07 : Division par zéro */
    result = eval_expr("10 / 0", &mem, &err);
    (void)result;
    TEST_ASSERT(err == ERR_DIV_ZERO, "10 / 0 = ERR_DIV_ZERO");

    /* F-CO-07 : Racine carrée de nombre négatif */
    result = eval_expr("sqrt(-1)", &mem, &err);
    TEST_ASSERT(err == ERR_DOMAIN, "sqrt(-1) = ERR_DOMAIN");

    /* F-CO-07 : Log de nombre négatif ou nul */
    result = eval_expr("ln(0)", &mem, &err);
    TEST_ASSERT(err == ERR_DOMAIN, "ln(0) = ERR_DOMAIN");

    result = eval_expr("log(-1)", &mem, &err);
    TEST_ASSERT(err == ERR_DOMAIN, "log(-1) = ERR_DOMAIN");

    /* F-CO-08 : Syntaxe invalide */
    result = eval_expr("3 + * 4", &mem, &err);
    TEST_ASSERT(err == ERR_SYNTAX, "3 + * 4 = ERR_SYNTAX");

    /* F-CO-08 : Token inconnu */
    result = eval_expr("3 @ 4", &mem, &err);
    TEST_ASSERT(err == ERR_SYNTAX, "3 @ 4 = ERR_SYNTAX (caractère invalide)");

    /* F-CO-10 : Nombre d'arguments incorrect */
    mem.angle_deg = 1;
    result = eval_expr("sin(30, 45)", &mem, &err);
    TEST_ASSERT(err == ERR_ARGUMENT, "sin(30, 45) = ERR_ARGUMENT (trop d'args)");
}

/* ==================== Fonctions mathématiques ==================== */

void test_math_functions(void) {
    printf("\n--- Test : Fonctions mathématiques ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* Trigonométrie - mode degrés */
    mem.angle_deg = 1;
    result = eval_expr("sin(30)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 0.5) < EPSILON, "sin(30°) = 0.5");

    result = eval_expr("cos(60)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 0.5) < EPSILON, "cos(60°) = 0.5");

    /* Trigonométrie - mode radians */
    mem.angle_deg = 0;
    result = eval_expr("sin(0)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 0.0) < EPSILON, "sin(0 rad) = 0");

    result = eval_expr("cos(0)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1.0) < EPSILON, "cos(0 rad) = 1");

    /* Vérifications de plage sin/cos (section 5.1 cahier) */
    mem.angle_deg = 1;  /* Mode degrés */
    result = eval_expr("sin(9000000000)", &mem, &err);  /* 9e9 = limite exacte */
    TEST_ASSERT(err == ERR_MATH, "sin(9e9°) = ERR_MATH (hors plage)");

    result = eval_expr("cos(9000000000)", &mem, &err);
    TEST_ASSERT(err == ERR_MATH, "cos(9e9°) = ERR_MATH (hors plage)");

    result = eval_expr("sin(1000000000)", &mem, &err);  /* 1e9 < 9e9 */
    TEST_ASSERT(err == ERR_NONE, "sin(1e9°) = OK (dans plage)");

    mem.angle_deg = 0;  /* Mode radians */
    result = eval_expr("sin(200000000)", &mem, &err);  /* 2e8 > 157079632.7 */
    TEST_ASSERT(err == ERR_MATH, "sin(2e8 rad) = ERR_MATH (hors plage)");

    result = eval_expr("cos(100000000)", &mem, &err);  /* 1e8 < 157079632.7 */
    TEST_ASSERT(err == ERR_NONE, "cos(1e8 rad) = OK (dans plage)");

    /* Logarithmes */
    mem.angle_deg = 1;
    result = eval_expr("log(100)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 2.0) < EPSILON, "log(100) = 2");

    result = eval_expr("log(2, 16)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 4.0) < EPSILON, "log(2, 16) = 4");

    result = eval_expr("ln(1)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 0.0) < EPSILON, "ln(1) = 0");

    /* Racines */
    result = eval_expr("sqrt(9)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 3.0) < EPSILON, "sqrt(9) = 3");

    result = eval_expr("cbrt(27)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 3.0) < EPSILON, "cbrt(27) = 3");
}

/* ==================== Variables et mémoire ==================== */

void test_variables(void) {
    printf("\n--- Test : Variables et mémoire ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* Affectation et utilisation */
    result = eval_expr("A = 5", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 5.0) < EPSILON, "A = 5");

    result = eval_expr("A + 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 8.0) < EPSILON, "A + 3 = 8");

    result = eval_expr("A * 2", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 10.0) < EPSILON, "A * 2 = 10");

    /* Variable B */
    result = eval_expr("B = 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 3.0) < EPSILON, "B = 3");

    result = eval_expr("A + B", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 8.0) < EPSILON, "A + B = 8");

    /* Mémoire M */
    mem.mem_M = cx_make(10.0, 0.0);
    result = eval_expr("M + 5", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 15.0) < EPSILON, "M + 5 = 15 (M=10)");

    /* Ans */
    mem.ans = cx_make(42.0, 0.0);
    result = eval_expr("Ans + 8", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 50.0) < EPSILON, "Ans + 8 = 50 (Ans=42)");
}

/* ==================== Cas du cahier des charges (section 6.3) ==================== */

void test_cahier_des_charges(void) {
    printf("\n--- Test : Cas de référence du cahier (section 6.3) ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* COMP : 4 × sin(30) × (30 + 10 × 3) = 120 */
    mem.angle_deg = 1;
    result = eval_expr("4 * sin(30) * (30 + 10 * 3)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 120.0) < EPSILON, 
                "4 * sin(30) * (30 + 10 * 3) = 120");

    /* FUNC : sinh(1) = 1.175201194 */
    result = eval_expr("sinh(1)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1.175201194) < 1e-6, 
                "sinh(1) = 1.175201194");

    /* FUNC : log(2, 16) = 4 */
    result = eval_expr("log(2, 16)", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 4.0) < EPSILON, 
                "log(2, 16) = 4");
}

/* ==================== Opérateurs postfixés ==================== */

void test_postfix_operators(void) {
    printf("\n--- Test : Opérateurs postfixés (F-CO-02 niveau 3) ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* Carré (x²) */
    result = eval_expr("5²", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 25.0) < EPSILON, "5² = 25");

    result = eval_expr("(3+2)²", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 25.0) < EPSILON, "(3+2)² = 25");

    /* Cube (x³) */
    result = eval_expr("3³", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 27.0) < EPSILON, "3³ = 27");

    /* Factorielle (n!) */
    result = eval_expr("5!", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 120.0) < EPSILON, "5! = 120");

    result = eval_expr("0!", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1.0) < EPSILON, "0! = 1");

    /* Erreurs factorielle */
    result = eval_expr("(-1)!", &mem, &err);
    TEST_ASSERT(err == ERR_DOMAIN, "(-1)! = ERR_DOMAIN");

    result = eval_expr("70!", &mem, &err);
    TEST_ASSERT(err == ERR_MATH, "70! = ERR_MATH (trop grand)");

    /* Priorité : postfixe avant multiplication */
    result = eval_expr("2*3²", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 18.0) < EPSILON, "2*3² = 18 (² avant *)");
}

/* ==================== nPr et nCr ==================== */

void test_npr_ncr(void) {
    printf("\n--- Test : Permutations nPr et Combinaisons nCr ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* nPr : 5P3 = 5!/(5-3)! = 5×4×3 = 60 */
    result = eval_expr("5 nPr 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 60.0) < EPSILON, "5 nPr 3 = 60");

    /* nPr : 10P4 = 10×9×8×7 = 5040 */
    result = eval_expr("10 nPr 4", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 5040.0) < EPSILON, "10 nPr 4 = 5040");

    /* nCr : 5C3 = 5!/(3!×2!) = 10 */
    result = eval_expr("5 nCr 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 10.0) < EPSILON, "5 nCr 3 = 10");

    /* nCr : 10C4 = 210 */
    result = eval_expr("10 nCr 4", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 210.0) < EPSILON, "10 nCr 4 = 210");

    /* Cas limites */
    result = eval_expr("5 nPr 0", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1.0) < EPSILON, "5 nPr 0 = 1");

    result = eval_expr("5 nCr 0", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1.0) < EPSILON, "5 nCr 0 = 1");

    result = eval_expr("5 nCr 5", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1.0) < EPSILON, "5 nCr 5 = 1");

    /* Erreurs */
    result = eval_expr("5 nPr 6", &mem, &err);  /* r > n */
    TEST_ASSERT(err == ERR_ARGUMENT, "5 nPr 6 = ERR_ARGUMENT (r > n)");

    result = eval_expr("5 nCr 6", &mem, &err);
    TEST_ASSERT(err == ERR_ARGUMENT, "5 nCr 6 = ERR_ARGUMENT (r > n)");

    result = eval_expr("-1 nPr 3", &mem, &err);  /* n < 0 */
    TEST_ASSERT(err == ERR_ARGUMENT, "-1 nPr 3 = ERR_ARGUMENT (n < 0)");

    /* Priorité : nPr/nCr avant multiplication */
    result = eval_expr("2 * 5 nPr 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 120.0) < EPSILON, "2 * 5 nPr 3 = 120 (nPr avant *)");
}

/* ==================== Opérateurs logiques ==================== */

void test_logical_operators(void) {
    printf("\n--- Test : Opérateurs logiques ---\n");
    CalcMemory mem;
    eval_memory_init(&mem);
    CalcError err;
    double result;

    /* AND */
    result = eval_expr("5 and 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 1.0) < EPSILON, "5 and 3 = 1 (101 & 011 = 001)");

    /* OR */
    result = eval_expr("5 or 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 7.0) < EPSILON, "5 or 3 = 7 (101 | 011 = 111)");

    /* XOR */
    result = eval_expr("5 xor 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - 6.0) < EPSILON, "5 xor 3 = 6 (101 ^ 011 = 110)");

    /* XNOR */
    result = eval_expr("5 xnor 3", &mem, &err);
    TEST_ASSERT(err == ERR_NONE && fabs(result - -7.0) < EPSILON, "5 xnor 3 = -7 ( ~(101 ^ 011) )");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Core\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_basic_operations();
    test_operator_precedence();
    test_implicit_multiplication();
    test_missing_closing_paren();
    test_max_length();
    test_error_handling();
    test_math_functions();
    test_variables();
    test_cahier_des_charges();
    test_postfix_operators();
    test_npr_ncr();
    test_logical_operators();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
