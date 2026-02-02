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



int remove_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *removeEmployee){
    struct employee_t *e = *employees;
    int maxPersonsCurrent = dbhdr->count;
    int indexToDelete = -1;
    for(int i = 0; i < maxPersonsCurrent; i++) {
        if(strcmp(removeEmployee,e[i].name) == 0) {
            indexToDelete = i;
            break;
        }
    }

    if(indexToDelete == -1) {
        printf("Employee %s not found!", removeEmployee);
        return STATUS_ERROR;
    } else {
        for(int i = indexToDelete; i < maxPersonsCurrent - 1; ++i ) {
            e[i] = e[i+ 1];
        }
    }

    dbhdr->count = maxPersonsCurrent - 1;
    e = realloc(e, sizeof(struct employee_t) * dbhdr->count);
    if(e == NULL) {
        printf("Reallocate Failed!");
        return STATUS_ERROR;
    }
    *employees = e;
    return STATUS_SUCCESS;
}

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
    if (NULL == dbhdr) return STATUS_ERROR;
    if (NULL == employees) return STATUS_ERROR;
    for (int i = 0; i < dbhdr->count; i++) {
        printf("Employee %d\n", i+1);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHours: %d\n", employees[i].hours);
    }
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring) {
    if (NULL == dbhdr) return STATUS_ERROR;
    if (NULL == employees) return STATUS_ERROR;
    if (NULL == *employees) return STATUS_ERROR;
    if (NULL == addstring) return STATUS_ERROR;

    
    char *name = strtok(addstring, ",");
    if (name == NULL) return STATUS_ERROR;    
    char *addr = strtok(NULL, ",");
    if (addr == NULL) return STATUS_ERROR;    
    char *hours = strtok(NULL, ",");
    if (hours == NULL) return STATUS_ERROR;    
    
    struct employee_t *e = *employees;
    e = realloc(e, sizeof(struct employee_t) * (dbhdr->count + 1));
    if(e == NULL) {
        return STATUS_ERROR;
    }

    dbhdr-> count++;
    strncpy(e[dbhdr->count-1].name,name,sizeof(e[dbhdr->count-1].name) - 1);
    strncpy(e[dbhdr->count-1].address,addr,sizeof(e[dbhdr->count-1].address) - 1);
    e[dbhdr->count-1].hours = atoi(hours);


    *employees = e;
    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
    if (fd < 0) {
        printf("Got a bad FD from the user\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count; 
    struct employee_t *employees = calloc(count,sizeof(struct employee_t));
    if (employees == -1 ) {
        printf("Malloc Faild\n");
        return STATUS_ERROR;
    }
    read(fd,employees, count*sizeof(struct employee_t)); 
    for (int i = 0; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }
    *employeesOut = employees;
    return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
    if (fd < 0 ) {
        printf("Got a bad FD from the user\n");
        return STATUS_ERROR;
    }
    int realcount = dbhdr->count;
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET);
    write(fd, dbhdr, sizeof(struct dbheader_t));

    for(int i = 0; i < realcount; i++) {
        employees[i].hours = htonl(employees[i].hours);
        write(fd,&employees[i], sizeof(struct employee_t));
    }

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

int create_db_header(struct dbheader_t **headerOut) {
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

	// if (fd == -1) {
    //     perror("header: ");
    //     return STATUS_ERROR;
    // }

    *headerOut = header;
    return STATUS_SUCCESS;
}


