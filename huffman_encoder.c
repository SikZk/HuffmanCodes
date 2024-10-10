#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRED_ARGUMENTS_NUMBER 3
#define ASCII_TABLE_SIZE 256
#define METADATA_MARKER "---METADATA_SECTION---"


typedef struct huffman_node {
    char character;
    int frequency;
    struct huffman_node *left;
    struct huffman_node *right;
} huffman_node;

typedef struct {
    huffman_node items[ASCII_TABLE_SIZE];
    int size;
} priority_queue;

typedef struct {
    char code[ASCII_TABLE_SIZE];
    char character;
} character_code;

typedef struct data_item {
    char data;
    char key[ASCII_TABLE_SIZE];
} data_item;

int *frequency_table;
FILE *input_file;
FILE *output_file;
data_item *hash_array[ASCII_TABLE_SIZE];
data_item *dummy_item;
data_item *item;

int hash_code(char code[ASCII_TABLE_SIZE]) {
    unsigned int hash = 0;
    for (int i = 0; code[i] != '\0'; i++) {
        hash = (hash * 31 + code[i]) % ASCII_TABLE_SIZE;
    }
    return hash;
}
data_item *search(char key[ASCII_TABLE_SIZE]) {
    int hash_index = hash_code(key);
    int original_index = hash_index;
    while (hash_array[hash_index] != NULL) {
        if (strcmp(hash_array[hash_index]->key, key) == 0) {
            return hash_array[hash_index];
        }
        ++hash_index;
        hash_index %= ASCII_TABLE_SIZE;
        if (hash_index == original_index) {
            break;
        }
    }
    return NULL;
}

void insert_to_hash_table(char key[ASCII_TABLE_SIZE], char data) {
    data_item *item = malloc(sizeof(data_item));
    item->data = data;
    strcpy(item->key, key);

    int hash_index = hash_code(key);
    int original_index = hash_index;
    while (hash_array[hash_index] != NULL) {
        ++hash_index;
        hash_index %= ASCII_TABLE_SIZE;
        if (hash_index == original_index) {
            fprintf(stderr, "Hash table is full\n");
            free(item);
            return;
        }
    }
    hash_array[hash_index] = item;
}

data_item *delete_to_hash_table(data_item *item) {
    char *key = item->key;
    int hash_index = hash_code(key);

    while(hash_array[hash_index] != NULL) {
        if(hash_array[hash_index]->key == key) {
            data_item *temp = hash_array[hash_index];
            hash_array[hash_index] = dummy_item;
            return temp;
        }
        ++hash_index;
        hash_index %= ASCII_TABLE_SIZE;
    }
    return NULL;
}


void print_help_message() {
    fprintf(stderr, "To properly use this executable you should call:\n./executable <encode/decode> <InputFile> <OutputFile>\n");
}
void free_huffman_tree(huffman_node *node) {
    if (node == NULL) {
        return;
    }

    free_huffman_tree(node->left);
    free_huffman_tree(node->right);

    free(node);
}

void swap_nodes(huffman_node *a, huffman_node *b) {
    huffman_node temp = *a;
    *a = *b;
    *b = temp;
}
void heapify_up(priority_queue *queue, int index) {
    if (index != 0 && queue->items[(index - 1) / 2].frequency > queue->items[index].frequency) {
        swap_nodes(&queue->items[(index - 1) / 2], &queue->items[index]);
        heapify_up(queue, (index -1) / 2);
    }
}
void heapify_down(priority_queue *queue, int index) {
    int current_index = index;
    const int left_child_index = 2 * index + 1;
    const int right_child_index = 2 * index + 2;

    if(left_child_index < queue->size && queue->items[left_child_index].frequency < queue->items[current_index].frequency) {
        current_index = left_child_index;
    }
    if(right_child_index < queue->size && queue->items[right_child_index].frequency < queue->items[current_index].frequency) {
        current_index = right_child_index;
    }
    if(current_index != index) {
        swap_nodes(&queue->items[index], &queue->items[current_index]);
        heapify_down(queue, current_index);
    }
}
int enqueue(priority_queue *queue, huffman_node node) {
    if (queue->size == ASCII_TABLE_SIZE) {
        return 1;
    }
    queue->items[queue->size++] = node;
    heapify_up(queue, queue->size - 1);
    return 0;
}

