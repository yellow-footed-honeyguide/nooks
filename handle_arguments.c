#include <stdio.h>
#include <string.h>
#include "handle_arguments.h"
#include "actions.h"

void handle_arguments(int argc, char *argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--save") == 0) {
            save_current_directory("default");
        } else if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--all") == 0) {
            show_all_spots();
        } else {
            go_to_directory(argv[1]);
        }
    } else if (argc == 3) {
        if ((strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--save") == 0)) {
            save_current_directory(argv[2]);
        }
    } else {
        go_to_directory("default");
    }
}
