#include "../../src/table/table.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

/* Définition M_PI pour C99 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * Tests unitaires — module table
 * Couverture : génération de tables f(x), bornes, pas variable
 * Référence : Cahier des charges Casio fx-570ES PLUS
 */

static int tests_passed = 0;
static int tests_total = 0;

#define EPSILON 1e-9

#define TEST_ASSERT(expr, msg) do { \
    tests_total++; \
    if (expr) { \
        tests_passed++; \
        printf("  [OK] %s\n", msg); \
    } else { \
        printf("  [FAIL] %s (ligne %d)\n", msg, __LINE__); \
    } \
} while(0)

/* ==================== Fonctions de test ==================== */

static double func_linear(double x) { return 2.0 * x + 1.0; }
static double func_square(double x) { return x * x; }
static double func_sin(double x) { return sin(x); }

/* ==================== Initialisation ==================== */

void test_init(void) {
    printf("\n--- Test : Initialisation ---\n");
    Table t;
    
    table_init(&t);
    TEST_ASSERT(t.count == 0, "count = 0 après init");
    TEST_ASSERT(t.start == 0.0, "start = 0 après init");
    TEST_ASSERT(t.end == 0.0, "end = 0 après init");
    TEST_ASSERT(t.step == 0.0, "step = 0 après init");
}

/* ==================== Configuration de plage ==================== */

void test_set_range(void) {
    printf("\n--- Test : Configuration de plage ---\n");
    Table t;
    
    table_init(&t);
    table_set_range(&t, 0.0, 10.0, 1.0);
    
    TEST_ASSERT(t.start == 0.0, "start = 0.0");
    TEST_ASSERT(t.end == 10.0, "end = 10.0");
    TEST_ASSERT(t.step == 1.0, "step = 1.0");
}

/* ==================== Génération de table simple ==================== */

void test_generate_linear(void) {
    printf("\n--- Test : Génération table linéaire ---\n");
    Table t;
    
    table_init(&t);
    table_set_range(&t, 0.0, 5.0, 1.0);
    int count = table_generate(&t, func_linear);
    
    TEST_ASSERT(count == 6, "6 points générés (0,1,2,3,4,5)");
    TEST_ASSERT(table_get_count(&t) == 6, "count = 6");
    
    /* Vérifier les valeurs : f(x) = 2x+1 */
    TEST_ASSERT(fabs(table_get_x(&t, 0) - 0.0) < EPSILON, "x[0] = 0");
    TEST_ASSERT(fabs(table_get_fx(&t, 0) - 1.0) < EPSILON, "f(0) = 1");
    
    TEST_ASSERT(fabs(table_get_x(&t, 1) - 1.0) < EPSILON, "x[1] = 1");
    TEST_ASSERT(fabs(table_get_fx(&t, 1) - 3.0) < EPSILON, "f(1) = 3");
    
    TEST_ASSERT(fabs(table_get_x(&t, 5) - 5.0) < EPSILON, "x[5] = 5");
    TEST_ASSERT(fabs(table_get_fx(&t, 5) - 11.0) < EPSILON, "f(5) = 11");
}

void test_generate_square(void) {
    printf("\n--- Test : Génération table x² ---\n");
    Table t;
    
    table_init(&t);
    table_set_range(&t, -2.0, 2.0, 1.0);
    int count = table_generate(&t, func_square);
    
    TEST_ASSERT(count == 5, "5 points générés (-2,-1,0,1,2)");
    
    /* Vérifier x² */
    TEST_ASSERT(fabs(table_get_fx(&t, 0) - 4.0) < EPSILON, "f(-2) = 4");
    TEST_ASSERT(fabs(table_get_fx(&t, 1) - 1.0) < EPSILON, "f(-1) = 1");
    TEST_ASSERT(fabs(table_get_fx(&t, 2) - 0.0) < EPSILON, "f(0) = 0");
    TEST_ASSERT(fabs(table_get_fx(&t, 3) - 1.0) < EPSILON, "f(1) = 1");
    TEST_ASSERT(fabs(table_get_fx(&t, 4) - 4.0) < EPSILON, "f(2) = 4");
}

