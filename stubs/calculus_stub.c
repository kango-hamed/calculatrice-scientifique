/**
 * calculus_stub.c — Implémentation factice du module Calculus (Intégral / Dérivée / SOLVE)
 *
 * Implémentations numériques minimales (méthodes simples) pour permettre
 * la compilation et les tests avant la version finale Gauss-Kronrod.
 *
 * NE PAS MODIFIER — Remplacé par src/calculus/calculus.c à la milestone "Calculus stable".
 */

#include "../src/calculus/calculus.h"
#include <math.h>

/* Intégrale numérique — méthode des trapèzes (stub simple) */
double fn_integral(const char *expr, double a, double b) {
    (void)expr;
    (void)a;
    (void)b;
    return 0.0; /* stub — nécessite le Core pour évaluer l'expression */
}

/* Dérivée numérique — différence centrée (stub simple) */
double fn_derivative(const char *expr, double x) {
    (void)expr;
    (void)x;
    return 0.0; /* stub */
}

/* Somme Σ */
double fn_sum(const char *expr, int start, int end) {
    (void)expr;
    (void)start;
    (void)end;
    return 0.0; /* stub */
}
