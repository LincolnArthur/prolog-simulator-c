#include "prolog.h"

/* ================================================================
 * main.c — menu interativo do banco de dados estilo Prolog
 * ================================================================ */

#define ARQUIVO_PADRAO "banco.txt"

static void ler_linha(const char *prompt, char *buf, int max) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(buf, max, stdin)) buf[0] = '\0';
    buf[strcspn(buf, "\n")] = '\0';
}

static void executar_query(BancoDados *bd, const char *entrada) {
    Atomo q;
    char vars[MAX_VARS][MAX_NOME];
    int  n_vars, encontrou;

    if (!parser_atomo(entrada, &q)) {
        printf("  Erro: consulta invalida. Use: predicado(arg1, X)\n");
        return;
    }

    n_vars    = extrair_vars(&q, vars, MAX_VARS);
    encontrou = 0;
    resolver(bd, &q, 1, amb_novo(), 0, vars, n_vars, &encontrou);

    if (encontrou == 0)
        printf("  Nenhuma solucao encontrada.\n");
    else
        printf("  Total: %d solucao(oes).\n", encontrou);
}

static void menu_adicionar(BancoDados *bd) {
    char buf[MAX_LINHA];
    Clausula *c;

    printf("\n");
    printf("  Exemplos:\n");
    printf("    Fato:  pai(carlos, joao)\n");
    printf("    Regra: irmao(X, Y) :- pai(Z, X), pai(Z, Y)\n\n");

    ler_linha("  Digite a clausula: ", buf, MAX_LINHA);

    if (buf[0] == '\0') {
        printf("  Operacao cancelada.\n");
        return;
    }

    c = NULL;
    if (parser_clausula(buf, &c)) {
        bd_assert(bd, c);
        printf("  Clausula adicionada: %s/", c->cabeca.nome);
        printf("%d (%s)\n", c->cabeca.n_args,
               c->tipo == FATO ? "fato" : "regra");
    } else {
        printf("  Erro: formato invalido.\n");
    }
}

static void menu_query(BancoDados *bd) {
    char buf[MAX_LINHA];

    printf("\n");
    printf("  Exemplos:\n");
    printf("    pai(joao, X)      - quem joao e pai?\n");
    printf("    avo(X, ana)       - quem e avo de ana?\n");
    printf("    mortal(X)         - quem e mortal?\n\n");

    ler_linha("  Digite a consulta: ", buf, MAX_LINHA);

    if (buf[0] == '\0') {
        printf("  Operacao cancelada.\n");
        return;
    }

    printf("\n");
    executar_query(bd, buf);
}

static void menu_carregar(BancoDados *bd) {
    char buf[MAX_LINHA];

    ler_linha("  Nome do arquivo [banco.txt]: ", buf, MAX_LINHA);
    if (buf[0] == '\0')
        strncpy(buf, ARQUIVO_PADRAO, MAX_LINHA - 1);

    printf("\n");
    bd_carregar_arquivo(bd, buf);
}

static void exibir_menu(void) {
    printf("\n");
    printf("  +------------------------------------------+\n");
    printf("  |   Banco de Dados Estilo Prolog  (BST)    |\n");
    printf("  +------------------------------------------+\n");
    printf("  |  1. Listar banco de dados                |\n");
    printf("  |  2. Fazer consulta (query)               |\n");
    printf("  |  3. Adicionar clausula                   |\n");
    printf("  |  4. Carregar arquivo .txt                |\n");
    printf("  |  0. Sair                                 |\n");
    printf("  +------------------------------------------+\n");
    printf("  Opcao: ");
    fflush(stdout);
}

int main(void) {
    BancoDados *bd;
    char opcao[8];
    int  rodando;

    bd = bd_criar();

    rodando = 1;
    while (rodando) {
        exibir_menu();
        if (!fgets(opcao, sizeof(opcao), stdin)) break;

        switch (opcao[0]) {
            case '1':
                printf("\n=== Banco de Dados ===\n");
                bd_imprimir(bd);
                break;
            case '2':
                printf("\n=== Consulta ===\n");
                menu_query(bd);
                break;
            case '3':
                printf("\n=== Adicionar Clausula ===\n");
                menu_adicionar(bd);
                break;
            case '4':
                printf("\n=== Carregar Arquivo ===\n");
                menu_carregar(bd);
                break;
            case '0':
                printf("\n  Encerrando. Ate mais!\n\n");
                rodando = 0;
                break;
            default:
                printf("\n  Opcao invalida.\n");
                break;
        }
    }

    return 0;
}
