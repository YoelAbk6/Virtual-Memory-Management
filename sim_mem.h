/*The program is a simulation of memory management
 * YoelAbk6
 */
#ifndef EX6_SIM_MEM_H
#define EX6_SIM_MEM_H
#define MEMORY_SIZE 50
extern char main_memory[MEMORY_SIZE];

typedef struct page_descriptor {
    int V; // valid
    int D; // dirty
    int P; // permission
    int frame; //the number of a frame if in case it is page-mapped
    int swap_index; // where the page is located in the swap file.
}
        page_descriptor;

//A doubly linked list for the LRU implementation
typedef struct DoublyLinkedListNode {
    int frameNumber;
    int pageNumber;
    DoublyLinkedListNode * prev;
    DoublyLinkedListNode * next;
}
        node;

class sim_mem {
    int swapfile_fd; //swap file fd
    int program_fd; //executable file fd
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    page_descriptor * page_table; //pointer to page table
    int * swapTable; //Available spots and what's already saved
    int * frameTable; //Available spots
    int swapSize; //Swap size in pages
    int numOfFrames; //Number of frames in the MM
    node * head, * tail; //Dummy head and tail of the recent use list

public:
    sim_mem(char exe_file_name[], char swap_file_name[], int text_size,
            int data_size, int bss_size, int heap_stack_size,
            int num_of_pages, int page_size);

    ~sim_mem();

    char load(int address);

    void store(int address, char value);

    void print_memory();

    void print_swap();

    void print_page_table();

private:
    int findPageNumber(int address) const;

    int pageIsIn(int pageNumber);

    static int findAvailableFrame(const int * table, int tableSize);

    int copyPageToMM(int from, int offset, int frame) const;

    int clearPage(int pageNumber);

    int swapFrame();

    void moveToTail(node * curr);

    void updateLRU(int frameNumber, int pageNumber);

    int caseSwap(int pageNumber, int frame);
};
#endif //EX6_SIM_MEM_H
