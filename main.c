/*
 * ATIVIDADE AVALIATIVA 04 - Cadastro de Loja
 * Une: ordenacao (Bubble Sort) + arquivos + struct + ponteiros + switch
 *
 * Arquivo de dados: produtos.txt, escrito como a propria tabela de texto:
 *   | ID | Produto | Categoria | Preço (R$) | Quantidade | Fornecedor |
 * Os produtos NAO ficam no codigo: o programa puxa a lista desse arquivo separado
 * (que deve estar na mesma pasta) ao iniciar (tambem aceita linhas no formato
 * ID;Produto;Categoria;Preco;Quantidade;Fornecedor) e regrava no mesmo formato.
 */

#include <stdio.h>   // printf, fopen, fgets, fprintf, remove, rename...
#include <stdlib.h>  // strtod
#include <string.h>  // strlen, strcmp, strcpy, strstr, strcspn...
#include <ctype.h>   // isspace, isdigit

#define ARQ "produtos.txt"   // arquivo principal com os dados
#define TMP "temp.txt"       // arquivo temporario (alterar/excluir)
#define CSV "produtos.csv"   // arquivo de exportacao CSV
#define MAX 500              // maximo de produtos no vetor

// Estrutura que representa um produto da loja
typedef struct {
    int id;               // identificador unico (gerado automaticamente)
    char produto[50];     // nome do produto (aumente se precisar de textos maiores)
    char categoria[30];   // categoria do produto
    float preco;          // preco em reais
    int quantidade;       // quantidade em estoque
    char fornecedor[40];  // nome do fornecedor
} Produto;

/* ===================== FUNCOES AUXILIARES DE ENTRADA ===================== */

// Le um texto do teclado com fgets (aceita espacos) e guarda em dest
void lerTexto(const char *msg, char *dest, int tam) {
    char buf[256];                                  // buffer temporario maior que o destino
    printf("%s", msg);                              // mostra a mensagem ao usuario
    if (fgets(buf, sizeof(buf), stdin) == NULL) {   // le a linha digitada
        dest[0] = '\0';                             // se falhar (EOF), devolve texto vazio
        return;                                     // sai da funcao
    }
    buf[strcspn(buf, "\r\n")] = '\0';               // remove o '\n' do final
    for (char *p = buf; *p; p++) {                  // percorre o texto com ponteiro
        if (*p == ';' || *p == '|') *p = ',';       // ';' e '|' sao separadores do arquivo, entao troca
    }
    strncpy(dest, buf, tam - 1);                    // copia respeitando o tamanho do campo
    dest[tam - 1] = '\0';                           // garante o terminador de string
}

// Le um inteiro validando a entrada
int lerInt(const char *msg) {
    char buf[64];                                   // buffer da linha
    int x;                                          // valor lido
    while (1) {                                     // repete ate o usuario digitar certo
        printf("%s", msg);                          // mostra a mensagem
        if (fgets(buf, sizeof(buf), stdin) == NULL) // se acabar a entrada (EOF)...
            return 0;                               // ...devolve 0 (sai do menu)
        if (sscanf(buf, "%d", &x) == 1)             // tenta converter para inteiro
            return x;                               // deu certo: devolve
        printf("Valor invalido.\n");                // senao, avisa e repete
    }
}

// Le um preco (aceita virgula ou ponto como separador decimal)
float lerFloat(const char *msg) {
    char buf[64];                                   // buffer da linha
    char *fim;                                      // aponta onde a conversao parou
    double v;                                       // valor convertido
    while (1) {
        printf("%s", msg);
        if (fgets(buf, sizeof(buf), stdin) == NULL)
            return 0;
        for (char *p = buf; *p; p++)                // troca virgula por ponto
            if (*p == ',') *p = '.';
        v = strtod(buf, &fim);                      // converte texto em numero
        if (fim != buf && v >= 0)                   // converteu algo e nao e negativo
            return (float)v;
        printf("Valor invalido.\n");
    }
}

