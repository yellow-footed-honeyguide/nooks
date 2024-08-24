#include <getopt.h>  // getopt_long (function), struct option (type)
#include <stdio.h>   // printf (function), fprintf (function), stderr (macro)
#include <stdlib.h>  // exit (function), EXIT_FAILURE (macro)
#include <unistd.h>  // getopt (function)

#include "actions.h"  // save_current_directory, go_to_directory, show_all_spots, quit_work (functions)
#include "handle_arguments.h"  // handle_arguments (function prototype)

void print_help(void);  // Function prototype for printing help information

void handle_arguments(int argc, char *argv[]) {  // Function to process command-line arguments
    int opt;                                     // Variable to store the current option
    char *directory_name = NULL;                 // Pointer to store the directory name
    int save_mark_option_used = 0;               // Flag for save-mark option usage
    int all_option_used = 0;                     // Flag for all option usage

    // Define long options for getopt_long
    static struct option long_options[] = {
        {"save-mark", required_argument, 0, 's'},  // Option to save current directory
        {"all", no_argument, 0, 'a'},              // Option to show all saved spots
        {"help", no_argument, 0, 'h'},             // Option to display help
        {"version", no_argument, 0, 'v'},          // Option to display version
        {"quiet", no_argument, 0, 'q'},            // Option for quiet mode
        {0, 0, 0, 0}                               // Array terminator
    };

    int long_index = 0;  // Index of the current option in long_options array

    // Parse command-line options using getopt_long
    while ((opt = getopt_long(argc, argv, "s:ahvq", long_options, &long_index)) != -1) {
        switch (opt) {                      // Switch on the current option
            case 's':                       // Save current directory with provided name
                directory_name = optarg;    // Store the provided directory name
                save_mark_option_used = 1;  // Set the flag for save-mark option
                break;
            case 'a':                 // Show all saved spots
                all_option_used = 1;  // Set the flag for all option
                break;
            case 'h':          // Print help information
                print_help();  // Call function to print help
                break;
            case 'v':             // Print version of nooks
                print_version();  // Call function to print version
                break;
            case 'q':         // Disable output printing (quiet mode)
                quit_work();  // Call function to enable quiet mode
                break;
            case '?':                 // Handle unknown options and missing arguments
                if (optopt == 's') {  // If 's' option is missing an argument
                    fprintf(stderr, "Option -%c requires an argument.\n",
                            optopt);  // Print error message
                    fprintf(stderr,
                            "Usage: %s [-s|--save-mark mark_name] [-a|--all] [-h|--help] "
                            "[directory_name]\n",
                            argv[0]);    // Print usage information
                    exit(EXIT_FAILURE);  // Exit the program with failure status
                } else {                 // For other unknown options
                    fprintf(stderr, "Unknown option '-%c'.\n", optopt);  // Print error message
                    fprintf(stderr,
                            "Usage: %s [-s|--save-mark mark_name] [-a|--all] [-h|--help] "
                            "[directory_name]\n",
                            argv[0]);    // Print usage information
                    exit(EXIT_FAILURE);  // Exit the program with failure status
                }
                break;
            default:      // Handle unexpected cases
                abort();  // Abort the program
        }
    }

    // Handle the -a or --all option
    if (all_option_used) {
        show_all_spots();  // Call function to display all saved spots
        return;            // Exit the function
    }

    // Handle the --save or -s option
    if (save_mark_option_used) {
        save_current_directory(
            directory_name);  // Call function to save current directory with provided name
    }

    // Handle non-option arguments (directory name without flag)
    if (optind < argc && !save_mark_option_used) {
        go_to_directory(argv[optind]);  // Call function to navigate to the provided directory
    }
}
