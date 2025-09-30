#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_NAME_SIZE 100

typedef struct No
{
    void *ch;
    void *frq;
    void *prx;
    void *esq;
    void *dir;
} No;

typedef struct Lista
{
    void *cabeca;
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
    else
    {
        printf("MODO SELECIONADO: >> DESCOMPRIMIR <<\n");
        return DESCOMPRIMIR;
    }
}

char *get_file_nome()
{
    char *file_nome = malloc(sizeof(char) * MAX_FILE_NAME_SIZE);

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("POR FAVOR, INSIRA O 'PATH' PARA O ARQUIVO DESEJADO:\n");
    fgets(file_nome, MAX_FILE_NAME_SIZE, stdin);
    file_nome[strcspn(file_nome, "\n")] = '\0';

    return file_nome;
}

int main(void)
{
    Mode modo = get_modo();
    char *file_nome = get_file_nome();

    free(file_nome);

    return 0;
}