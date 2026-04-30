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

/* Soustraction de deux matrices */
Matrix mat_sub(Matrix a, Matrix b) {
    Matrix result;
    result.rows = a.rows;
    result.cols = a.cols;

    for (int i = 0; i < a.rows; i++)
        for (int j = 0; j < a.cols; j++)
            result.data[i][j] = a.data[i][j] - b.data[i][j];

    return result;
}

/* Multiplication par un scalaire */
Matrix mat_scalar_mul(Matrix a, double scalar) {
    Matrix result;
    result.rows = a.rows;
    result.cols = a.cols;

    for (int i = 0; i < a.rows; i++)
        for (int j = 0; j < a.cols; j++)
            result.data[i][j] = a.data[i][j] * scalar;

    return result;
}

/* Division par un scalaire */
Matrix mat_scalar_div(Matrix a, double scalar) {
    Matrix result;
    result.rows = a.rows;
    result.cols = a.cols;

    if (scalar == 0) return a; /* Eviter crash, gere par le moteur */

    for (int i = 0; i < a.rows; i++)
        for (int j = 0; j < a.cols; j++)
            result.data[i][j] = a.data[i][j] / scalar;

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

/* Transposee */
Matrix mat_trans(Matrix m) {
    Matrix result;
    result.rows = m.cols;
    result.cols = m.rows;
    for (int i = 0; i < m.rows; i++)
        for (int j = 0; j < m.cols; j++)
            result.data[j][i] = m.data[i][j];
    return result;
}

/* Trace */
double mat_tr(Matrix m) {
    double sum = 0.0;
    int n = (m.rows < m.cols) ? m.rows : m.cols;
    for (int i = 0; i < n; i++) sum += m.data[i][i];
    return sum;
}

/* Comatrice (pour l'inverse) */
static Matrix mat_comatrix(Matrix m) {
    Matrix result;
    result.rows = m.rows;
    result.cols = m.cols;
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            Matrix sub = sub_matrix(m, i, j);
            double sign = ((i + j) % 2 == 0) ? 1.0 : -1.0;
            result.data[i][j] = sign * mat_det(sub);
        }
    }
    return result;
}

/* Inverse */
Matrix mat_inv(Matrix m, int *success) {
    Matrix result;
    result.rows = m.rows;
    result.cols = m.cols;
    
    double det = mat_det(m);
    if (fabs(det) < 1e-12) {
        *success = 0;
        return result;
    }
    
    *success = 1;
    if (m.rows == 1) {
        result.data[0][0] = 1.0 / m.data[0][0];
        return result;
    }
    
    Matrix comat = mat_comatrix(m);
    Matrix adj = mat_trans(comat);
    return mat_scalar_div(adj, det);
}