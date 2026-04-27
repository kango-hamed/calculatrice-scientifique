#ifndef CALC_TABLE_H
#define CALC_TABLE_H

/**
 * Module : table
 * Mode TABLE — Génération de tables de valeurs f(x)
 * Référence : Cahier des charges Casio fx-570ES PLUS
 *
 * Groupe responsable : (à compléter)
 * NE PAS MODIFIER ce fichier sans accord du groupe responsable
 * et validation du chef de projet.
 */

#define TABLE_MAX_POINTS 1024  /* Nombre max de points dans une table */

typedef struct {
    double x;
    double fx;
} TablePoint;

typedef struct {
    TablePoint points[TABLE_MAX_POINTS];
    int count;
    double start;
    double end;
    double step;
} Table;

/* Initialisation et configuration */
void table_init(Table *t);
void table_set_range(Table *t, double start, double end, double step);

/* Génération de table */
int table_generate(Table *t, double (*func)(double));

/* Accès aux valeurs */
int table_get_count(const Table *t);
double table_get_x(const Table *t, int index);
double table_get_fx(const Table *t, int index);

/* Affichage */
void table_print(const Table *t, int max_rows);

#endif /* CALC_TABLE_H */
