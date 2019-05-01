#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

size_t gatherArg(char* arg[], char* buffer, size_t size);
size_t vectorToString( char* arg[],char* string,int init, int end);
ssize_t readln(int fildes, void *buf, size_t nbyte);
int isNumber(char *string);
