#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include "hash.h"

#define PATH_MAX 1024

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: ./child_process <blocks_folder> <hashes_folder> <N> <child_id>\n");
        return 1;
    }
    pid_t pid;
    int parent_id = atoi(argv[4]); // arg[4] is the parents_id
    int n = atoi(argv[3]); // arg[3] is the value of n
    int i;
    int child_id;

    if(parent_id >= n - 1){ // leaf node
        char block_file_name[PATH_MAX];
        char hash_file_name[PATH_MAX];
        char result[32*2+1]; //the size from hash.h in /include
        
        // get the filename of the corresponding block of the leaf node
        snprintf(block_file_name, sizeof(block_file_name), "%s/%d.txt", argv[1], parent_id-(n-1)); //argv[1] is the blocks folder
        
        // Hash the data from the corresponding block file of the leaf (using the id)
        hash_data_block(result, block_file_name);
        
        // Write the computed hash to the appropriate output file.
        snprintf(hash_file_name, sizeof(hash_file_name), "%s/%d.out", argv[2], parent_id); //argv[2] is the hash folder
        FILE *hash_file = fopen(hash_file_name, "w");
        if (!hash_file) {
            exit(-1);
        }
        fwrite(result, 1, strlen(result), hash_file);
        fclose(hash_file);
        
        exit(0); // making sure the computer doesn't feel pain...
    }

    else{ // parent node
        for(i = 0; i < 2; i++){
            pid = fork();
            if(pid == 0){
                break;
            }
        }
        if(pid > 0){ // parent waits for both children
            while(wait(NULL) > 0);
        }
        else{ // children call exec
            if(i == 0){ // left child calculates its id
                child_id = parent_id * 2 + 1;
            }
            else{ // right child calculates its id
                child_id = parent_id * 2 + 2;
            }
            char* childArgs[6]; // args to be sent to exec
            for(int j = 0; j < argc; j++){ // for loop initializing childArgs
                childArgs[j] = argv[j];
            }
            childArgs[5] = NULL;
            char childChar[4];
            sprintf(childChar, "%d", child_id);
            childArgs[4] = childChar; // puts child id into childArgs
            if(execv("./child_process", childArgs) == -1){
                exit(-1);
            }
        }

        // create char array for the file name
        char hash_file_name[PATH_MAX];
        char *hash_folder = argv[2];
        // uses snprintf to create file name
        snprintf(hash_file_name, sizeof(hash_file_name), "%s/%d.out", hash_folder, parent_id);
        // opens created hash file
        FILE *hash_file = fopen(hash_file_name, "w");
        if (!hash_file) {
            exit(-1);
        }
        // find left child id
        child_id = parent_id * 2 + 1;
        char child1_hash_file_name[PATH_MAX];
        // find and open childs hash file
        snprintf(child1_hash_file_name, sizeof(child1_hash_file_name), "%s/%d.out", hash_folder, child_id);
        FILE *child1_hash_file = fopen(child1_hash_file_name, "r");
        if (!child1_hash_file) {
            exit(-1);
        }
        // read in data from childs hash file into child_hash
        char child1_hash[128];
        fread(child1_hash, 1, sizeof(child1_hash), child1_hash_file);
        
        // find right child id
        child_id = parent_id * 2 + 2;
        char child2_hash_file_name[PATH_MAX];
        // find and open childs hash file
        snprintf(child2_hash_file_name, sizeof(child2_hash_file_name), "%s/%d.out", hash_folder, child_id);
        FILE *child2_hash_file = fopen(child2_hash_file_name, "r");
        if (!child2_hash_file) {
            exit(-1);
        }
        // read in data from childs hash file into child_hash2
        char child2_hash[128];
        fread(child2_hash, 1, sizeof(child2_hash), child2_hash_file);
        //use compute_dual_hash to combine the childrens hashes and return them in result
        char result[128];
        compute_dual_hash(result, child1_hash, child2_hash);
        // write to parents hash file
        fwrite(result, 1, strlen(result), hash_file);

        fclose(child1_hash_file);
        fclose(child2_hash_file);
        fclose(hash_file);
        exit(0);
    }
    // TODO: If the current process is a leaf process, read in the associated block file 
    // and compute the hash of the block.

    // TODO: If the current process is not a leaf process, spawn two child processes using  
    // exec() and ./child_process. 

    // TODO: Wait for the two child processes to finish

    // TODO: Retrieve the two hashes from the two child processes from output/hashes/
    // and compute and output the hash of the concatenation of the two hashes.
    
}

