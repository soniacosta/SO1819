#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


/*ma.h*/
#define N 512

size_t gatherArg(char* arg[], char* buffer, size_t size);
size_t vectorToString( char* arg[],char* string,int init, int end);
ssize_t readln(int fildes, void *buf, size_t nbyte);
/*----*/

int main(int argc, char argv[]){

    //vars
    size_t tamLinhaArtigos = 25; //calculos feitos para 24caracteres (25 é para ter espaço para o \0)
    char linha[tamLinhaArtigos]; //linha que vai ser escrita no artigos, inicializada a 0
    for(int i = 0; i<tamLinhaArtigos-1;i++){linha[i]='0'; linha[tamLinhaArtigos-1] = '\0';}
    
    char* buffRead = malloc(N); //o buff dado ao read
    size_t lidos;   //return do read(quantos caracteres leu)
    char* palavras[32]; //o maximo de palavras lidas é 32 por convençao
    int numPalavrasInput = 0; 
    char prog;  //opcao : i , n , p

    char* nome; //nome do artigo
    int tamNome = 0; 

    int idArtigo = 0;
    char bitsCampoLinha[8]; 
    for(int i = 0; i<7;i++){bitsCampoLinha[i]='0'; bitsCampoLinha[7] = '\0';
    }
    
    char* novoPreco;
    
    /* abrir ficheiros e verificar */
    int fdArtigos = open("./artigos.txt", O_CREAT | O_RDWR, 0666);
    int fdStrings = open("./strings.txt", O_CREAT | O_RDWR, 0666);
    if(fdArtigos == -1 || fdStrings == -1){
        perror(0);
        _exit(errno);
    }

    /*verificar "wc -l" do ficheiro artigos para obter o numero de ids existentes 
    e verificar "wc -c" para obter o numero de bits do ficheiros strings para usar como offset*/
    int fd[2];
    pipe(fd);
    if(!fork()){ //filho
        close(fd[0]); //fechamos o 0 (leitura)
        dup2(fd[1],1);
        close(fd[1]);
        execlp("wc","wc", "-l", "artigos.txt",(char*)0); //queremos saber quantas linhas o ficheiro tem
        _exit(0);
    }

    close(fd[1]);//0 e de leitura
    char buf[10];
    
    int n = read(fd[0],buf, sizeof(buf)); //lê do pipe para o buf
    if(n <=0){
        perror(0);
        _exit(errno);
    }
    close(fd[0]);

    int wcArtigos = 0;
    sscanf(buf, "%d",&wcArtigos); //do buf queremos só a primeira parte (ver wc -l)
    int numOff_set = wcArtigos*(tamLinhaArtigos-1); //o offset vai ser o numero de linhas * numero de caracteres de uma linha
    lseek(fdArtigos,numOff_set,SEEK_SET);
    /************************************/
    int fd2[2];
    pipe(fd2);
    if(!fork()){ //filho
        close(fd2[0]); //fechamos o 0 (leitura)
        dup2(fd2[1],1);
        close(fd2[1]);
        execlp("wc","wc", "-c", "strings.txt",(char*)0); //queremos saber quantos bits existem no ficheiro
        _exit(0);
    }

    close(fd2[1]);//0 e de leitura
    char buf2[10];
    
    int n2 = read(fd2[0],buf2, sizeof(buf2)); //lê do pipe para o buf2
    if(n2 <=0){
        perror(0);
        _exit(errno);
    }
    close(fd2[0]);

    int wcStrings = 0;
    sscanf(buf2, "%d",&wcStrings); //do buf2 queremos só a primeira parte (ver wc -c)
    int numOff_setStrings = wcStrings;
    lseek(fdStrings,numOff_setStrings,SEEK_SET);

    while(1){
        
        lidos = readln(0, buffRead, N);
        if(lidos <= 0) break;

        numPalavrasInput = gatherArg(palavras,buffRead,lidos);
        prog = buffRead[0];
        //printf("%c\n",prog);

        
        switch (prog){
            case 'i'://escrever artigo
                nome = malloc(lidos * sizeof(char));
                tamNome = vectorToString(palavras, nome, 1, numPalavrasInput-2);
                sprintf(linha,"%7d %7s %7d\n", wcArtigos, palavras[numPalavrasInput-1], numOff_setStrings);
                write(fdStrings,nome,tamNome);
                
                write(fdArtigos,linha,tamLinhaArtigos-1);
                wcArtigos++;
                numOff_setStrings+=tamNome;
                break;

            case 'n':
                nome = malloc(lidos * sizeof(char));
                tamNome = vectorToString(palavras, nome, 2, numPalavrasInput-1);
                sprintf(bitsCampoLinha,"%7d", numOff_setStrings); // guardar só o campo refNome com o novo valor
                write(fdStrings,nome,tamNome);
                numOff_setStrings+=tamNome;

                idArtigo = atoi(palavras[1]); //ler o id
                lseek(fdArtigos,16+(24*idArtigo),SEEK_SET); //mudar o off_set para o inicio da linha+bits dos outros campos deste artigo
                write(fdArtigos,bitsCampoLinha,7); //escrever só o campo
                lseek(fdArtigos,numOff_set,SEEK_SET); //repor o off_set no fim
                break;

            case 'p':
                novoPreco = malloc(lidos * sizeof(char));
                tamNome = vectorToString(palavras, novoPreco, 2, numPalavrasInput-1); //a variavel tamNome é reutilizada no mesmo contexto
                sprintf(bitsCampoLinha,"%7s", novoPreco); // guardar só o campo preço com o novo valor
                printf("%s",novoPreco);
                idArtigo = atoi(palavras[1]); //ler o id
                lseek(fdArtigos,9+(24*idArtigo),SEEK_SET); //mudar o off_set para o inicio da linhabits dos outros campos deste artigo
                write(fdArtigos,bitsCampoLinha,6); //escrever só o campo
                lseek(fdArtigos,numOff_set,SEEK_SET); //repor o off_set no fim
                break;
                
            default: 
                break;
        }

        
    }
    close(fdStrings);
    close(fdArtigos);
    return 0;
    
}

size_t gatherArg(char* arg[], char* buffer, size_t size){
	int i, j, l = 0, x;
	for(i = 0; i < size ; i++){
		arg[l] = (char*)malloc(32 *sizeof(char));
		char *elem = (char*) arg[l];
		for(j = i ,x = 0; j < size && buffer[j] != ' ' && buffer[j] != '\n'; j++, x++){
			elem[x] = buffer[j];
		}
		elem[x] = '\0';
		l++;
		i = j;
	}
	arg[l] = NULL;
	return l;
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