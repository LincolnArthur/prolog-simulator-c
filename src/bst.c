#include "prolog.h"

/* ================================================================
 * bst.c - operacoes na Binary Search Tree de predicados
 * ================================================================ */

NoBST *bst_novo_no(const char *nome) {
    NoBST *no = calloc(1, sizeof(NoBST));
    if (!no) { fprintf(stderr, "Erro: sem memoria\n"); exit(1); }
    strncpy(no->nome, nome, MAX_NOME - 1);
    return no;
}

NoBST *bst_inserir(NoBST *raiz, const char *nome) {
    int cmp;
    if (!raiz) return bst_novo_no(nome);
    cmp = strcmp(nome, raiz->nome);
    if      (cmp < 0) raiz->esq = bst_inserir(raiz->esq, nome);
    else if (cmp > 0) raiz->dir = bst_inserir(raiz->dir, nome);
    return raiz;
}

NoBST *bst_buscar(NoBST *raiz, const char *nome) {
    int cmp;
    if (!raiz) return NULL;
    cmp = strcmp(nome, raiz->nome);
    if      (cmp < 0) return bst_buscar(raiz->esq, nome);
    else if (cmp > 0) return bst_buscar(raiz->dir, nome);
    return raiz;
}

void bst_imprimir(NoBST *raiz) {
    Clausula *c;
    int i, j;
    if (!raiz) return;
    bst_imprimir(raiz->esq);

    printf("  [%s]\n", raiz->nome);
    for (c = raiz->clausulas; c; c = c->prox) {
        printf("    %s(", c->cabeca.nome);
        for (i = 0; i < c->cabeca.n_args; i++)
            printf("%s%s", c->cabeca.args[i].valor,
                   i < c->cabeca.n_args - 1 ? ", " : "");
        printf(")");
        if (c->tipo == REGRA) {
            printf(" :- ");
            for (i = 0; i < c->n_subs; i++) {
                printf("%s(", c->subs[i].nome);
                for (j = 0; j < c->subs[i].n_args; j++)
                    printf("%s%s", c->subs[i].args[j].valor,
                           j < c->subs[i].n_args - 1 ? ", " : "");
                printf(")%s", i < c->n_subs - 1 ? ", " : "");
            }
        }
        printf("\n");
    }
    bst_imprimir(raiz->dir);
}
