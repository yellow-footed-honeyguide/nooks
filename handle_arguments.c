#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "handle_arguments.h"
#include "actions.h"

void handle_arguments(int argc, char *argv[]) {
    int opt;
    char *directory_name = NULL;
    
    // Define long options
    static struct option long_options[] = {
        {"save", required_argument, 0, 's'},
        {"all",  no_argument,       0, 'a'},
        {"help", no_argument,       0, 'h'},
        {"version", no_argument,    0, 'v'},
        {0,      0,                 0,  0 }
    };
    
    int long_index = 0;
    
    // Parse command-line options
    while ((opt = getopt_long(argc, argv, "s:ahv", long_options, &long_index)) != -1) {
        switch (opt) {
            case 's':
                // Save current directory with provided name
                directory_name = optarg;
                save_current_directory(directory_name);
                break;
            case 'a':
                // Show all saved spots
                show_all_spots();
                break;
            case 'h':
                // Print help information
                print_help();
                break;
            case 'v':
                // Print verison of nooks
                print_version();
                break;
            case '?':
                // Handle unknown options and missing arguments
                if (optopt == 's') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                }
                fprintf(stderr, "Usage: %s [-s|--save directory_name] [-a|--all] [-h|--help] [directory_name]\n", argv[0]);
                exit(EXIT_FAILURE);
            default:
                abort();
        }
    }
    
    // Handle non-option arguments
    if (optind < argc) {
        // Go to the provided directory
        go_to_directory(argv[optind]);
    } else if (optind == 1) {
        // Go to the default directory if no options or arguments provided
        go_to_directory("default");
    }
}
