/**
 * @file main.c
 * @brief Implementação completa do algoritmo de compressão e extração Huffman
 * @details
 * Este programa realiza compressão e descompressão de arquivos utilizando o
 * algoritmo de Huffman, preservando a estrutura da árvore de codificação no
 * cabeçalho do arquivo gerado (.huff).
 *
 * O código é dividido em módulos lógicos:
 * - Utilitários gerais (funções auxiliares)
 * - Estruturas e manipulação de lista ordenada
 * - Construção e destruição da árvore de Huffman
 * - Geração e leitura do cabeçalho
 * - Compressão e extração de arquivos
 * - Interface simples via terminal
 *
 * @author
 * Victor Oliveira, Lorenzo Holanda, Lucitanea Lopes
 *
 * @version 1.0
 * @date 2025-10-23
 */

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FREQ_SIZE 256
#define MAX_FILE_NAME_SIZE 1000
#define TREE_BUFFER_MAX_SIZE 10000

// === COLOR MACROS ===
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define MAGENTA "\033[35m"

/**
 * @struct Node
 * @brief Nó da árvore de Huffman
 *
 * Cada nó pode representar um caractere (folha) ou um nó interno
 * com dois filhos (esquerdo e direito).
 */
typedef struct Node {
  void* c;      ///< Ponteiro genérico para o caractere armazenado
  void* freq;   ///< Frequência do caractere
  void* next;   ///< Próximo nó da lista encadeada
  void* left;   ///< Filho esquerdo (subárvore esquerda)
  void* right;  ///< Filho direito (subárvore direita)
} Node;

/**
 * @struct List
 * @brief Lista encadeada ordenada usada para construir a árvore
 */
typedef struct List {
  void* head;  ///< Cabeça da lista
  void* size;  ///< Quantidade de nós armazenados
} List;

// === UTILITÁRIOS ===

/**
 * @brief Limpa o terminal
 */
void clear_screen() { printf("\033[2J\033[H"); }

/**
 * @brief Conta a frequência de cada caractere em um conteúdo
 * @param content Vetor de bytes do arquivo
 * @param freq Vetor de frequências (tamanho 256)
 * @param size Tamanho do conteúdo
 */
void get_freq(unsigned char* content, unsigned int freq[], size_t size) {
  for (size_t i = 0; i < size; i++) freq[content[i]]++;
}

/**
 * @brief Inicializa uma lista vazia
 */
void create_list(List* list) {
  list->head = NULL;
  list->size = malloc(sizeof(int));
  *(unsigned int*)list->size = 0;
}

/**
 * @brief Insere um nó na lista em ordem crescente de frequência
 */
void insert_sorted(List* list, Node* node) {
  if (!list->head) {
    list->head = node;
  } else if (*(unsigned int*)node->freq <
             *(unsigned int*)((Node*)list->head)->freq) {
    node->next = list->head;
    list->head = node;
  } else {
    Node* aux = list->head;
    while (aux->next && *(unsigned int*)((Node*)aux->next)->freq <=
                            *(unsigned int*)node->freq)
      aux = aux->next;
    node->next = aux->next;
    aux->next = node;
  }
  *(unsigned int*)list->size += 1;
}

/**
 * @brief Preenche a lista com nós criados a partir das frequências
 */
void fill_list(unsigned int freq[], List* list) {
  for (int i = 0; i < FREQ_SIZE; i++) {
    if (freq[i]) {
      Node* new = malloc(sizeof(Node));
      if (new) {
        new->c = malloc(sizeof(unsigned char));
        new->freq = malloc(sizeof(unsigned int));
        *(unsigned char*)new->c = i;
        *(unsigned int*)new->freq = freq[i];
        new->next = NULL;
        new->left = NULL;
        new->right = NULL;
        insert_sorted(list, new);
      } else {
        printf(RED "Erro: falha ao alocar memória em fill_list\n" RESET);
        break;
      }
    }
  }
}

/**
 * @brief Remove e retorna o primeiro nó da lista
 */
