#include "complex.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Module : complex
 * TODO : implémenter les fonctions déclarées dans complex.h
 */
#include "complex.h"
#include <math.h>
#include <stdlib.h>

/* =========================
   CREATION
========================= */ 

Complex complex_create(double real, double imag) {
    Complex z;
    z.real = real;
    z.imag = imag;
    return z;
}

/* =========================
   OPERATIONS DE BASE
========================= */

Complex complex_add(Complex a, Complex b) {
    return complex_create(a.real + b.real, a.imag + b.imag);
}

Complex complex_sub(Complex a, Complex b) {
    return complex_create(a.real - b.real, a.imag - b.imag);
}

Complex complex_mul(Complex a, Complex b) {
    return complex_create(
        a.real * b.real - a.imag * b.imag,
        a.real * b.imag + a.imag * b.real
    );
}

Complex complex_div(Complex a, Complex b) {
    double denom = b.real * b.real + b.imag * b.imag;

    if (denom == 0) {
        printf("Erreur: division par zero\n");
        exit(EXIT_FAILURE);
    }

    return complex_create(
        (a.real * b.real + a.imag * b.imag) / denom,
        (a.imag * b.real - a.real * b.imag) / denom
    );
}

/* =========================
   PROPRIETES
========================= */

double complex_modulus(Complex z) {
    return sqrt(z.real * z.real + z.imag * z.imag);
}

double complex_argument(Complex z) {
    return atan2(z.imag, z.real);
}

Complex complex_conjugate(Complex z) {
    return complex_create(z.real, -z.imag);
}

/* =========================
   CONVERSIONS
========================= */

Polar complex_to_polar(Complex z) {
    Polar p;
    p.r = complex_modulus(z);
    p.theta = complex_argument(z);
    return p;
}

Complex polar_to_complex(Polar p) {
    return complex_create(
        p.r * cos(p.theta),
        p.r * sin(p.theta)
    );
}

/* =========================
   AFFICHAGE
========================= */

void complex_print(Complex z) {
    if (z.imag < 0)
        printf("%.2f - %.2fi\n", z.real, -z.imag);
    else
        printf("%.2f + %.2fi\n", z.real, z.imag);
}

void polar_print(Polar p) {
    printf("r = %.2f, theta = %.2f rad\n", p.r, p.theta);
}