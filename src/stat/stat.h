#ifndef CALC_STAT_H
#define CALC_STAT_H

/**
 * Module : stat
 * Statistiques — mode STAT, 1-var, 2-var, régressions
 *
 * Groupe responsable : (à compléter)
 * NE PAS MODIFIER ce fichier sans accord du groupe responsable
 * et validation du chef de projet. Toute demande de modification
 * passe par une GitHub Issue avec le label "interface".
 */

enum RegressionType {
    REGRESSION_LINEAR = 1,
    REGRESSION_QUAD,
    REGRESSION_LOG,
    REGRESSION_EXP_E,
    REGRESSION_EXP_AB,
    REGRESSION_POWER,
    REGRESSION_INVERSE
};

void   stat_push(double x);
void   stat_push2(double x, double y);
void   stat_load(const double *x, const double *freq, int n);
void   stat_input(const double *x, const double *freq, int n);
double stat_sum(void);
double stat_mean(void);
double stat_stddev_pop(void);
double stat_stddev_samp(void);
double stat_min(void);
double stat_max(void);
double normalize(double x);
double normal_P(double t);
double normal_Q(double t);
double normal_R(double t);

int regression(int type,
               const double *x,
               const double *y,
               const double *freq,
               int n,
               double *coef,
               int count);

int regression_linear(const double *x,
                      const double *y,
                      const double *freq,
                      int n,
                      double *A,
                      double *B);
int regression_quad(const double *x,
                    const double *y,
                    const double *freq,
                    int n,
                    double *A,
                    double *B,
                    double *C);
int regression_log(const double *x,
                   const double *y,
                   const double *freq,
                   int n,
                   double *A,
                   double *B);
int regression_exp_e(const double *x,
                     const double *y,
                     const double *freq,
                     int n,
                     double *A,
                     double *B);
int regression_exp_ab(const double *x,
                      const double *y,
                      const double *freq,
                      int n,
                      double *A,
                      double *B);
int regression_power(const double *x,
                     const double *y,
                     const double *freq,
                     int n,
                     double *A,
                     double *B);
int regression_inverse(const double *x,
                       const double *y,
                       const double *freq,
                       int n,
                       double *A,
                       double *B);

void stat_clear(void);

#endif /* CALC_STAT_H */
