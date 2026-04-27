#include "../../src/eqn/eqn.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

/*
 * Tests unitaires — module eqn
 * Couverture : quadratique, cubique, système linéaire 2×2
 * Référence : Cahier des charges Casio fx-570ES PLUS
 */

static int tests_passed = 0;
static int tests_total = 0;

#define EPSILON 1e-9

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TEST_ASSERT(expr, msg) do { \
    tests_total++; \
    if (expr) { \
        tests_passed++; \
        printf("  [OK] %s\n", msg); \
    } else { \
        printf("  [FAIL] %s (ligne %d)\n", msg, __LINE__); \
    } \
} while(0)

/* ==================== Équation quadratique ==================== */

void test_quadratic(void) {
    printf("\n--- Test : Équation quadratique ax² + bx + c = 0 ---\n");
    double r1, r2;
    int nb;

    /* x² - 5x + 6 = 0 → x = 2, 3 */
    nb = eqn_quadratic(1.0, -5.0, 6.0, &r1, &r2);
    TEST_ASSERT(nb == 2, "x² - 5x + 6 = 0 a 2 solutions");
    TEST_ASSERT((fabs(r1 - 2.0) < EPSILON && fabs(r2 - 3.0) < EPSILON) ||
                (fabs(r1 - 3.0) < EPSILON && fabs(r2 - 2.0) < EPSILON),
                "Solutions x = 2 et x = 3");

    /* x² - 2x + 1 = 0 → x = 1 (double) */
    nb = eqn_quadratic(1.0, -2.0, 1.0, &r1, &r2);
    TEST_ASSERT(nb == 1, "x² - 2x + 1 = 0 a 1 solution");
    TEST_ASSERT(fabs(r1 - 1.0) < EPSILON, "Solution x = 1");

    /* x² + 1 = 0 → pas de solution réelle */
    nb = eqn_quadratic(1.0, 0.0, 1.0, &r1, &r2);
    TEST_ASSERT(nb == 0, "x² + 1 = 0 n'a pas de solution réelle");

    /* Cas dégénéré : 2x + 4 = 0 → x = -2 */
    nb = eqn_quadratic(0.0, 2.0, 4.0, &r1, &r2);
    TEST_ASSERT(nb == 1, "2x + 4 = 0 (linéaire) a 1 solution");
    TEST_ASSERT(fabs(r1 - (-2.0)) < EPSILON, "Solution x = -2");
}

/* ==================== Équation cubique ==================== */

void test_cubic(void) {
    printf("\n--- Test : Équation cubique ax³ + bx² + cx + d = 0 ---\n");
    double r[3];
    int nb;

    /* x³ - 6x² + 11x - 6 = 0 → (x-1)(x-2)(x-3) = 0 → x = 1, 2, 3 */
    nb = eqn_cubic(1.0, -6.0, 11.0, -6.0, r);
    TEST_ASSERT(nb == 3, "x³ - 6x² + 11x - 6 = 0 a 3 solutions");
    /* Vérifier que 1, 2, 3 sont racines (dans n'importe quel ordre) */
    int has1 = 0, has2 = 0, has3 = 0;
    for (int i = 0; i < nb; i++) {
        if (fabs(r[i] - 1.0) < 0.01) has1 = 1;
        if (fabs(r[i] - 2.0) < 0.01) has2 = 1;
        if (fabs(r[i] - 3.0) < 0.01) has3 = 1;
    }
    TEST_ASSERT(has1 && has2 && has3, "Racines 1, 2, 3 trouvées");

    /* x³ - 3x² + 3x - 1 = 0 → (x-1)³ = 0 → x = 1 (triple) */
    nb = eqn_cubic(1.0, -3.0, 3.0, -1.0, r);
    TEST_ASSERT(nb >= 1, "x³ - 3x² + 3x - 1 = 0 a au moins 1 solution");
    TEST_ASSERT(fabs(r[0] - 1.0) < 0.01, "Racine x = 1");

    /* x³ + 1 = 0 → x = -1 (une racine réelle) */
    nb = eqn_cubic(1.0, 0.0, 0.0, 1.0, r);
    TEST_ASSERT(nb >= 1, "x³ + 1 = 0 a au moins 1 solution");
    TEST_ASSERT(fabs(r[0] - (-1.0)) < 0.01 || fabs(r[0] - 1.0) < 0.01,
                "Racine réelle trouvée");

    /* Cas dégénéré : réduit à quadratique x² - 5x + 6 = 0 */
    nb = eqn_cubic(0.0, 1.0, -5.0, 6.0, r);
    TEST_ASSERT(nb == 2, "Forme dégénérée donne 2 solutions");
}

/* ==================== Système linéaire 2×2 ==================== */

void test_linear2(void) {
    printf("\n--- Test : Système linéaire 2×2 ---\n");
    double x, y;
    int nb;

    /* 2x + 3y = 8 */
    /* x - y = 1  */
    /* Solution : x = 2.2, y = 1.2 */
    double coeffs1[2][3] = {{2.0, 3.0, 8.0}, {1.0, -1.0, 1.0}};
    nb = eqn_linear2(coeffs1, &x, &y);
    TEST_ASSERT(nb == 1, "Système 1 a solution unique");
    TEST_ASSERT(fabs(x - 2.2) < EPSILON, "x = 2.2");
    TEST_ASSERT(fabs(y - 1.2) < EPSILON, "y = 1.2");

    /* x + y = 5 */
    /* 2x + 2y = 10  (même équation) */
    /* Système indéterminé */
    double coeffs2[2][3] = {{1.0, 1.0, 5.0}, {2.0, 2.0, 10.0}};
    nb = eqn_linear2(coeffs2, &x, &y);
    TEST_ASSERT(nb == 0, "Système singulier n'a pas de solution unique");

    /* 3x + 2y = 12 */
    /* x - y = 1   */
    /* Solution : x = 2.8, y = 1.8 */
    double coeffs3[2][3] = {{3.0, 2.0, 12.0}, {1.0, -1.0, 1.0}};
    nb = eqn_linear2(coeffs3, &x, &y);
    TEST_ASSERT(nb == 1, "Système 3 a solution unique");
    TEST_ASSERT(fabs(x - 2.8) < EPSILON, "x = 2.8");
    TEST_ASSERT(fabs(y - 1.8) < EPSILON, "y = 1.8");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module EQN\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_quadratic();
    test_cubic();
    test_linear2();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