// Retorna 1 se 'texto' contem 'busca' (os dados sao todos em MAIUSCULAS)
int contemTexto(const char *texto, const char *busca) {
    return strstr(texto, busca) != NULL;            // strstr procura substring
}

// Retorna 1 se os dois textos sao iguais
int iguais(const char *a, const char *b) {
    return strcmp(a, b) == 0;                       // strcmp devolve 0 se iguais
}

/* ======================== FUNCOES DE ARQUIVO ============================= */

// Remove espacos do inicio e do fim de um texto (modifica o proprio texto)
void trim(char *s) {
    int ini = 0;                                    // primeira posicao nao vazia
    int fim = (int)strlen(s);                       // fim do texto
    while (s[ini] != '\0' && isspace((unsigned char)s[ini])) ini++;   // pula espacos iniciais
    while (fim > ini && isspace((unsigned char)s[fim - 1])) fim--;    // ignora espacos finais
    memmove(s, s + ini, fim - ini);                 // desloca o texto util para o comeco
    s[fim - ini] = '\0';                            // fecha a string
}

// Copia 'orig' para 'dest' respeitando o tamanho 'tam' do campo
void copiar(char *dest, const char *orig, int tam) {
    strncpy(dest, orig, tam - 1);                   // copia no maximo tam-1 caracteres
    dest[tam - 1] = '\0';                           // garante o terminador
}

// Converte UMA linha do arquivo em um Produto. Retorna 1 se for uma linha valida.
// Aceita o formato de tabela "| 1 | Nome | Cat | 49.90 | 30 | Forn |"
// e o formato "1;Nome;Cat;49.90;30;Forn". Linhas de separador/cabecalho sao ignoradas.
int lerLinha(const char *linha, Produto *p) {
    if (strchr(linha, '|') != NULL) {               // formato de tabela
        char copia[300];                            // strtok altera o texto, entao usa copia
        char campos[6][60];                         // os 6 campos da linha
        int qtd = 0;                                // quantos campos foram lidos
        copiar(copia, linha, sizeof(copia));
        char *tok = strtok(copia, "|\r\n");         // separa o texto pelos '|'
        while (tok != NULL && qtd < 6) {
            copiar(campos[qtd], tok, sizeof(campos[qtd]));
            trim(campos[qtd]);                      // tira espacos ao redor
            qtd++;
            tok = strtok(NULL, "|\r\n");            // proximo campo
        }
        if (qtd < 6 || !isdigit((unsigned char)campos[0][0]))   // cabecalho ("ID") ou linha incompleta
            return 0;
        for (char *c = campos[3]; *c; c++)          // aceita virgula no preco
            if (*c == ',') *c = '.';
        p->id = atoi(campos[0]);                    // ID
        copiar(p->produto, campos[1], sizeof(p->produto));
        copiar(p->categoria, campos[2], sizeof(p->categoria));
        p->preco = (float)atof(campos[3]);          // preco
        p->quantidade = atoi(campos[4]);            // quantidade
        copiar(p->fornecedor, campos[5], sizeof(p->fornecedor));
        return 1;
    }
    // formato com ';'  (%49[^;] le ate o proximo ';', no maximo 49 caracteres)
    return sscanf(linha, "%d;%49[^;];%29[^;];%f;%d;%39[^\r\n]",
                  &p->id, p->produto, p->categoria,
                  &p->preco, &p->quantidade, p->fornecedor) == 6;
}

// Carrega o arquivo produtos.txt para o vetor 'lista' e devolve a quantidade
int carregar(Produto *lista) {
    FILE *f = fopen(ARQ, "r");                      // abre para leitura
    char linha[300];                                // guarda cada linha lida
    int n = 0;                                      // contador de produtos
    if (f == NULL)                                  // arquivo nao existe
        return 0;
    while (n < MAX && fgets(linha, sizeof(linha), f) != NULL) {
        if (lerLinha(linha, &lista[n]))             // converte a linha em produto
            n++;                                    // so conta se a linha for valida
    }
    fclose(f);                                      // fecha o arquivo
    return n;
}

