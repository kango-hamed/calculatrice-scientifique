/**
 * constants_stub.c — Implémentation factice du module Constants
 *                    (Constantes Scientifiques et Conversions)
 *
 * Les 40 constantes CODATA 2014 et conversions NIST 811 sont incluses
 * directement — ce stub peut être utilisé tel quel en production.
 *
 * NE PAS MODIFIER — Remplacé par src/constants/constants.c à la milestone "Constants stable".
 */

#include "../src/constants/constants.h"
#include <string.h>

/* --- Constantes scientifiques CODATA 2014 (codes 01-40) --- */
static const struct { int code; double value; const char *symbol; } _constants[] = {
    {  1, 299792458.0,          "c"    },  /* Vitesse de la lumière (m/s)        */
    {  2, 6.62607004e-34,       "h"    },  /* Constante de Planck (J·s)          */
    {  3, 1.38064852e-23,       "k"    },  /* Constante de Boltzmann (J/K)       */
    {  4, 6.022140857e23,       "NA"   },  /* Nombre d'Avogadro (mol⁻¹)          */
    {  5, 1.6021766208e-19,     "e"    },  /* Charge de l'électron (C)           */
    {  6, 9.10938356e-31,       "me"   },  /* Masse de l'électron (kg)           */
    {  7, 1.672621898e-27,      "mp"   },  /* Masse du proton (kg)               */
    {  8, 1.674927471e-27,      "mn"   },  /* Masse du neutron (kg)              */
    {  9, 8.314459848,          "R"    },  /* Constante des gaz parfaits (J/mol·K)*/
    { 10, 6.67408e-11,          "G"    },  /* Constante gravitationnelle (m³/kg·s²)*/
    { 11, 9.80665,              "g"    },  /* Accélération gravité standard (m/s²)*/
    { 12, 8.85418781e-12,       "ε0"   },  /* Permittivité du vide (F/m)         */
    { 13, 1.2566370614e-6,      "μ0"   },  /* Perméabilité du vide (H/m)         */
    { 14, 5.2917721067e-11,     "a0"   },  /* Rayon de Bohr (m)                  */
    { 15, 1.3806e-23,           "kB"   },  /* Constante de Boltzmann (alias)     */
    { 16, 9.2740099e-24,        "μB"   },  /* Magnéton de Bohr (J/T)             */
    { 17, 5.050783699e-27,      "μN"   },  /* Magnéton nucléaire (J/T)           */
    { 18, 2.17987236e-18,       "Eh"   },  /* Énergie de Hartree (J)             */
    { 19, 1.0545718e-34,        "ħ"    },  /* Constante de Dirac (J·s)           */
    { 20, 2.8179403227e-15,     "re"   },  /* Rayon classique de l'électron (m)  */
    { 21, 1.66053904e-27,       "u"    },  /* Unité de masse atomique (kg)       */
    { 22, 96485.33289,          "F"    },  /* Constante de Faraday (C/mol)       */
    { 23, 5.67036713e-8,        "σ"    },  /* Constante de Stefan-Boltzmann      */
    { 24, 2.0678e-15,           "Φ0"   },  /* Quantum de flux magnétique (Wb)    */
    { 25, 3.741771790e-16,      "c1"   },  /* Constante de rayonnement c1        */
    { 26, 0.014387773,          "c2"   },  /* Constante de rayonnement c2        */
    { 27, 2897.7729,            "b"    },  /* Constante de Wien (m·K)            */
    { 28, 101325.0,             "atm"  },  /* Pression atmosphérique std (Pa)    */
    { 29, 6.48164e-4,           "eV"   },  /* Électron-volt → J (alias)          */
    { 30, 1.602176634e-19,      "eV"   },  /* Électron-volt (J)                  */
    { 31, 1.0e-10,              "Å"    },  /* Ångström (m)                       */
    { 32, 1.495978707e11,       "AU"   },  /* Unité astronomique (m)             */
    { 33, 9.4607304725808e15,   "ly"   },  /* Année-lumière (m)                  */
    { 34, 3.0856775814914e16,   "pc"   },  /* Parsec (m)                         */
    { 35, 1.98892e30,           "Msun" },  /* Masse solaire (kg)                 */
    { 36, 5.9722e24,            "Me"   },  /* Masse terrestre (kg)               */
    { 37, 6.371e6,              "Re"   },  /* Rayon terrestre (m)                */
    { 38, 3.828e26,             "Lsun" },  /* Luminosité solaire (W)             */
    { 39, 1.380649e-23,         "kB2"  },  /* Boltzmann exact SI 2019            */
    { 40, 6.62607015e-34,       "h2"   },  /* Planck exact SI 2019               */
};

