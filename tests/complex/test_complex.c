#include "../../src/complex/complex.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

/* Définition M_PI pour C99 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * Tests unitaires — module complex
 * Couverture : addition, multiplication, module, argument, polaire
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

/* Helper pour créer un nombre complexe */
static ComplexValue cx(double re, double im) {
    ComplexValue c = {re, im};
    return c;
}

/* ==================== Addition ==================== */

void test_add(void) {
    printf("\n--- Test : Addition ---\n");
    ComplexValue result;

    /* (3+2i) + (1+4i) = (4+6i) */
    result = cx_add(cx(3, 2), cx(1, 4));
    TEST_ASSERT(fabs(result.re - 4.0) < EPSILON && fabs(result.im - 6.0) < EPSILON,
                "(3+2i) + (1+4i) = (4+6i)");

    /* (5+0i) + (0+3i) = (5+3i) */
    result = cx_add(cx(5, 0), cx(0, 3));
    TEST_ASSERT(fabs(result.re - 5.0) < EPSILON && fabs(result.im - 3.0) < EPSILON,
                "(5) + (3i) = (5+3i)");

    /* Réels purs */
    result = cx_add(cx(2, 0), cx(3, 0));
    TEST_ASSERT(fabs(result.re - 5.0) < EPSILON && fabs(result.im) < EPSILON,
                "2 + 3 = 5 (réels purs)");
}

/* ==================== Soustraction ==================== */

void test_sub(void) {
    printf("\n--- Test : Soustraction ---\n");
    ComplexValue result;

    /* (5+3i) - (2+i) = (3+2i) */
    result = cx_sub(cx(5, 3), cx(2, 1));
    TEST_ASSERT(fabs(result.re - 3.0) < EPSILON && fabs(result.im - 2.0) < EPSILON,
                "(5+3i) - (2+i) = (3+2i)");
}

/* ==================== Multiplication ==================== */

void test_mul(void) {
    printf("\n--- Test : Multiplication ---\n");
    ComplexValue result;

    /* (1+2i) * (3+4i) = (-5+10i) */
    result = cx_mul(cx(1, 2), cx(3, 4));
    TEST_ASSERT(fabs(result.re - (-5.0)) < EPSILON && fabs(result.im - 10.0) < EPSILON,
                "(1+2i) * (3+4i) = (-5+10i)");

    /* i * i = -1 */
    result = cx_mul(cx(0, 1), cx(0, 1));
    TEST_ASSERT(fabs(result.re - (-1.0)) < EPSILON && fabs(result.im) < EPSILON,
                "i * i = -1");

    /* (a+bi) * conjugué = |z|² */
    ComplexValue z = cx(3, 4);
    ComplexValue conj = cx_conj(z);
    result = cx_mul(z, conj);
    TEST_ASSERT(fabs(result.re - 25.0) < EPSILON && fabs(result.im) < EPSILON,
                "(3+4i) * (3-4i) = 25 (module au carré)");
}

/* ==================== Division ==================== */

void test_div(void) {
    printf("\n--- Test : Division ---\n");
    ComplexValue result;

    /* (5+10i) / (1+2i) = 5 */
    result = cx_div(cx(5, 10), cx(1, 2));
    TEST_ASSERT(fabs(result.re - 5.0) < EPSILON && fabs(result.im) < EPSILON,
                "(5+10i) / (1+2i) = 5");

    /* 1 / i = -i */
    result = cx_div(cx(1, 0), cx(0, 1));
    TEST_ASSERT(fabs(result.re) < EPSILON && fabs(result.im - (-1.0)) < EPSILON,
                "1 / i = -i");
}

/* ==================== Module et Argument ==================== */

void test_mod_arg(void) {
    printf("\n--- Test : Module et Argument ---\n");
    double result;

    /* |3+4i| = 5 */
    result = cx_mod(cx(3, 4));
    TEST_ASSERT(fabs(result - 5.0) < EPSILON, "|3+4i| = 5");

    /* |1+0i| = 1 */
    result = cx_mod(cx(1, 0));
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "|1| = 1");

    /* |0+1i| = 1 */
    result = cx_mod(cx(0, 1));
    TEST_ASSERT(fabs(result - 1.0) < EPSILON, "|i| = 1");

    /* arg(1+0i) = 0 */
    result = cx_arg(cx(1, 0));
    TEST_ASSERT(fabs(result - 0.0) < EPSILON, "arg(1) = 0");

    /* arg(0+1i) = π/2 */
    result = cx_arg(cx(0, 1));
    TEST_ASSERT(fabs(result - M_PI/2) < EPSILON, "arg(i) = π/2");

    /* arg(-1+0i) = π */
    result = cx_arg(cx(-1, 0));
    TEST_ASSERT(fabs(result - M_PI) < EPSILON, "arg(-1) = π");
}

