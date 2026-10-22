#ifndef PROLOG_H
#define PROLOG_H

//Tudo que é usado em mais de um arquivo fica aqui (constantes, structs e protótipos)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

//LIMITES
#define MAX_NOME    64  //tamanho máximo de um nome
#define MAX_ARGS     4  //argumentos por predicado
#define MAX_SUBS     3  //subobjetivos por regra (o que vem depois do :-)
#define MAX_VARS    32  //variáveis ligadas ao mesmo tempo
#define MAX_LINHA   256 //tamanho máximo de uma linha do arquivo

//TERMO
//Menor parte do sistema, pode ser constante (zeus) ou variável (X)
typedef enum { CONSTANTE, VARIAVEL } TipoTermo;

typedef struct {
    TipoTermo tipo;
    char valor[MAX_NOME];
} Termo;

//ÁTOMO
//nome(arg0, arg1, ...), ex: pai(zeus, X)
typedef struct {
    char nome[MAX_NOME];
    int  n_args;
    Termo args[MAX_ARGS];
} Atomo;

//CLÁUSULA
//Fato:  pai(zeus, atena).
//Regra: avo(X, Y) :- pai(X, Z), pai(Z, Y).
typedef enum { FATO, REGRA } TipoClausula;

typedef struct Clausula {
    TipoClausula tipo;
    Atomo cabeca;
    int   n_subs;
    Atomo subs[MAX_SUBS];
    struct Clausula *prox; //próxima cláusula do mesmo predicado (lista encadeada)
} Clausula;

//NÓ DA BST
//A chave é o nome do predicado, e cada nó guarda a lista de cláusulas dele
typedef struct NoBST {
    char nome[MAX_NOME];
    Clausula *clausulas;
    struct NoBST *esq;
    struct NoBST *dir;
} NoBST;

//BANCO DE DADOS
//No fim é só a raiz da árvore
typedef struct {
    NoBST *raiz;
} BancoDados;

//AMBIENTE
//Lista de ligações tipo X = zeus, que vai sendo montada na unificação
typedef struct {
    char var[MAX_NOME];
    char val[MAX_NOME];
} Ligacao;

typedef struct {
    Ligacao ligacoes[MAX_VARS];
    int n;
} Ambiente;

// PROTÓTIPOS - bst.c
NoBST    *bst_novo_no(const char *nome);
NoBST    *bst_inserir(NoBST *raiz, const char *nome);
NoBST    *bst_buscar(NoBST *raiz, const char *nome);
void      bst_imprimir(NoBST *raiz);

// PROTÓTIPOS - banco.c
BancoDados *bd_criar(void);
void        bd_assert(BancoDados *bd, Clausula *c);
void        bd_imprimir(BancoDados *bd);
void        bd_limpar(BancoDados *bd);
int         bd_carregar_arquivo(BancoDados *bd, const char *caminho);

// PROTÓTIPOS - termos.c
Termo     termo_const(const char *v);
Termo     termo_var(const char *v);
Clausula *novo_fato(const char *pred, int n, ...);
int       extrair_vars(Atomo *a, char vars[][MAX_NOME], int max);

// PROTÓTIPOS - unificacao.c
Ambiente  amb_novo(void);
Ambiente  amb_copiar(Ambiente *a);
const char *amb_resolver(Ambiente *amb, const char *nome);
int       amb_ligar(Ambiente *amb, const char *var, const char *val);
int       unificar(Atomo *consulta, Atomo *clausula, Ambiente *amb);
void      renomear_clausula(Clausula *c, int profundidade);

// PROTÓTIPOS - motor.c
void resolver(BancoDados *bd, Atomo *objetivos, int n_obj,
              Ambiente amb, int prof,
              char vars_orig[][MAX_NOME], int n_vars_orig,
              int *encontrou);

// PROTÓTIPOS - parser.c
int  parser_atomo(const char *entrada, Atomo *resultado);
int  parser_clausula(const char *linha, Clausula **resultado);

#endif
