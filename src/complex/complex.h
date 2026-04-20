#ifndef COMPLEX_H
#define COMPLEX_H

#include <stdio.h>

/* =========================
   STRUCTURES
========================= */

typedef struct {
    double real;
    double imag;
} Complex;

typedef struct {
    double r;
    double theta;
} Polar;

/* =========================
   CREATION
========================= */

Complex complex_create(double real, double imag);

/* =========================
   OPERATIONS DE BASE
========================= */

Complex complex_add(Complex a, Complex b);
Complex complex_sub(Complex a, Complex b);
Complex complex_mul(Complex a, Complex b);
Complex complex_div(Complex a, Complex b);

/* =========================
   PROPRIETES
========================= */

double complex_modulus(Complex z);
double complex_argument(Complex z);
Complex complex_conjugate(Complex z);

/* =========================
   CONVERSIONS
========================= */

Polar complex_to_polar(Complex z);
Complex polar_to_complex(Polar p);

/* =========================
   AFFICHAGE
========================= */

void complex_print(Complex z);
void polar_print(Polar p);

#endif