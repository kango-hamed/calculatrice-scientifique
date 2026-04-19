#include "../../src/matrix/matrix.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

/*
 * Tests unitaires — module matrix
 * Responsable tests : (à compléter)
 *
 * Convention :
 *   - Une fonction test_xxx() par fonction testée
 *   - Utiliser assert() pour les vérifications
 *   - Afficher un message de succès en fin de chaque test
 */

#define EPSILON 1e-9
#define assert_double(a, b) assert(fabs((a) - (b)) < EPSILON)

/* ── Affichage d'une matrice ── */
void print_matrix(const char *name, Matrix m) {
    printf("%s (%dx%d) :\n", name, m.rows, m.cols);
    for (int i = 0; i < m.rows; i++) {
        printf("  [");
        for (int j = 0; j < m.cols; j++)
            printf(" %7.2f", m.data[i][j]);
        printf(" ]\n");
    }
    printf("\n");
}

/* ── test mat_add ── */
void test_mat_add(void) {
    printf("--- test_mat_add ---\n");

    /* Addition classique 2x2 */
    Matrix a = {2, 2, {{1, 2}, {3, 4}}};
    Matrix b = {2, 2, {{5, 6}, {7, 8}}};
    Matrix r = mat_add(a, b);
    print_matrix("A", a);
    print_matrix("B", b);
    print_matrix("A + B", r);

    assert(r.rows == 2 && r.cols == 2);
    assert(r.data[0][0] == 6);
    assert(r.data[0][1] == 8);
    assert(r.data[1][0] == 10);
    assert(r.data[1][1] == 12);

    /* A + 0 = A */
    Matrix zero = {2, 2, {{0, 0}, {0, 0}}};
    Matrix r2 = mat_add(a, zero);
    print_matrix("Zero", zero);
    print_matrix("A + Zero", r2);

    assert(r2.data[0][0] == a.data[0][0]);
    assert(r2.data[0][1] == a.data[0][1]);
    assert(r2.data[1][0] == a.data[1][0]);
    assert(r2.data[1][1] == a.data[1][1]);

    /* Addition 3x3 */
    Matrix c = {3, 3, {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
    Matrix d = {3, 3, {{9, 8, 7}, {6, 5, 4}, {3, 2, 1}}};
    Matrix r3 = mat_add(c, d);
    print_matrix("C (identite 3x3)", c);
    print_matrix("D", d);
    print_matrix("C + D", r3);

    assert(r3.data[0][0] == 10);
    assert(r3.data[1][1] == 6);
    assert(r3.data[2][2] == 2);

    printf("  test_mat_add        OK\n\n");
}

/* ── test mat_mul ── */
void test_mat_mul(void) {
    printf("--- test_mat_mul ---\n");

    /* A * I = A */
    Matrix I = {2, 2, {{1, 0}, {0, 1}}};
    Matrix a = {2, 2, {{3, 7}, {2, 5}}};
    Matrix r = mat_mul(a, I);
    print_matrix("A", a);
    print_matrix("I (identite)", I);
    print_matrix("A * I", r);

    assert(r.data[0][0] == 3);
    assert(r.data[0][1] == 7);
    assert(r.data[1][0] == 2);
    assert(r.data[1][1] == 5);

    /* Produit classique 2x2 */
    Matrix b = {2, 2, {{1, 2}, {3, 4}}};
    Matrix c = {2, 2, {{5, 6}, {7, 8}}};
    Matrix r2 = mat_mul(b, c);
    print_matrix("B", b);
    print_matrix("C", c);
    print_matrix("B * C", r2);

    assert(r2.data[0][0] == 19);
    assert(r2.data[0][1] == 22);
    assert(r2.data[1][0] == 43);
    assert(r2.data[1][1] == 50);

    /* A * 0 = 0 */
    Matrix zero = {2, 2, {{0, 0}, {0, 0}}};
    Matrix r3 = mat_mul(a, zero);
    print_matrix("Zero", zero);
    print_matrix("A * Zero", r3);

    assert(r3.data[0][0] == 0);
    assert(r3.data[0][1] == 0);
    assert(r3.data[1][0] == 0);
    assert(r3.data[1][1] == 0);

    printf("  test_mat_mul        OK\n\n");
}

/* ── test mat_det ── */
void test_mat_det(void) {
    printf("--- test_mat_det ---\n");

    /* Matrice 1x1 */
    Matrix m1 = {1, 1, {{42}}};
    print_matrix("M1", m1);
    printf("  det(M1) = %.2f\n\n", mat_det(m1));
    assert_double(mat_det(m1), 42.0);

    /* Matrice 2x2 : det = 3*6 - 8*4 = -14 */
    Matrix m2 = {2, 2, {{3, 8}, {4, 6}}};
    print_matrix("M2", m2);
    printf("  det(M2) = %.2f  (attendu : -14.00)\n\n", mat_det(m2));
    assert_double(mat_det(m2), -14.0);

    /* Identite 3x3 : det = 1 */
    Matrix I3 = {3, 3, {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
    print_matrix("I3 (identite 3x3)", I3);
    printf("  det(I3) = %.2f  (attendu : 1.00)\n\n", mat_det(I3));
    assert_double(mat_det(I3), 1.0);

    /* Matrice singuliere 3x3 : det = 0 */
    Matrix sing = {3, 3, {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};
    print_matrix("Singuliere", sing);
    printf("  det(Singuliere) = %.2f  (attendu : 0.00)\n\n", mat_det(sing));
    assert_double(mat_det(sing), 0.0);

    /* Matrice 3x3 : det = -306 */
    Matrix m3 = {3, 3, {{6, 1, 1}, {4, -2, 5}, {2, 8, 7}}};
    print_matrix("M3", m3);
    printf("  det(M3) = %.2f  (attendu : -306.00)\n\n", mat_det(m3));
    assert_double(mat_det(m3), -306.0);

    /* Identite 4x4 : det = 1 */
    Matrix I4 = {4, 4, {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}};
    print_matrix("I4 (identite 4x4)", I4);
    printf("  det(I4) = %.2f  (attendu : 1.00)\n\n", mat_det(I4));
    assert_double(mat_det(I4), 1.0);

    /* Matrice 4x4 : det = 24 */
    Matrix m4 = {4, 4, {{3,2,0,1},{4,0,1,2},{3,0,2,1},{9,2,3,1}}};
    print_matrix("M4", m4);
    printf("  det(M4) = %.2f  (attendu : 24.00)\n\n", mat_det(m4));
    assert_double(mat_det(m4), 24.0);

    printf("  test_mat_det        OK\n\n");
}

/* ── Main ── */
int main(void) {
    printf("Tests module matrix - debut\n\n");

    test_mat_add();
    test_mat_mul();
    test_mat_det();

    printf("Tests module matrix - OK\n");
    return 0;
}