// Escreve um texto em 'f' ocupando 'larg' CARACTERES (nao bytes), para que
// letras acentuadas (2 bytes em UTF-8) nao desalinhem a tabela
void campo(FILE *f, const char *s, int larg) {
    int chars = 0;                                  // quantidade de caracteres visiveis
    for (const char *p = s; *p; p++)                // percorre com ponteiro
        if (((unsigned char)*p & 0xC0) != 0x80)     // ignora bytes de continuacao UTF-8
            chars++;
    fputs(s, f);                                    // escreve o texto
    for (int i = chars; i < larg; i++)              // completa com espacos
        fputc(' ', f);
}

// Escreve uma linha de tracos
void linhaSeparadora(FILE *f) {
    for (int i = 0; i < 93; i++)
        fputc('-', f);
    fputc('\n', f);
}

// Escreve a tabela completa (cabecalho + produtos) em 'f'.
// Serve para a tela (stdout) e para o arquivo produtos.txt.
void escreverTabela(FILE *f, const Produto *lista, int n) {
    linhaSeparadora(f);
    fputs("| ", f);  campo(f, "ID", 3);
    fputs(" | ", f); campo(f, "Produto", 20);
    fputs(" | ", f); campo(f, "Categoria", 16);
    fputs(" | ", f); campo(f, "Preço (R$)", 10);
    fputs(" | ", f); campo(f, "Quantidade", 10);
    fputs(" | ", f); campo(f, "Fornecedor", 15);
    fputs(" |\n", f);
    linhaSeparadora(f);
    for (int i = 0; i < n; i++) {
        const Produto *p = &lista[i];               // ponteiro para o produto atual
        fprintf(f, "| %-3d | ", p->id);
        campo(f, p->produto, 20);
        fputs(" | ", f);
        campo(f, p->categoria, 16);
        fprintf(f, " | %10.2f | %10d | ", p->preco, p->quantidade);
        campo(f, p->fornecedor, 15);
        fputs(" |\n", f);
    }
    linhaSeparadora(f);
}

// Salva TODA a lista usando arquivo temporario e depois substitui o original
// (tecnica que evita corromper os dados se algo falhar no meio da gravacao)
int salvarComTemp(const Produto *lista, int n) {
    FILE *f = fopen(TMP, "w");                      // cria o arquivo temporario
    if (f == NULL)
        return 0;
    escreverTabela(f, lista, n);                    // grava a tabela no temporario
    fclose(f);                                      // fecha antes de renomear
    remove(ARQ);                                    // apaga o original (necessario no Windows)
    return rename(TMP, ARQ) == 0;                   // temporario passa a ser o arquivo oficial
}

/* ======================== FUNCOES DE APOIO =============================== */

// Gera o proximo ID: maior ID existente + 1 (incremental, sem digitar)
int proximoId(const Produto *lista, int n) {
    int maior = 0;
    for (int i = 0; i < n; i++)
        if (lista[i].id > maior)
            maior = lista[i].id;
    return maior + 1;
}

// Retorna o indice do produto com esse nome, ou -1 se nao existir
// 'ignorarId' permite ignorar o proprio produto ao alterar (use 0 na insercao)
int indicePorNome(const Produto *lista, int n, const char *nome, int ignorarId) {
    for (int i = 0; i < n; i++)
        if (lista[i].id != ignorarId && iguais(lista[i].produto, nome))
            return i;
    return -1;
}

// Retorna o ponteiro do produto com esse ID, ou NULL se nao achar
Produto *buscarPorId(Produto *lista, int n, int id) {
    for (int i = 0; i < n; i++)
        if (lista[i].id == id)
            return &lista[i];                       // endereco do elemento encontrado
    return NULL;
}

// Mostra uma lista de produtos em formato de tabela na tela
void imprimirTabela(const Produto *lista, int n) {
    printf("\n");
    escreverTabela(stdout, lista, n);               // mesma tabela, so que na tela
    printf("Total: %d produto(s)\n", n);
}

/* ======================== FUNCOES DO MENU ================================ */