Node* remove_begin(List* list) {
  Node* aux = NULL;
  if (list->head) {
    aux = list->head;
    list->head = aux->next;
    aux->next = NULL;
    *(unsigned int*)list->size -= 1;
  }
  return aux;
}

/**
 * @brief Constrói a árvore de Huffman a partir da lista ordenada
 */
Node* build_tree(List* list) {
  Node *first, *second, *new;
  while (*(unsigned int*)list->size > 1) {
    first = remove_begin(list);
    second = remove_begin(list);
    new = malloc(sizeof(Node));
    if (new) {
      new->c = malloc(sizeof(unsigned char));
      new->freq = malloc(sizeof(unsigned int));
      *(unsigned char*)new->c = '*';
      *(unsigned int*)new->freq =
          *(unsigned int*)first->freq + *(unsigned int*)second->freq;
      new->left = first;
      new->right = second;
      new->next = NULL;
      insert_sorted(list, new);
    } else {
      printf(RED "Erro: falha ao alocar memória em build_tree\n" RESET);
      break;
    }
  }
  return list->head;
}

/**
 * @brief Libera recursivamente toda a árvore de Huffman
 */
void free_tree(Node* root) {
  if (!root) return;
  free_tree(root->left);
  free_tree(root->right);
  free(root->c);
  free(root->freq);
  free(root);
}

/**
 * @brief Calcula a altura da árvore
 */
int tree_height(Node* root) {
  if (!root) return -1;
  int left = tree_height(root->left) + 1;
  int right = tree_height(root->right) + 1;
  return (left > right) ? left : right;
}

/**
 * @brief Aloca a tabela de códigos binários dos caracteres
 */
char** allocate_table(int columns) {
  char** table = malloc(sizeof(char*) * FREQ_SIZE);
  for (int i = 0; i < FREQ_SIZE; i++) table[i] = calloc(columns, sizeof(char));
  return table;
}

/**
 * @brief Constrói a tabela de códigos binários recursivamente
 */
void build_table(char** table, Node* root, char* path, int columns) {
  if (!root->left && !root->right)
    strcpy(table[*(unsigned char*)root->c], path);
  else {
    char left[columns], right[columns];
    strcpy(left, path);
    strcpy(right, path);
    strcat(left, "0");
    strcat(right, "1");
    build_table(table, root->left, left, columns);
    build_table(table, root->right, right, columns);
  }
}

/**
 * @brief Retorna o tamanho total da string codificada
 */
int size_str(char** table, unsigned char* content, size_t size) {
  int total = 0;
  for (size_t i = 0; i < size; i++) total += strlen(table[content[i]]);
  return total + 1;
}

/**
 * @brief Codifica o conteúdo original usando a tabela de Huffman
 */
char* encode(char** table, unsigned char* content, size_t size) {
  char* codigo = calloc(size_str(table, content, size), sizeof(char));
  for (size_t i = 0; i < size; i++) strcat(codigo, table[content[i]]);
  return codigo;
}

/**
 * @brief Verifica se um determinado bit é 1 em um byte
 */
unsigned int is_bit_one(unsigned char byte, int i) {
  unsigned char mask = 1 << i;
  return byte & mask;
}

/**
 * @brief Gera a sequência binária da árvore de Huffman (pré-ordem)
 */
void build_tree_buffer(Node* root, char* buffer, int* index) {
  if (!root->left && !root->right) {
    buffer[(*index)++] = '1';
    unsigned char c = *(unsigned char*)root->c;
    for (int i = 7; i >= 0; i--)
      buffer[(*index)++] = (c & (1 << i)) ? '1' : '0';
  } else {
    buffer[(*index)++] = '0';
    build_tree_buffer(root->left, buffer, index);
    build_tree_buffer(root->right, buffer, index);
  }
}

/**
 * @brief Escreve uma sequência de bits como bytes em arquivo
 */
