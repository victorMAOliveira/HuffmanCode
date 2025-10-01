#include <locale.h>
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

typedef struct Node
{
    void *c, *freq, *next, *left, *right;
} Node;

typedef struct List
{
    void *head, *size;
} List;

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

void create_list(List *list)
{
    list->head = NULL;
    list->size = malloc(sizeof(int));
    *(int *)list->size = 0;
}

void insert_sorted(List *list, Node *node)
{
    if (!list->head)
    {
        list->head = node;
    }
    else if (*(unsigned int *)node->freq < *(unsigned int *)((Node *)list->head)->freq)
    {
        node->next = list->head;
        list->head = node;
    }
    else
    {
        Node *aux = list->head;
        while (aux->next && *(unsigned int *)((Node *)aux->next)->freq <= *(unsigned int *)node->freq)
            aux = aux->next;
        node->next = aux->next;
        aux->next = node;
    }

    *(int *)list->size += 1;
}

void fill_list(unsigned int freq[], List *list)
{
    for (int i = 0; i < FREQ_SIZE; i++)
    {
        if (freq[i])
        {
            Node *new = malloc(sizeof(Node));
            if (new)
            {
                new->c = malloc(sizeof(unsigned char));
                new->freq = malloc(sizeof(unsigned int));
                *(unsigned char *)new->c = i;
                *(unsigned int *)new->freq = freq[i];
                new->next = NULL;
                new->left = NULL;
                new->right = NULL;

                insert_sorted(list, new);
            }
            else
            {
                printf("erro alocar memoria em fill_list\n");
                break;
            }
        }
    }
}

void print_list(List *list)
{
    Node *aux = list->head;

    while (aux)
    {
        printf("\tCaracter: %c Frequencia: %u\n", *(char *)aux->c, *(unsigned int *)aux->freq);
        aux = aux->next;
    }
}

int main()
{
    setlocale(LC_ALL, "Portuguese");

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

        List list;
        create_list(&list);
        fill_list(freq, &list);


    }
    else
    {
        // TODO
    }

    free(file_name);
    free(content);

    return 0;
}