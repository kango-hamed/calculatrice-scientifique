#include "constants.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Module : constants
 * 40 constantes scientifiques — référence Casio fx-570ES PLUS
 * Source: CODATA 2018, sauf indication contraire
 */

/* ==================== Table des constantes ==================== */

typedef struct {
    const char *name;      /* Nom de la constante */
    const char *symbol;    /* Symbole */
    double value;          /* Valeur */
    const char *unit;      /* Unité */
} Constant;

static const Constant constants_table[] = {
    /* === Constantes mathématiques === */
    {"pi",          "π",        3.14159265358979323846,     ""},
    {"e",           "e",        2.71828182845904523536,     ""},
    {"sqrt2",       "√2",       1.41421356237309504880,     ""},
    {"phi",         "φ",        1.61803398874989484820,     ""},  /* Nombre d'or */

    /* === Physique fondamentale === */
    {"c",           "c",        299792458.0,                "m/s"},    /* Vitesse lumière */
    {"G",           "G",        6.67430e-11,                "N·m²/kg²"}, /* Gravitation */
    {"h",           "h",        6.62607015e-34,             "J·s"},    /* Planck */
    {"hbar",        "ℏ",        1.054571817e-34,            "J·s"},    /* Planck réduite */

    /* === Électromagnétisme === */
    {"e0",          "ε₀",       8.854187817e-12,            "F/m"},    /* Permittivité vide */
    {"mu0",         "μ₀",       1.2566370614e-6,            "N/A²"},   /* Perméabilité vide */
    {"qe",          "e",        1.602176634e-19,            "C"},      /* Charge élémentaire */
    {"ke",          "kₑ",       8.9875517923e9,             "N·m²/C²"}, /* Coulomb */

    /* === Atome === */
    {"me",          "mₑ",       9.1093837015e-31,           "kg"},     /* Masse électron */
    {"mp",          "mₚ",       1.67262192369e-27,          "kg"},     /* Masse proton */
    {"mn",          "mₙ",       1.67492749804e-27,          "kg"},     /* Masse neutron */
    {"a0",          "a₀",       5.29177210903e-11,          "m"},      /* Rayon Bohr */
    {"alpha",       "α",        7.2973525693e-3,            ""},       /* Constante structure fine */

    /* === Chimie === */
    {"NA",          "Nₐ",       6.02214076e23,              "mol⁻¹"},  /* Avogadro */
    {"R",           "R",        8.314462618,                "J/(mol·K)"}, /* Gaz parfait */
    {"k",           "k",        1.380649e-23,               "J/K"},    /* Boltzmann */
    {"F",           "F",        96485.33212,                "C/mol"},  /* Faraday */
    {"Vm",          "Vₘ",       22.41396954e-3,             "m³/mol"}, /* Volume molaire (STP) */

    /* === Terre === */
    {"g",           "g",        9.80665,                    "m/s²"},   /* Gravité terrestre */
    {"Re",          "R⊕",       6.371e6,                    "m"},      /* Rayon Terre */
    {"Me",          "M⊕",       5.9722e24,                  "kg"},     /* Masse Terre */
    {"au",          "au",       1.495978707e11,             "m"},      /* Unité astronomique */
    {"pc",          "pc",       3.085677581e16,             "m"},      /* Parsec */
    {"ly",          "ly",       9.4607304725808e15,          "m"},      /* Année-lumière */

    /* === Autres === */
    {"sigma",       "σ",        5.670374419e-8,             "W/(m²·K⁴)"}, /* Stefan-Boltzmann */
    {"b",           "b",        2.897771955e-3,             "m·K"},    /* Wien */
    {"muB",         "μB",       9.2740100783e-24,           "J/T"},    /* Magnéton Bohr */
    {"muN",         "μN",       5.0507837461e-27,           "J/T"},    /* Magnéton nucléaire */
    {"gf",          "g_f",      1.1663787e-5,               "GeV⁻²"}, /* Fermi */

    /* === Conversions d'unités communes === */
    {"in",          "in",       0.0254,                     "m"},      /* Pouce → mètre */
    {"ft",          "ft",       0.3048,                     "m"},      /* Pied → mètre */
    {"mi",          "mi",       1609.344,                   "m"},      /* Mile → mètre */
    {"gal",         "gal",      3.785411784e-3,             "m³"},     /* Gallon US → m³ */
    {"lb",          "lb",       0.45359237,                 "kg"},     /* Livre → kg */
    {"atm",         "atm",      101325.0,                   "Pa"},     /* Atmosphère → Pascal */
    {"cal",         "cal",      4.184,                      "J"},      /* Calorie → Joule */
    {"eV",          "eV",       1.602176634e-19,            "J"},      /* eV → Joule */
    {"hp",          "hp",       745.699872,                 "W"},      /* Cheval-vapeur → Watt */
};

