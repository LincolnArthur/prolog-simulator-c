#include "prolog.h"

//BANCO DE DADOS
//A BST guarda os predicados e cada nó tem uma lista encadeada com as cláusulas

// 1. CRIAR BANCO VAZIO
BancoDados *bd_criar(void) {
    BancoDados *bd = calloc(1, sizeof(BancoDados));
    if (!bd) { fprintf(stderr, "Erro: sem memoria\n"); exit(1); }
    return bd;
}

// 2. LIBERAR A LISTA DE CLÁUSULAS
static void liberar_clausulas(Clausula *c) {
    Clausula *prox;
    while (c) {
        prox = c->prox;
        free(c);
        c = prox;
    }
}

// 3. LIBERAR A ÁRVORE
//Pós-ordem: libera os filhos antes do pai, senão perde o ponteiro
static void liberar_bst(NoBST *no) {
    if (!no) return;
    liberar_bst(no->esq);
    liberar_bst(no->dir);
    liberar_clausulas(no->clausulas);
    free(no);
}

// 4. LIMPAR O BANCO
void bd_limpar(BancoDados *bd) {
    liberar_bst(bd->raiz);
    bd->raiz = NULL;
}

// 5. ADICIONAR CLÁUSULA (assert)
//Acha o nó do predicado (ou cria) e coloca a cláusula no FIM da lista, pra manter a ordem do arquivo
void bd_assert(BancoDados *bd, Clausula *c) {
    NoBST *no;
    Clausula *cur;
    bd->raiz = bst_inserir(bd->raiz, c->cabeca.nome);
    no = bst_buscar(bd->raiz, c->cabeca.nome);
    if (!no->clausulas) {
        no->clausulas = c;
    } else {
        cur = no->clausulas;
        while (cur->prox) cur = cur->prox;
        cur->prox = c;
    }
    c->prox = NULL;
}

// 6. IMPRIMIR BANCO
void bd_imprimir(BancoDados *bd) {
    if (!bd->raiz) {
        printf("  (banco vazio)\n");
        return;
    }
    bst_imprimir(bd->raiz);
}

// 7. CARREGAR ARQUIVO .txt
//Lê linha por linha, pula linha vazia e comentário (%)
int bd_carregar_arquivo(BancoDados *bd, const char *caminho) {
    FILE *f;
    char linha[MAX_LINHA];
    int  n_lidas, n_linha;
    Clausula *c;
    const char *p;

    f = fopen(caminho, "r");
    if (!f) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", caminho);
        return 0;
    }

    //Apaga o que tinha antes de carregar o novo
    bd_limpar(bd);

    n_lidas = 0;
    n_linha = 0;

    while (fgets(linha, sizeof(linha), f)) {
        n_linha++;
        linha[strcspn(linha, "\n")] = '\0';

        p = linha;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '%') continue;

        c = NULL;
        if (parser_clausula(linha, &c)) {
            bd_assert(bd, c);
            n_lidas++;
        } else {
            fprintf(stderr, "  Aviso: linha %d ignorada: '%s'\n", n_linha, linha);
        }
    }

    fclose(f);
    printf("  %d clausula(s) carregada(s) de '%s'\n", n_lidas, caminho);
    return n_lidas;
}