void write_bit_str(FILE* file, char* bit_str, int bit_qnt) {
  int i = 0, j = 7;
  unsigned char byte = 0;
  while (i < bit_qnt) {
    if (bit_str[i] == '1') byte |= (1 << j);
    j--;
    if (j < 0) {
      fwrite(&byte, sizeof(unsigned char), 1, file);
      byte = 0;
      j = 7;
    }
    i++;
  }
  if (j != 7) fwrite(&byte, sizeof(unsigned char), 1, file);
}

/**
 * @brief Monta e grava o cabeçalho do arquivo .huff
 */
void mount_header(FILE* file, unsigned char str[], Node* root) {
  unsigned int trash_size = strlen((char*)str) % 8;
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

/**
 * @brief Comprime o conteúdo e escreve o arquivo .huff
 */
void compress(unsigned char str[], char file_name[], Node* root) {
  FILE* huff_file = fopen(file_name, "wb");
  if (!huff_file) {
    printf(RED "Erro: falha ao abrir arquivo em compress()\n" RESET);
    return;
  }
  mount_header(huff_file, str, root);
  int i = 0, j = 7;
  unsigned char byte = 0;
  while (str[i] != '\0') {
    if (str[i] == '1') byte |= (1 << j);
    j--;
    if (j < 0) {
      fwrite(&byte, sizeof(unsigned char), 1, huff_file);
      byte = 0;
      j = 7;
    }
    i++;
  }
  if (j != 7) fwrite(&byte, sizeof(unsigned char), 1, huff_file);
  fclose(huff_file);
}

/**
 * @brief Reconstrói a árvore a partir dos bits do cabeçalho
 */
Node* build_tree_from_bits(unsigned char* buffer, int* index, int total_bits) {
  if (*index >= total_bits) return NULL;
  if (buffer[*index] == '1') {
    (*index)++;
    unsigned char c = 0;
    for (int i = 0; i < 8 && *index < total_bits; i++, (*index)++) {
      c <<= 1;
      if (buffer[*index] == '1') c |= 1;
    }
    Node* leaf = malloc(sizeof(Node));
    leaf->c = malloc(sizeof(unsigned char));
    *(unsigned char*)leaf->c = c;
    leaf->freq = malloc(sizeof(unsigned int));
    *(unsigned int*)leaf->freq = 0;
    leaf->left = leaf->right = leaf->next = NULL;
    return leaf;
  } else if (buffer[*index] == '0') {
    (*index)++;
    Node* node = malloc(sizeof(Node));
    node->c = malloc(sizeof(unsigned char));
    *(unsigned char*)node->c = '*';
    node->freq = malloc(sizeof(unsigned int));
    *(unsigned int*)node->freq = 0;
    node->next = NULL;
    node->left = build_tree_from_bits(buffer, index, total_bits);
    node->right = build_tree_from_bits(buffer, index, total_bits);
    return node;
  }
  return NULL;
}

/**
 * @brief Lê o cabeçalho do arquivo .huff e reconstrói a árvore
 */
Node* build_tree_from_header(FILE* file) {
  unsigned char byte1, byte2;
  fread(&byte1, sizeof(unsigned char), 1, file);
  fread(&byte2, sizeof(unsigned char), 1, file);

  unsigned int trash_size = byte1 >> 5;
  unsigned int tree_size = ((byte1 & 31) << 8) | byte2;

  unsigned char* tree_bytes = malloc(tree_size);
  fread(tree_bytes, sizeof(unsigned char), tree_size, file);

  unsigned char* tree_bits = malloc(tree_size * 8 + 1);
  int index = 0;
  for (unsigned int i = 0; i < tree_size; i++)
    for (int j = 7; j >= 0; j--)
      tree_bits[index++] = (is_bit_one(tree_bytes[i], j)) ? '1' : '0';
  tree_bits[index] = '\0';

  int bit_index = 0;
  Node* root = build_tree_from_bits(tree_bits, &bit_index, index);

  free(tree_bytes);
  free(tree_bits);
  (void)trash_size;
  return root;
}

/**
 * @brief Gera o nome do arquivo comprimido (.huff)
 */
char* file_with_extra_huff(char* file_name) {
  char* new_name = malloc(MAX_FILE_NAME_SIZE);
  snprintf(new_name, MAX_FILE_NAME_SIZE, "%s.huff", file_name);
  return new_name;
}

/**
 * @brief Remove a extensão .huff do nome do arquivo
 */
char* remove_huff_extension(const char* file_name) {
  char* new_name = strdup(file_name);
  char* ext = strstr(new_name, ".huff");
  if (ext) *ext = '\0';
  return new_name;
}

/**
 * @brief Extrai um arquivo comprimido (.huff) para o original
 */
void extract(char huff_name[]) {
  FILE* file = fopen(huff_name, "rb");
  if (!file) {
    printf(RED "Erro: falha ao abrir arquivo em extract()\n" RESET);
    return;
  }

  char* file_out = remove_huff_extension(huff_name);
  FILE* extracted = fopen(file_out, "wb");
  Node* root = build_tree_from_header(file);

  unsigned char byte = 0;
  Node* aux = root;
  while (fread(&byte, sizeof(unsigned char), 1, file)) {
    for (int i = 7; i >= 0; i--) {
      aux = is_bit_one(byte, i) ? aux->right : aux->left;
      if (!aux->left && !aux->right) {
        fwrite((char*)aux->c, sizeof(char), 1, extracted);
        aux = root;
      }
    }
  }

  printf(GREEN "\n✔ Arquivo extraído: %s\n" RESET, file_out);

  free_tree(root);
  free(file_out);
  fclose(extracted);
  fclose(file);
}

/**
 * @brief Função principal do programa (menu interativo)
 */
int main() {
  setlocale(LC_ALL, "C");
  clear_screen();

  printf(BOLD CYAN "=== COMPRESSOR HUFFMAN ===\n" RESET);
  printf(YELLOW "1" RESET " - Comprimir arquivo\n");
  printf(YELLOW "2" RESET " - Extrair arquivo (.huff)\n");
  printf(MAGENTA "Escolha uma opcao: " RESET);

  int opcao;
  scanf("%d", &opcao);
  getchar();

  char file_name[MAX_FILE_NAME_SIZE];

  if (opcao == 1) {
    printf(MAGENTA "\nCaminho do arquivo a ser comprimido: " RESET);
    scanf("%s", file_name);

    FILE* file = fopen(file_name, "rb");
    if (!file) {
      printf(RED "Erro: falha ao abrir o arquivo.\n" RESET);
      return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* content = calloc(file_size, sizeof(unsigned char));
    fread(content, sizeof(unsigned char), file_size, file);
    fclose(file);

    unsigned int freq[FREQ_SIZE] = {0};
    get_freq(content, freq, file_size);

    List list;
    create_list(&list);
    fill_list(freq, &list);

    Node* root = build_tree(&list);
    int columns = tree_height(root) + 2;

    char** table = allocate_table(columns);
    char path[columns];
    strcpy(path, "");
    build_table(table, root, path, columns);

    char* encoded = encode(table, content, file_size);
    char* compressed_name = file_with_extra_huff(file_name);

    printf(CYAN "\n→ Comprimindo arquivo...\n" RESET);
    compress((unsigned char*)encoded, compressed_name, root);
    printf(GREEN "✔ Arquivo comprimido: %s\n" RESET, compressed_name);

    free_tree(root);
    for (int i = 0; i < FREQ_SIZE; i++) free(table[i]);
    free(table);
    free(content);
    free(encoded);
    free(compressed_name);

  } else if (opcao == 2) {
    printf(MAGENTA "\nCaminho do arquivo .huff a ser extraido: " RESET);
    scanf("%s", file_name);
    printf(CYAN "\n→ Extraindo arquivo...\n" RESET);
    extract(file_name);
  } else {
    printf(RED "Opcao invalida.\n" RESET);
  }

  printf(YELLOW "\nPrograma finalizado.\n" RESET);
  return 0;
}