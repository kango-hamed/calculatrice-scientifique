/**
 * main.c — Point d'entrée de la calculatrice scientifique
 *
 * Lance l'interface CLI. Remplacé progressivement au fur et à mesure
 * que les modules sont implémentés.
 */

#include "src/interface/interface.h"
#include "src/core/core.h"
#include <stdio.h>
#include <string.h>

#define EXPR_MAX 256

int main(void) {
    char expr[EXPR_MAX];

    cli_start();

    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(expr, sizeof(expr), stdin))
            break;

        /* Supprimer le \n final */
        expr[strcspn(expr, "\n")] = '\0';

        if (strcmp(expr, "quit") == 0 || strcmp(expr, "exit") == 0)
            break;

        if (strlen(expr) == 0)
            continue;

        if (!validate_syntax(expr)) {
            cli_display_error("Syntax ERROR");
            continue;
        }

        double result = evaluate(expr);
        cli_display_result(result);
    }

    printf("Au revoir.\n");
    return 0;
}
