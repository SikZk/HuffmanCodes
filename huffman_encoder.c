#include <stdio.h>
#include <string.h>

#define REQUIRED_ARGUMENTS_NUMBER 3
#define ASCII_TABLE_SIZE 256

struct huffman_node {
    char character;
    int frequency;
    struct huffman_node *left;
    struct huffman_node *right;
};


int frequency_table[ASCII_TABLE_SIZE];
FILE *input_file;
FILE *output_file;

void print_help_message() {
    fprintf(stderr, "To properly use this executable you should call:\n./executable <encode/decode> <InputFile> <OutputFile>\n");
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

int huffman_encode(FILE *input_file, FILE *output_file) {


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