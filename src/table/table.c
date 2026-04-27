#include "table.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Module : table
 * Mode TABLE — Génération de tables de valeurs f(x)
 * Référence : Cahier des charges Casio fx-570ES PLUS
 */

/* ==================== Initialisation ==================== */

void table_init(Table *t) {
    if (t == NULL) return;
    t->count = 0;
    t->start = 0.0;
    t->end = 0.0;
    t->step = 0.0;
    memset(t->points, 0, sizeof(t->points));
}

void table_set_range(Table *t, double start, double end, double step) {
    if (t == NULL) return;
    if (step == 0.0) return;  /* Éviter division par zéro */
    
    t->start = start;
    t->end = end;
    t->step = step;
}

/* ==================== Génération de table ==================== */

int table_generate(Table *t, double (*func)(double)) {
    if (t == NULL || func == NULL) return 0;
    if (t->step == 0.0) return 0;
    
    t->count = 0;
    
    /* Déterminer la direction (pas positif ou négatif) */
    int ascending = (t->step > 0.0);
    
    double x = t->start;
    int i = 0;
    
    /* Générer les points */
    while (i < TABLE_MAX_POINTS) {
        /* Vérifier si on a dépassé la fin */
        if (ascending && x > t->end + 1e-12) break;
        if (!ascending && x < t->end - 1e-12) break;
        
        t->points[i].x = x;
        t->points[i].fx = func(x);
        i++;
        
        x += t->step;
    }
    
    t->count = i;
    return i;  /* Nombre de points générés */
}

/* ==================== Accès aux valeurs ==================== */

int table_get_count(const Table *t) {
    if (t == NULL) return 0;
    return t->count;
}

double table_get_x(const Table *t, int index) {
    if (t == NULL || index < 0 || index >= t->count) return 0.0;
    return t->points[index].x;
}

double table_get_fx(const Table *t, int index) {
    if (t == NULL || index < 0 || index >= t->count) return 0.0;
    return t->points[index].fx;
}

/* ==================== Affichage ==================== */

void table_print(const Table *t, int max_rows) {
    if (t == NULL) return;
    
    int rows = (max_rows > 0 && max_rows < t->count) ? max_rows : t->count;
    
    printf("\n========== TABLE DE VALEURS ==========\n");
    printf("  f(x) de %.6g à %.6g, pas = %.6g\n", t->start, t->end, t->step);
    printf("  %d points générés\n", t->count);
    printf("---------------------------------------\n");
    printf("       x          |       f(x)          \n");
    printf("---------------------------------------\n");
    
    for (int i = 0; i < rows; i++) {
        printf("  %15.9g  |  %15.9g\n", t->points[i].x, t->points[i].fx);
    }
    
    if (t->count > rows) {
        printf("  ... (%d lignes supplémentaires)\n", t->count - rows);
    }
    
    printf("=======================================\n");
}
