#include "stat.h"
#include <math.h>
#include <stddef.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define STAT_MAX_N 1024

static double stat_x[STAT_MAX_N];
static double stat_y[STAT_MAX_N];
static double stat_freq[STAT_MAX_N];
static int stat_n = 0;
static int stat_n2 = 0;
static int current_regression_type = REGRESSION_LINEAR;

void stat_set_regression_type(int type) {
    current_regression_type = type;
}

int stat_get_regression_type(void) {
    return current_regression_type;
}

static double safe_freq_value(const double *freq, int i) {
    return freq == NULL ? 1.0 : freq[i];
}

static double total_frequency(const double *freq, int n) {
    double total = 0.0;
    for (int i = 0; i < n; ++i) {
        total += safe_freq_value(freq, i);
    }
    return total;
}

static double weighted_sum(const double *values, const double *freq, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        sum += values[i] * safe_freq_value(freq, i);
    }
    return sum;
}

static int solve_3x3(double M[3][4], double result[3]) {
    for (int i = 0; i < 3; ++i) {
        int pivot = i;
        for (int j = i + 1; j < 3; ++j) {
            if (fabs(M[j][i]) > fabs(M[pivot][i])) {
                pivot = j;
            }
        }
        if (fabs(M[pivot][i]) < 1e-12) {
            return 0;
        }
        if (pivot != i) {
            for (int k = i; k < 4; ++k) {
                double tmp = M[i][k];
                M[i][k] = M[pivot][k];
                M[pivot][k] = tmp;
            }
        }
        double pivot_value = M[i][i];
        for (int k = i; k < 4; ++k) {
            M[i][k] /= pivot_value;
        }
        for (int j = 0; j < 3; ++j) {
            if (j == i) continue;
            double factor = M[j][i];
            for (int k = i; k < 4; ++k) {
                M[j][k] -= factor * M[i][k];
            }
        }
    }
    for (int i = 0; i < 3; ++i) {
        result[i] = M[i][3];
    }
    return 1;
}

void stat_clear(void) {
    stat_n = 0;
    stat_n2 = 0;
    memset(stat_x, 0, sizeof(stat_x));
    memset(stat_y, 0, sizeof(stat_y));
    memset(stat_freq, 0, sizeof(stat_freq));
}

void stat_push(double x) {
    if (stat_n < STAT_MAX_N) {
        stat_x[stat_n] = x;
        stat_freq[stat_n] = 1.0;
        stat_n += 1;
    }
}

void stat_push2(double x, double y) {
    if (stat_n2 < STAT_MAX_N) {
        stat_x[stat_n2] = x;
        stat_y[stat_n2] = y;
        stat_freq[stat_n2] = 1.0;
        stat_n2 += 1;
    }
}

void stat_load(const double *x, const double *freq, int n) {
    stat_clear();
    if (x == NULL || n <= 0) {
        return;
    }
    for (int i = 0; i < n && i < STAT_MAX_N; ++i) {
        stat_x[i] = x[i];
        stat_freq[i] = safe_freq_value(freq, i);
    }
    stat_n = n < STAT_MAX_N ? n : STAT_MAX_N;
}

void stat_input(const double *x, const double *freq, int n) {
    stat_load(x, freq, n);
}

double stat_sum(void) {
    if (stat_n <= 0) {
        return 0.0;
    }
    return weighted_sum(stat_x, stat_freq, stat_n);
}

double stat_mean(void) {
    if (stat_n <= 0) {
        return NAN;
    }
    double total = total_frequency(stat_freq, stat_n);
    if (total == 0.0) {
        return NAN;
    }
    return stat_sum() / total;
}

double stat_stddev_pop(void) {
    if (stat_n <= 0) {
        return NAN;
    }
    double mean = stat_mean();
    double total = total_frequency(stat_freq, stat_n);
    if (total == 0.0) {
        return NAN;
    }
    double sum_sq = 0.0;
    for (int i = 0; i < stat_n; ++i) {
        double d = stat_x[i] - mean;
        sum_sq += stat_freq[i] * d * d;
    }
    return sqrt(sum_sq / total);
}

double stat_stddev_samp(void) {
    if (stat_n <= 1) {
        return NAN;
    }
    double mean = stat_mean();
    double total = total_frequency(stat_freq, stat_n);
    if (total <= 1.0) {
        return NAN;
    }
    double sum_sq = 0.0;
    for (int i = 0; i < stat_n; ++i) {
        double d = stat_x[i] - mean;
        sum_sq += stat_freq[i] * d * d;
    }
    return sqrt(sum_sq / (total - 1.0));
}

