#include "../../src/basic/basic.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

/*
 * Tests unitaires — module basic
 * Couverture : fractions, DMS, notation ingénieur
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

/* ==================== Fractions ==================== */

void test_fractions(void) {
    printf("\n--- Test : Fractions ---\n");
    double result;

    /* Fraction simple */
    result = fraction_eval(1, 2);
    TEST_ASSERT(fabs(result - 0.5) < EPSILON, "1/2 = 0.5");

    /* Fraction négative */
    result = fraction_eval(-3, 4);
    TEST_ASSERT(fabs(result - (-0.75)) < EPSILON, "-3/4 = -0.75");

    /* Fraction > 1 */
    result = fraction_eval(7, 3);
    TEST_ASSERT(fabs(result - 2.333333333) < EPSILON, "7/3 ≈ 2.333");

    /* Division par zéro */
    result = fraction_eval(5, 0);
    TEST_ASSERT(isnan(result), "5/0 = NaN (division par zéro)");

    /* Numérateur nul */
    result = fraction_eval(0, 5);
    TEST_ASSERT(fabs(result - 0.0) < EPSILON, "0/5 = 0");
}

/* ==================== DMS (Degrés-Minutes-Secondes) ==================== */

void test_dms(void) {
    printf("\n--- Test : DMS (Degrés-Minutes-Secondes) ---\n");
    double result;
    char *str;

    /* from_dms : conversion DMS → décimal */
    result = from_dms(12, 30, 0);
    TEST_ASSERT(fabs(result - 12.5) < EPSILON, "12°30'0'' = 12.5°");

    result = from_dms(90, 0, 0);
    TEST_ASSERT(fabs(result - 90.0) < EPSILON, "90°0'0'' = 90°");

    result = from_dms(45, 30, 30);
    TEST_ASSERT(fabs(result - 45.5083333333) < 1e-6, "45°30'30'' ≈ 45.508333°");

    /* Degrés négatifs */
    result = from_dms(-30, 15, 0);
    TEST_ASSERT(fabs(result - (-30.25)) < EPSILON, "-30°15'0'' = -30.25°");

    /* to_dms : conversion décimal → DMS */
    str = to_dms(12.5);
    TEST_ASSERT(str != NULL && strstr(str, "12") != NULL, "12.5° → DMS contient 12");
    if (str) free(str);

    str = to_dms(90.0);
    TEST_ASSERT(str != NULL && strstr(str, "90") != NULL, "90° → DMS contient 90");
    if (str) free(str);

    /* Cas limite : 0° */
    str = to_dms(0.0);
    TEST_ASSERT(str != NULL && strstr(str, "0") != NULL, "0° → DMS contient 0");
    if (str) free(str);

    /* Cas : négatif */
    str = to_dms(-45.5);
    TEST_ASSERT(str != NULL && strstr(str, "-45") != NULL, "-45.5° → DMS contient -45");
    if (str) free(str);
}

/* ==================== Notation ingénieur ==================== */

void test_engineering(void) {
    printf("\n--- Test : Notation ingénieur ---\n");
    char *str;

    /* Valeur simple */
    str = to_engineering(12345.0);
    TEST_ASSERT(str != NULL && strstr(str, "e3") != NULL, "12345 → notation ingénieur (*10^3)");
    if (str) free(str);

    /* Zéro */
    str = to_engineering(0.0);
    TEST_ASSERT(str != NULL && strcmp(str, "0") == 0, "0 → 0");
    if (str) free(str);

    /* Petite valeur */
    str = to_engineering(0.00123);
    TEST_ASSERT(str != NULL && strstr(str, "e") != NULL, "0.00123 → notation ingénieur");
    if (str) free(str);

    /* Grande valeur */
    str = to_engineering(123456789.0);
    TEST_ASSERT(str != NULL && strstr(str, "e") != NULL, "123456789 → notation ingénieur");
    if (str) free(str);

    /* Valeur négative */
    str = to_engineering(-5000.0);
    TEST_ASSERT(str != NULL && strstr(str, "e") != NULL, "-5000 → notation ingénieur");
    if (str) free(str);
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Basic\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_fractions();
    test_dms();
    test_engineering();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
