#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_NAME_SIZE 1024
#define FREQ_SIZE 256

typedef enum Mode
{
    COMP,
    EXTR
} Mode;

Mode get_mode()
{
    char resp;

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("OLA, AVENTUREIRO DA COMPUTACAO, QUAL FUNCAO DE NOSSO PROGRAMA QUERES USUFRUIR?\n");
    printf("PRESSIONE 'c' PARA COMPRIMIR ARQUIVO OU 'd' PARA DESCOMPRIMIR: ");
    scanf("%c", &resp);
    while (getchar() != '\n');

    if (resp == 'c')
    {
        printf("MODO SELECIONADO: >> COMPRIMIR <<\n");
        return COMP;
    }
    else if (resp == 'd')
    {
        printf("MODO SELECIONADO: >> DESCOMPRIMIR <<\n");
        return EXTR;
    }
    else
    {
        fprintf(stderr, "MODO INVALIDO SELECIONADO\n");
        exit(1);
    }
}

void get_file_content(unsigned char **content, char *file_name)
{
    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    printf("POR FAVOR, INSIRA O 'PATH' PARA O ARQUIVO DESEJADO:\n");
    fgets(file_name, MAX_FILE_NAME_SIZE, stdin);
    if (file_name)
        file_name[strcspn(file_name, "\n")] = '\0';
    else
    {
        fprintf(stderr, "NOME DE ARQUIVO INVALIDO\n");
        exit(1);
    }

    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        fprintf(stderr, "ERRO: NAO FOI POSSIVEL ABRIR O ARQUIVO '%s'\n", file_name);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *content = (unsigned char *)malloc(sizeof(unsigned char) * (file_size + 1));
    if (*content == NULL) {
        fprintf(stderr, "ERRO: FALHA NA ALOCACAO DE MEMORIA\n");
        exit(1);
    }

    size_t bytes_read = fread(*content, sizeof(unsigned char), file_size, file);
    if (bytes_read != file_size) {
        fprintf(stderr, "ERRO: NAO FOI POSSIVEL LER O ARQUIVO COMPLETAMENTE\n");
        exit(1);
    }
    (*content)[file_size] = '\0';

    fclose(file);
}

void get_freq(unsigned char *content, unsigned int freq[])
{
    unsigned int i = 0;
    while (content[i] != '\0')
    {
        freq[content[i]]++;
        i++;
    }
}

void print_freq(unsigned int freq[])
{
    for (int i = 0; i < FREQ_SIZE; i++)
    {
        if (freq[i])
            printf("\t%d = %d = %c\n", i, freq[i], i);
    }
}

int main()
{
    Mode mode = get_mode();

    unsigned char *content = NULL;
    char *file_name = malloc(MAX_FILE_NAME_SIZE * sizeof(char));;
    get_file_content(&content, file_name);

    if (mode == COMP)
    {
        unsigned int freq[FREQ_SIZE];
        for (int i = 0; i < FREQ_SIZE; i++)
        {
            freq[i] = 0;
        }
        get_freq(content, freq);


    }
    else
    {

    }

    free(file_name);
    free(content);

    return 0;
}