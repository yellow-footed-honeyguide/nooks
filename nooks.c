#include <stdio.h>  // printf (function), fprintf (function), stderr (macro)

#include "handle_arguments.h"  // handle_arguments (function)

int main(int argc, char *argv[]) {  // Main function, entry point of the program
    handle_arguments(argc, argv);   // Process command-line arguments
    return 0;                       // Exit the program successfully
}
