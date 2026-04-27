#include "../../src/basen/basen.h"
#include <assert.h>
#include <stdio.h>

/*
 * Tests unitaires — module basen
 * Couverture : conversions de base, opérations logiques bit à bit
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

/* ==================== Conversions de base ==================== */

void test_convert(void) {
    printf("\n--- Test : Conversions de base ---\n");

    /* Conversion identique (même base) */
    TEST_ASSERT(basen_convert(42, 10, 10) == 42, "42(DEC) → DEC = 42");
    TEST_ASSERT(basen_convert(0b1010, 2, 2) == 0b1010, "1010(BIN) → BIN = 1010");

    /* Conversions de base valides */
    TEST_ASSERT(basen_convert(10, 10, 2) == 10, "10 → BIN (valeur stockée en décimal)");
    TEST_ASSERT(basen_convert(255, 10, 16) == 255, "255 → HEX");
    TEST_ASSERT(basen_convert(64, 10, 8) == 64, "64 → OCT");

    /* Base invalide retourne 0 */
    TEST_ASSERT(basen_convert(10, 3, 10) == 0, "Base 3 invalide retourne 0");
    TEST_ASSERT(basen_convert(10, 10, 5) == 0, "Base 5 invalide retourne 0");
}

/* ==================== Opérations logiques AND ==================== */

void test_and(void) {
    printf("\n--- Test : AND bit à bit ---\n");

    /* 0b1010 & 0b1100 = 0b1000 (8) */
    TEST_ASSERT(basen_and(0b1010, 0b1100) == 0b1000, "0b1010 & 0b1100 = 0b1000");

    /* 0b1111 & 0b0000 = 0 */
    TEST_ASSERT(basen_and(0b1111, 0b0000) == 0, "0b1111 & 0b0000 = 0");

    /* 0b1111 & 0b1111 = 0b1111 */
    TEST_ASSERT(basen_and(0b1111, 0b1111) == 0b1111, "0b1111 & 0b1111 = 0b1111");

    /* 255 & 15 = 15 */
    TEST_ASSERT(basen_and(255, 15) == 15, "255 & 15 = 15");
}

/* ==================== Opérations logiques OR ==================== */

void test_or(void) {
    printf("\n--- Test : OR bit à bit ---\n");

    /* 0b1010 | 0b1100 = 0b1110 (14) */
    TEST_ASSERT(basen_or(0b1010, 0b1100) == 0b1110, "0b1010 | 0b1100 = 0b1110");

    /* 0b1111 | 0b0000 = 0b1111 */
    TEST_ASSERT(basen_or(0b1111, 0b0000) == 0b1111, "0b1111 | 0b0000 = 0b1111");

    /* 0 | 0 = 0 */
    TEST_ASSERT(basen_or(0, 0) == 0, "0 | 0 = 0");
}

/* ==================== Opérations logiques XOR ==================== */

void test_xor(void) {
    printf("\n--- Test : XOR bit à bit ---\n");

    /* 0b1010 ^ 0b1100 = 0b0110 (6) */
    TEST_ASSERT(basen_xor(0b1010, 0b1100) == 0b0110, "0b1010 ^ 0b1100 = 0b0110");

    /* 0b1111 ^ 0b1111 = 0 */
    TEST_ASSERT(basen_xor(0b1111, 0b1111) == 0, "0b1111 ^ 0b1111 = 0");

    /* 0b1111 ^ 0b0000 = 0b1111 */
    TEST_ASSERT(basen_xor(0b1111, 0b0000) == 0b1111, "0b1111 ^ 0b0000 = 0b1111");

    /* 5 ^ 3 = 6 */
    TEST_ASSERT(basen_xor(5, 3) == 6, "5 ^ 3 = 6");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module BASE-N\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_convert();
    test_and();
    test_or();
    test_xor();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
