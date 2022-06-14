/*The program is a simulation of memory management
 * YoelABk6
 */
#include "sim_mem.h"

#include <iostream>

#include <unistd.h>

#include <fcntl.h>

enum pageLocation {
    mainMem,
    readOnly,
    swap,
    bss,
    stackOrHeap,
    data
};

//sim_mem constructor
sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size, int data_size,
                 int bss_size, int heap_stack_size, int num_of_pages, int page_size) {
    this -> text_size = text_size;
    this -> data_size = data_size;
    this -> bss_size = bss_size;
    this -> heap_stack_size = heap_stack_size;
    this -> num_of_pages = num_of_pages;
    this -> page_size = page_size;

    numOfFrames = MEMORY_SIZE / page_size;
    int i;

    //Initialize the LRU list
    head = new node;
    tail = new node;
    head -> next = nullptr;
    head -> prev = tail;
    head -> frameNumber = -1;
    tail -> next = head;
    tail -> frameNumber = -1;

    this -> swapSize = num_of_pages - (text_size / page_size);
    this -> swapTable = new int[swapSize];
    for (i = 0; i < swapSize; i++)
        swapTable[i] = -1; //Available swap 'frame'

    frameTable = new int[numOfFrames];
    for (i = 0; i < numOfFrames; i++)
        frameTable[i] = -1; //Available MM frame

    program_fd = open(exe_file_name, O_RDONLY);
    if (program_fd == -1) {
        perror("Err: ");
        exit(1);
    }
    swapfile_fd = open(swap_file_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (swapfile_fd == -1) {
        perror("Err: ");
        exit(1);
    }

    for (i = 0; i < MEMORY_SIZE; i++)
        main_memory[i] = '0'; //Initialize the MM with zeros

    char zero = '0';
    for (i = 0; i < page_size * swapSize; i++) {
        if (write(swapfile_fd, & zero, 1) == -1) { //Initialize the swap with zeros
            perror("Err");
            exit(1);
        }
    }

    //Initialize the page table
    page_table = new page_descriptor[num_of_pages];
    for (i = 0; i < num_of_pages; i++) {
        page_table[i].V = 0;
        page_table[i].D = 0;
        if (i < text_size / page_size)
            page_table[i].P = 0; //Read
        else
            page_table[i].P = 1; //Read + write
        page_table[i].frame = -1;
        page_table[i].swap_index = -1;
    }
}

//sim_mem destructor
sim_mem::~sim_mem() {
    close(program_fd);
    close(swapfile_fd);
    delete(page_table);
    delete(swapTable);
    delete(frameTable);
    node * curr = tail;
    while (curr) {
        tail = tail -> next;
        delete(curr);
        curr = tail;
    }
}

//Receives a logical address, try to reach this address in the MM and return the right value if exists, returns '/0' on ERR
char sim_mem::load(int address) {
    int pageNumber = findPageNumber(address);
    if (pageNumber == -1)
        return '\0';
    int offset = address % this -> page_size, frame, pageLoc;
    pageLoc = pageIsIn(pageNumber);
    frame = findAvailableFrame(frameTable, numOfFrames);
    if (frame == -1 && pageLoc != mainMem) {
        frame = swapFrame();
        if (frame == -1)
            return '\0';
    }
    switch (pageLoc) {
        case mainMem:
            frame = page_table[pageNumber].frame;
            break;
        case readOnly:
            copyPageToMM(program_fd, pageNumber, frame);
            break;
        case swap:
            if (caseSwap(pageNumber, frame) == -1)
                return '\0';
            break;
        case bss:
            for (int i = 0; i < page_size; i++)
                main_memory[frame * page_size + i] = '0';
            break;
        case stackOrHeap:
            fprintf(stderr, "couldn't read a page that doesn't exists\n");
            return '\0';
        case data:
            copyPageToMM(program_fd, pageNumber, frame);
            break;
        default:
            return '\0';
    }
    page_table[pageNumber].V = 1;
    page_table[pageNumber].frame = frame;
    frameTable[frame] = pageNumber;
    updateLRU(frame, pageNumber);
    return main_memory[frame * page_size + offset];
}

//Receives a logical address and a value, and stores the value at the address
void sim_mem::store(int address, char value) {
    int pageNumber = findPageNumber(address);
    if (pageNumber == -1)
        return;
    int offset = address % this -> page_size, frame, pageLoc;
    pageLoc = pageIsIn(pageNumber);
    frame = findAvailableFrame(frameTable, numOfFrames);
    if (frame == -1 && pageLoc != mainMem) {
        frame = swapFrame();
        if (frame == -1)
            return;
    }
    switch (pageLoc) {
        case mainMem:
            frame = page_table[pageNumber].frame;
            break;
        case readOnly:
            fprintf(stderr, "pageNumber %d is meant for reading only\n", pageNumber);
            return;
        case swap:
            if (caseSwap(pageNumber, frame) == -1)
                return;
            break;
        case bss:
        case stackOrHeap:
            for (int i = 0; i < page_size; i++)
                main_memory[frame * page_size + i] = '0';
            break;
        case data:
            copyPageToMM(program_fd, pageNumber, frame);
            break;
        default:
            return;
    }
    main_memory[frame * page_size + offset] = value;
    page_table[pageNumber].V = 1;
    page_table[pageNumber].D = 1;
    page_table[pageNumber].frame = frame;
    frameTable[frame] = pageNumber;
    updateLRU(frame, pageNumber);
}

//Bring the page from the Swap in case its there and empties its place in the swap
int sim_mem::caseSwap(int pageNumber, int frame) {
    int status1 = copyPageToMM(swapfile_fd, page_table[pageNumber].swap_index, frame);
    int status2 = clearPage(pageNumber);
    if (status1 == -1 || status2 == -1)
        return -1;
    swapTable[page_table[pageNumber].swap_index] = -1;
    page_table[pageNumber].swap_index = -1;
    return 0;
}

//Chooses the page that will be swapped (the LRU), and makes the swap
int sim_mem::swapFrame() {
    int frameToSwap = head -> prev -> frameNumber;
    int pageToSwap = head -> prev -> pageNumber;
    if (page_table[pageToSwap].D != 0) {//If it's not a text page or a data page that has not been modified
        int availableSwap = findAvailableFrame(swapTable, swapSize);//Finds an available swap spot
        swapTable[availableSwap] = 1;
        page_table[pageToSwap].swap_index = availableSwap;
        char buffer[page_size];
        for (int i = 0; i < page_size; i++) {
            buffer[i] = main_memory[frameToSwap * page_size + i];
        }
        //Sets cursor at the right place
        if (lseek(swapfile_fd, availableSwap * page_size, SEEK_SET) < 0) {
            perror("Err:");
            return -1;
        }
        //Writes to the Swap
        if (write(swapfile_fd, buffer, page_size) == -1) {
            perror("Err:");
            return -1;
        }
    }
    page_table[pageToSwap].frame = -1;
    page_table[pageToSwap].V = 0;
    return frameToSwap;
}

//Updates the least recent used
void sim_mem::updateLRU(int frameNumber, int pageNumber) {
    node * curr = tail;
    while (curr) {
        if (curr -> frameNumber == frameNumber)
            break;
        else
            curr = curr -> next;
    }
    if (curr) { //If the frame is in the nodeList
        moveToTail(curr);
    } else {
        curr = new node;
        curr -> next = nullptr;
        curr -> prev = nullptr;
        moveToTail(curr);
    }
    curr -> pageNumber = pageNumber;
    curr -> frameNumber = frameNumber;
}

//Moves curr node to the tail of the linkedList
void sim_mem::moveToTail(node * curr) {
    if (curr -> prev != tail) { //If not already the MRU(Most recent used)
        if (curr -> prev) { //If an existing, detach it
            curr -> prev -> next = curr -> next;
            curr -> next -> prev = curr -> prev;
        }
        curr -> next = tail -> next;
        curr -> prev = tail;
        tail -> next = curr;
        curr -> next -> prev = curr;
    }
}

//Clears one page from the Swap
int sim_mem::clearPage(int pageNumber) {
    //Sets cursor at the right place
    if (lseek(swapfile_fd, page_table[pageNumber].swap_index * page_size, SEEK_SET) < 0) {
        perror("Err:2");
        return -1;
    }
    //Writes zeros into the emptied swap indexes
    char zero = '0';
    for (int i = 0; i < page_size; i++) {
        if (write(swapfile_fd, & zero, 1) < 0) {
            perror("Err");
            return -1;
        }
    }
    return 0;
}

//Copies one page - 'pageNumber' from - 'from' with a given offset into the main memory at location - 'frame'
int sim_mem::copyPageToMM(int from, int offset, int frame) const {
    //Sets cursor at the right place
    if (lseek(from, offset * page_size, SEEK_SET) < 0) {
        perror("Err4:");
        return -1;
    }

    //Reads page_size bytes to the MM
    if (read(from, & main_memory[frame * page_size], page_size) < 0) {
        perror("Err5");
        return -1;
    }
    return 0;
}

//Receives an array 'table' and its size and returns the index of the first not '-1' or -1 if there's non
int sim_mem::findAvailableFrame(const int * table, int tableSize) {
    for (int i = 0; i < tableSize; i++) {
        if (table[i] == -1)
            return i;
    }
    return -1;
}

//Receives a pageNumber number and returns the number representing the pageNumber location
int sim_mem::pageIsIn(int pageNumber) {
    if (page_table[pageNumber].V == 1) //The pageNumber is in the MM
        return mainMem;
    if (page_table[pageNumber].P == 0) //Read permission only
        return readOnly;
    if (page_table[pageNumber].D == 1) //Dirty pageNumber -> in Swap
        return swap;
    if (pageNumber >= (text_size + data_size) / page_size) //Bss, Stack or Heap
        return pageNumber < ((text_size + data_size + bss_size) / page_size) ? bss : stackOrHeap;
    return data; //Data
}

//Computes the page number and returns it if valid, print error and returns '-1' if not
int sim_mem::findPageNumber(int address) const {
    int page = address / this -> page_size;
    if (page >= this -> num_of_pages) {
        fprintf(stderr, "incorrect address, there's only %d pages\n", num_of_pages);
        return -1;
    }
    return page;
}

//Prints the MM
void sim_mem::print_memory() {
    int i;
    printf("\n Physical memory\n");
    for (i = 0; i < MEMORY_SIZE; i++) {
        printf("[%c]\n", main_memory[i]);
    }
}

//Prints the SWAP
void sim_mem::print_swap() {
    char * str = (char * )(malloc(this -> page_size * sizeof(char)));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while (read(swapfile_fd, str, this -> page_size) == this -> page_size) {
        for (i = 0; i < page_size; i++) {
            printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");
    }
    free(str);
}

//Prints the page table
void sim_mem::print_page_table() {
    int i;
    printf("\n page table \n");
    printf("Valid\t Dirty\t Permission \t Frame\t Swap index\n");
    for (i = 0; i < num_of_pages; i++) {
        printf("[%d]\t[%d]\t[%d]\t[%d]\t[%d]\n",
               page_table[i].V,
               page_table[i].D,
               page_table[i].P,
               page_table[i].frame,
               page_table[i].swap_index);
    }
}
