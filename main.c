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
    else
    {
        printf("MODO SELECIONADO: >> DESCOMPRIMIR <<\n");
        return DESCOMPRIMIR;
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

int main(void)
{
    Mode modo = get_modo();
    char *file_nome = malloc(sizeof(char) * MAX_FILE_NAME_SIZE);
    get_file_nome(file_nome);

    int frequencias[FREQ_SIZE];
    for (int i = 0; i < FREQ_SIZE; i++)
        frequencias[i] = 0;
    get_frequencias(frequencias, file_nome);

    // TODO

    free(file_nome);

    return 0;
}