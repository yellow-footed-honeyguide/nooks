#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "handle_arguments.h"
#include "actions.h"

/**
 * Handles command line arguments using the getopt_long library.
 * Supports options to save the current directory, show all saved spots, or navigate to a specific directory.
 * 
 * @param argc The argument count.
 * @param argv The argument vector.
 */
void handle_arguments(int argc, char *argv[]) {
    int opt;  // Variable to store the option character
    char *directory_name = NULL;  // Pointer to store directory name if provided

    // Define long options
    static struct option long_options[] = {
        {"save", optional_argument, 0, 's'},
        {"all",  no_argument,       0, 'a'},
        {"help",  no_argument,      0, 'h'},
        {0,      0,                 0,  0 }
    };

    int long_index = 0;  // Index of the long option

    // Loop through the command line options
    while ((opt = getopt_long(argc, argv, "s:a:h", long_options, &long_index)) != -1) {
        switch (opt) {
            case 's':  // Option to save the current directory
                directory_name = optarg ? optarg : "default";  // Use provided name or default
                save_current_directory(directory_name);
                break;
            case 'a':  // Option to show all saved spots
                show_all_spots();
                break;

            case 'h':  // Option to show all saved spots
                print_help();
                break;

            default:  // Unrecognized option, print usage information
                fprintf(stderr, "Usage: %s [-s|--save [directory_name]] [-a|--all] [directory_name]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Check for non-option arguments (directory names)
    if (optind < argc) {
        go_to_directory(argv[optind]);  // Go to the directory specified as a non-option argument
    } else if (optind == 1) {  // No options or directory names provided
        go_to_directory("default");  // Go to the default directory
    }
}
