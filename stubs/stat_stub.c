/**
 * stat_stub.c — Implémentation factice du module Stat (Statistiques)
 *
 * Stockage et calculs de base fonctionnels (moyenne, écart-type).
 * Les régressions retournent 0.0 en attendant l'implémentation finale.
 *
 * NE PAS MODIFIER — Remplacé par src/stat/stat.c à la milestone "Stat stable".
 */

#include "../src/stat/stat.h"
#include <math.h>
#include <string.h>

#define STAT_MAX 256

static double _x[STAT_MAX];
static double _freq[STAT_MAX];
static int    _n = 0;

void stat_push(double x) {
    if (_n < STAT_MAX) {
        _x[_n]    = x;
        _freq[_n] = 1.0;
        _n++;
    }
}

void stat_push2(double x, double y) {
    (void)y;
    stat_push(x); /* stub — ignore y */
}

void stat_clear(void) {
    _n = 0;
    memset(_x,    0, sizeof(_x));
    memset(_freq, 0, sizeof(_freq));
}

double stat_mean(void) {
    if (_n == 0) return 0.0;
    double sum = 0.0, total_freq = 0.0;
    for (int i = 0; i < _n; i++) {
        sum        += _x[i] * _freq[i];
        total_freq += _freq[i];
    }
    return (total_freq > 0.0) ? sum / total_freq : 0.0;
}

double stat_stddev(void) {
    if (_n == 0) return 0.0;
    double mean = stat_mean();
    double sum  = 0.0, total_freq = 0.0;
    for (int i = 0; i < _n; i++) {
        double diff = _x[i] - mean;
        sum        += _freq[i] * diff * diff;
        total_freq += _freq[i];
    }
    return (total_freq > 1.0) ? sqrt(sum / (total_freq - 1.0)) : 0.0;
}
