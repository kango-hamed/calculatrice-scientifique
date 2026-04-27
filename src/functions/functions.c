#include "functions.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Module : functions
 * Fonctions mathématiques de base
 * Référence : Cahier des charges Casio fx-570ES PLUS
 */

/* ==================== Trigonométrie ==================== */

double fn_sin(double x) {
    return sin(x);
}

double fn_cos(double x) {
    return cos(x);
}

double fn_tan(double x) {
    return tan(x);
}

/* ==================== Logarithmes ==================== */

double fn_log(double base, double x) {
    if (base <= 0.0 || x <= 0.0 || base == 1.0) {
        return NAN;  /* Erreur de domaine */
    }
    return log(x) / log(base);
}

/* ==================== Racines ==================== */

double fn_nrt(int n, double x) {
    if (n == 0) return NAN;
    if (x < 0.0 && n % 2 == 0) return NAN;  /* Racine paire d'un nombre négatif */
    if (x == 0.0) return 0.0;
    return pow(fabs(x), 1.0 / n) * (x < 0.0 ? -1.0 : 1.0);
}

/* ==================== Combinatoire ==================== */

double fn_permut(int n, int r) {
    if (n < 0 || r < 0 || r > n) {
        return NAN;  /* Erreur d'argument */
    }
    if (n >= 1e10) return NAN;
    
    double result = 1.0;
    for (int i = 0; i < r; i++) {
        result *= (n - i);
    }
    return result;
}

double fn_combin(int n, int r) {
    if (n < 0 || r < 0 || r > n) {
        return NAN;  /* Erreur d'argument */
    }
    if (n >= 1e10) return NAN;
    
    /* Optimisation : r = min(r, n-r) */
    if (r > n - r) r = n - r;
    
    double result = 1.0;
    for (int i = 0; i < r; i++) {
        result = result * (n - i) / (i + 1);
    }
    return result;
}
