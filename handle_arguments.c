#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "handle_arguments.h"
#include "actions.h"

void handle_arguments(int argc, char *argv[]) {
    int opt;
    char *directory_name = NULL;
    int save_option_used = 0;
    int all_option_used = 0;
    int save_current_dir_option_used = 0;

    // Define long options
    static struct option long_options[] = {
        {"save",            required_argument, 0, 's'},
        {"all",             no_argument,       0, 'a'},
        {"help",            no_argument,       0, 'h'},
        {"version",         no_argument,       0, 'v'},
        {"quiet",           no_argument,       0, 'q'},
        {"save-current-dir", no_argument,      0, 'c'},
        {0,                 0,                 0,  0 }
    };

    int long_index = 0;

    // Parse command-line options
    while ((opt = getopt_long(argc, argv, "s:ahvqc", long_options, &long_index)) != -1) {
        switch (opt) {
            case 's':
                // Save current directory with provided name
                directory_name = optarg;
                save_option_used = 1;
                break;
            case 'a':
                // Show all saved spots
                all_option_used = 1;
                break;
            case 'h':
                // Print help information
                print_help();
                break;
            case 'v':
                // Print version of nooks
                print_version();
                break;
            case 'q':
                // Disable output printing
                quit_work();
                break;
            case 'c':
                // Save current directory with default name
                save_current_dir_option_used = 1;
                break;
            case '?':
                // Handle unknown options and missing arguments
                if (optopt == 's') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                    fprintf(stderr, "Usage: %s [-s|--save directory_name] [-c|--save-current-dir] [-a|--all] [-h|--help] [directory_name]\n", argv[0]);
                    exit(EXIT_FAILURE);
                } else {
                    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                    fprintf(stderr, "Usage: %s [-s|--save directory_name] [-c|--save-current-dir] [-a|--all] [-h|--help] [directory_name]\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                abort();
        }
    }

    // Handle the -a or --all option
    if (all_option_used) {
        show_all_spots();
        return;
    }

    // Handle the --save or -s option
    if (save_option_used) {
        save_current_directory(directory_name);
    }

    // Handle the --save-current-dir or -c option
    if (save_current_dir_option_used) {
        save_current_directory("default");
    }

    // Handle non-option arguments
    if (optind < argc && !save_option_used && !save_current_dir_option_used) {
        // Go to the provided directory
        go_to_directory(argv[optind]);
    }
    else {
        go_to_directory("default");
    }
}
