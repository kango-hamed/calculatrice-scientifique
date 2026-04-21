/**
 * functions_stub.c — Implémentation factice du module Functions (Fonctions Mathématiques)
 *
 * Délègue directement à <math.h> pour les fonctions standards.
 * Les fonctions spécifiques retournent 0.0 en attendant l'implémentation réelle.
 *
 * NE PAS MODIFIER — Remplacé par src/functions/functions.c à la milestone "Functions stable".
 */

#include "../src/functions/functions.h"
#include <math.h>

/* --- Mode angulaire partagé (0=DEG, 1=RAD, 2=GRA) --- */
static int _angle_mode = 0;

static double _to_rad(double x) {
    if (_angle_mode == 0) return x * (M_PI / 180.0);
    if (_angle_mode == 2) return x * (M_PI / 200.0);
    return x; /* déjà en radians */
}

double fn_sin(double x)  { return sin(_to_rad(x)); }
double fn_cos(double x)  { return cos(_to_rad(x)); }
double fn_tan(double x)  { return tan(_to_rad(x)); }

double fn_log(double base, double x) {
    if (base <= 0.0 || base == 1.0 || x <= 0.0) return 0.0;
    return log(x) / log(base);
}

double fn_nrt(int n, double x) {
    if (n == 0) return 0.0;
    return pow(x, 1.0 / (double)n);
}

double fn_permut(int n, int r) {
    if (r < 0 || r > n) return 0.0;
    double result = 1.0;
    for (int i = n; i > n - r; i--) result *= i;
    return result;
}

double fn_combin(int n, int r) {
    if (r < 0 || r > n) return 0.0;
    return fn_permut(n, r) / fn_permut(r, r);
}
