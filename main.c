#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FREQ_SIZE 256
#define MAX_FILE_NAME_SIZE 1000
#define TREE_BUFFER_MAX_SIZE 10000

typedef struct Node
{
    void *c, *freq, *next, *left, *right;
} Node;

typedef struct List
{
    void *head, *size;
} List;

void get_freq(unsigned char *content, unsigned int freq[], size_t size)
{
    for (size_t i = 0; i < size; i++)
        freq[content[i]]++;
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
    *(unsigned int *)list->size = 0;
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

    *(unsigned int *)list->size += 1;
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

Node *remove_begin(List *list)
{
    Node *aux = NULL;
    if (list->head)
    {
        aux = list->head;
        list->head = aux->next;
        aux->next = NULL;
        *(unsigned int *)list->size -= 1;
    }
    return aux;
}

Node *build_tree(List *list)
{
    Node *first, *second, *new;
    while (*(unsigned int *)list->size > 1)
    {
        first = remove_begin(list);
        second = remove_begin(list);
        new = malloc(sizeof(Node));
        if (new)
        {
            new->c = malloc(sizeof(unsigned char));
            new->freq = malloc(sizeof(unsigned int));
            *(unsigned char *)new->c = '*';
            *(unsigned int *)new->freq = *(unsigned int *)first->freq + *(unsigned int *)second->freq;
            new->left = first;
            new->right = second;
            new->next = NULL;
            insert_sorted(list, new);
        }
        else
        {
            printf("erro alocar mem em build_tree\n");
            break;
        }
    }
    return list->head;
}

void free_tree(Node *root)
{
    if (!root)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root->c);
    free(root->freq);
    free(root);
}

int tree_height(Node *root)
{
    if (!root)
        return -1;
    int left = tree_height(root->left) + 1;
    int right = tree_height(root->right) + 1;
    return (left > right) ? left : right;
}

char **allocate_table(int columns)
{
    char **table = malloc(sizeof(char *) * FREQ_SIZE);
    for (int i = 0; i < FREQ_SIZE; i++)
        table[i] = calloc(columns, sizeof(char));
    return table;
}

void build_table(char **table, Node *root, char *path, int columns)
{
    if (!root->left && !root->right)
        strcpy(table[*(unsigned char *)root->c], path);
    else
    {
        char left[columns], right[columns];
        strcpy(left, path);
        strcpy(right, path);
        strcat(left, "0");
        strcat(right, "1");
        build_table(table, root->left, left, columns);
        build_table(table, root->right, right, columns);
    }
}

int size_str(char **table, unsigned char *content, size_t size)
{
    int total = 0;
    for (size_t i = 0; i < size; i++)
        total += strlen(table[content[i]]);
    return total + 1;
}

char *encode(char **table, unsigned char *content, size_t size)
{
    char *codigo = calloc(size_str(table, content, size), sizeof(char));
    for (size_t i = 0; i < size; i++)
        strcat(codigo, table[content[i]]);
    return codigo;
}

unsigned int is_bit_one(unsigned char byte, int i)
{
    unsigned char mask = 1 << i;
    return byte & mask;
}

void build_tree_buffer(Node *root, char *buffer, int *index)
{
    if (!root->left && !root->right)
    {
        buffer[(*index)++] = '1';
        unsigned char c = *(unsigned char *)root->c;
        for (int i = 7; i >= 0; i--)
            buffer[(*index)++] = (c & (1 << i)) ? '1' : '0';
    }
    else
    {
        buffer[(*index)++] = '0';
        build_tree_buffer(root->left, buffer, index);
        build_tree_buffer(root->right, buffer, index);
    }
}

void write_bit_str(FILE *file, char *bit_str, int bit_qnt)
{
    int i = 0, j = 7;
    unsigned char byte = 0;
    while (i < bit_qnt)
    {
        if (bit_str[i] == '1')
            byte |= (1 << j);
        j--;
        if (j < 0)
        {
            fwrite(&byte, sizeof(unsigned char), 1, file);
            byte = 0;
            j = 7;
        }
        i++;
    }
    if (j != 7)
        fwrite(&byte, sizeof(unsigned char), 1, file);
}

void mount_header(FILE *file, unsigned char str[], Node *root)
{
    unsigned int trash_size = strlen((char *)str) % 8;

    char tree_bits[TREE_BUFFER_MAX_SIZE];
    int index = 0;
    build_tree_buffer(root, tree_bits, &index);

    int bit_qnt = index;
    unsigned int tree_size = (bit_qnt + 7) / 8;

    unsigned char byte1 = (trash_size << 5) | (tree_size >> 8);
    unsigned char byte2 = tree_size & 255;

    fwrite(&byte1, sizeof(unsigned char), 1, file);
    fwrite(&byte2, sizeof(unsigned char), 1, file);
    write_bit_str(file, tree_bits, bit_qnt);
}

void compress(unsigned char str[], char file_name[], Node *root)
{
    FILE *huff_file = fopen(file_name, "wb");
    if (!huff_file)
    {
        printf("erro ao abrir arquivo em compress()\n");
        return;
    }
    mount_header(huff_file, str, root);
    int i = 0, j = 7;
    unsigned char byte = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '1')
            byte |= (1 << j);
        j--;
        if (j < 0)
        {
            fwrite(&byte, sizeof(unsigned char), 1, huff_file);
            byte = 0;
            j = 7;
        }
        i++;
    }
    if (j != 7)
        fwrite(&byte, sizeof(unsigned char), 1, huff_file);
    fclose(huff_file);
}

