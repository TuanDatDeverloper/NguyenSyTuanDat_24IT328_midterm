#include <stdio.h>
#include <stdlib.h>
#include "options.h"
#include "traverse.h"

int main(int argc, char *argv[]) {
    Options opts;
    int opt_index = parse_options(argc, argv, &opts);
    if (opt_index < 0) {
        fprintf(stderr, "usage: ls [-AacdFfhiklnqRrSstuw] [file ...]\n");
        return 1;
    }

    int operand_count = argc - opt_index;
    char **operands = argv + opt_index;

    int exit_status = process_operands(operand_count, operands, &opts);

    return exit_status;
}
