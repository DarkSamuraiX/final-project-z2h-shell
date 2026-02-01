#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <file path>\n\t-n - create new file\n\t-f - filename path\n\n" ,argv[0]);
    return;
}

int main(int argc, char *argv[]) { 
    int dbfd = -1;
    bool newFile = false;
    char *filePath = NULL;
    int c;

    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employe = NULL;

    while((c = getopt(argc,argv,"nf:")) != -1) {
        switch(c) {
            case 'n':
                 newFile = true;
                 break;
            case 'f':
                filePath = optarg;
                break;
            case '?':
                printf("Unknown Option -%c\n",c);
                break;
            default:
                return -1;
        }
    }
    if (filePath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv);
    }

    if (newFile) {
        dbfd = create_db_file(filePath);
        if(dbfd == STATUS_ERROR) {
            printf("Unable to create database file\n");
            return -1;
        }

        if (create_db_header(dbfd,&dbhdr) == STATUS_ERROR) {
            printf("Failt to create database header\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filePath);
        if(dbfd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return -1;
        }
        if(validate_db_header(dbfd, &dbhdr ,&employe ) == STATUS_ERROR) {
            printf("Unable to Validate the DB Header!\n");
            return -1;
        }
    }

    printf("Newfile: %d\n", newFile);
    printf("File Path: %s\n", filePath);

    output_file(dbfd, dbhdr);
}
