#include <stdio.h>
#include "memory.h"

/* ========================= */
/* Variables globales internes */
/* ========================= */

static double ANS = 0.0;        // dernier résultat
static double MEM = 0.0;        // mémoire M

/* Variables A-Z */
static double VARS[26] = {0.0};

/* ========================= */
/* ANS (dernier résultat)    */
/* ========================= */

void ans_store(double val) {
    ANS = val;
}

double ans_recall(void) {
    return ANS;
}

/* ========================= */
/* Variables A-Z             */
/* ========================= */

void var_store(char name, double val) {
    if (name >= 'A' && name <= 'Z') {
        VARS[name - 'A'] = val;
    } else if (name >= 'a' && name <= 'z') {
        VARS[name - 'a'] = val;
    } else {
        fprintf(stderr, "Erreur: variable invalide '%c'\n", name);
    }
}

double var_recall(char name) {
    if (name >= 'A' && name <= 'Z') {
        return VARS[name - 'A'];
    } else if (name >= 'a' && name <= 'z') {
        return VARS[name - 'a'];
    }

    fprintf(stderr, "Erreur: variable invalide '%c'\n", name);
    return 0.0;
}

/* ========================= */
/* Mémoire M                */
/* ========================= */

void mem_plus(double val) {
    MEM += val;
}

double mem_recall(void) {
    return MEM;
}

void mem_clear(void) {
    MEM = 0.0;
}
