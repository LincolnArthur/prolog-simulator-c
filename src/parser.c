#include "prolog.h"

//PARSER
//Transforma o texto em Atomo e Clausula
//Fato:  pai(zeus, atena).
//Regra: avo(X, Y) :- pai(X, Z), pai(Z, Y).
//Variável começa com MAIÚSCULA ou _, constante começa com minúscula ou número

//pula os espaços e devolve onde parou
static const char *pula_espacos(const char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

//lê um nome (letra, número ou _)
static const char *le_ident(const char *s, char *buf, int max) {
    int i = 0;
    while (*s && (isalnum((unsigned char)*s) || *s == '_') && i < max - 1)
        buf[i++] = *s++;
    buf[i] = '\0';
    return s;
}

// 1. LER UM ÁTOMO
int parser_atomo(const char *entrada, Atomo *resultado) {
    const char *p;
    char token[MAX_NOME];
    Termo t;

    memset(resultado, 0, sizeof(Atomo));
    p = pula_espacos(entrada);
    p = le_ident(p, resultado->nome, MAX_NOME);
    if (resultado->nome[0] == '\0') return 0;

    p = pula_espacos(p);
    if (*p != '(') return 0;
    p++;

    while (*p && *p != ')') {
        p = pula_espacos(p);
        if (*p == ')') break;

        memset(token, 0, sizeof(token));
        p = le_ident(p, token, MAX_NOME);
        if (token[0] == '\0') return 0;

        //MAIÚSCULA ou _ = variável
        if (isupper((unsigned char)token[0]) || token[0] == '_')
            t = termo_var(token);
        else
            t = termo_const(token);

        if (resultado->n_args >= MAX_ARGS) return 0;
        resultado->args[resultado->n_args++] = t;

        p = pula_espacos(p);
        if (*p == ',') p++;
    }
    if (*p == ')') p++;
    return 1;
}

// 2. PRÓXIMO ÁTOMO DO CORPO DA REGRA
//Corta na vírgula, mas só a que tá fora dos parênteses
static int parser_proximo_atomo(const char **pos, Atomo *resultado) {
    const char *p;
    const char *inicio;
    char buf[MAX_LINHA];
    int prof, tam;

    p = pula_espacos(*pos);
    if (*p == '\0') return 0;

    inicio = p;
    prof   = 0;
    while (*p) {
        if      (*p == '(') prof++;
        else if (*p == ')') prof--;
        else if (*p == ',' && prof == 0) break;
        p++;
    }

    tam = (int)(p - inicio);
    if (tam <= 0 || tam >= MAX_LINHA) return 0;
    strncpy(buf, inicio, tam);
    buf[tam] = '\0';

    if (!parser_atomo(buf, resultado)) return 0;
    *pos = (*p == ',') ? p + 1 : p;
    return 1;
}

// 3. LER UMA CLÁUSULA (fato ou regra)
int parser_clausula(const char *linha, Clausula **resultado) {
    char buf[MAX_LINHA];
    char *coment;
    char *neck;
    int len, prof, i;
    Clausula *c;
    const char *corpo;
    const char *p;

    strncpy(buf, linha, MAX_LINHA - 1);
    buf[MAX_LINHA - 1] = '\0';

    coment = strchr(buf, '%'); //corta o comentário
    if (coment) *coment = '\0';

    len = (int)strlen(buf);
    while (len > 0 && (buf[len-1] == '.' || isspace((unsigned char)buf[len-1])))
        buf[--len] = '\0';
    if (len == 0) return 0;

    c = calloc(1, sizeof(Clausula));
    if (!c) return 0;

    //Procura o :- fora dos parênteses
    prof = 0;
    neck = NULL;
    for (i = 0; buf[i]; i++) {
        if      (buf[i] == '(') prof++;
        else if (buf[i] == ')') prof--;
        else if (prof == 0 && buf[i] == ':' && buf[i+1] == '-') {
            neck = &buf[i];
            break;
        }
    }

    if (neck) {
        *neck = '\0';
        corpo = neck + 2;
        if (!parser_atomo(buf, &c->cabeca)) { free(c); return 0; }
        c->tipo = REGRA;
        p = corpo;
        while (c->n_subs < MAX_SUBS) {
            Atomo sub;
            if (!parser_proximo_atomo(&p, &sub)) break;
            c->subs[c->n_subs++] = sub;
        }
        if (c->n_subs == 0) { free(c); return 0; }
    } else {
        if (!parser_atomo(buf, &c->cabeca)) { free(c); return 0; }
        c->tipo = FATO;
    }

    *resultado = c;
    return 1;
}
