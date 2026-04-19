#include "matrix.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Module : matrix
 * Implémentation des fonctions de manipulation de matrices
 */

/* Addition de deux matrices */
Matrix mat_add(Matrix a, Matrix b) {
    Matrix result;
    result.rows = a.rows;
    result.cols = a.cols;

    for (int i = 0; i < a.rows; i++)
        for (int j = 0; j < a.cols; j++)
            result.data[i][j] = a.data[i][j] + b.data[i][j];

    return result;
}

/* Multiplication de deux matrices */
Matrix mat_mul(Matrix a, Matrix b) {
    Matrix result;
    result.rows = a.rows;
    result.cols = b.cols;

    for (int i = 0; i < a.rows; i++)
        for (int j = 0; j < b.cols; j++) {
            result.data[i][j] = 0.0;
            for (int k = 0; k < a.cols; k++)
                result.data[i][j] += a.data[i][k] * b.data[k][j];
        }

    return result;
}

/* Déterminant par expansion de Laplace (récursif) */
static Matrix sub_matrix(Matrix m, int row, int col) {
    Matrix sub;
    sub.rows = m.rows - 1;
    sub.cols = m.cols - 1;

    int si = 0;
    for (int i = 0; i < m.rows; i++) {
        if (i == row) continue;
        int sj = 0;
        for (int j = 0; j < m.cols; j++) {
            if (j == col) continue;
            sub.data[si][sj] = m.data[i][j];
            sj++;
        }
        si++;
    }
    return sub;
}

double mat_det(Matrix m) {
    /* Cas de base : matrice 1x1 */
    if (m.rows == 1)
        return m.data[0][0];

    /* Cas de base : matrice 2x2 */
    if (m.rows == 2)
        return m.data[0][0] * m.data[1][1]
             - m.data[0][1] * m.data[1][0];

    /* Expansion sur la première ligne */
    double det = 0.0;
    for (int j = 0; j < m.cols; j++) {
        Matrix sub = sub_matrix(m, 0, j);
        double sign = (j % 2 == 0) ? 1.0 : -1.0;
        det += sign * m.data[0][j] * mat_det(sub);
    }
    return det;
}