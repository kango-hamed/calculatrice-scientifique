#include "../../src/constants/constants.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

/* Définition M_PI pour C99 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * Tests unitaires — module constants
 * Couverture : 40 constantes scientifiques, conversions d'unités
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

/* ==================== Constantes mathématiques ==================== */

void test_math_constants(void) {
    printf("\n--- Test : Constantes mathématiques ---\n");

    TEST_ASSERT(fabs(get_constant("pi") - M_PI) < EPSILON, "π = 3.14159...");
    TEST_ASSERT(fabs(get_constant("e") - 2.718281828) < EPSILON, "e = 2.71828...");
    TEST_ASSERT(fabs(get_constant("sqrt2") - sqrt(2.0)) < EPSILON, "√2 = 1.41421...");
    TEST_ASSERT(fabs(get_constant("phi") - 1.618033989) < EPSILON, "φ (nombre d'or)");
}

/* ==================== Constantes physiques ==================== */

void test_physics_constants(void) {
    printf("\n--- Test : Constantes physiques ---\n");

    /* Vitesse de la lumière */
    TEST_ASSERT(fabs(get_constant("c") - 299792458.0) < 1.0, "c = 299792458 m/s");

    /* Constante gravitationnelle */
    TEST_ASSERT(fabs(get_constant("G") - 6.67430e-11) < 1e-15, "G = 6.67430e-11");

    /* Constante de Planck */
    TEST_ASSERT(fabs(get_constant("h") - 6.62607015e-34) < 1e-42, "h = 6.62607e-34 J·s");

    /* Charge élémentaire */
    TEST_ASSERT(fabs(get_constant("qe") - 1.602176634e-19) < 1e-28, "e = 1.602e-19 C");
}

/* ==================== Constantes chimiques ==================== */

void test_chemistry_constants(void) {
    printf("\n--- Test : Constantes chimiques ---\n");

    /* Nombre d'Avogadro */
    TEST_ASSERT(fabs(get_constant("NA") - 6.02214076e23) < 1e15, "Nₐ = 6.022e23 mol⁻¹");

    /* Constante des gaz parfaits */
    TEST_ASSERT(fabs(get_constant("R") - 8.314462618) < EPSILON, "R = 8.314 J/(mol·K)");

    /* Constante de Boltzmann */
    TEST_ASSERT(fabs(get_constant("k") - 1.380649e-23) < 1e-32, "k = 1.381e-23 J/K");

    /* Constante de Faraday */
    TEST_ASSERT(fabs(get_constant("F") - 96485.33212) < 0.001, "F = 96485 C/mol");
}

/* ==================== Constantes terrestres ==================== */

void test_earth_constants(void) {
    printf("\n--- Test : Constantes terrestres ---\n");

    /* Gravité terrestre */
    TEST_ASSERT(fabs(get_constant("g") - 9.80665) < EPSILON, "g = 9.80665 m/s²");

    /* Rayon Terre */
    TEST_ASSERT(fabs(get_constant("Re") - 6.371e6) < 1.0, "R⊕ = 6.371e6 m");

    /* Masse Terre */
    TEST_ASSERT(fabs(get_constant("Me") - 5.9722e24) < 1e19, "M⊕ = 5.9722e24 kg");
}

/* ==================== Conversions d'unités ==================== */

void test_unit_conversions(void) {
    printf("\n--- Test : Conversions d'unités ---\n");

    /* Longueur : mètre ↔ pied */
    TEST_ASSERT(fabs(convert_unit(1.0, "m", "ft") - 3.28084) < 0.001, "1 m ≈ 3.281 ft");
    TEST_ASSERT(fabs(convert_unit(3.28084, "ft", "m") - 1.0) < 0.001, "3.281 ft ≈ 1 m");

    /* Longueur : mètre ↔ pouce */
    TEST_ASSERT(fabs(convert_unit(1.0, "m", "in") - 39.3701) < 0.001, "1 m ≈ 39.37 in");

    /* Masse : kg ↔ livre */
    TEST_ASSERT(fabs(convert_unit(1.0, "kg", "lb") - 2.20462) < 0.001, "1 kg ≈ 2.205 lb");
    TEST_ASSERT(fabs(convert_unit(2.20462, "lb", "kg") - 1.0) < 0.001, "2.205 lb ≈ 1 kg");

    /* Température : Celsius ↔ Fahrenheit */
    TEST_ASSERT(fabs(convert_unit(0.0, "C", "F") - 32.0) < EPSILON, "0°C = 32°F");
    TEST_ASSERT(fabs(convert_unit(100.0, "C", "F") - 212.0) < EPSILON, "100°C = 212°F");
    TEST_ASSERT(fabs(convert_unit(32.0, "F", "C") - 0.0) < EPSILON, "32°F = 0°C");

    /* Température : Celsius ↔ Kelvin */
    TEST_ASSERT(fabs(convert_unit(0.0, "C", "K") - 273.15) < EPSILON, "0°C = 273.15 K");
    TEST_ASSERT(fabs(convert_unit(273.15, "K", "C") - 0.0) < EPSILON, "273.15 K = 0°C");

    /* Énergie : Joule ↔ calorie */
    TEST_ASSERT(fabs(convert_unit(1.0, "cal", "J") - 4.184) < EPSILON, "1 cal = 4.184 J");
    TEST_ASSERT(fabs(convert_unit(4.184, "J", "cal") - 1.0) < EPSILON, "4.184 J = 1 cal");

    /* Pression : Pascal ↔ atmosphère */
    TEST_ASSERT(fabs(convert_unit(101325.0, "Pa", "atm") - 1.0) < EPSILON, "101325 Pa = 1 atm");
    TEST_ASSERT(fabs(convert_unit(1.0, "atm", "Pa") - 101325.0) < EPSILON, "1 atm = 101325 Pa");
}

/* ==================== Constantes inexistantes ==================== */

void test_invalid_constants(void) {
    printf("\n--- Test : Constantes inexistantes ---\n");

    TEST_ASSERT(get_constant("xyz") == 0.0, "Constante 'xyz' inexistante retourne 0");
    TEST_ASSERT(get_constant("") == 0.0, "Chaîne vide retourne 0");
    TEST_ASSERT(get_constant(NULL) == 0.0, "NULL retourne 0");
}

/* ==================== Main ==================== */

int main(void) {
    printf("========================================\n");
    printf("Tests unitaires — Module Constants\n");
    printf("Référence : Cahier des charges Casio fx-570ES PLUS\n");
    printf("========================================\n");

    test_math_constants();
    test_physics_constants();
    test_chemistry_constants();
    test_earth_constants();
    test_unit_conversions();
    test_invalid_constants();

    printf("\n========================================\n");
    printf("Résultat : %d / %d tests passés\n", tests_passed, tests_total);
    printf("========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
