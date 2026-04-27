#include "interface.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Module : interface
 * Interface utilisateur CLI
 * Référence : Cahier des charges Casio fx-570ES PLUS
 */

/* Variables globales pour le format d'affichage */
static int display_format = 0;  /* 0=Norm, 1=Fix, 2=Sci */
static int display_digits = 9;  /* Nombre de chiffres pour Fix/Sci */

/* ==================== Démarrage CLI ==================== */

void cli_start(void) {
    printf("\n");
    printf("  ============================================\n");
    printf("  Calculatrice Scientifique -- Core v1.0\n");
    printf("  Reference : Casio fx-570ES PLUS\n");
    printf("  Mode : Normal  |  tape 'help' pour l'aide\n");
    printf("  ============================================\n");
    printf("\n");
}

/* ==================== Affichage des résultats ==================== */

void cli_display_result(double val) {
    if (isnan(val)) {
        printf("  = Math ERROR\n");
        return;
    }
    if (isinf(val)) {
        printf("  = Overflow ERROR\n");
        return;
    }

    switch (display_format) {
        case 0:  /* Norm : notation scientifique si |x| < 10^-2 ou |x| >= 10^10 */
            if (fabs(val) < 1e-2 || fabs(val) >= 1e10) {
                printf("  = %.*e\n", display_digits, val);
            } else if (val == floor(val) && fabs(val) <= 1e15) {
                /* Entier */
                printf("  = %.0f\n", val);
            } else {
                printf("  = %g\n", val);
            }
            break;

        case 1:  /* Fix : nombre fixe de décimales */
            printf("  = %.*f\n", display_digits, val);
            break;

        case 2:  /* Sci : notation scientifique */
            printf("  = %.*e\n", display_digits, val);
            break;

        default:
            printf("  = %g\n", val);
    }
}

/* ==================== Affichage des erreurs ==================== */

void cli_display_error(const char *msg) {
    fprintf(stderr, "  [ERREUR] %s\n", msg);
}

/* ==================== Configuration du format ==================== */

void cli_set_format(int fmt, int digits) {
    if (fmt >= 0 && fmt <= 2) {
        display_format = fmt;
    }
    if (digits >= 0 && digits <= 9) {
        display_digits = digits;
    }
}
