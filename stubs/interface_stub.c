/**
 * interface_stub.c — Implémentation factice du module Interface (CLI)
 *
 * Fournit une interface CLI minimale fonctionnelle :
 * lecture d'expression, affichage du résultat, affichage d'erreur.
 * Les fonctions avancées (historique, formats) sont des stubs.
 *
 * NE PAS MODIFIER — Remplacé par src/interface/interface.c à la milestone "Interface stable".
 */

#include "../src/interface/interface.h"
#include <stdio.h>
#include <string.h>

#define EXPR_MAX 256

static int _fmt    = 0; /* 0=Norm, 1=Fix, 2=Sci */
static int _digits = 6;

void cli_start(void) {
    printf("=== Calculatrice Scientifique (stub) ===\n");
    printf("Tapez une expression et appuyez sur Entree. 'quit' pour quitter.\n");
}

void cli_display_result(double val) {
    if (_fmt == 1)
        printf("= %.*f\n", _digits, val);
    else if (_fmt == 2)
        printf("= %.*e\n", _digits, val);
    else
        printf("= %g\n", val);
}

void cli_display_error(const char *msg) {
    printf("[ERREUR] %s\n", msg ? msg : "Erreur inconnue");
}

void cli_set_format(int fmt, int digits) {
    _fmt    = fmt;
    _digits = digits;
}
