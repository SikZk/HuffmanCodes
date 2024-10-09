#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRED_ARGUMENTS_NUMBER 3
#define ASCII_TABLE_SIZE 256


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

int *frequency_table;
FILE *input_file;
FILE *output_file;

void print_help_message() {
    fprintf(stderr, "To properly use this executable you should call:\n./executable <encode/decode> <InputFile> <OutputFile>\n");
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
    return node;
}
huffman_node peek(priority_queue *queue) {
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
int validate_huff_file(const char *file_path) {

    const char *dot = strrchr(file_path, '.');
    if(!dot || dot == file_path) {
        return 1;
    }
    if (strcmp(dot, ".huff") != 0) {
        return 1;
    }

    output_file = fopen(file_path, "w");
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
void create_huffman_tree(priority_queue *queue) {
    while (queue->size > 1) {
        huffman_node lowest = dequeue(queue);
        huffman_node second_lowest = dequeue(queue);
        huffman_node new_node = {'\0', lowest.frequency + second_lowest.frequency, &lowest, &second_lowest};
        enqueue(queue, new_node);
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

    create_huffman_tree(queue);
    huffman_node root = dequeue(queue);
    free(queue);



    fclose(input_file);
    fclose(output_file);
    return 0;
}




int main(const int argc, char *argv[]) {

    if(argc != REQUIRED_ARGUMENTS_NUMBER+1) {
        print_help_message();
        return 1;
    }

    if(strcmp(argv[1], "encode") == 0) {

        if(validate_txt_file(argv[2]) != 0) {
            fprintf(stderr, "The file extension is invalid, or the file does not exist, you need to add .txt file\n");
            return 1;
        }
        if(validate_huff_file(argv[3]) != 0) {
            fprintf(stderr, "The output file is not a valid .huff file\n");
            return 1;
        }
        if(huffman_encode(input_file, output_file) != 0) {
            fprintf(stderr, "Error encoding the file\n");
            return 1;
        }



    } else if(strcmp(argv[1], "decode") == 0) {

        if(validate_huff_file(argv[2]) != 0) {
            fprintf(stderr, "The output file is not a valid .huff file\n");
            return 1;
        }

        if(validate_txt_file(argv[3]) != 0) {
            fprintf(stderr, "The file extension is invalid, or the file does not exist, you need to add .txt file\n");
            return 1;
        }

    } else {
        fprintf(stderr, "The first argument you specify must be either \"encode\", or \"decode\"\n");
        return 1;
    }

    return 0;

}