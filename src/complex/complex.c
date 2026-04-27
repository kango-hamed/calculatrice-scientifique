#include "complex.h"
#include <math.h>

ComplexValue cx_make(double re, double im) {
    ComplexValue c; c.re = re; c.im = im; return c;
}

ComplexValue cx_add(ComplexValue a, ComplexValue b) {
    ComplexValue c; c.re = a.re + b.re; c.im = a.im + b.im; return c;
}

ComplexValue cx_sub(ComplexValue a, ComplexValue b) {
    ComplexValue c; c.re = a.re - b.re; c.im = a.im - b.im; return c;
}

ComplexValue cx_mul(ComplexValue a, ComplexValue b) {
    ComplexValue c;
    c.re = a.re * b.re - a.im * b.im;
    c.im = a.re * b.im + a.im * b.re;
    return c;
}

ComplexValue cx_div(ComplexValue a, ComplexValue b) {
    ComplexValue c;
    double denom = b.re * b.re + b.im * b.im;
    if (denom == 0.0) return cx_make(0.0, 0.0);
    c.re = (a.re * b.re + a.im * b.im) / denom;
    c.im = (a.im * b.re - a.re * b.im) / denom;
    return c;
}

ComplexValue cx_conj(ComplexValue a) {
    ComplexValue c; c.re = a.re; c.im = -a.im; return c;
}

double cx_mod(ComplexValue a) {
    return sqrt(a.re * a.re + a.im * a.im);
}

double cx_arg(ComplexValue a) {
    return atan2(a.im, a.re);
}

int cx_is_real(ComplexValue a) {
    return fabs(a.im) < 1e-10;
}

ComplexValue cx_from_polar(double mod, double arg) {
    return cx_make(mod * cos(arg), mod * sin(arg));
}

void pol_to_rec(double r, double theta, double *x, double *y) {
    *x = r * cos(theta);
    *y = r * sin(theta);
}

void rec_to_pol(double x, double y, double *r, double *theta) {
    *r     = sqrt(x * x + y * y);
    *theta = atan2(y, x);
}