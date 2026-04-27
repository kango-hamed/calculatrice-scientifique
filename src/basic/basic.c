#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "basic.h"

/* ========================= */
/* Fraction                  */
/* ========================= */

double fraction_eval(int num, int den) {
    if (den == 0) {
        fprintf(stderr, "Erreur: division par zéro (fraction)\n");
        return NAN;
    }
    return (double)num / (double)den;
}

/* ========================= */
/* Conversion DMS            */
/* ========================= */

/*
 * Convertit degrés décimaux → "D° M' S\""
 * Exemple : 12.345 → "12° 20' 42.00\""
 */
char *to_dms(double deg) {
    int d = (int)deg;
    double frac = fabs(deg - d);

    double total_seconds = frac * 3600.0;
    int m = (int)(total_seconds / 60);
    double s = total_seconds - (m * 60);

    char *result = malloc(50);
    if (!result) return NULL;

    snprintf(result, 50, "%d° %d' %.2f\"", d, m, s);
    return result;
}

/*
 * Convertit DMS → degrés décimaux
 */
double from_dms(int d, int m, double s) {
    double sign = (d < 0) ? -1.0 : 1.0;
    double deg = fabs(d) + (m / 60.0) + (s / 3600.0);
    return sign * deg;
}

/* ========================= */
/* Notation ingénieur        */
/* ========================= */

/*
 * Convertit un nombre en notation ingénieur
 * Ex: 12345 → "12.345e3"
 */
char *to_engineering(double val) {
    if (val == 0.0) {
        char *res = malloc(10);
        if (!res) return NULL;
        strcpy(res, "0");
        return res;
    }

    int exponent = (int)floor(log10(fabs(val)));
    int eng_exp = exponent - (exponent % 3);

    double mantissa = val / pow(10, eng_exp);

    char *result = malloc(50);
    if (!result) return NULL;

    snprintf(result, 50, "%.6fe%d", mantissa, eng_exp);
    return result;
}