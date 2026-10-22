#include "prolog.h"

/* ================================================================
 * unificacao.c - ambiente de variaveis, unificacao e renomeacao
 * ================================================================ */

Ambiente amb_novo(void) {
    Ambiente a;
    a.n = 0;
    return a;
}

Ambiente amb_copiar(Ambiente *a) {
    return *a;
}

const char *amb_resolver(Ambiente *amb, const char *nome) {
    int i;
    for (i = 0; i < amb->n; i++)
        if (strcmp(amb->ligacoes[i].var, nome) == 0)
            return amb->ligacoes[i].val;
    return NULL;
}

int amb_ligar(Ambiente *amb, const char *var, const char *val) {
    if (amb->n >= MAX_VARS) return 0;
    strncpy(amb->ligacoes[amb->n].var, var, MAX_NOME - 1);
    strncpy(amb->ligacoes[amb->n].val, val, MAX_NOME - 1);
    amb->n++;
    return 1;
}

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

        if (tc->tipo == VARIAVEL && vc == NULL) {
            amb_ligar(amb, tc->valor, vf ? vf : tf->valor);
        } else if (tf->tipo == VARIAVEL && vf == NULL) {
            amb_ligar(amb, tf->valor, vc ? vc : tc->valor);
        } else {
            if (!vc || !vf || strcmp(vc, vf) != 0) return 0;
        }
    }
    return 1;
}

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
