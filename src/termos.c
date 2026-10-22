#include "prolog.h"

/* ================================================================
 * termos.c - construtores de Termo, Atomo e Clausula
 * ================================================================ */

Termo termo_const(const char *v) {
    Termo t;
    t.tipo = CONSTANTE;
    strncpy(t.valor, v, MAX_NOME - 1);
    t.valor[MAX_NOME - 1] = '\0';
    return t;
}

Termo termo_var(const char *v) {
    Termo t;
    t.tipo = VARIAVEL;
    strncpy(t.valor, v, MAX_NOME - 1);
    t.valor[MAX_NOME - 1] = '\0';
    return t;
}

Clausula *novo_fato(const char *pred, int n, ...) {
    Clausula *c;
    va_list ap;
    int i;
    c = calloc(1, sizeof(Clausula));
    if (!c) { fprintf(stderr, "Erro: sem memoria\n"); exit(1); }
    c->tipo = FATO;
    strncpy(c->cabeca.nome, pred, MAX_NOME - 1);
    c->cabeca.n_args = n;
    va_start(ap, n);
    for (i = 0; i < n; i++)
        c->cabeca.args[i] = va_arg(ap, Termo);
    va_end(ap);
    return c;
}

int extrair_vars(Atomo *a, char vars[][MAX_NOME], int max) {
    int n = 0;
    int i, j, duplicata;
    for (i = 0; i < a->n_args && n < max; i++) {
        if (a->args[i].tipo != VARIAVEL) continue;
        duplicata = 0;
        for (j = 0; j < n; j++)
            if (strcmp(vars[j], a->args[i].valor) == 0) { duplicata = 1; break; }
        if (!duplicata)
            strncpy(vars[n++], a->args[i].valor, MAX_NOME - 1);
    }
    return n;
}
