#include "functions.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
/*
 * Module : functions
 * TODO : implémenter les fonctions déclarées dans functions.h
 */






/* ---- Mode angulaire courant ---- */
static AngleMode current_angle_mode = ANGLE_RAD;

void fn_set_angle_mode(AngleMode mode) {
    current_angle_mode = mode;
}

AngleMode fn_get_angle_mode(void) {
    return current_angle_mode;
}

/* Convertit l'angle en radians selon le mode courant */
static double to_rad(double x) {
    switch (current_angle_mode) {
        case ANGLE_DEG: return x * M_PI / 180.0;
        case ANGLE_GRA: return x * M_PI / 200.0;
        default:        return x;
    }
}

/* Convertit un angle en radians vers le mode courant */
static double from_rad(double x) {
    switch (current_angle_mode) {
        case ANGLE_DEG: return x * 180.0 / M_PI;
        case ANGLE_GRA: return x * 200.0 / M_PI;
        default:        return x;
    }
}

/* ---- Trigonométrie ---- */

double fn_sin(double x) {
    return sin(to_rad(x));
}

double fn_cos(double x) {
    return cos(to_rad(x));
}

double fn_tan(double x) {
    return tan(to_rad(x));
}

double fn_asin(double x) {
    return from_rad(asin(x));
}

double fn_acos(double x) {
    return from_rad(acos(x));
}

double fn_atan(double x) {
    return from_rad(atan(x));
}

/* ---- Hyperboliques ---- */

double fn_sinh(double x) {
    return sinh(x);
}

double fn_cosh(double x) {
    return cosh(x);
}

double fn_tanh(double x) {
    return tanh(x);
}

double fn_asinh(double x) {
    return asinh(x);
}

double fn_acosh(double x) {
    return acosh(x);
}

double fn_atanh(double x) {
    return atanh(x);
}

/* ---- Exponentielles ---- */

double fn_exp(double x) {
    return exp(x);
}

double fn_pow10(double x) {
    return pow(10.0, x);
}

/* ---- Logarithmes ---- */

double fn_log10(double x) {
    return log10(x);
}

double fn_logn(double base, double x) {
    return log(x) / log(base);
}



/* ---- Racines ---- */

double fn_sqrt(double x) {
    return sqrt(x);
}

double fn_cbrt(double x) {
    return cbrt(x);
}

double fn_nrt(int n, double x) {
    return pow(x, 1.0 / n);
}

/* ---- Puissances ---- */

double fn_pow(double x, double y) {
    return pow(x, y);
}

double fn_sq(double x) {
    return x * x;
}

double fn_cube(double x) {
    return x * x * x;
}

double fn_inv(double x) {
    return 1.0 / x;
}

/* ---- Valeur absolue ---- */

double fn_abs_val(double x) {
    return fabs(x);
}

/* ---- Factorielle ---- */

double fn_factorial(int n) {
    if (n < 0)  return -1.0; /* erreur */
    if (n == 0) return 1.0;
    double result = 1.0;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/* ---- Combinatoire ---- */

double fn_permut(int n, int r) {
    /* nPr = n! / (n-r)! */
    if (r < 0 || r > n) return -1.0;
    double result = 1.0;
    for (int i = n - r + 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

double fn_combin(int n, int r) {
    /* nCr = nPr / r! */
    if (r < 0 || r > n) return -1.0;
    return fn_permut(n, r) / fn_factorial(r);
}

/* ---- Arrondi ---- */

double fn_round_val(double x) {
    return round(x);
}

/* ---- Coordonnées polaires <-> rectangulaires ---- */

void fn_pol_to_rec(double r, double theta, double *x, double *y) {
    double rad = to_rad(theta);
    *x = r * cos(rad);
    *y = r * sin(rad);
}

void fn_rec_to_pol(double x, double y, double *r, double *theta) {
    *r     = sqrt(x * x + y * y);
    *theta = from_rad(atan2(y, x));
}

/* ---- Aléatoire ---- */

double fn_rand_num(void) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }
    return (double)rand() / RAND_MAX;
}

double fn_rand_int(int a, int b) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }
    return (double)(a + rand() % (b - a + 1));
}
