#ifndef CALC_MATRIX_H
#define CALC_MATRIX_H

/**
 * Module : matrix
 * Matrices et vecteurs — modes MATRIX/VECTOR/TABLE
 *
 * Groupe responsable : (à compléter)
 * NE PAS MODIFIER ce fichier sans accord du groupe responsable
 * et validation du chef de projet. Toute demande de modification
 * passe par une GitHub Issue avec le label "interface".
 */

typedef struct { int rows; int cols; double data[4][4]; } Matrix;
Matrix   mat_add(Matrix a, Matrix b);
Matrix   mat_sub(Matrix a, Matrix b);
Matrix   mat_mul(Matrix a, Matrix b);
Matrix   mat_scalar_mul(Matrix a, double scalar);
Matrix   mat_scalar_div(Matrix a, double scalar);
double   mat_det(Matrix m);
Matrix   mat_trans(Matrix m);
double   mat_tr(Matrix m);
Matrix   mat_inv(Matrix m, int *success);

#endif /* CALC_MATRIX_H */
