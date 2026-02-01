#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <file path>\n\t-n - create new file\n\t-f - filename path\n\n-a - add the employees\n\n -l - list the employees" ,argv[0]);
    return;
}

int main(int argc, char *argv[]) { 
    int dbfd = -1;
    bool newFile = false;
    bool listEmployees = false;
    char *filePath = NULL;
    char *addstring = NULL;
    int c;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;


    while((c = getopt(argc,argv,"nf:a:l")) != -1) {
        switch(c) {
            case 'n':
                 newFile = true;
                 break;
            case 'f':
                filePath = optarg;
                break;
            case 'a':
                addstring = optarg;
                break;
            case 'l':
                listEmployees = true;
                break;
            case '?':
                printf("Unknswn Option -%c\n",c);
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

        if (create_db_header(&dbhdr) == STATUS_ERROR) {
            printf("Failt to create database header\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filePath);
        if(dbfd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return -1;
        }
        if(validate_db_header(dbfd, &dbhdr ) == STATUS_ERROR) {
            printf("Unable to Validate the DB Header!\n");
            return -1;
        }
    }

    if (read_employees(dbfd,dbhdr,&employees) != STATUS_SUCCESS) {
        printf("Faild to read employees!");
        return 0;
    }

    if(addstring) { 
        add_employee(dbhdr,&employees,addstring);
    }

    if(listEmployees) {
        list_employees(dbhdr,employees);
    }

    output_file(dbfd, dbhdr, employees);
}
