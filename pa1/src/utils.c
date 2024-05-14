#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create N files and distribute the data from the input file evenly among them
// See section 3.1 of the project writeup for important implementation details
void partition_file_data(char *input_file, int n, char *blocks_folder) {
    // Hint: Use fseek() and ftell() to determine the size of the file
    FILE *input = fopen(input_file, "r");
    if (!input) {
        exit(-1);
    }
    // Get the file size
    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    // Move the file pointer back to the beginning
    fseek(input, 0, SEEK_SET);

    //size of each of the blocks
    long block_size = file_size / n;
    char *buffer = (char *) malloc(block_size * sizeof(char));
    if (!buffer) {
        fclose(input);
        exit(-1);
    }

    // Write data to files
    for (int i = 0; i < n - 1; i++) {
        // create char array for the file name (handled with snprintf below)
        char block_file_name[32];
        //using snprintf so we don't have to cast anything, also more secure
        snprintf(block_file_name, sizeof(block_file_name), "%s/%d.txt", blocks_folder, i);

        FILE *block_file = fopen(block_file_name, "wb");
        if (!block_file) {
            free(buffer);
            fclose(input);
            exit(-1);
        }

        fread(buffer, 1, block_size, input);
        fwrite(buffer, 1, block_size, block_file);
        fclose(block_file);
    }

    // Handle the Nth file
    long last_block_size = block_size + file_size % n;
    //use the previously allocated space since we're not using it anymore
    char *last_buffer = (char *) realloc(buffer, last_block_size);
    if (!last_buffer) {
        free(buffer);
        fclose(input);
        exit(-1);
    }
    // do the same thing in the for loop above for the last block
    char last_block_file_name[32];
    snprintf(last_block_file_name, sizeof(last_block_file_name), "%s/%d.txt", blocks_folder, n - 1);

    FILE *last_block_file = fopen(last_block_file_name, "wb");
    if (!last_block_file) {
        free(last_buffer);
        fclose(input);
        exit(-1);
    }

    fread(last_buffer, 1, last_block_size, input);
    fwrite(last_buffer, 1, last_block_size, last_block_file);
    
    // Clean up :)
    free(last_buffer);
    fclose(last_block_file);
    fclose(input);
}



// ##### DO NOT MODIFY THIS FUNCTION #####
void setup_output_directory(char *block_folder, char *hash_folder) {
    // Remove output directory
    pid_t pid = fork();
    if (pid == 0) {
        char *argv[] = {"rm", "-rf", "./output/", NULL};
        if (execvp(*argv, argv) < 0) {
            printf("ERROR: exec failed\n");
            exit(1);
        }
        exit(0);
    } else {
        wait(NULL);
    }

    sleep(1);

    // Creating output directory
    if (mkdir("output", 0777) < 0) {
        printf("ERROR: mkdir output failed\n");
        exit(1);
    }

    sleep(1);

    // Creating blocks directory
    if (mkdir(block_folder, 0777) < 0) {
        printf("ERROR: mkdir output/blocks failed\n");
        exit(1);
    }

    // Creating hashes directory
    if (mkdir(hash_folder, 0777) < 0) {
        printf("ERROR: mkdir output/hashes failed\n");
        exit(1);
    }
}
