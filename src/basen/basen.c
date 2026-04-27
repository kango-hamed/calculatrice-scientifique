#include "basen.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/*
 * Module : basen
 * Calcul en base N — mode BASE-N
 * Référence : Cahier des charges Casio fx-570ES PLUS
 * Supporte : BIN(2), OCT(8), DEC(10), HEX(16)
 */

/* ==================== Validation de base ==================== */

static int is_valid_base(int base) {
    return base == 2 || base == 8 || base == 10 || base == 16;
}

/* ==================== Conversion de base ==================== */

/* Convertit une valeur d'une base à une autre */
/* La valeur est toujours stockée en interne en base 10 (long) */
long basen_convert(long val, int from_base, int to_base) {
    if (!is_valid_base(from_base) || !is_valid_base(to_base)) {
        return 0;  /* Base invalide */
    }

    /* Si même base, pas de conversion nécessaire */
    if (from_base == to_base) {
        return val;
    }

    /* Étape 1 : Convertir from_base → base 10 (si nécessaire) */
    long decimal_val;
    if (from_base == 10) {
        decimal_val = val;
    } else {
        /* La valeur est déjà en base 10 en mémoire, on la considère comme telle */
        /* Ce cas gère l'affichage en différentes bases */
        decimal_val = val;
    }

    /* Étape 2 : Convertir base 10 → to_base (représentation) */
    if (to_base == 10) {
        return decimal_val;
    }

    /* Pour les autres bases, on retourne la valeur telle quelle */
    /* L'affichage/formattage est géré par l'interface */
    return decimal_val;
}

/* ==================== Opérations logiques bit à bit ==================== */

/* AND bit à bit */
long basen_and(long a, long b) {
    return a & b;
}

/* OR bit à bit */
long basen_or(long a, long b) {
    return a | b;
}

/* XOR bit à bit */
long basen_xor(long a, long b) {
    return a ^ b;
}

/* ==================== Fonctions utilitaires supplémentaires ==================== */

/* NOT bit à bit (complément à 1) */
long basen_not(long a) {
    return ~a;
}

/* Décalage à gauche */
long basen_shl(long a, int n) {
    return a << n;
}

/* Décalage à droite */
long basen_shr(long a, int n) {
    return a >> n;
}
