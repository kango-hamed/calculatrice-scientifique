#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "calculus.h"

/* ===================== */
/* Calculs de base       */
/* ===================== */

double add(double a, double b) {
    return a + b;
}

double sub(double a, double b) {
    return a - b;
}

double mul(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) {
        fprintf(stderr, "Erreur: division par zéro\n");
        return NAN;
    }
    return a / b;
}

/* ===================== */
/* Quadratique           */
/* ===================== */

int solve_quadratic(double a, double b, double c, double *x1, double *x2) {
    if (a == 0) return 0;

    double delta = b * b - 4 * a * c;

    if (delta < 0) {
        return 0;
    }

    if (delta == 0) {
        *x1 = -b / (2 * a);
        return 1;
    }

    double sqrt_d = sqrt(delta);
    *x1 = (-b - sqrt_d) / (2 * a);
    *x2 = (-b + sqrt_d) / (2 * a);

    return 2;
}

/* ===================== */
/* NOTE IMPORTANTE       */
/* ===================== */
/*
 * Les fonctions ci-dessous sont des STUBS.
 * Pour un vrai projet, il faut un parseur d'expression
 * (type shunting-yard ou lib externe).
 */

/* intégrale numérique simplifiée (approximation trapezoïdale) */
double fn_integral(const char *expr, double a, double b) {
    (void)expr;

    int n = 1000;
    double h = (b - a) / n;
    double sum = 0;

    for (int i = 0; i < n; i++) {
        double x1 = a + i * h;
        double x2 = a + (i + 1) * h;

        /* exemple dummy : f(x) = x² */
        double f1 = x1 * x1;
        double f2 = x2 * x2;

        sum += (f1 + f2) / 2 * h;
    }

    return sum;
}

/* dérivée numérique */
double fn_derivative(const char *expr, double x) {
    (void)expr;

    double h = 1e-6;

    /* f(x) = x² */
    double f1 = (x + h) * (x + h);
    double f0 = (x - h) * (x - h);

    return (f1 - f0) / (2 * h);
}

/* somme discrète */
double fn_sum(const char *expr, int start, int end) {
    (void)expr;

    double sum = 0;

    for (int i = start; i <= end; i++) {
        /* f(i) = i² */
        sum += i * i;
    }

    return sum;
}