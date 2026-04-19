#include "basen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Module : basen
 * TODO : implémenter les fonctions déclarées dans basen.h
 */








/* Taille max des buffers de conversion */
#define BUF_SIZE 70

/* Buffers statiques pour les retours de chaînes */
static char hex_buf[BUF_SIZE];
static char bin_buf[BUF_SIZE];
static char oct_buf[BUF_SIZE];
static char dec_buf[BUF_SIZE];

/* ---- Conversions de base ---- */

char *to_hex(long val) {
    snprintf(hex_buf, sizeof(hex_buf), "%lX", val);
    return hex_buf;
}

char *to_bin(long val) {
    if (val == 0) {
        strcpy(bin_buf, "0");
        return bin_buf;
    }
    int i = 0;
    char tmp[BUF_SIZE];
    unsigned long uval = (unsigned long)val;
    while (uval > 0) {
        tmp[i++] = (uval & 1) ? '1' : '0';
        uval >>= 1;
    }
    /* inverse */
    int j;
    for (j = 0; j < i; j++) {
        bin_buf[j] = tmp[i - 1 - j];
    }
    bin_buf[j] = '\0';
    return bin_buf;
}

char *to_oct(long val) {
    snprintf(oct_buf, sizeof(oct_buf), "%lo", val);
    return oct_buf;
}

 char *basen_convert(long val, int from_base, int to_base) {
    /* Étape 1 : val est déjà en décimal, from_base sert si val vient d'une chaîne */
    /* Étape 2 : convertir vers la base cible */
    switch (to_base) {
        case  2: return to_bin(val);
        case  8: return to_oct(val);
        case 16: return to_hex(val);
        default: /* base 10 */
            snprintf(dec_buf, sizeof(dec_buf), "%ld", val);
            return dec_buf;
    }
}

long from_base(const char *s, int base) {
    return strtol(s, NULL, base);
}

long from_binary(const char *s) {
    return from_base(s, 2);
}

long from_octal(const char *s) {
    return from_base(s, 8);
}

long from_hex(const char *s) {
    return from_base(s, 16);
}

/* ---- Opérations arithmétiques en base courante ---- */

long basen_add(long a, long b) {
    return a + b;
}

long basen_sub(long a, long b) {
    return a - b;
}

long basen_mul(long a, long b) {
    return a * b;
}

long basen_div(long a, long b) {
    if (b == 0) return 0; /* division par zéro */
    return a / b;
}

/* ---- Opérateurs logiques bit à bit ---- */

long logic_and(long a, long b) {
    return a & b;
}

long logic_or(long a, long b) {
    return a | b;
}

long logic_xor(long a, long b) {
    return a ^ b;
}

long logic_xnor(long a, long b) {
    return ~(a ^ b);
}

long logic_not(long a) {
    return ~a;  /* complément à 1 */
}

long logic_neg(long a) {
    return -a;  /* complément à 2 */
}

/* ---- Détection de préfixe inline ---- */
/*
 * Retourne la base détectée :
 *   16 → préfixe "0x" ou "0X"
 *    2 → préfixe "0b" ou "0B"
 *    8 → préfixe "0o" ou "0O"
 *   10 → préfixe "d"  ou "D", ou aucun préfixe reconnu
 */
int prefix_detect(const char *token) {
    if (token == NULL || token[0] == '\0') return 10;

    if (token[0] == '0') {
        if (token[1] == 'x' || token[1] == 'X') return 16;
        if (token[1] == 'b' || token[1] == 'B') return  2;
        if (token[1] == 'o' || token[1] == 'O') return  8;
    }
    if (token[0] == 'd' || token[0] == 'D')     return 10;

    return 10; /* décimal par défaut */
}