/* ==================== Conversion polaire ==================== */

void test_polar(void) {
    printf("\n--- Test : Conversion polaire → algébrique ---\n");
    ComplexValue result;

    /* 5∠0° = (5, 0) */
    result = cx_from_polar(5, 0);
    TEST_ASSERT(fabs(result.re - 5.0) < EPSILON && fabs(result.im) < EPSILON,
                "5∠0° = (5, 0)");

    /* 1∠90° = (0, 1) ≈ i */
    result = cx_from_polar(1, M_PI/2);
    TEST_ASSERT(fabs(result.re) < EPSILON && fabs(result.im - 1.0) < EPSILON,
                "1∠90° = (0, 1) = i");

    /* 1∠180° = (-1, 0) */
    result = cx_from_polar(1, M_PI);
    TEST_ASSERT(fabs(result.re - (-1.0)) < EPSILON && fabs(result.im) < EPSILON,
                "1∠180° = (-1, 0)");
}

/* ==================== Conjugué ==================== */

void test_conj(void) {
    printf("\n--- Test : Conjugué ---\n");
    ComplexValue result;

    /* conj(3+4i) = (3-4i) */
    result = cx_conj(cx(3, 4));
    TEST_ASSERT(fabs(result.re - 3.0) < EPSILON && fabs(result.im - (-4.0)) < EPSILON,
                "conj(3+4i) = (3-4i)");

    /* conj(i) = -i */
    result = cx_conj(cx(0, 1));
    TEST_ASSERT(fabs(result.re) < EPSILON && fabs(result.im - (-1.0)) < EPSILON,
                "conj(i) = -i");
}

/* ==================== Conversions Pol/Rec ==================== */

void test_pol_rec(void) {
    printf("\n--- Test : Conversions Pol/Rec (coordonnées) ---\n");
    double x, y, r, theta;

    /* Pol → Rec : 5∠0° = (5, 0) */
    pol_to_rec(5.0, 0.0, &x, &y);
    TEST_ASSERT(fabs(x - 5.0) < EPSILON && fabs(y) < EPSILON,
                "Pol→Rec: 5∠0° = (5, 0)");

    /* Pol → Rec : 1∠90° = (0, 1) */
    pol_to_rec(1.0, M_PI/2, &x, &y);
    TEST_ASSERT(fabs(x) < EPSILON && fabs(y - 1.0) < EPSILON,
                "Pol→Rec: 1∠90° = (0, 1)");

    /* Pol → Rec : 10∠180° = (-10, 0) */
    pol_to_rec(10.0, M_PI, &x, &y);
    TEST_ASSERT(fabs(x - (-10.0)) < EPSILON && fabs(y) < EPSILON,
                "Pol→Rec: 10∠180° = (-10, 0)");

    /* Rec → Pol : (3, 4) = 5∠~53.13° */
    rec_to_pol(3.0, 4.0, &r, &theta);
    TEST_ASSERT(fabs(r - 5.0) < EPSILON, "Rec→Pol: |(3,4)| = 5");
    TEST_ASSERT(fabs(theta - atan2(4.0, 3.0)) < EPSILON, "Rec→Pol: arg(3,4) = atan2(4,3)");

    /* Rec → Pol : (5, 0) = 5∠0° */
    rec_to_pol(5.0, 0.0, &r, &theta);
    TEST_ASSERT(fabs(r - 5.0) < EPSILON && fabs(theta) < EPSILON,
                "Rec→Pol: (5, 0) = 5∠0°");

    /* Rec → Pol : (0, 5) = 5∠90° */
    rec_to_pol(0.0, 5.0, &r, &theta);
    TEST_ASSERT(fabs(r - 5.0) < EPSILON && fabs(theta - M_PI/2) < EPSILON,
                "Rec→Pol: (0, 5) = 5∠90°");

    /* Round-trip test : Rec → Pol → Rec */
    double x_orig = 3.0, y_orig = 4.0;
    rec_to_pol(x_orig, y_orig, &r, &theta);
    pol_to_rec(r, theta, &x, &y);
    TEST_ASSERT(fabs(x - x_orig) < EPSILON && fabs(y - y_orig) < EPSILON,
                "Round-trip: (3,4) → Pol → Rec = (3,4)");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Complex\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_add();
    test_sub();
    test_mul();
    test_div();
    test_mod_arg();
    test_polar();
    test_conj();
    test_pol_rec();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