double stat_min(void) {
    if (stat_n <= 0) {
        return NAN;
    }
    double value = stat_x[0];
    for (int i = 1; i < stat_n; ++i) {
        if (stat_x[i] < value) {
            value = stat_x[i];
        }
    }
    return value;
}

double stat_max(void) {
    if (stat_n <= 0) {
        return NAN;
    }
    double value = stat_x[0];
    for (int i = 1; i < stat_n; ++i) {
        if (stat_x[i] > value) {
            value = stat_x[i];
        }
    }
    return value;
}

static int linear_regression(const double *x,
                             const double *y,
                             const double *freq,
                             int n,
                             double *A,
                             double *B) {
    if (x == NULL || y == NULL || n <= 1 || A == NULL || B == NULL) {
        return 0;
    }
    double S = total_frequency(freq, n);
    if (S == 0.0) {
        return 0;
    }
    double Sx = 0.0;
    double Sy = 0.0;
    double Sxx = 0.0;
    double Sxy = 0.0;
    for (int i = 0; i < n; ++i) {
        double w = safe_freq_value(freq, i);
        double xi = x[i];
        double yi = y[i];
        Sx += w * xi;
        Sy += w * yi;
        Sxx += w * xi * xi;
        Sxy += w * xi * yi;
    }
    double denom = S * Sxx - Sx * Sx;
    if (fabs(denom) < 1e-12) {
        return 0;
    }
    *B = (S * Sxy - Sx * Sy) / denom;
    *A = (Sy - (*B) * Sx) / S;
    return 1;
}

int regression_linear(const double *x,
                      const double *y,
                      const double *freq,
                      int n,
                      double *A,
                      double *B) {
    return linear_regression(x, y, freq, n, A, B);
}

int regression_quad(const double *x,
                    const double *y,
                    const double *freq,
                    int n,
                    double *A,
                    double *B,
                    double *C) {
    if (x == NULL || y == NULL || n <= 2 || A == NULL || B == NULL || C == NULL) {
        return 0;
    }
    double S0 = 0.0;
    double S1 = 0.0;
    double S2 = 0.0;
    double S3 = 0.0;
    double S4 = 0.0;
    double T0 = 0.0;
    double T1 = 0.0;
    double T2 = 0.0;
    for (int i = 0; i < n; ++i) {
        double w = safe_freq_value(freq, i);
        double xi = x[i];
        double yi = y[i];
        double x2 = xi * xi;
        S0 += w;
        S1 += w * xi;
        S2 += w * x2;
        S3 += w * x2 * xi;
        S4 += w * x2 * x2;
        T0 += w * yi;
        T1 += w * yi * xi;
        T2 += w * yi * x2;
    }
    double M[3][4] = {
        {S0, S1, S2, T0},
        {S1, S2, S3, T1},
        {S2, S3, S4, T2}
    };
    double result[3];
    if (!solve_3x3(M, result)) {
        return 0;
    }
    *A = result[0];
    *B = result[1];
    *C = result[2];
    return 1;
}

int regression_log(const double *x,
                   const double *y,
                   const double *freq,
                   int n,
                   double *A,
                   double *B) {
    if (x == NULL || y == NULL || n <= 1 || A == NULL || B == NULL) {
        return 0;
    }
    double tx[STAT_MAX_N];
    for (int i = 0; i < n; ++i) {
        if (x[i] <= 0.0) {
            return 0;
        }
        tx[i] = log(x[i]);
    }
    return linear_regression(tx, y, freq, n, A, B);
}

int regression_exp_e(const double *x,
                     const double *y,
                     const double *freq,
                     int n,
                     double *A,
                     double *B) {
    if (x == NULL || y == NULL || n <= 1 || A == NULL || B == NULL) {
        return 0;
    }
    double ty[STAT_MAX_N];
    for (int i = 0; i < n; ++i) {
        if (y[i] <= 0.0) {
            return 0;
        }
        ty[i] = log(y[i]);
    }
    double intercept = 0.0;
    if (!linear_regression(x, ty, freq, n, &intercept, B)) {
        return 0;
    }
    *A = exp(intercept);
    return 1;
}