/* ==================== Pas fractionnaire ==================== */

void test_fractional_step(void) {
    printf("\n--- Test : Pas fractionnaire ---\n");
    Table t;
    
    table_init(&t);
    table_set_range(&t, 0.0, 1.0, 0.25);
    int count = table_generate(&t, func_linear);
    
    TEST_ASSERT(count == 5, "5 points avec pas=0.25");
    TEST_ASSERT(fabs(table_get_x(&t, 0) - 0.0) < EPSILON, "x[0] = 0");
    TEST_ASSERT(fabs(table_get_x(&t, 1) - 0.25) < EPSILON, "x[1] = 0.25");
    TEST_ASSERT(fabs(table_get_x(&t, 4) - 1.0) < EPSILON, "x[4] = 1.0");
}

/* ==================== Pas négatif (descendant) ==================== */

void test_negative_step(void) {
    printf("\n--- Test : Pas négatif (descendant) ---\n");
    Table t;
    
    table_init(&t);
    table_set_range(&t, 5.0, 0.0, -1.0);
    int count = table_generate(&t, func_linear);
    
    TEST_ASSERT(count == 6, "6 points avec pas négatif");
    TEST_ASSERT(fabs(table_get_x(&t, 0) - 5.0) < EPSILON, "x[0] = 5");
    TEST_ASSERT(fabs(table_get_x(&t, 5) - 0.0) < EPSILON, "x[5] = 0");
    TEST_ASSERT(fabs(table_get_fx(&t, 0) - 11.0) < EPSILON, "f(5) = 11");
    TEST_ASSERT(fabs(table_get_fx(&t, 5) - 1.0) < EPSILON, "f(0) = 1");
}

/* ==================== Fonction trigonométrique ==================== */

void test_generate_sin(void) {
    printf("\n--- Test : Table de sin(x) ---\n");
    Table t;
    
    table_init(&t);
    table_set_range(&t, 0.0, M_PI, M_PI/4);
    int count = table_generate(&t, func_sin);
    
    TEST_ASSERT(count == 5, "5 points pour sin(x) sur [0,π]");
    TEST_ASSERT(fabs(table_get_fx(&t, 0) - 0.0) < EPSILON, "sin(0) = 0");
    TEST_ASSERT(fabs(table_get_fx(&t, 2) - 1.0) < EPSILON, "sin(π/2) = 1");
    TEST_ASSERT(fabs(table_get_fx(&t, 4) - 0.0) < EPSILON, "sin(π) = 0");
}

/* ==================== Valeurs invalides ==================== */

void test_invalid_cases(void) {
    printf("\n--- Test : Cas invalides ---\n");
    Table t;
    
    /* Pas nul */
    table_init(&t);
    table_set_range(&t, 0.0, 10.0, 0.0);
    int count = table_generate(&t, func_linear);
    TEST_ASSERT(count == 0, "Pas nul = 0 points");
    
    /* Pointeur NULL */
    count = table_generate(NULL, func_linear);
    TEST_ASSERT(count == 0, "Table NULL = 0 points");
    
    /* Fonction NULL */
    table_init(&t);
    table_set_range(&t, 0.0, 5.0, 1.0);
    count = table_generate(&t, NULL);
    TEST_ASSERT(count == 0, "Fonction NULL = 0 points");
    
    /* Index invalide */
    table_generate(&t, func_linear);
    TEST_ASSERT(table_get_x(&t, -1) == 0.0, "Index -1 = 0");
    TEST_ASSERT(table_get_x(&t, 100) == 0.0, "Index 100 = 0 (hors limites)");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Table\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_init();
    test_set_range();
    test_generate_linear();
    test_generate_square();
    test_fractional_step();
    test_negative_step();
    test_generate_sin();
    test_invalid_cases();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
