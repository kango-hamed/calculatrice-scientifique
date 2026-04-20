#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "token.h"
#include "errors.h"

/* =========================================================
 * tokenizer.h -- Decoupage de l'expression en tokens
 * Exigences : F-CO-01, F-CO-03, F-CO-05
 * ========================================================= */

#define MAX_TOKENS 256

/**
 * @brief Decoupe une expression en tableau de tokens.
 *
 * Gere automatiquement la multiplication implicite :
 *   4sin(30) -> 4 * sin(30)
 *   4(x+1)   -> 4 * (x+1)
 *   4A        -> 4 * A
 *
 * @param expr    Expression a analyser (max 99 octets, F-CO-05).
 * @param out     Tableau de tokens (taille MAX_TOKENS).
 * @param count   Nombre de tokens produits.
 * @param err_pos Position du premier caractere invalide (-1 si OK).
 * @return ERR_NONE si succes, ERR_SYNTAX sinon.
 */
CalcError tokenize(const char *expr, Token *out, int *count, int *err_pos);

#endif /* TOKENIZER_H */
