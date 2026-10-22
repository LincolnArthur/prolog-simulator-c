#include "prolog.h"

//UNIFICAÇÃO

// 1. AMBIENTE VAZIO
Ambiente amb_novo(void) {
    Ambiente a;
    a.n = 0;
    return a;
}

// 2. COPIAR AMBIENTE
//Devolve a struct inteira (cópia), assim cada tentativa tem a sua e o backtracking não estraga a anterior
Ambiente amb_copiar(Ambiente *a) {
    return *a;
}

// 3. VALOR DE UMA VARIÁVEL
//Retorna NULL se ela ainda estiver livre
const char *amb_resolver(Ambiente *amb, const char *nome) {
    int i;
    for (i = 0; i < amb->n; i++)
        if (strcmp(amb->ligacoes[i].var, nome) == 0)
            return amb->ligacoes[i].val;
    return NULL;
}

// 4. LIGAR VARIÁVEL A UM VALOR
int amb_ligar(Ambiente *amb, const char *var, const char *val) {
    if (amb->n >= MAX_VARS) return 0;
    strncpy(amb->ligacoes[amb->n].var, var, MAX_NOME - 1);
    strncpy(amb->ligacoes[amb->n].val, val, MAX_NOME - 1);
    amb->n++;
    return 1;
}

// 5. UNIFICAR
//Tenta casar a consulta com a cabeça da cláusula, argumento por argumento
//Retorna 1 se deu certo e 0 se não
int unificar(Atomo *consulta, Atomo *clausula, Ambiente *amb) {
    int i;
    const char *vc, *vf;
    Termo *tc, *tf;

    if (strcmp(consulta->nome, clausula->nome) != 0) return 0;
    if (consulta->n_args != clausula->n_args)         return 0;

    for (i = 0; i < consulta->n_args; i++) {
        tc = &consulta->args[i];
        tf = &clausula->args[i];

        vc = (tc->tipo == VARIAVEL) ? amb_resolver(amb, tc->valor) : tc->valor;
        vf = (tf->tipo == VARIAVEL) ? amb_resolver(amb, tf->valor) : tf->valor;

        if (tc->tipo == VARIAVEL && vc == NULL) { //variável da consulta livre: liga
            amb_ligar(amb, tc->valor, vf ? vf : tf->valor);
        } else if (tf->tipo == VARIAVEL && vf == NULL) { //variável da cláusula livre: liga
            amb_ligar(amb, tf->valor, vc ? vc : tc->valor);
        } else { //os dois já têm valor, tem que ser igual
            if (!vc || !vf || strcmp(vc, vf) != 0) return 0;
        }
    }
    return 1;
}

// 6. RENOMEAR VARIÁVEIS
//Coloca $profundidade no fim de cada variável (X vira X$2)
//Senão o X de uma chamada se mistura com o X de outra
void renomear_clausula(Clausula *c, int profundidade) {
    char suf[16];
    int s, i;
    sprintf(suf, "$%d", profundidade);

    for (i = 0; i < c->cabeca.n_args; i++)
        if (c->cabeca.args[i].tipo == VARIAVEL)
            strncat(c->cabeca.args[i].valor, suf,
                    MAX_NOME - strlen(c->cabeca.args[i].valor) - 1);

    for (s = 0; s < c->n_subs; s++)
        for (i = 0; i < c->subs[s].n_args; i++)
            if (c->subs[s].args[i].tipo == VARIAVEL)
                strncat(c->subs[s].args[i].valor, suf,
                        MAX_NOME - strlen(c->subs[s].args[i].valor) - 1);
}
