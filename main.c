#include "work.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

//Size of large/huge page
#define PAGE_SIZE (2 * 1024 * 1024)
//Number of large/huge pages to allocate 
#define NUM_OF_PAGES 8 

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: main <last 5 digits of your reg. no>\n");
        return EXIT_FAILURE;
    }

    work_init(atoi(argv[1]));

    FILE *address_file = fopen("largepages.txt", "r");
    if (address_file == NULL) {
        perror("Error opening largepages.txt");
        return EXIT_FAILURE;
    }

    uintptr_t address[NUM_OF_PAGES];

    //Read addresses from the file
    for (int i = 0; i < NUM_OF_PAGES; i++) {
        if (fscanf(address_file, "%lu", (unsigned long *)&address[i]) != 1) {
            fprintf(stderr, "Error reading address %d from largepages.txt\n", i);
            fclose(address_file);
            return EXIT_FAILURE;
        }
    }
    fclose(address_file);

    //Allocate large/huge pages at the addresses extracted from largepages.txt
    for (int i = 0; i < NUM_OF_PAGES; i++) {
        void *mapping_address = (void *)(uintptr_t)address[i];
        void *mapping_start_address = mmap(mapping_address, PAGE_SIZE, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
        if (mapping_start_address == MAP_FAILED) {
            perror("Error allocating large/huge pages");
            return EXIT_FAILURE;
        }

        //Suggest the kernel to use huge pages
        if (madvise(mapping_start_address, PAGE_SIZE, MADV_HUGEPAGE) != 0) {
            perror("madvise");
            munmap(mapping_start_address, PAGE_SIZE); 
            return EXIT_FAILURE;
        }
    }

    if (work_run() == 0) {
        printf("Work completed successfully\n");
    }

    return 0;
}
