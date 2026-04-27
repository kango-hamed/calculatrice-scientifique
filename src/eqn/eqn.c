#include "eqn.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Définition M_PI pour C99 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * Module : eqn
 * Équations — mode EQN
 * Référence : Cahier des charges Casio fx-570ES PLUS
 */

/* ==================== Équation quadratique ax² + bx + c = 0 ==================== */

int eqn_quadratic(double a, double b, double c, double *r1, double *r2) {
    if (a == 0.0) {
        /* Cas dégénéré : bx + c = 0 */
        if (b == 0.0) return 0;  /* Pas de solution ou infini */
        *r1 = -c / b;
        return 1;
    }

    double delta = b * b - 4.0 * a * c;

    if (delta < 0.0) {
        return 0;  /* Pas de solution réelle */
    }

    if (delta == 0.0) {
        *r1 = -b / (2.0 * a);
        return 1;  /* Solution double */
    }

    double sqrt_d = sqrt(delta);
    *r1 = (-b - sqrt_d) / (2.0 * a);
    *r2 = (-b + sqrt_d) / (2.0 * a);
    return 2;  /* Deux solutions */
}

/* ==================== Équation cubique ax³ + bx² + cx + d = 0 ==================== */

int eqn_cubic(double a, double b, double c, double d, double *r) {
    if (a == 0.0) {
        /* Réduit à une quadratique */
        return eqn_quadratic(b, c, d, &r[0], &r[1]);
    }

    /* Méthode de Cardan simplifiée */
    double p = (3.0 * a * c - b * b) / (3.0 * a * a);
    double q = (2.0 * b * b * b - 9.0 * a * b * c + 27.0 * a * a * d) / (27.0 * a * a * a);
    double discriminant = (q * q) / 4.0 + (p * p * p) / 27.0;

    if (discriminant > 0.0) {
        /* Une racine réelle */
        double u = cbrt(-q / 2.0 + sqrt(discriminant));
        double v = cbrt(-q / 2.0 - sqrt(discriminant));
        r[0] = u + v - b / (3.0 * a);
        return 1;
    } else if (discriminant == 0.0) {
        /* Racines multiples */
        double u = cbrt(-q / 2.0);
        r[0] = 2.0 * u - b / (3.0 * a);
        r[1] = -u - b / (3.0 * a);
        return 2;
    } else {
        /* Trois racines réelles (cas irreductible) */
        double rho = sqrt(-(p * p * p) / 27.0);
        double theta = acos(-q / (2.0 * rho));
        r[0] = 2.0 * cbrt(rho) * cos(theta / 3.0) - b / (3.0 * a);
        r[1] = 2.0 * cbrt(rho) * cos((theta + 2.0 * M_PI) / 3.0) - b / (3.0 * a);
        r[2] = 2.0 * cbrt(rho) * cos((theta + 4.0 * M_PI) / 3.0) - b / (3.0 * a);
        return 3;
    }
}

/* ==================== Système linéaire 2×2 ==================== */
/* a1x + b1y = c1 */
/* a2x + b2y = c2 */

int eqn_linear2(double coeffs[2][3], double *x, double *y) {
    double a1 = coeffs[0][0], b1 = coeffs[0][1], c1 = coeffs[0][2];
    double a2 = coeffs[1][0], b2 = coeffs[1][1], c2 = coeffs[1][2];

    double det = a1 * b2 - a2 * b1;

    if (fabs(det) < 1e-12) {
        return 0;  /* Système singulier (pas de solution unique) */
    }

    *x = (c1 * b2 - c2 * b1) / det;
    *y = (a1 * c2 - a2 * c1) / det;
    return 1;  /* Solution unique */
}
