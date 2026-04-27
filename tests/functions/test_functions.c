#include "../../src/functions/functions.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

/* Définitions pour C99 (M_PI et M_E ne sont pas standard) */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

/*
 * Tests unitaires — module functions
 * Couverture : trigonométrie, log, racines, combinatoire
 * Référence : Cahier des charges Casio fx-570ES PLUS
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

/* ==================== Trigonométrie ==================== */

void test_trig(void) {
    printf("\n--- Test : Trigonométrie ---\n");
    double result;

    /* sin */
    result = fn_sin(0.0);
    TEST_ASSERT(fabs(result - 0.0) < EPSILON, "sin(0) = 0");

    result = fn_sin(M_PI / 2);
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "sin(π/2) = 1");

    /* cos */
    result = fn_cos(0.0);
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "cos(0) = 1");

    result = fn_cos(M_PI);
    TEST_ASSERT(fabs(result - (-1.0)) < EPSILON, "cos(π) = -1");

    /* tan */
    result = fn_tan(0.0);
    TEST_ASSERT(fabs(result - 0.0) < EPSILON, "tan(0) = 0");

    result = fn_tan(M_PI / 4);
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "tan(π/4) = 1");
}

/* ==================== Logarithmes ==================== */

void test_log(void) {
    printf("\n--- Test : Logarithmes ---\n");
    double result;

    /* log base 10 */
    result = fn_log(10.0, 100.0);
    TEST_ASSERT(fabs(result - 2.0) < EPSILON, "log₁₀(100) = 2");

    /* log base 2 */
    result = fn_log(2.0, 16.0);
    TEST_ASSERT(fabs(result - 4.0) < EPSILON, "log₂(16) = 4");

    /* log base e (ln) */
    result = fn_log(M_E, M_E);
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "logₑ(e) = 1");

    /* Erreurs */
    result = fn_log(1.0, 10.0);  /* base = 1 */
    TEST_ASSERT(isnan(result), "log₁(10) = NaN");

    result = fn_log(2.0, -4.0);  /* x négatif */
    TEST_ASSERT(isnan(result), "log₂(-4) = NaN");

    result = fn_log(-2.0, 4.0);  /* base négative */
    TEST_ASSERT(isnan(result), "log₋₂(4) = NaN");
}

/* ==================== Racines ==================== */

void test_nrt(void) {
    printf("\n--- Test : Racines n-ièmes ---\n");
    double result;

    /* Racine carrée */
    result = fn_nrt(2, 9.0);
    TEST_ASSERT(fabs(result - 3.0) < EPSILON, "²√9 = 3");

    /* Racine cubique */
    result = fn_nrt(3, 27.0);
    TEST_ASSERT(fabs(result - 3.0) < EPSILON, "³√27 = 3");

    /* Racine quatrième */
    result = fn_nrt(4, 16.0);
    TEST_ASSERT(fabs(result - 2.0) < EPSILON, "⁴√16 = 2");

    /* Racine cubique d'un nombre négatif (impair = OK) */
    result = fn_nrt(3, -8.0);
    TEST_ASSERT(fabs(result - (-2.0)) < EPSILON, "³√(-8) = -2");

    /* Racine paire d'un nombre négatif = erreur */
    result = fn_nrt(2, -4.0);
    TEST_ASSERT(isnan(result), "²√(-4) = NaN");

    /* n = 0 = erreur */
    result = fn_nrt(0, 4.0);
    TEST_ASSERT(isnan(result), "⁰√4 = NaN");
}

/* ==================== Combinatoire ==================== */

void test_combinatoire(void) {
    printf("\n--- Test : Combinatoire ---\n");
    double result;

    /* Permutations */
    result = fn_permut(5, 3);
    TEST_ASSERT(fabs(result - 60.0) < EPSILON, "5P3 = 60");

    result = fn_permut(10, 4);
    TEST_ASSERT(fabs(result - 5040.0) < EPSILON, "10P4 = 5040");

    /* Combinaisons */
    result = fn_combin(5, 3);
    TEST_ASSERT(fabs(result - 10.0) < EPSILON, "5C3 = 10");

    result = fn_combin(10, 4);
    TEST_ASSERT(fabs(result - 210.0) < EPSILON, "10C4 = 210");

    /* Cas limites */
    result = fn_permut(5, 0);
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "5P0 = 1");

    result = fn_combin(5, 0);
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "5C0 = 1");

    result = fn_combin(5, 5);
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "5C5 = 1");

    /* Erreurs */
    result = fn_permut(5, 6);  /* r > n */
    TEST_ASSERT(isnan(result), "5P6 = NaN");

    result = fn_combin(-1, 3);  /* n < 0 */
    TEST_ASSERT(isnan(result), "(-1)C3 = NaN");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Functions\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_trig();
    test_log();
    test_nrt();
    test_combinatoire();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
