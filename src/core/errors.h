#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>

/* =========================================================
 * errors.h -- Gestion des erreurs (F-CO-07 a F-CO-11)
 * ========================================================= */

typedef enum {
    ERR_NONE = 0,
    ERR_SYNTAX,       /* expression mal formee          */
    ERR_MATH,         /* erreur mathematique generale   */
    ERR_STACK,        /* pile trop profonde             */
    ERR_ARGUMENT,     /* mauvais argument               */
    ERR_MEMORY,       /* allocation memoire echouee     */
    ERR_DIV_ZERO,     /* division par zero              */
    ERR_DOMAIN,       /* argument hors domaine          */
    ERR_OVERFLOW      /* depassement de plage           */
} CalcError;

static inline const char *error_message(CalcError err) {
    switch (err) {
        case ERR_NONE:      return "OK";
        case ERR_SYNTAX:    return "Syntax ERROR";
        case ERR_MATH:      return "Math ERROR";
        case ERR_STACK:     return "Stack ERROR";
        case ERR_ARGUMENT:  return "Argument ERROR";
        case ERR_MEMORY:    return "Memory ERROR";
        case ERR_DIV_ZERO:  return "Math ERROR: division par zero";
        case ERR_DOMAIN:    return "Math ERROR: domaine invalide";
        case ERR_OVERFLOW:  return "Math ERROR: depassement de plage";
        default:            return "Erreur inconnue";
    }
}

static inline void error_print(CalcError err, const char *expr, int position) {
    fprintf(stderr, "\n");
    if (expr) fprintf(stderr, "  %s\n", expr);
    if (position >= 0 && expr) {
        int i;
        fprintf(stderr, "  ");
        for (i = 0; i < position; i++) fprintf(stderr, " ");
        fprintf(stderr, "^\n");
    }
    fprintf(stderr, "  [%s]\n\n", error_message(err));
}

#endif /* ERRORS_H */
