#ifndef CALCULUS_H
#define CALCULUS_H

/**
 * Module : calculus
 * Calcul intégral et différentiel — intégrale numérique, dérivée, somme Σ
 *
 * Groupe responsable : (à compléter)
 * NE PAS MODIFIER ce fichier sans accord du groupe responsable
 * et validation du chef de projet.
 */

/* ===== Calculs de base ===== */
double add(double a, double b);
double sub(double a, double b);
double mul(double a, double b);
double divide(double a, double b);

/* ===== Résolution équation du second degré =====
 * Retour :
 *  0 = pas de solution réelle
 *  1 = une solution
 *  2 = deux solutions
 */
int solve_quadratic(double a, double b, double c, double *x1, double *x2);

/* ===== Calcul symbolique simplifié =====
 * expr est une chaîne (ex: "x*x + 2*x")
 */
double fn_integral(const char *expr, double a, double b);
double fn_derivative(const char *expr, double x);
double fn_sum(const char *expr, int start, int end);

#endif /* CALCULUS_H */