// Chamada no inicio do programa: so AVISA se o produtos.txt nao for encontrado.
// Os produtos ficam no arquivo separado; o programa apenas le (puxa) esses dados.
void verificarArquivo(void) {
    FILE *f = fopen(ARQ, "r");                      // tenta abrir para leitura
    if (f == NULL) {                                // nao achou o arquivo
        printf("Arquivo '%s' nao encontrado na pasta do programa.\n", ARQ);
        printf("Coloque-o ali ou use a opcao 1 para criar um arquivo vazio.\n");
        return;
    }
    fclose(f);                                      // existe: nada a fazer
}

// 1) CRIAR: cria um produtos.txt vazio (so com o cabecalho da tabela)
void criarArquivo(void) {
    char resp[8];
    Produto vazio[1];                               // nenhum produto sera gravado
    FILE *teste = fopen(ARQ, "r");                  // testa se ja existe
    if (teste != NULL) {                            // se existe, pede confirmacao
        fclose(teste);
        lerTexto("O arquivo ja existe e sera APAGADO. Continuar? (s/n): ", resp, sizeof(resp));
        if (resp[0] != 's' && resp[0] != 'S') {
            printf("Operacao cancelada.\n");
            return;
        }
    }
    if (salvarComTemp(vazio, 0))                    // grava so o cabecalho
        printf("Arquivo '%s' criado (vazio). Use a opcao 3 para cadastrar produtos.\n", ARQ);
    else
        printf("Erro ao criar o arquivo.\n");
}

// 3) INSERIR: cadastra novo produto com ID automatico e sem nome duplicado
void inserir(Produto *lista, int n) {
    Produto novo;                                   // produto que sera cadastrado
    if (n >= MAX) {                                 // protege o limite do vetor
        printf("Limite de %d produtos atingido.\n", MAX);
        return;
    }
    lerTexto("Nome do produto: ", novo.produto, sizeof(novo.produto));
    if (novo.produto[0] == '\0') {                  // nome vazio nao vale
        printf("Nome nao pode ser vazio.\n");
        return;
    }
    if (indicePorNome(lista, n, novo.produto, 0) != -1) {   // verifica duplicata
        printf("Ja existe um produto com esse nome. Cadastro cancelado.\n");
        return;
    }
    lerTexto("Categoria: ", novo.categoria, sizeof(novo.categoria));
    novo.preco = lerFloat("Preco (R$): ");
    novo.quantidade = lerInt("Quantidade: ");
    if (novo.quantidade < 0) novo.quantidade = 0;   // quantidade nao pode ser negativa
    lerTexto("Fornecedor: ", novo.fornecedor, sizeof(novo.fornecedor));

    novo.id = proximoId(lista, n);                  // gera o ID automaticamente

    lista[n] = novo;                                // coloca o novo produto no fim do vetor
    if (!salvarComTemp(lista, n + 1)) {             // regrava a tabela com o novo produto
        printf("Erro ao gravar o arquivo.\n");
        return;
    }
    printf("Produto cadastrado com ID %d.\n", novo.id);
    imprimirTabela(lista, n + 1);                   // mostra a tabela ja com o novo produto
}

