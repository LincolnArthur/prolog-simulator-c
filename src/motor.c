#include "prolog.h"

/* ================================================================
 * motor.c - motor de resolucao com backtracking
 * ================================================================ */

static void imprimir_solucao(Ambiente *amb,
                              char vars_orig[][MAX_NOME], int n_vars) {
    int i, passos;
    const char *v, *res, *prox;
    for (i = 0; i < n_vars; i++) {
        v   = vars_orig[i];
        res = amb_resolver(amb, v);
        passos = 0;
        while (res && passos < MAX_VARS) {
            prox = amb_resolver(amb, res);
            if (!prox) break;
            res = prox;
            passos++;
        }
        printf("    %s = %s\n", v, res ? res : "(livre)");
    }
}

void resolver(BancoDados *bd, Atomo *objetivos, int n_obj,
              Ambiente amb, int prof,
              char vars_orig[][MAX_NOME], int n_vars_orig,
              int *encontrou) {
    Atomo *atual;
    NoBST *no;
    Clausula *c;
    int i, total;
    Ambiente novo_amb;
    Clausula copia;
    Atomo fila[MAX_SUBS + 8];

    if (prof > 30) return;

    if (n_obj == 0) {
        (*encontrou)++;
        printf("  Solucao %d:\n", *encontrou);
        imprimir_solucao(&amb, vars_orig, n_vars_orig);
        return;
    }

    atual = &objetivos[0];
    no    = bst_buscar(bd->raiz, atual->nome);

    if (!no) {
        printf("  (predicado '%s' desconhecido)\n", atual->nome);
        return;
    }

    for (c = no->clausulas; c; c = c->prox) {
        novo_amb = amb_copiar(&amb);
        copia    = *c;
        renomear_clausula(&copia, prof);

        if (!unificar(atual, &copia.cabeca, &novo_amb)) continue;

        if (copia.tipo == FATO) {
            resolver(bd, objetivos + 1, n_obj - 1,
                     novo_amb, prof + 1, vars_orig, n_vars_orig, encontrou);
        } else {
            total = copia.n_subs + (n_obj - 1);
            for (i = 0; i < copia.n_subs; i++)
                fila[i] = copia.subs[i];
            for (i = 1; i < n_obj; i++)
                fila[copia.n_subs + i - 1] = objetivos[i];
            resolver(bd, fila, total,
                     novo_amb, prof + 1, vars_orig, n_vars_orig, encontrou);
        }
    }
}
