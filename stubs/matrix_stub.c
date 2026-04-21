/**
 * matrix_stub.c — Implémentation factice du module Matrix (Matrices et Vecteurs)
 *
 * Opérations matricielles de base fonctionnelles (add, mul, det 2x2 et 3x3).
 * Inverse et puissance retournent une matrice nulle (stub).
 *
 * NE PAS MODIFIER — Remplacé par src/matrix/matrix.c à la milestone "Matrix stable".
 */

#include "../src/matrix/matrix.h"
#include <math.h>
#include <string.h>

/* Retourne une matrice nulle */
static Matrix _zero(int rows, int cols) {
    Matrix m;
    memset(&m, 0, sizeof(Matrix));
    m.rows = rows;
    m.cols = cols;
    return m;
}

Matrix mat_add(Matrix a, Matrix b) {
    if (a.rows != b.rows || a.cols != b.cols) return _zero(a.rows, a.cols);
    Matrix r = _zero(a.rows, a.cols);
    for (int i = 0; i < a.rows; i++)
        for (int j = 0; j < a.cols; j++)
            r.data[i][j] = a.data[i][j] + b.data[i][j];
    return r;
}

Matrix mat_mul(Matrix a, Matrix b) {
    if (a.cols != b.rows) return _zero(a.rows, b.cols);
    Matrix r = _zero(a.rows, b.cols);
    for (int i = 0; i < a.rows; i++)
        for (int j = 0; j < b.cols; j++)
            for (int k = 0; k < a.cols; k++)
                r.data[i][j] += a.data[i][k] * b.data[k][j];
    return r;
}

double mat_det(Matrix m) {
    if (m.rows != m.cols) return 0.0;
    if (m.rows == 2)
        return m.data[0][0] * m.data[1][1]
             - m.data[0][1] * m.data[1][0];
    if (m.rows == 3)
        return m.data[0][0] * (m.data[1][1]*m.data[2][2] - m.data[1][2]*m.data[2][1])
             - m.data[0][1] * (m.data[1][0]*m.data[2][2] - m.data[1][2]*m.data[2][0])
             + m.data[0][2] * (m.data[1][0]*m.data[2][1] - m.data[1][1]*m.data[2][0]);
    return 0.0; /* stub pour n > 3 */
}

Matrix mat_inverse(Matrix m) {
    /* stub — retourne matrice nulle */
    (void)m;
    return _zero(m.rows, m.cols);
}
