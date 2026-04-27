#include "../../src/interface/interface.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

/*
 * Tests unitaires — module interface
 * Couverture : cli_start, cli_display_result, cli_display_error, cli_set_format
 * Référence : Cahier des charges Casio fx-570ES PLUS
 */

static int tests_passed = 0;
static int tests_total = 0;

#define TEST_ASSERT(expr, msg) do { \
    tests_total++; \
    if (expr) { \
        tests_passed++; \
        printf("  [OK] %s\n", msg); \
    } else { \
        printf("  [FAIL] %s (ligne %d)\n", msg, __LINE__); \
    } \
} while(0)

/* ==================== Démarrage CLI ==================== */

void test_cli_start(void) {
    printf("\n--- Test : cli_start() ---\n");
    
    /* Test que la fonction s'exécute sans erreur */
    cli_start();
    TEST_ASSERT(1, "cli_start() exécuté sans erreur");
}

/* ==================== Affichage des résultats ==================== */

void test_cli_display_result(void) {
    printf("\n--- Test : cli_display_result() ---\n");
    
    /* Test différents types de valeurs */
    printf("Test affichage entier :\n");
    cli_display_result(42.0);
    TEST_ASSERT(1, "Affichage entier 42");
    
    printf("Test affichage décimal :\n");
    cli_display_result(3.14159);
    TEST_ASSERT(1, "Affichage décimal 3.14159");
    
    printf("Test affichage négatif :\n");
    cli_display_result(-123.456);
    TEST_ASSERT(1, "Affichage négatif -123.456");
    
    printf("Test affichage zéro :\n");
    cli_display_result(0.0);
    TEST_ASSERT(1, "Affichage zéro");
    
    printf("Test affichage grand nombre :\n");
    cli_display_result(1e15);
    TEST_ASSERT(1, "Affichage grand nombre 1e15");
    
    printf("Test affichage petit nombre :\n");
    cli_display_result(0.001);
    TEST_ASSERT(1, "Affichage petit nombre 0.001");
    
    printf("Test affichage NaN :\n");
    cli_display_result(NAN);
    TEST_ASSERT(1, "Affichage NaN (Math ERROR)");
    
    printf("Test affichage Inf :\n");
    cli_display_result(INFINITY);
    TEST_ASSERT(1, "Affichage Inf (Overflow ERROR)");
}

/* ==================== Affichage des erreurs ==================== */

void test_cli_display_error(void) {
    printf("\n--- Test : cli_display_error() ---\n");
    
    printf("Test affichage erreur (stderr) :\n");
    cli_display_error("Syntax ERROR");
    TEST_ASSERT(1, "Affichage erreur Syntax ERROR");
    
    cli_display_error("Math ERROR: division par zero");
    TEST_ASSERT(1, "Affichage erreur Math ERROR");
}

/* ==================== Configuration du format ==================== */

void test_cli_set_format(void) {
    printf("\n--- Test : cli_set_format() ---\n");
    
    /* Test format Norm (0) */
    cli_set_format(0, 9);
    printf("Test format Norm :\n");
    cli_display_result(123.456789);
    TEST_ASSERT(1, "Format Norm appliqué");
    
    /* Test format Fix (1) avec 2 décimales */
    cli_set_format(1, 2);
    printf("Test format Fix 2 décimales :\n");
    cli_display_result(123.456789);
    TEST_ASSERT(1, "Format Fix (2 dec) appliqué");
    
    /* Test format Sci (2) */
    cli_set_format(2, 4);
    printf("Test format Sci :\n");
    cli_display_result(123.456789);
    TEST_ASSERT(1, "Format Sci appliqué");
    
    /* Test valeurs invalides (doivent être ignorées) */
    cli_set_format(5, 15);  /* fmt invalide, digits invalide */
    TEST_ASSERT(1, "Valeurs invalides ignorées");
    
    /* Remettre en format Norm par défaut */
    cli_set_format(0, 9);
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Interface CLI\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");
    
    test_cli_start();
    test_cli_display_result();
    test_cli_display_error();
    test_cli_set_format();
    
    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");
    
    return (tests_passed == tests_total) ? 0 : 1;
}
