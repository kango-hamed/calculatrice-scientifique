/**
 * basic_stub.c — Implémentation factice du module Basic (Opérations de base)
 *
 * Fournit des retours neutres pour permettre la compilation et le
 * développement des autres modules avant que basic soit finalisé.
 *
 * NE PAS MODIFIER — Remplacé par src/basic/basic.c à la milestone "Basic stable".
 */

#include "../src/basic/basic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double fraction_eval(int num, int den) {
    if (den == 0) return 0.0;
    return (double)num / (double)den;
}

char *to_dms(double deg) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "0°0'0\""); /* stub */
    return buf;
}

double from_dms(int d, int m, double s) {
    (void)d; (void)m; (void)s;
    return 0.0; /* stub */
}

char *to_engineering(double val) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "0e0"); /* stub */
    (void)val;
    return buf;
}
