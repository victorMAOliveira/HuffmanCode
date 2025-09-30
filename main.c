#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_NAME_SIZE 100
#define FREQ_SIZE 250

typedef struct NoArvore
{
    void *ch;
    void *frq;
    void *prx;
    void *esq;
    void *dir;
} NoArvore;

typedef struct Lista
{
    NoArvore *cabeca;
} Lista;

typedef enum Mode
{
    COMPRIMIR,
    DESCOMPRIMIR
} Mode;

Mode get_modo()
{
    char resp;

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("OLA, AVENTUREIRO DA COMPUTACAO, QUAL FUNCAO DE NOSSO PROGRAMA QUERES USUFRUIR?\n");
    printf("PRESSIONE 'c' PARA COMPRIMIR ARQUIVO OU 'd' PARA DESCOMPRIMIR: ");
    scanf("%c", &resp);

    if (resp == 'c')
    {
        printf("MODO SELECIONADO: >> COMPRIMIR <<\n");
        return COMPRIMIR;
    }
    else if (resp == 'd')
    {
        printf("MODO SELECIONADO: >> DESCOMPRIMIR <<\n");
        return DESCOMPRIMIR;
    }
    else
    {
        fprintf(stderr, "MODO INVALIDO SELECIONADO\n");
        exit(1);
    }
}

void get_file_nome(char *file_nome)
{
    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("POR FAVOR, INSIRA O 'PATH' PARA O ARQUIVO DESEJADO:\n");
    fgets(file_nome, MAX_FILE_NAME_SIZE, stdin);
    if (file_nome)
        file_nome[strcspn(file_nome, "\n")] = '\0';
    else
    {
        fprintf(stderr, "NOME DE ARQUIVO INVALIDO\n");
        exit(1);
    }
}

void get_frequencias(int frequencias[], const char *file_nome)
{
    FILE *file = fopen(file_nome, "r");
    if (!file)
    {
        fprintf(stderr, "NAO FOI POSSIVEL ABRIR O ARQUIVO EM get_frequencias\n");
        exit(1);
    }
    char c;

    while ((c = fgetc(file)) != EOF)
        frequencias[c]++;

    fclose(file);
}

void add_ordenado(Lista *lista, char c, int freq)
{
    NoArvore *novo_no = malloc(sizeof(NoArvore));
    *(char *)novo_no->ch = c;
    *(int *)novo_no->frq = freq;
    novo_no->dir = NULL;
    novo_no->esq = NULL;
    novo_no->prx = NULL;

    if (!lista->cabeca)
        lista->cabeca = novo_no;
    else
    {
        NoArvore *atual = lista->cabeca;
        while (atual->prx != NULL && atual->prx < novo_no->frq)
            atual = atual->prx;
        novo_no->prx = atual->prx;
        atual->prx = novo_no;
    }
}

Lista *get_lista(int frequencias[])
{
    Lista *lista = malloc(sizeof(lista));
    lista->cabeca = NULL;

    for (int i = 0; i < FREQ_SIZE; i++)
    {
        if (frequencias[i])
            add_ordenado(lista, (char)i, frequencias[i]);
    }

    return lista;
}

int main(void)
{
    Mode modo = get_modo();
    char *file_nome = malloc(sizeof(char) * MAX_FILE_NAME_SIZE);
    get_file_nome(file_nome);

    int frequencias[FREQ_SIZE];
    for (int i = 0; i < FREQ_SIZE; i++)
        frequencias[i] = 0;
    get_frequencias(frequencias, file_nome);

    Lista *lista = get_lista(frequencias);

    // TODO

    free(file_nome);
    free(lista);

    return 0;
}