int regression_exp_ab(const double *x,
                      const double *y,
                      const double *freq,
                      int n,
                      double *A,
                      double *B) {
    if (x == NULL || y == NULL || n <= 1 || A == NULL || B == NULL) {
        return 0;
    }
    double ty[STAT_MAX_N];
    for (int i = 0; i < n; ++i) {
        if (y[i] <= 0.0) {
            return 0;
        }
        ty[i] = log(y[i]);
    }
    double intercept = 0.0;
    double slope = 0.0;
    if (!linear_regression(x, ty, freq, n, &intercept, &slope)) {
        return 0;
    }
    *A = exp(intercept);
    *B = exp(slope);
    return 1;
}

int regression_power(const double *x,
                     const double *y,
                     const double *freq,
                     int n,
                     double *A,
                     double *B) {
    if (x == NULL || y == NULL || n <= 1 || A == NULL || B == NULL) {
        return 0;
    }
    double tx[STAT_MAX_N];
    double ty[STAT_MAX_N];
    for (int i = 0; i < n; ++i) {
        if (x[i] <= 0.0 || y[i] <= 0.0) {
            return 0;
        }
        tx[i] = log(x[i]);
        ty[i] = log(y[i]);
    }
    double intercept = 0.0;
    if (!linear_regression(tx, ty, freq, n, &intercept, B)) {
        return 0;
    }
    *A = exp(intercept);
    return 1;
}

int regression_inverse(const double *x,
                       const double *y,
                       const double *freq,
                       int n,
                       double *A,
                       double *B) {
    if (x == NULL || y == NULL || n <= 1 || A == NULL || B == NULL) {
        return 0;
    }
    double tx[STAT_MAX_N];
    for (int i = 0; i < n; ++i) {
        if (x[i] == 0.0) {
            return 0;
        }
        tx[i] = 1.0 / x[i];
    }
    return linear_regression(tx, y, freq, n, A, B);
}

int regression(int type,
               const double *x,
               const double *y,
               const double *freq,
               int n,
               double *coef,
               int count) {
    if (coef == NULL || x == NULL || y == NULL) {
        return 0;
    }
    switch (type) {
        case REGRESSION_LINEAR:
            if (count < 2) return 0;
            return regression_linear(x, y, freq, n, &coef[0], &coef[1]);
        case REGRESSION_QUAD:
            if (count < 3) return 0;
            return regression_quad(x, y, freq, n, &coef[0], &coef[1], &coef[2]);
        case REGRESSION_LOG:
            if (count < 2) return 0;
            return regression_log(x, y, freq, n, &coef[0], &coef[1]);
        case REGRESSION_EXP_E:
            if (count < 2) return 0;
            return regression_exp_e(x, y, freq, n, &coef[0], &coef[1]);
        case REGRESSION_EXP_AB:
            if (count < 2) return 0;
            return regression_exp_ab(x, y, freq, n, &coef[0], &coef[1]);
        case REGRESSION_POWER:
            if (count < 2) return 0;
            return regression_power(x, y, freq, n, &coef[0], &coef[1]);
        case REGRESSION_INVERSE:
            if (count < 2) return 0;
            return regression_inverse(x, y, freq, n, &coef[0], &coef[1]);
        default:
            return 0;
    }
}

double normal_R(double t) {
    return exp(-0.5 * t * t) / sqrt(2.0 * M_PI);
}

double normal_P(double t) {
    return 0.5 * (1.0 + erf(t / sqrt(2.0)));
}

double normal_Q(double t) {
    return 1.0 - normal_P(t);
}

double stat_var(void) {
    double sd = stat_stddev_samp();
    return sd * sd;
}

int stat_count(void) {
    return stat_n;
}

double normalize(double x) {
    double mean = stat_mean();
    double sd = stat_stddev_pop();
    if (!isfinite(mean) || !isfinite(sd) || fabs(sd) < 1e-15) {
        return 0.0;
    }
    return (x - mean) / sd;
}

double stat_reg_A(int type) {
    double coef[3] = {0, 0, 0};
    regression(type, stat_x, stat_y, stat_freq, stat_n, coef, (type == REGRESSION_QUAD) ? 3 : 2);
    return coef[0];
}

double stat_reg_B(int type) {
    double coef[3] = {0, 0, 0};
    regression(type, stat_x, stat_y, stat_freq, stat_n, coef, (type == REGRESSION_QUAD) ? 3 : 2);
    return coef[1];
}

double stat_reg_C(int type) {
    double coef[3] = {0, 0, 0};
    if (type == REGRESSION_QUAD) {
        regression(type, stat_x, stat_y, stat_freq, stat_n, coef, 3);
        return coef[2];
    }
    return 0.0;
}
