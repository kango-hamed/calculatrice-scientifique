#ifndef COMPLEX_H
#define COMPLEX_H

#include "core.h"

/* Fonctions supplémentaires du module complex */
ComplexValue cx_from_polar(double mod, double arg);
void         pol_to_rec(double r, double theta, double *x, double *y);
void         rec_to_pol(double x, double y, double *r, double *theta);

#endif /* COMPLEX_H */