// 4) ALTERAR: muda os dados de um produto (Enter mantem o valor atual)
void alterar(Produto *lista, int n) {
    char buf[64];
    char *fim;
    int id = lerInt("ID do produto a alterar: ");
    Produto *p = buscarPorId(lista, n, id);         // ponteiro para o produto no vetor
    if (p == NULL) {
        printf("Produto nao encontrado.\n");
        return;
    }
    printf("Editando '%s' (Enter mantem o valor atual)\n", p->produto);

    lerTexto("Novo nome: ", buf, sizeof(p->produto));
    if (buf[0] != '\0') {                           // so altera se digitou algo
        if (indicePorNome(lista, n, buf, p->id) != -1) {    // evita nome duplicado
            printf("Ja existe outro produto com esse nome. Alteracao cancelada.\n");
            return;
        }
        strcpy(p->produto, buf);                    // altera via ponteiro
    }
    lerTexto("Nova categoria: ", buf, sizeof(p->categoria));
    if (buf[0] != '\0') strcpy(p->categoria, buf);

    lerTexto("Novo preco: ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        for (char *c = buf; *c; c++) if (*c == ',') *c = '.';
        double v = strtod(buf, &fim);
        if (fim != buf && v >= 0) p->preco = (float)v;
        else printf("Preco invalido, mantido o anterior.\n");
    }
    lerTexto("Nova quantidade: ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        long q = strtol(buf, &fim, 10);
        if (fim != buf && q >= 0) p->quantidade = (int)q;
        else printf("Quantidade invalida, mantida a anterior.\n");
    }
    lerTexto("Novo fornecedor: ", buf, sizeof(p->fornecedor));
    if (buf[0] != '\0') strcpy(p->fornecedor, buf);

    if (salvarComTemp(lista, n))                    // grava tudo via arquivo temporario
        printf("Produto alterado com sucesso.\n");
    else
        printf("Erro ao gravar o arquivo.\n");
}

// 5) EXCLUIR: remove um produto pelo ID
void excluir(Produto *lista, int n) {
    char resp[8];
    int id = lerInt("ID do produto a excluir: ");
    int pos = -1;                                   // posicao do produto no vetor
    for (int i = 0; i < n; i++)
        if (lista[i].id == id) { pos = i; break; }
    if (pos == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }
    printf("Excluir '%s'? ", lista[pos].produto);
    lerTexto("(s/n): ", resp, sizeof(resp));
    if (resp[0] != 's' && resp[0] != 'S') {
        printf("Exclusao cancelada.\n");
        return;
    }
    for (int i = pos; i < n - 1; i++)               // desloca os seguintes uma posicao para tras
        lista[i] = lista[i + 1];
    n--;                                            // um produto a menos
    if (salvarComTemp(lista, n)) {                  // regrava sem o produto excluido
        printf("Produto excluido.\n");
        imprimirTabela(lista, n);                   // mostra a tabela atualizada, sem o excluido
    } else {
        printf("Erro ao gravar o arquivo.\n");
    }
}

// 6) BUSCAR por nome (opcao 1) ou categoria (opcao 2) - digite em MAIUSCULAS
void buscar(const Produto *lista, int n) {
    Produto achados[MAX];                           // resultados da busca
    char termo[50];
    int qtd = 0;
    int tipo;
    printf("\n1 - Buscar por nome\n2 - Buscar por categoria\n");
    tipo = lerInt("Escolha: ");
    if (tipo != 1 && tipo != 2) {
        printf("Opcao invalida.\n");
        return;
    }
    lerTexto("Termo da busca: ", termo, sizeof(termo));
    for (int i = 0; i < n; i++) {
        const char *campo = (tipo == 1) ? lista[i].produto : lista[i].categoria; // escolhe o campo
        if (contemTexto(campo, termo))              // procura o termo dentro do campo
            achados[qtd++] = lista[i];              // copia o produto achado
    }
    if (qtd == 0)
        printf("Nenhum produto encontrado.\n");
    else
        imprimirTabela(achados, qtd);
}

/* ======================== ORDENACAO (BUBBLE SORT) ======================== */

// Comparadores: retornam >0 se 'a' deve vir DEPOIS de 'b'
int cmpNomeAZ(const Produto *a, const Produto *b) {
    return strcmp(a->produto, b->produto);          // ordem alfabetica crescente
}
int cmpNomeZA(const Produto *a, const Produto *b) {
    return cmpNomeAZ(b, a);                         // inverte os argumentos = ordem decrescente
}
int cmpPrecoAsc(const Produto *a, const Produto *b) {
    return (a->preco > b->preco) - (a->preco < b->preco);   // 1, 0 ou -1
}
int cmpPrecoDesc(const Produto *a, const Produto *b) {
    return cmpPrecoAsc(b, a);
}

