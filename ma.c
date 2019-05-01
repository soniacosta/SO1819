#include "lib/auxiliar.h"

/*ma.h*/
#define N 512

/*----*/

int main(int argc, char* argv[]){
    //vars
    size_t tamLinhaArtigos = 48; //25; //calculos feitos para 24caracteres (25 é para ter espaço para o \0)
    char linha[tamLinhaArtigos]; //linha que vai ser escrita no artigos, inicializada a 0
    //for(int i = 0; i<tamLinhaArtigos-1;i++) { linha[i]='0'; } 
    linha[tamLinhaArtigos-1] = '\0';

    char* buffRead = malloc(N); //o buff dado ao read
    size_t lidos;   //return do read(quantos caracteres leu)
    char* palavras[32]; //o maximo de palavras lidas é 32 por convençao
    int numPalavrasInput = 0;
    char prog;  //opcao : i , n , p

    char* nome; //nome do artigo
    int tamNome = 0;

    int idArtigo = 0;
    char bitsCampoLinha[16];
    //for(int i = 0; i<15;i++){ bitsCampoLinha[i]='0'; } 
    bitsCampoLinha[15] = '\0';
    
    char * nomeFifo = "./queue";
    //criar fifo e abrir:
    int fifoQ=mkfifo(nomeFifo, 0666);
    //if(fifoQ == -1){
    //        perror(0);
    //        _exit(errno);
    //}

    int fdqueue;

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

    int n = readln(fd[0],buf, sizeof(buf)); //lê do pipe para o buf
    if(n <=0){
        perror(0);
        _exit(errno);
    }
    close(fd[0]);

    int wcArtigos = 0;
    sscanf(buf, "%d",&wcArtigos); //do buf queremos só a primeira parte (ver wc -l)
    int numOff_set = lseek(fdArtigos,0,SEEK_END);
    
    int numOff_setStrings = lseek(fdStrings,0,SEEK_END);
    //write(1,&wcArtigos,sizeof(int));
    char tmp[16]={0x0};

    while(1){

        lidos = readln(0, buffRead, N);
        if(lidos <= 0) break;

        numPalavrasInput = gatherArg(palavras,buffRead,lidos);
        prog = buffRead[0];
        switch (prog){

            case 'i'://escrever artigo
                /*
                *
                * Inserir novo artigo
                */
                nome = malloc(lidos * sizeof(char));
                tamNome = vectorToString(palavras, nome, 1, numPalavrasInput-2);
                sprintf(linha,"%14d %15s %15d\n", wcArtigos, palavras[numPalavrasInput-1], numOff_setStrings);
                write(fdStrings,nome,tamNome);

                /*imprimir o valor do idArtigo para o terminal*/
                sprintf(tmp,"%d", wcArtigos);
                /**/
                write(fdArtigos,linha,tamLinhaArtigos-1);
                numOff_setStrings+=tamNome;

                //enviar informaçao de alterar stock para a queue
                //printf("%d %s", sizeof(tmp),tmp);
                fdqueue=open(nomeFifo, O_WRONLY);
                write(fdqueue, tmp, sizeof(tmp));
                close(fdqueue);
                write(1, tmp, sizeof(wcArtigos));
                write(1,"\n",1);
                wcArtigos++;

                break;

            case 'n':

                /*
                *
                * Alterar nome artigo
                */
                nome = malloc(lidos * sizeof(char));
                tamNome = vectorToString(palavras, nome, 2, numPalavrasInput-1);
                sprintf(bitsCampoLinha,"%14d\n", numOff_setStrings); // guardar só o campo refNome com o novo valor
                write(fdStrings,nome,tamNome);
                numOff_setStrings+=tamNome;
                idArtigo = atoi(palavras[1]); //ler o id
                lseek(fdArtigos,32+(47*idArtigo),SEEK_SET); //mudar o off_set para o inicio da linha+bits dos outros campos deste artigo
                write(fdArtigos,bitsCampoLinha,15); //escrever só o campo
                lseek(fdArtigos,0,SEEK_END); //repor o off_set no fim
                break;

            case 'p':

                /*
                *
                * Inserir preco artigo
                */
                novoPreco = malloc(lidos * sizeof(char));
                tamNome = vectorToString(palavras, novoPreco, 2, numPalavrasInput-1); //a variavel tamNome é reutilizada no mesmo contexto
                sprintf(bitsCampoLinha,"%15s", novoPreco); // guardar só o campo preço com o novo valor
                idArtigo = atoi(palavras[1]); //ler o id
                lseek(fdArtigos,16+(47*idArtigo),SEEK_SET); //mudar o off_set para o inicio da linhabits dos outros campos deste artigo
                write(fdArtigos,bitsCampoLinha,14); //escrever só o campo
                lseek(fdArtigos,0,SEEK_END); //repor o off_set no fim
                break;

            default:
                break;
        }


    }
    close(fdStrings);
    close(fdArtigos);
    free(buffRead);
    free(nome);
    free(novoPreco);
    return 0;

}
