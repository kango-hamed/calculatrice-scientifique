/**
 * complex_stub.c — Implémentation factice du module Complex (Nombres Complexes)
 *
 * Arithmétique complexe de base entièrement fonctionnelle.
 * Ce stub peut être utilisé directement par les modules eqn et functions.
 *
 * NE PAS MODIFIER — Remplacé par src/complex/complex.c à la milestone "Complex stable".
 */

#include "../src/complex/complex.h"
#include <math.h>

Complex cx_add(Complex a, Complex b) {
    return (Complex){ a.re + b.re, a.im + b.im };
}

Complex cx_sub(Complex a, Complex b) {
    return (Complex){ a.re - b.re, a.im - b.im };
}

Complex cx_mul(Complex a, Complex b) {
    return (Complex){
        a.re * b.re - a.im * b.im,
        a.re * b.im + a.im * b.re
    };
}

Complex cx_div(Complex a, Complex b) {
    double denom = b.re * b.re + b.im * b.im;
    if (denom == 0.0) return (Complex){0.0, 0.0};
    return (Complex){
        (a.re * b.re + a.im * b.im) / denom,
        (a.im * b.re - a.re * b.im) / denom
    };
}

Complex cx_conj(Complex a) {
    return (Complex){ a.re, -a.im };
}

double cx_mod(Complex a) {
    return sqrt(a.re * a.re + a.im * a.im);
}

double cx_arg(Complex a) {
    return atan2(a.im, a.re);
}
