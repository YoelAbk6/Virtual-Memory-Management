/*The program is a simulation of memory management
 * YoelABK6
 * 
 */
#include <iostream>
#include "sim_mem.h"

char main_memory[MEMORY_SIZE];

int main()
{
    char val;
    
    //Full path to 'exec_file.txt' file
    char execPath [100] = "/home/exec_file.txt";
    //Full path to 'swap_file.txt' file
    char swapPath [100] = "/home/swap_file.txt";
    sim_mem mem_sm(execPath, swapPath ,25, 50, 25,25, 25, 5);

    mem_sm.store (25, 'h');
    val = mem_sm.load(25);
    std::cout << val ;
    val = mem_sm.load(24);
    std::cout << val ;
    mem_sm.store (113, 'l');
    val = mem_sm.load(113);
    std::cout << val ;
    val = mem_sm.load(55);
    std::cout << val ;
    mem_sm.store (77, 'o');
    val = mem_sm.load(77);
    std::cout << val ;

    mem_sm.store (124, ' ');
    val = mem_sm.load(124);
    std::cout << val ;

    mem_sm.store (85, 'b');
    val = mem_sm.load(85);
    std::cout << val ;
    mem_sm.store (100, 'o');
    val = mem_sm.load(100);
    std::cout << val ;
    mem_sm.store (75, 'd');
    val = mem_sm.load(75);
    std::cout << val ;
    val = mem_sm.load(24);
    std::cout << val ;
    mem_sm.store (99, 'k');
    val = mem_sm.load(99);
    std::cout << val ;
    mem_sm.store (75, 'e');
    val = mem_sm.load(75);
    std::cout << val ;
    mem_sm.store (104, 't');
    val = mem_sm.load(104);
    std::cout << val ;
    mem_sm.store (52, '!');
    val = mem_sm.load(52);
    std::cout << val <<'\n';


    mem_sm.print_page_table();
    mem_sm.print_memory();
    mem_sm.print_swap();

//    mem_sm.load(4);
//    mem_sm.load(9);
//    mem_sm.load(14);
//    mem_sm.load(19);
//    mem_sm.load(24);
//    //Text end
//    mem_sm.store(29, 'B');
//    mem_sm.store(34, 'C');
//    mem_sm.store(39, 'D');
//    mem_sm.store(44, 'E');
//    mem_sm.store(49, 'F');
//    //MM full from now on
//    mem_sm.store(54, 'G');
//    mem_sm.store(59, 'H');
//    mem_sm.store(64, 'R');
//    mem_sm.store(69, 'S');
//    mem_sm.store(74, 'T');
//    //Data end
//    //From here on, should enter swap
//    mem_sm.store(79, 'I');
//    mem_sm.store(84,'J');
//    mem_sm.store(89, 'K');
//    mem_sm.store(94, 'L');
//    mem_sm.store(99, 'M');
//    //Bss end
//    mem_sm.store(104, 'N');
//    mem_sm.store(109, 'O');
//    mem_sm.store(114, 'P');
//    mem_sm.store(119, 'Q');
//    mem_sm.store(124, 'R');
//    //Heap and stack end
//
//    mem_sm.load(4);
//    mem_sm.load(9);
//    mem_sm.load(14);
//    mem_sm.load(19);
//    mem_sm.load(24);
//    mem_sm.load(29);
//    mem_sm.load(34);
//    mem_sm.load(39);
//    mem_sm.load(44);
//    mem_sm.load(49);
//    mem_sm.load(54);
//    mem_sm.load(4);
//    mem_sm.load(9);
//    mem_sm.load(14);
//    mem_sm.load(19);
//    mem_sm.load(24);
//    mem_sm.load(114);
//    mem_sm.print_page_table();
//    mem_sm.print_memory();
//    mem_sm.print_swap();



}