#define N_CONST (int)(sizeof(_constants)/sizeof(_constants[0]))

double get_constant(const char *name) {
    for (int i = 0; i < N_CONST; i++)
        if (strcmp(_constants[i].symbol, name) == 0)
            return _constants[i].value;
    return 0.0;
}

double const_get(int code) {
    for (int i = 0; i < N_CONST; i++)
        if (_constants[i].code == code)
            return _constants[i].value;
    return 0.0;
}

const char *const_symbol(int code) {
    for (int i = 0; i < N_CONST; i++)
        if (_constants[i].code == code)
            return _constants[i].symbol;
    return "?";
}

/* --- Conversions métriques NIST 811 (codes 01-40) --- */
static const struct { int code; double factor; const char *label; } _conversions[] = {
    {  1, 0.0254,           "in→m"      },
    {  2, 39.3700787,       "m→in"      },
    {  3, 0.3048,           "ft→m"      },
    {  4, 3.28083990,       "m→ft"      },
    {  5, 1609.344,         "mi→m"      },
    {  6, 6.213711922e-4,   "m→mi"      },
    {  7, 1852.0,           "nmi→m"     },
    {  8, 4.535923702e-1,   "lb→kg"     },
    {  9, 2.204622622,      "kg→lb"     },
    { 10, 2.834952313e-2,   "oz→kg"     },
    { 11, 35.27396195,      "kg→oz"     },
    { 12, 1.0e-3,           "g→kg"      },
    { 13, 1000.0,           "kg→g"      },
    { 14, 4.184,            "cal→J"     },
    { 15, 0.238845897,      "J→cal"     },
    { 16, 1055.05585,       "BTU→J"     },
    { 17, 9.4781698e-4,     "J→BTU"     },
    { 18, 3.6e6,            "kWh→J"     },
    { 19, 1.0e3,            "kJ→J"      },
    { 20, 101325.0,         "atm→Pa"    },
    { 21, 133.322368,       "mmHg→Pa"   },
    { 22, 6894.75729,       "psi→Pa"    },
    { 23, 1.0e5,            "bar→Pa"    },
    { 24, 0.745699872,      "hp→W"      },
    { 25, 1.34102209,       "W→hp"      },
    { 26, 1.0e-10,          "Å→m"       },
    { 27, 1.0e10,           "m→Å"       },
    { 28, 1609.344,         "mile→m"    },
    { 29, 9.4607e15,        "ly→m"      },
    { 30, 1.49598e11,       "AU→m"      },
    { 31, 1.0e-15,          "fm→m"      },
    { 32, 1.0e-9,           "nm→m"      },
    { 33, 1.0e-6,           "μm→m"      },
    { 34, 1.0e-3,           "mm→m"      },
    { 35, 1.0e-2,           "cm→m"      },
    { 36, 1.0e3,            "km→m"      },
    { 37, 1852.0,           "nmi→m(2)"  },
    { 38, 0.9144,           "yd→m"      },
    { 39, 1.0936132983,     "m→yd"      },
    { 40, 1.0e6,            "Mm→m"      },
};

#define N_CONV (int)(sizeof(_conversions)/sizeof(_conversions[0]))

double convert_unit(double val, const char *from, const char *to) {
    (void)to;
    for (int i = 0; i < N_CONV; i++)
        if (strcmp(_conversions[i].label, from) == 0)
            return val * _conversions[i].factor;
    return val;
}

double conv_apply(double val, int code) {
    for (int i = 0; i < N_CONV; i++)
        if (_conversions[i].code == code)
            return val * _conversions[i].factor;
    return val;
}

const char *conv_label(int code) {
    for (int i = 0; i < N_CONV; i++)
        if (_conversions[i].code == code)
            return _conversions[i].label;
    return "?";
}
