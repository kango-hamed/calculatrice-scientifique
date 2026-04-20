#include "../../src/stat/stat.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

static int eq(double a, double b) {
    return fabs(a - b) < 1e-9;
}

static void test_stat_basic(void) {
    double x[] = {1.0, 2.0, 3.0, 4.0};
    double freq[] = {1.0, 2.0, 1.0, 0.0};
    stat_load(x, freq, 4);

    assert(eq(stat_sum(), 8.0));
    assert(eq(stat_mean(), 2.0));
    assert(eq(stat_min(), 1.0));
    assert(eq(stat_max(), 4.0));
    assert(eq(stat_stddev_pop(), sqrt(2.0 / 4.0)));
    assert(eq(stat_stddev_samp(), sqrt(2.0 / 3.0)));
    assert(eq(normal_P(0.0), 0.5));
    assert(eq(normal_Q(0.0), 0.5));
    assert(eq(normal_R(0.0), 1.0 / sqrt(2.0 * M_PI)));
}

static void test_regression_linear(void) {
    double x[] = {1.0, 2.0, 3.0};
    double y[] = {3.0, 5.0, 7.0};
    double A = 0.0, B = 0.0;
    assert(regression_linear(x, y, NULL, 3, &A, &B));
    assert(eq(A, 1.0));
    assert(eq(B, 2.0));
}

static void test_regression_quad(void) {
    double x[] = {0.0, 1.0, 2.0};
    double y[] = {2.0, 9.0, 24.0};
    double A = 0.0, B = 0.0, C = 0.0;
    assert(regression_quad(x, y, NULL, 3, &A, &B, &C));
    assert(eq(A, 2.0));
    assert(eq(B, 3.0));
    assert(eq(C, 4.0));
}

static void test_regression_power(void) {
    double x[] = {1.0, 2.0, 3.0};
    double y[] = {2.0, 16.0, 54.0};
    double A = 0.0, B = 0.0;
    assert(regression_power(x, y, NULL, 3, &A, &B));
    assert(eq(A, 2.0));
    assert(eq(B, 3.0));
}

static void test_regression_exp_e(void) {
    double x[] = {0.0, 1.0, 2.0};
    double y[] = {2.0, 2.0 * M_E, 2.0 * M_E * M_E};
    double A = 0.0, B = 0.0;
    assert(regression_exp_e(x, y, NULL, 3, &A, &B));
    assert(eq(A, 2.0));
    assert(eq(B, 1.0));
}

static void test_regression_inverse(void) {
    double x[] = {1.0, 2.0, 4.0};
    double y[] = {4.0, 2.5, 1.75};
    double A = 0.0, B = 0.0;
    assert(regression_inverse(x, y, NULL, 3, &A, &B));
    assert(eq(A, 1.0));
    assert(eq(B, 3.0));
}

int main(void) {
    printf("Tests module stat — debut\n");

    test_stat_basic();
    test_regression_linear();
    test_regression_quad();
    test_regression_power();
    test_regression_exp_e();
    test_regression_inverse();

    printf("Tests module stat — OK\n");
    return 0;
}
