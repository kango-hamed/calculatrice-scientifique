/**
 * basen_stub.c — Implémentation factice du module BaseN (Calcul en Base-N)
 *
 * Conversions et opérateurs logiques entièrement fonctionnels.
 *
 * NE PAS MODIFIER — Remplacé par src/basen/basen.c à la milestone "BaseN stable".
 */

#include "../src/basen/basen.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

long basen_convert(long val, int from_base, int to_base) {
    /* La valeur est stockée en long (base 10 en interne) */
    (void)from_base;
    (void)to_base;
    return val; /* stub — retourne la valeur telle quelle */
}

long basen_and(long a, long b)  { return a & b; }
long basen_or(long a, long b)   { return a | b; }
long basen_xor(long a, long b)  { return a ^ b; }
long basen_xnor(long a, long b) { return ~(a ^ b); }
long basen_not(long a)          { return ~a; }
long basen_neg(long a)          { return -a; }
