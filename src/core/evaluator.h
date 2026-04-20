#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "errors.h"

/* =========================================================
 * evaluator.h -- Evaluation de l'AST
 * Exigences : F-CO-07 a F-CO-10, F-FN-01 a F-FN-22
 * ========================================================= */

#define NB_VARS 26   /* variables A-Z */

/* Memoire de la calculatrice */
typedef struct {
    double vars[NB_VARS];  /* variables A-Z              */
    double mem_M;          /* memoire independante M     */
    double ans;            /* derniere reponse (Ans)     */
    int    angle_deg;      /* 1 = degres, 0 = radians    */
} CalcMemory;

/**
 * @brief Initialise la memoire (tout a zero, mode degres).
 */
void eval_memory_init(CalcMemory *mem);

/**
 * @brief Evalue un AST et retourne le resultat numerique.
 *
 * Met a jour mem->ans si l'evaluation reussit.
 *
 * @param node   Racine de l'AST.
 * @param mem    Memoire courante de la calculatrice.
 * @param result Pointeur vers le resultat.
 * @return ERR_NONE si succes, code d'erreur sinon.
 */
CalcError eval(const ASTNode *node, CalcMemory *mem, double *result);

/**
 * @brief Affiche le resultat de facon adaptee (entier ou decimal).
 */
void eval_print_result(double result);

#endif /* EVALUATOR_H */