Node *build_tree_from_bits(unsigned char *buffer, int *index, int total_bits)
{
    if (*index >= total_bits)
        return NULL;
    if (buffer[*index] == '1')
    {
        (*index)++;
        unsigned char c = 0;
        for (int i = 0; i < 8 && *index < total_bits; i++, (*index)++)
        {
            c <<= 1;
            if (buffer[*index] == '1')
                c |= 1;
        }
        Node *leaf = malloc(sizeof(Node));
        leaf->c = malloc(sizeof(unsigned char));
        *(unsigned char *)leaf->c = c;
        leaf->freq = malloc(sizeof(unsigned int));
        *(unsigned int *)leaf->freq = 0;
        leaf->left = leaf->right = leaf->next = NULL;
        return leaf;
    }
    else if (buffer[*index] == '0')
    {
        (*index)++;
        Node *node = malloc(sizeof(Node));
        node->c = malloc(sizeof(unsigned char));
        *(unsigned char *)node->c = '*';
        node->freq = malloc(sizeof(unsigned int));
        *(unsigned int *)node->freq = 0;
        node->next = NULL;
        node->left = build_tree_from_bits(buffer, index, total_bits);
        node->right = build_tree_from_bits(buffer, index, total_bits);
        return node;
    }
    return NULL;
}

Node *build_tree_from_header(FILE *file)
{
    unsigned char byte1, byte2;
    fread(&byte1, sizeof(unsigned char), 1, file);
    fread(&byte2, sizeof(unsigned char), 1, file);

    unsigned int trash_size = byte1 >> 5;
    unsigned int tree_size = ((byte1 & 31) << 8) | byte2;

    unsigned char *tree_bytes = malloc(tree_size);
    fread(tree_bytes, sizeof(unsigned char), tree_size, file);

    unsigned char *tree_bits = malloc(tree_size * 8 + 1);
    int index = 0;
    for (unsigned int i = 0; i < tree_size; i++)
        for (int j = 7; j >= 0; j--)
            tree_bits[index++] = (is_bit_one(tree_bytes[i], j)) ? '1' : '0';
    tree_bits[index] = '\0';

    int bit_index = 0;
    Node *root = build_tree_from_bits(tree_bits, &bit_index, index);

    free(tree_bytes);
    free(tree_bits);
    (void)trash_size;
    return root;
}

char *file_without_dot_huff(char *file_name)
{
    char *new_name = malloc(MAX_FILE_NAME_SIZE);
    strcpy(new_name, file_name);
    new_name[strcspn(new_name, ".")] = '\0';
    return new_name;
}

void extract(char huff_name[])
{
    FILE *file = fopen(huff_name, "rb");
    if (!file)
    {
        printf("erro ao abrir arquivo em extract()\n");
        return;
    }

    char *file_without_huff = file_without_dot_huff(huff_name);
    FILE *extracted = fopen(file_without_huff, "wb");
    Node *root = build_tree_from_header(file);

    unsigned char byte = 0;
    Node *aux = root;
    while (fread(&byte, sizeof(unsigned char), 1, file))
    {
        for (int i = 7; i >= 0; i--)
        {
            aux = is_bit_one(byte, i) ? aux->right : aux->left;
            if (!aux->left && !aux->right)
            {
                fwrite((char *)aux->c, sizeof(char), 1, extracted);
                aux = root;
            }
        }
    }

    free_tree(root);
    free(file_without_huff);
    fclose(extracted);
    fclose(file);
}

int main()
{
    setlocale(LC_ALL, "C");

    int opcao;
    char file_name[MAX_FILE_NAME_SIZE];

    printf("=== COMPRESSOR HUFFMAN ===\n");
    printf("1 - Comprimir arquivo\n");
    printf("2 - Extrair arquivo (.huff)\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &opcao);
    getchar();

    if (opcao == 1)
    {
        printf("Caminho do arquivo a ser comprimido: ");
        scanf("%s", file_name);

        FILE *file = fopen(file_name, "rb");
        if (!file)
        {
            printf("Erro ao abrir arquivo para leitura.\n");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        unsigned char *content = calloc(file_size, sizeof(unsigned char));
        fread(content, sizeof(unsigned char), file_size, file);
        fclose(file);

        unsigned int freq[FREQ_SIZE] = {0};
        get_freq(content, freq, file_size);

        List list;
        create_list(&list);
        fill_list(freq, &list);

        Node *root = build_tree(&list);

        int columns = tree_height(root) + 2;
        char **table = allocate_table(columns);
        char path[columns];
        strcpy(path, "");
        build_table(table, root, path, columns);

        char *encoded = encode(table, content, file_size);

        char compressed_name[MAX_FILE_NAME_SIZE];
        snprintf(compressed_name, sizeof(compressed_name), "%s.huff", file_name);

        compress((unsigned char *)encoded, compressed_name, root);
        printf("Arquivo comprimido com sucesso: %s\n", compressed_name);

        free_tree(root);
        for (int i = 0; i < FREQ_SIZE; i++)
            free(table[i]);
        free(table);
        free(content);
        free(encoded);
    }
    else if (opcao == 2)
    {
        printf("Caminho do arquivo .huff a ser extraido: ");
        scanf("%s", file_name);
        extract(file_name);
        printf("Arquivo extraido com sucesso.\n");
    }
    else
    {
        printf("Opcao invalida.\n");
    }

    return 0;
}