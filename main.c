#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FREQ_SIZE 256

typedef struct Node
{
    void *c, *freq, *next, *left, *right;
} Node;

typedef struct List
{
    void *head, *size;
} List;

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

void print_tree(Node *root, unsigned int size)
{
    if (!root->left && !root->right)
        printf("folha: %c\taltura: %u\n", *(unsigned char *)root->c, size);
    else
    {
        print_tree(root->left, size + 1);
        print_tree(root->right, size + 1);
    }
}

int tree_height(Node *root)
{
    if (!root)
        return -1;
    else
    {
        int left = tree_height(root->left) + 1;
        int right = tree_height(root->right) + 1;
        if (left > right)
            return left;
        else
            return right;
    }
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

void print_table(char **table)
{
    for (int i = 0; i < FREQ_SIZE; i++)
    {
        if (strlen(table[i]) > 0)
            printf("%3d: %s\n", i, table[i]);
    }
}

int size_str(char **table, unsigned char *content)
{
    int i = 0, size = 0;
    while (content[i] != '\0')
        size += strlen(table[content[i++]]);
    return size + 1;
}

char *encode(char **table, unsigned char *content)
{
    char *codigo = calloc(size_str(table, content), sizeof(char));
    int i = 0;
    while (content[i] != '\0')
        strcat(codigo, table[content[i++]]);
    return codigo;
}

char *decode(unsigned char content[], Node *root)
{
    int i = 0;
    Node *aux = root;
    char *decoded = calloc(strlen(content), sizeof(char));
    char temp[2];
    while (content[i] != '\0')
    {   
        if (content[i] == '0')
            aux = aux->left;
        else
            aux = aux->right;
        if (!aux->left && !aux->right)
        {
            temp[0] = (unsigned char *)aux->c;
            temp[1] = '\0';
            strcat(decoded, temp);
        }
        i++;
    }
}

int main()
{
    setlocale(LC_ALL, "Portuguese");

    unsigned char content[] = "Vamos aprender a programar";

    unsigned int freq[FREQ_SIZE];
    for (int i = 0; i < FREQ_SIZE; i++)
    {
        freq[i] = 0;
    }
    get_freq(content, freq);
    printf("\nfrequencias:\n");
    print_freq(freq);

    List list;
    create_list(&list);
    fill_list(freq, &list);
    printf("\nlista:\n");
    print_list(&list);

    Node *tree = build_tree(&list);
    printf("\narvore:\n");
    print_tree(tree, 0);

    int columns = tree_height(tree) + 1;
    char **table = allocate_table(columns);
    build_table(table, tree, "", columns);
    printf("\ntabela:\n");
    print_table(table);

    char *encoded = encode(table, content);
    printf("\ncodificado: %s\n", encoded);

    return 0;
}