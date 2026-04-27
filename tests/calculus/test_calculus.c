#include "../../src/calculus/calculus.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

/*
 * Tests unitaires — module calculus
 * Couverture : opérations de base, quadratique, intégrale (stub), somme (stub)
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

/* ==================== Opérations de base ==================== */

void test_basic_ops(void) {
    printf("\n--- Test : Opérations de base ---\n");

    TEST_ASSERT(fabs(add(3.0, 4.0) - 7.0) < EPSILON, "3 + 4 = 7");
    TEST_ASSERT(fabs(sub(10.0, 3.0) - 7.0) < EPSILON, "10 - 3 = 7");
    TEST_ASSERT(fabs(mul(3.0, 4.0) - 12.0) < EPSILON, "3 * 4 = 12");
    TEST_ASSERT(fabs(divide(10.0, 2.0) - 5.0) < EPSILON, "10 / 2 = 5");

    /* Division par zéro */
    double result = divide(5.0, 0.0);
    TEST_ASSERT(isnan(result), "5 / 0 = NaN");
}

/* ==================== Équation quadratique ==================== */

void test_quadratic(void) {
    printf("\n--- Test : Équation quadratique (ax² + bx + c = 0) ---\n");
    double x1, x2;
    int nb_solutions;

    /* x² - 5x + 6 = 0 → (x-2)(x-3) = 0 → x = 2, 3 */
    nb_solutions = solve_quadratic(1.0, -5.0, 6.0, &x1, &x2);
    TEST_ASSERT(nb_solutions == 2, "x² - 5x + 6 = 0 a 2 solutions");
    TEST_ASSERT(fabs(x1 - 2.0) < EPSILON || fabs(x1 - 3.0) < EPSILON, "Solution 1 correcte");
    TEST_ASSERT(fabs(x2 - 2.0) < EPSILON || fabs(x2 - 3.0) < EPSILON, "Solution 2 correcte");

    /* x² - 2x + 1 = 0 → (x-1)² = 0 → x = 1 (double) */
    nb_solutions = solve_quadratic(1.0, -2.0, 1.0, &x1, &x2);
    TEST_ASSERT(nb_solutions == 1, "x² - 2x + 1 = 0 a 1 solution double");
    TEST_ASSERT(fabs(x1 - 1.0) < EPSILON, "Solution x = 1");

    /* x² + 1 = 0 → Δ < 0 → pas de solution réelle */
    nb_solutions = solve_quadratic(1.0, 0.0, 1.0, &x1, &x2);
    TEST_ASSERT(nb_solutions == 0, "x² + 1 = 0 n'a pas de solution réelle");

    /* Cas dégénéré a = 0 (pas une quadratique) */
    nb_solutions = solve_quadratic(0.0, 2.0, 4.0, &x1, &x2);
    TEST_ASSERT(nb_solutions == 0, "a = 0 retourne 0 (pas une quadratique)");
}

/* ==================== Intégrale numérique (stub) ==================== */

void test_integral(void) {
    printf("\n--- Test : Intégrale numérique (stub x²) ---\n");

    /* ∫₀¹ x² dx = [x³/3]₀¹ = 1/3 ≈ 0.333... */
    double result = fn_integral("x*x", 0.0, 1.0);
    TEST_ASSERT(fabs(result - 1.0/3.0) < 0.01, "∫₀¹ x² dx ≈ 1/3 (stub)");

    /* ∫₀² x² dx = [x³/3]₀² = 8/3 ≈ 2.666... */
    result = fn_integral("x*x", 0.0, 2.0);
    TEST_ASSERT(fabs(result - 8.0/3.0) < 0.01, "∫₀² x² dx ≈ 8/3 (stub)");
}

/* ==================== Somme discrète (stub) ==================== */

void test_sum(void) {
    printf("\n--- Test : Somme discrète Σ (stub i²) ---\n");

    /* Σ₁³ i² = 1 + 4 + 9 = 14 */
    double result = fn_sum("i*i", 1, 3);
    TEST_ASSERT(fabs(result - 14.0) < EPSILON, "Σ₁³ i² = 14 (stub)");

    /* Σ₁⁵ i² = 1 + 4 + 9 + 16 + 25 = 55 */
    result = fn_sum("i*i", 1, 5);
    TEST_ASSERT(fabs(result - 55.0) < EPSILON, "Σ₁⁵ i² = 55 (stub)");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Calculus\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_basic_ops();
    test_quadratic();
    test_integral();
    test_sum();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
