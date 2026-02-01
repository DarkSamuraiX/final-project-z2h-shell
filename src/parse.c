#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {

}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {

}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {

}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
    if (fd < 0 ) {
        printf("Got a bad FD from the user\n");
        return STATUS_ERROR;
    }

    dbhdr->filesize = htonl(dbhdr->filesize);
    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET);
    write(fd, dbhdr, sizeof(struct dbheader_t));
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if(fd < 0) {
        printf("File Descriptor Error");
        return -1;
    }

    struct dbheader_t *header = calloc(1,sizeof(struct dbheader_t));
    if(header == NULL) {
        printf("Calloc Failed to check db file header!");
        return STATUS_ERROR;
    }

    if(read(fd,header,sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if(header->version != 1) {
        printf("Improper header version\n");
        free(header);
        return STATUS_ERROR;
    }

    if(header->magic != HEADER_MAGIC) {
        printf("Improper Magic Header\n");
        free(header);
        return STATUS_ERROR;
    } 

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if(header->filesize != dbstat.st_size) {
        printf("Corrupted Database\n");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header; 
    return STATUS_SUCCESS;

}

int create_db_header(int fd, struct dbheader_t **headerOut) {
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == -1) {
        printf("calloc faild to create db header!");
        free(header);
        return STATUS_ERROR;
    }
    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t); 

	if (fd == -1) {
        perror("header: ");
        return STATUS_ERROR;
    }

    *headerOut = header;
    return STATUS_SUCCESS;
}