// Bubble Sort generico: recebe um PONTEIRO PARA FUNCAO que define o criterio
void bubbleSort(Produto *v, int n, int (*cmp)(const Produto *, const Produto *)) {
    Produto aux;                                    // auxiliar para a troca
    for (int i = 0; i < n - 1; i++) {               // passadas
        for (int j = 0; j < n - 1 - i; j++) {       // compara pares adjacentes
            if (cmp(&v[j], &v[j + 1]) > 0) {        // fora de ordem?
                aux = v[j];                         // troca os dois elementos
                v[j] = v[j + 1];
                v[j + 1] = aux;
            }
        }
    }
}

// 7) ORDENAR: ordena uma COPIA do vetor e exibe (o arquivo nao e alterado)
void ordenar(const Produto *lista, int n) {
    Produto copia[MAX];                             // copia para nao mexer no original
    int op;
    if (n == 0) {
        printf("Nao ha produtos para ordenar.\n");
        return;
    }
    printf("\n1 - Nome (A-Z)\n2 - Nome (Z-A)\n3 - Preco (menor -> maior)\n4 - Preco (maior -> menor)\n");
    op = lerInt("Escolha: ");
    for (int i = 0; i < n; i++)                     // copia os dados
        copia[i] = lista[i];

    switch (op) {                                   // escolhe o criterio de ordenacao
        case 1: bubbleSort(copia, n, cmpNomeAZ);    break;
        case 2: bubbleSort(copia, n, cmpNomeZA);    break;
        case 3: bubbleSort(copia, n, cmpPrecoAsc);  break;
        case 4: bubbleSort(copia, n, cmpPrecoDesc); break;
        default:
            printf("Opcao invalida.\n");
            return;
    }
    imprimirTabela(copia, n);                       // exibe em formato de tabela
}

// 8) EXPORTAR para CSV (separado por virgula, textos entre aspas)
void exportarCSV(const Produto *lista, int n) {
    FILE *f = fopen(CSV, "w");
    if (f == NULL) {
        printf("Erro ao criar '%s'.\n", CSV);
        return;
    }
    fprintf(f, "ID,Produto,Categoria,Preco,Quantidade,Fornecedor\n");   // cabecalho
    for (int i = 0; i < n; i++)
        fprintf(f, "%d,\"%s\",\"%s\",%.2f,%d,\"%s\"\n",
                lista[i].id, lista[i].produto, lista[i].categoria,
                lista[i].preco, lista[i].quantidade, lista[i].fornecedor);
    fclose(f);
    printf("Dados exportados para '%s'.\n", CSV);
}

/* ============================== MAIN ===================================== */

int main(void) {
    Produto lista[MAX];                             // vetor de estruturas
    int n;                                          // quantidade de produtos carregados
    int opcao;

    verificarArquivo();                             // avisa se o produtos.txt nao existir

    do {
        n = carregar(lista);                        // recarrega do arquivo a cada ciclo
        printf("\n===== CADASTRO DA LOJA =====\n");
        printf("1 - Criar arquivo produtos.txt (vazio)\n");
        printf("2 - Listar produtos\n");
        printf("3 - Inserir produto\n");
        printf("4 - Alterar produto\n");
        printf("5 - Excluir produto\n");
        printf("6 - Buscar (nome/categoria)\n");
        printf("7 - Ordenar e exibir\n");
        printf("8 - Exportar para CSV\n");
        printf("0 - Sair\n");
        opcao = lerInt("Escolha uma opcao: ");

        switch (opcao) {
            case 1: criarArquivo();                 break;
            case 2:
                if (n == 0) printf("Nenhum produto cadastrado.\n");
                else imprimirTabela(lista, n);
                break;
            case 3: inserir(lista, n);              break;
            case 4: alterar(lista, n);              break;
            case 5: excluir(lista, n);              break;
            case 6: buscar(lista, n);               break;
            case 7: ordenar(lista, n);              break;
            case 8: exportarCSV(lista, n);          break;
            case 0: printf("Saindo...\n");          break;
            default: printf("Opcao invalida!\n");   break;
        }
    } while (opcao != 0);

    return 0;
}
