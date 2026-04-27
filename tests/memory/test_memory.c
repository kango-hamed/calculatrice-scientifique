#include "../../src/memory/memory.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

/*
 * Tests unitaires — module memory
 * Couverture : Ans, variables A-Z, mémoire M
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

/* ==================== Ans (dernier résultat) ==================== */

void test_ans(void) {
    printf("\n--- Test : Ans (dernier résultat) ---\n");
    
    ans_store(42.0);
    double result = ans_recall();
    TEST_ASSERT(fabs(result - 42.0) < EPSILON, "Ans = 42 après stockage");
    
    ans_store(3.14159);
    result = ans_recall();
    TEST_ASSERT(fabs(result - 3.14159) < EPSILON, "Ans = π après stockage");
    
    ans_store(0.0);
    result = ans_recall();
    TEST_ASSERT(fabs(result - 0.0) < EPSILON, "Ans = 0 après stockage");
    
    ans_store(-123.456);
    result = ans_recall();
    TEST_ASSERT(fabs(result - (-123.456)) < EPSILON, "Ans = -123.456 (négatif)");
}

/* ==================== Variables A-Z ==================== */

void test_variables(void) {
    printf("\n--- Test : Variables A-Z ---\n");
    double result;
    
    /* Variable A */
    var_store('A', 100.0);
    result = var_recall('A');
    TEST_ASSERT(fabs(result - 100.0) < EPSILON, "A = 100");
    
    /* Variable Z */
    var_store('Z', 999.0);
    result = var_recall('Z');
    TEST_ASSERT(fabs(result - 999.0) < EPSILON, "Z = 999");
    
    /* Variable minuscule (doit fonctionner) */
    var_store('b', 50.0);
    result = var_recall('b');
    TEST_ASSERT(fabs(result - 50.0) < EPSILON, "b = 50 (minuscule)");
    
    /* Variable partagée min/MAJ */
    var_store('C', 75.0);
    result = var_recall('c');
    TEST_ASSERT(fabs(result - 75.0) < EPSILON, "C et c partagent la même valeur");
    
    /* Variable invalide */
    result = var_recall('1');  /* Caractère non alphabétique */
    TEST_ASSERT(fabs(result - 0.0) < EPSILON, "Caractère invalide retourne 0");
    
    /* Toutes les variables A-Z */
    for (char c = 'A'; c <= 'Z'; c++) {
        var_store(c, (double)(c - 'A' + 1));
    }
    int all_ok = 1;
    for (char c = 'A'; c <= 'Z'; c++) {
        if (fabs(var_recall(c) - (c - 'A' + 1)) > EPSILON) {
            all_ok = 0;
            break;
        }
    }
    TEST_ASSERT(all_ok, "Toutes les variables A-Z fonctionnent");
}

/* ==================== Mémoire M ==================== */

void test_memory_m(void) {
    printf("\n--- Test : Mémoire M ---\n");
    double result;
    
    /* Initialisation */
    mem_clear();
    result = mem_recall();
    TEST_ASSERT(fabs(result - 0.0) < EPSILON, "M = 0 après clear");
    
    /* M+ */
    mem_plus(10.0);
    result = mem_recall();
    TEST_ASSERT(fabs(result - 10.0) < EPSILON, "M = 10 après M+10");
    
    /* M+ cumulatif */
    mem_plus(5.0);
    result = mem_recall();
    TEST_ASSERT(fabs(result - 15.0) < EPSILON, "M = 15 après M+5");
    
    /* M+ avec négatif */
    mem_plus(-20.0);
    result = mem_recall();
    TEST_ASSERT(fabs(result - (-5.0)) < EPSILON, "M = -5 après M+(-20)");
    
    /* Clear puis nouveau stockage */
    mem_clear();
    mem_plus(100.0);
    result = mem_recall();
    TEST_ASSERT(fabs(result - 100.0) < EPSILON, "M = 100 après clear puis M+100");
    
    /* M- (simulation avec M+ valeur négative) */
    mem_clear();
    mem_plus(50.0);
    mem_plus(-30.0);  /* Équivalent à M-30 */
    result = mem_recall();
    TEST_ASSERT(fabs(result - 20.0) < EPSILON, "M = 20 après M+50 puis M-30");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Memory\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");
    
    test_ans();
    test_variables();
    test_memory_m();
    
    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");
    
    return (tests_passed == tests_total) ? 0 : 1;
}
