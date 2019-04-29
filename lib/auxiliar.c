#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>



size_t gatherArg(char* arg[], char* buffer, size_t size){
	int i, j, l = 0, x;
    int letra = 0;
	for(i = 0; i < size ; i++){
		arg[l] = (char*)malloc(32 *sizeof(char));
		char *elem = (char*) arg[l];
        letra = 0;
		for(j = i ,x = 0; j < size && buffer[j] != ' ' && buffer[j] != '\n'; j++, x++){
            letra ++; printf("%c\n", buffer[j]);
			elem[x] = buffer[j];
		}

        if(letra > 0) l++;
		letra = 0;
        elem[x] = '\0';
		
		i = j;
	}
	arg[l] = NULL;
	return l;
}
/*
int main(){
    char* palavras[32];
    char buff[10];
    int s = read(0, buff, 10);
    int i = gatherArg(palavras,buff,s);
    printf("%d\n",i);
}*/


ssize_t readln(int fildes, void *buf, size_t nbyte){
    char* b = buf;
    int i = 0;
    while(i<nbyte){
        int n = read(fildes,&b[i] ,1);
        //if(n <=0) return i;
        if(n <=0) break;
        char c = b[i];
        i++;
        if(c == '\n') break;
    }
    return i;
}



size_t vectorToString( char* arg[], char* string, int init, int end){ /*init e end inclusive*/

    int j = 0;
    for(int i = init; i <= end; i++){
        for(int k = 0; arg[i][k]; k++){
            string[j++] = arg[i][k];
        }
        if(i != end) string[j++] = ' ';
        
    }
    string[j++] = '\n';
    string[j] = '\0';
    return j;
}