huffman_node dequeue(priority_queue *queue) {
    if (!queue->size) {
        return (huffman_node) {0};
    }
    huffman_node node = queue->items[0];
    queue->items[0] = queue->items[--queue->size];
    heapify_down(queue, 0);

    queue->items[queue->size] = (huffman_node){0};

    return node;
}
huffman_node peek(const priority_queue *queue) {
    if (!queue->size) {
        return (huffman_node) {0};
    }
    return queue->items[0];
}

int validate_txt_file(const char *file_path) {

    const char *dot = strrchr(file_path, '.');
    if(!dot || dot == file_path) {
        return 1;
    }
    if (strcmp(dot, ".txt") != 0) {
        return 1;
    }

    input_file = fopen(file_path, "r");
    if(input_file == NULL) {
        perror("Error opening .txt file");
        return 1;
    }
    return 0;

}
int validate_huff_file(const char *file_path, int mode) {

    const char *dot = strrchr(file_path, '.');
    if(!dot || dot == file_path) {
        return 1;
    }
    if (strcmp(dot, ".huff") != 0) {
        return 1;
    }
    if (mode == 0) {
        output_file = fopen(file_path, "w");
    }
    else {
        output_file = fopen(file_path, "a+");
    }
    if(output_file == NULL) {
        perror("Error opening .huff file");
        return 1;
    }
    return 0;

}
int *create_frequency_table(FILE *input_file) {

    int *table = calloc(ASCII_TABLE_SIZE, sizeof(int));
    if (table == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }
    int ch;
    while ((ch = fgetc(input_file)) != EOF) {
        table[ch]++;
    }

    return table;
}
huffman_node* create_huffman_tree(priority_queue *queue) {
    while (queue->size > 1) {
        huffman_node *lowest = malloc(sizeof(huffman_node));
        *lowest = dequeue(queue);

        huffman_node *second_lowest = malloc(sizeof(huffman_node));
        *second_lowest = dequeue(queue);

        huffman_node *new_node = malloc(sizeof(huffman_node));
        new_node->character = '\0';
        new_node->frequency = lowest->frequency + second_lowest->frequency;
        new_node->left = lowest;
        new_node->right = second_lowest;

        enqueue(queue, *new_node);
    }

    huffman_node *root = malloc(sizeof(huffman_node));
    *root = dequeue(queue);
    return root;
}
void create_code_table(const huffman_node *node, char code_table[][ASCII_TABLE_SIZE], char *current_code, int depth) {
    if (node->character != '\0') {
        current_code[depth] = '\0';
        strcpy(code_table[(unsigned char) node->character], current_code);
        return;
    }

    if (node->left != NULL) {
        current_code[depth] = '0';
        create_code_table(node->left, code_table, current_code, depth + 1);
    }

    if (node->right != NULL) {
        current_code[depth] = '1';
        create_code_table(node->right, code_table, current_code, depth + 1);
    }
}
void write_encoded_file(char code_table[][ASCII_TABLE_SIZE], FILE *output_file, FILE *input_file) {
    rewind(input_file);

    int ch;
    while ((ch = fgetc(input_file)) != EOF) {
        char *code = code_table[(unsigned char)ch];
        fputs(code, output_file);
    }

    fprintf(output_file,"\n%s\n", METADATA_MARKER);

    for (int i = 0; i < ASCII_TABLE_SIZE; i++) {
        if (code_table[i][0] != '\0') {
            fprintf(output_file, "%c:%s\n", (char) i, code_table[i]);
        }
    }
}
int huffman_encode(FILE *input_file, FILE *output_file) {
    priority_queue *queue = malloc(sizeof(priority_queue));
    if (queue == NULL) {
        return 1;
    }

    frequency_table = create_frequency_table(input_file);
    if (frequency_table == NULL) {
        return 1;
    }

    for(int i = 0; i<ASCII_TABLE_SIZE; i++) {
        if(frequency_table[i] > 0) {
            huffman_node node = {i, frequency_table[i], NULL, NULL};
            enqueue(queue, node);
        }
    }
    free(frequency_table);

    huffman_node *root = create_huffman_tree(queue);
    free(queue);

    char code_table[ASCII_TABLE_SIZE][ASCII_TABLE_SIZE] = {{0}};
    char current_code[ASCII_TABLE_SIZE];
    create_code_table(root, code_table, current_code,0);
    free_huffman_tree(root);

    write_encoded_file(code_table, output_file, input_file);


    fclose(input_file);
    fclose(output_file);
    return 0;
}
int huffman_decode(FILE *huffman_file) {

    fseek(huffman_file, 0, SEEK_END);
    long file_size = ftell(huffman_file);
    rewind(huffman_file);

    char *file_buffer = malloc(file_size + 1);
    if (file_buffer == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    size_t bytes_read = fread(file_buffer, 1, file_size, huffman_file);
    file_buffer[bytes_read] = '\0';

    char *metadata_marker = strstr(file_buffer, "\n" METADATA_MARKER "\n");
    if (metadata_marker == NULL) {
        fprintf(stderr, "Metadata marker not found in the file\n");
        free(file_buffer);
        return 1;
    }

    long encoded_data_length = metadata_marker - file_buffer;
    char *encoded_data = file_buffer;

    char *metadata_section = metadata_marker + strlen("\n" METADATA_MARKER "\n");
    char *line = strtok(metadata_section, "\n");
    while (line != NULL) {
        char *colon = strchr(line, ':');
        if (colon != NULL) {
            char character = line[0];
            char *code = colon + 1;
            insert_to_hash_table(code, character);
        }
        line = strtok(NULL, "\n");
    }
    FILE *output = fopen("decoded.txt", "w");
    char bit_buffer[ASCII_TABLE_SIZE] = {0};
    int bit_buffer_index = 0;
    for (long i = 0; i < encoded_data_length; i++) {
        char bit_char = encoded_data[i];
        if (bit_char != '0' && bit_char != '1') {
            continue;
        }
        bit_buffer[bit_buffer_index++] = bit_char;
        bit_buffer[bit_buffer_index] = '\0';

        data_item *item = search(bit_buffer);
        if (item != NULL) {
            fprintf(output,"%c", item->data);

            bit_buffer_index = 0;
            bit_buffer[0] = '\0';
        }
    }
    printf("\n");

    free(file_buffer);
    return 0;
}



int main(const int argc, char *argv[]) {

    if(argc != REQUIRED_ARGUMENTS_NUMBER+1 && argc != REQUIRED_ARGUMENTS_NUMBER ) {
        print_help_message();
        return 1;
    }

    if(strcmp(argv[1], "encode") == 0) {

        if(validate_txt_file(argv[2]) != 0) {
            fprintf(stderr, "The file extension is invalid, or the file does not exist, you need to add .txt file\n");
            return 1;
        }
        if(validate_huff_file(argv[3],0) != 0) {
            fprintf(stderr, "The output file is not a valid .huff file\n");
            return 1;
        }
        if(huffman_encode(input_file, output_file) != 0) {
            fprintf(stderr, "Error encoding the file\n");
            return 1;
        }



    } else if(strcmp(argv[1], "decode") == 0) {

        if(validate_huff_file(argv[2], 1) != 0) {
            fprintf(stderr, "The output file is not a valid .huff file\n");
            return 1;
        }
        if(huffman_decode(output_file) != 0) {
            fprintf(stderr, "Error decodeing the file\n");
            return 1;
        }

    } else {
        fprintf(stderr, "The first argument you specify must be either \"encode\", or \"decode\"\n");
        return 1;
    }

    return 0;

}