#define NUM_CONSTANTS (sizeof(constants_table) / sizeof(constants_table[0]))

/* ==================== Fonction d'accès ==================== */

double get_constant(const char *name) {
    if (name == NULL) return 0.0;

    for (size_t i = 0; i < NUM_CONSTANTS; i++) {
        if (strcmp(constants_table[i].name, name) == 0) {
            return constants_table[i].value;
        }
    }
    return 0.0;  /* Constante non trouvée */
}

/* ==================== Conversions d'unités ==================== */

double convert_unit(double val, const char *from, const char *to) {
    if (from == NULL || to == NULL) return val;

    /* Si même unité, pas de conversion */
    if (strcmp(from, to) == 0) return val;

    /* Conversions de longueur */
    if (strcmp(from, "m") == 0 && strcmp(to, "ft") == 0) return val / 0.3048;
    if (strcmp(from, "ft") == 0 && strcmp(to, "m") == 0) return val * 0.3048;
    if (strcmp(from, "m") == 0 && strcmp(to, "in") == 0) return val / 0.0254;
    if (strcmp(from, "in") == 0 && strcmp(to, "m") == 0) return val * 0.0254;
    if (strcmp(from, "m") == 0 && strcmp(to, "mi") == 0) return val / 1609.344;
    if (strcmp(from, "mi") == 0 && strcmp(to, "m") == 0) return val * 1609.344;

    /* Conversions de masse */
    if (strcmp(from, "kg") == 0 && strcmp(to, "lb") == 0) return val / 0.45359237;
    if (strcmp(from, "lb") == 0 && strcmp(to, "kg") == 0) return val * 0.45359237;

    /* Conversions de température */
    if (strcmp(from, "C") == 0 && strcmp(to, "F") == 0) return val * 9.0/5.0 + 32.0;
    if (strcmp(from, "F") == 0 && strcmp(to, "C") == 0) return (val - 32.0) * 5.0/9.0;
    if (strcmp(from, "C") == 0 && strcmp(to, "K") == 0) return val + 273.15;
    if (strcmp(from, "K") == 0 && strcmp(to, "C") == 0) return val - 273.15;

    /* Conversions d'énergie */
    if (strcmp(from, "J") == 0 && strcmp(to, "cal") == 0) return val / 4.184;
    if (strcmp(from, "cal") == 0 && strcmp(to, "J") == 0) return val * 4.184;
    if (strcmp(from, "J") == 0 && strcmp(to, "eV") == 0) return val / 1.602176634e-19;
    if (strcmp(from, "eV") == 0 && strcmp(to, "J") == 0) return val * 1.602176634e-19;

    /* Conversions de pression */
    if (strcmp(from, "Pa") == 0 && strcmp(to, "atm") == 0) return val / 101325.0;
    if (strcmp(from, "atm") == 0 && strcmp(to, "Pa") == 0) return val * 101325.0;

    /* Pas de conversion connue, retourne la valeur inchangée */
    return val;
}
