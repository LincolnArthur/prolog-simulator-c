# Prolog Simulator: mini-interpretador Prolog em C

Um pequeno **motor de inferência no estilo Prolog** escrito em C puro. Você declara **fatos** e **regras**, faz uma **consulta**, e o programa descobre sozinho as respostas usando **unificação** e **backtracking**.

Projeto 2 da disciplina **Algoritmos e Estruturas de Dados**, do curso de Engenharia de Computação da UNIVASF. [Slides da apresentação](slides-apresentacao.pdf)

## Demonstração

Usando o banco de exemplo `bancos/mitologia/mitologia_grega.txt`:

```prolog
% fatos
pai(cronos, zeus).
pai(zeus, atena).
pai(zeus, ares).
...
% regras
filho_de_zeus(X) :- pai(zeus, X).
mesmo_pai(X, Y)  :- pai(Z, X), pai(Z, Y).
```

```
=== Carregar Arquivo ===
  Nome do arquivo [banco.txt]: bancos/mitologia/mitologia_grega.txt
  39 clausula(s) carregada(s) de 'bancos/mitologia/mitologia_grega.txt'

=== Consulta ===
  Digite a consulta: mesmo_pai(atena, Y)

  Solucao 1:
    Y = atena
  Solucao 2:
    Y = ares
  Solucao 3:
    Y = apolo
  Solucao 4:
    Y = artemis
  Solucao 5:
    Y = hermes
  Total: 5 solucao(oes).
```

## Funcionalidades

- Fatos (`pai(zeus, atena).`) e regras com até 3 subobjetivos (`avo(X, Y) :- pai(X, Z), pai(Z, Y).`)
- Consultas com constantes e variáveis (variáveis começam com letra **maiúscula** ou `_`)
- Busca de **todas** as soluções, com backtracking
- Carregamento de bancos de conhecimento a partir de arquivos `.txt`, com suporte a comentários `%`
- Inclusão de novas cláusulas pelo menu, durante a execução
- Listagem do banco em ordem alfabética de predicado

## Estruturas de dados

BST gerada ao carregar `mitologia_grega.txt` (os nós entram na ordem em que os predicados aparecem no arquivo). Cada nó aponta para a lista encadeada das suas cláusulas:

```
[deus] ─► deus(zeus) ─► deus(hera) ─► …
    \
  [dominio] ─► dominio(zeus, ceu) ─► dominio(zeus, trovo) ─► …
        \
       [pai] ─► pai(cronos, zeus) ─► pai(cronos, hera) ─► …
       /
  [mesmo_pai] ─► mesmo_pai(X, Y) :- pai(Z, X), pai(Z, Y)
    /       \
[filho_de_cronos]   [olimpico] ─► olimpico(X) :- dominio(X, ceu) ─► …
        \
   [filho_de_zeus]
```

| Estrutura | Arquivo | Papel |
|---|---|---|
| **Árvore Binária de Busca** (`NoBST`) | `bst.c` | Indexa os predicados pelo nome, para achar `pai/2` sem percorrer o banco inteiro |
| **Lista encadeada** (`Clausula *prox`) | `banco.c` | Guarda as cláusulas de cada predicado **na ordem em que foram declaradas**, que é a ordem em que o motor deve tentá-las |
| **Ambiente de ligações** (vetor de `Ligacao`) | `unificacao.c` | Registra quais variáveis já estão ligadas a quais valores |
| **Pilha de recursão** | `motor.c` | Lista de objetivos pendentes; cada chamada recursiva é um ponto de escolha do backtracking |

## Como o motor funciona

1. **Parser** (`parser.c`): transforma texto como `irmao(X, Y) :- pai(Z, X), pai(Z, Y).` em structs `Atomo` e `Clausula`.
2. **Busca** (`motor.c`): para o primeiro objetivo da consulta, procura o predicado na BST e percorre a lista de cláusulas.
3. **Renomeação**: antes de usar uma cláusula, suas variáveis recebem um sufixo com a profundidade (`X` vira `X$2`), para que variáveis de chamadas diferentes não se confundam.
4. **Unificação** (`unificacao.c`): tenta casar o objetivo com a cabeça da cláusula, ligando variáveis a constantes.
5. Se a cláusula é uma **regra**, seus subobjetivos são colocados na frente da lista de objetivos e o motor chama a si mesmo recursivamente.
6. Quando não sobra nenhum objetivo, uma **solução** foi encontrada e é impressa. A recursão então volta e tenta a próxima cláusula: isso é o **backtracking**.

## Estrutura do repositório

```
prolog-simulator-c/
├── src/
│   ├── prolog.h       # constantes, structs e protótipos
│   ├── main.c         # menu interativo
│   ├── parser.c       # texto → Atomo / Clausula
│   ├── termos.c       # construtores de termos e extração de variáveis
│   ├── bst.c          # árvore binária de busca de predicados
│   ├── banco.c        # banco de dados e leitura de arquivos
│   ├── unificacao.c   # ambiente de variáveis, unificação e renomeação
│   └── motor.c        # motor de resolução com backtracking
├── bancos/            # bancos de conhecimento de exemplo
│   ├── bancario/      # app, crédito, investimentos
│   ├── hospital/      # cirurgias, recepção, UTI
│   └── mitologia/     # egípcia, grega, nórdica
├── slides-apresentacao.pdf
├── Makefile
└── README.md
```

## Como compilar e executar

É preciso ter o compilador **GCC** instalado.

**Linux / macOS**

```bash
make
./prolog_simulator
```

**Windows (MinGW)**

```bash
mingw32-make
prolog_simulator.exe
```

Sem `make`:

```bash
gcc src/*.c -o prolog_simulator
```

Ao carregar um banco (opção 4), informe o caminho **relativo à pasta de onde você executou o programa**, por exemplo `bancos/hospital/hospital_uti.txt`.

## Limitações (escopo do projeto)

Por ser uma prova de conceito acadêmica, o simulador trabalha apenas com termos atômicos (constantes e variáveis). Algumas limitações:

- até 4 argumentos por predicado e até 3 subobjetivos por regra;
- profundidade máxima de recursão igual a 30;
- não há suporte a listas, aritmética, operador de corte (`!`) ou negação.

## Autores

- **Arthur Lincoln P. H. Almeida**
- **Caio Benevides Pontes**

Engenharia de Computação, Universidade Federal do Vale do São Francisco (UNIVASF).
