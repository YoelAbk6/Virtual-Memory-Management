# Virtual-Memory-Management

==Description==

The program is a simulation of a processor approach to the memory. We use the "paging mechanism" that allows running a program when only the relevant parts of it are in the memory. The virtual memory is divided into pages that are brought to the main memory by need.

When there is no more room in the MM, we store dirty pages (pages that have been modified) in a swap file. The page that we choose to swap is chosen by an LRU algorithm (least recently used). The LRU algorithm is implemented by a doubly-linked list, where the node after the dummy tail is the MRU, and the node before the dummy head is the LRU.

The program has three main functions:

1. sim_mem - The constructor. Initialize the given logical memory sizes and the page table and creates the head and tail of the doubly linked list.
Also opens the execute file and creates and initializes the swap file.

2. Load - Receives a logical address, brings the right page to the MM if it's not already there, and returns the specified address value.

3. Store - Receives a logical address and a char value, brings the right page to the MM if it's not already there, and then writes the value to the specified address.

Some helper functions:

1. swapFrame - Chooses the page that will be swapped (The LRU) and store it in the Swap file, returns the newly available frame number.
2. updateLRU - Receives the current brought to the MM page and frame numbers, and updates its location in the LRU linked list.
3. pageIsIn - Receives a page number, figures out where this page is, and returns its location as an enum


==Program Files==

main.cpp - contains the main only
sim_mem.cpp- the file contain only functions
sim_mem.h- an header file ,contain structs, declarations of functions
README.txt

==How to compile?==

compile: g++ main.cpp sim_mem.h sim_mem.cpp -o ex6

run: ./ex6

==Input:==

no input

==Output:==

Writes pages to the Swap file

Chars that the load function returns
