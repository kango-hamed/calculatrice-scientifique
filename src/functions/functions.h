 #ifndef FUNCTIONS_H
#define FUNCTIONS_H

/**
 * Module : functions
 * Fonctions mathématiques — trigonométrie, log, puissances, combinatoire
 *
 * Groupe responsable : (à compléter)
 * NE PAS MODIFIER ce fichier sans accord du groupe responsable
 * et validation du chef de projet. Toute demande de modification
 * passe par une GitHub Issue avec le label "interface".
 */






/* --- Mode angulaire --- */
typedef enum {
    ANGLE_DEG,
    ANGLE_RAD,
    ANGLE_GRA
} AngleMode;

void fn_set_angle_mode(AngleMode mode);
AngleMode fn_get_angle_mode(void);

/* --- Trigonométrie --- */
double fn_sin(double x);
double fn_cos(double x);
double fn_tan(double x);
double fn_asin(double x);
double fn_acos(double x);
double fn_atan(double x);

/* --- Hyperboliques --- */
double fn_sinh(double x);
double fn_cosh(double x);
double fn_tanh(double x);
double fn_asinh(double x);
double fn_acosh(double x);
double fn_atanh(double x);

/* --- Exponentielles --- */
double fn_exp(double x);
double fn_pow10(double x);

/* --- Logarithmes --- */

double fn_log10(double x);
double fn_logn(double base, double x);

/* --- Racines --- */
double fn_sqrt(double x);
double fn_cbrt(double x);
double fn_nrt(int n, double x);

/* --- Puissances --- */
double fn_pow(double x, double y);
double fn_sq(double x);
double fn_cube(double x);
double fn_inv(double x);

/* --- Valeur absolue --- */
double fn_abs_val(double x);

/* --- Factorielle --- */
double fn_factorial(int n);

/* --- Combinatoire --- */
double fn_permut(int n, int r);
double fn_combin(int n, int r);

/* --- Arrondi --- */
double fn_round_val(double x);

/* --- Coordonnées polaires <-> rectangulaires --- */
void fn_pol_to_rec(double r, double theta, double *x, double *y);
void fn_rec_to_pol(double x, double y, double *r, double *theta);

/* --- Aléatoire --- */
double fn_rand_num(void);
double fn_rand_int(int a, int b);





#endif /* FUNCTIONS_H */
