/**
 * memory_stub.c — Implémentation factice du module Memory (Mémoire et Variables)
 *
 * Fournit un stockage minimal fonctionnel pour permettre à l'interface
 * et aux autres modules de s'y connecter avant la finalisation de memory.
 *
 * NE PAS MODIFIER — Remplacé par src/memory/memory.c à la milestone "Memory stable".
 */

#include "../src/memory/memory.h"
#include <stdio.h>
#include <string.h>

/* Stockage interne du stub */
static double _ans = 0.0;
static double _vars[26] = {0}; /* A=0 ... Z=25 */
static double _mem  = 0.0;

void ans_store(double val) {
    _ans = val;
}

double ans_recall(void) {
    return _ans;
}

void var_store(char name, double val) {
    int idx = name - 'A';
    if (idx >= 0 && idx < 26)
        _vars[idx] = val;
}

double var_recall(char name) {
    int idx = name - 'A';
    if (idx >= 0 && idx < 26)
        return _vars[idx];
    return 0.0;
}

void mem_plus(double val) {
    _mem += val;
}

double mem_recall(void) {
    return _mem;
}

void mem_clear(void) {
    _mem = 0.0;
}
