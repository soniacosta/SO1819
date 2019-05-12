#include "lib/auxiliar.h"

/*ma.h*/
#define N 512

/*----*/

int main(int argc, char* argv[]){

    size_t tamLinhaArtigos = 48;   
    char linha[tamLinhaArtigos]; 
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
    bitsCampoLinha[15] = '\0';
    
    char * nomeFifo = "./queue";
    //criar fifo e abrir:
    int fifoQ=mkfifo(nomeFifo, 0666);

    int fdqueue;

    char* novoPreco;


    /* abrir ficheiros e verificar */
    int fdArtigos = open("./artigos", O_CREAT | O_RDWR, 0666);
    int fdStrings = open("./strings", O_CREAT | O_RDWR, 0666);
    if(fdArtigos == -1 || fdStrings == -1){
        perror(0);
        _exit(errno);
    }

    /*verificar "wc -l" do ficheiro artigos para obter o numero de ids existentes
    e verificar "wc -c" para obter o numero de bits do ficheiros strings para usar como offset*/


    //Verificar o numero de linhas do ficheiro artigos:
    int fd[2];
    pipe(fd);
    if(!fork()){ //filho
        close(fd[0]); //fechamos o 0 (leitura)
        dup2(fd[1],1);
        close(fd[1]);
        execlp("wc","wc", "-l", "artigos",(char*)0); //queremos saber quantas linhas o ficheiro tem
        _exit(0);
    }
    wait(0);
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
    

    //Verificar o numero de linhas do ficheiro strings

    int fd1[2];
    pipe(fd1);
    if(!fork()){ //filho
        close(fd1[0]); //fechamos o 0 (leitura)
        dup2(fd1[1],1);
        close(fd1[1]);
        execlp("wc","wc", "-l", "strings",(char*)0); //queremos saber quantas linhas o ficheiro tem
        _exit(0);
    }
    wait(0);
    close(fd1[1]);//0 e de leitura
    char buf2[10];

     int n2 = readln(fd[0],buf2, sizeof(buf2)); //lê do pipe para o buf
    if(n2 <=0){
        perror(0);
        _exit(errno);
    }
    close(fd[0]);

    int wcStrings = 0;
    sscanf(buf2, "%d",&wcStrings); //do buf queremos só a primeira parte (ver wc -l)


    int numOff_setStrings = lseek(fdStrings,0,SEEK_END);


    //VARIAVEIS PARA COMPACTAR STRINGS.TXT
    float totalstrings=wcStrings;
    float desperdicio=wcStrings-wcArtigos;
    float razao=0;  //para determinar se o desperdicio > 20%


    //write(1,&wcArtigos,sizeof(int));
    char tmp[16]={0x0};

    while(1){
        memset(buffRead, ' ',N);

        lidos = readln(0, buffRead, N);
        if(lidos <= 0) break;

        numPalavrasInput = gatherArg(palavras,buffRead,lidos);
        prog = buffRead[0];
        if(numPalavrasInput >= 3 || prog == 'a'){
            
            switch (prog){

                case 'i'://escrever artigo
                    /*
                    *
                    * Inserir novo artigo
                    */
                    nome = malloc(lidos * sizeof(char));
                    tamNome = vectorToString(palavras, nome, 1, numPalavrasInput-2);
                    if(!isNumber(palavras[numPalavrasInput-1])) break;
                    sprintf(linha,"%14d %15s %15d\n", wcArtigos, palavras[numPalavrasInput-1], numOff_setStrings);
                    write(fdStrings,nome,tamNome);

                    totalstrings++;

                    /*imprimir o valor do idArtigo para o terminal*/
                    sprintf(tmp,"%0d", wcArtigos);
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
                    
                    idArtigo = atoi(palavras[1]); //ler o id
                    if(!isNumber(palavras[1]) || wcArtigos-1 < idArtigo) break;
                    tamNome = vectorToString(palavras, nome, 2, numPalavrasInput-1);

                    desperdicio++;
                    totalstrings++; 
                    
                    sprintf(bitsCampoLinha,"%14d\n", numOff_setStrings); // guardar só o campo refNome com o novo valor
                    write(fdStrings,nome,tamNome);
                    numOff_setStrings+=tamNome;
                    lseek(fdArtigos,32+(47*idArtigo),SEEK_SET); //mudar o off_set para o inicio da linha+bits dos outros campos deste artigo
                    write(fdArtigos,bitsCampoLinha,15); //escrever só o campo
                    free(nome);

                    razao = desperdicio/totalstrings;
                    if(razao>=0.2){
                        int fd3[2];
                        pipe(fd3);
                        if(!fork()){ 
                            close(fd3[0]);
                            dup2(fd[1],1);
                            close(fd3[1]);
                            execl("./cmp","cmp",(char*)0); 
                            _exit(0);
                           
                        }
                        int status = 0;
                        wait(&status);
                        
                        close(fd3[1]);
                        char buf3[10];

                        n = readln(fd3[0],buf3, sizeof(buf3)); //lê do pipe para o buf
                        /*if(n <0){
                            perror(0);
                            _exit(errno);
                        }*/
                        close(fd3[0]);

                        sscanf(buf3, "%d",&numOff_setStrings);
                        desperdicio=0;
                        totalstrings=(lseek(fdArtigos, 0, SEEK_END)/48);
                        
                    }
                    lseek(fdStrings,0,SEEK_END);
                    lseek(fdArtigos,0,SEEK_END); //repor o off_set no fim

                    break;

                case 'p':
                    /*
                    *
                    * Inserir preco artigo
                    */
                    novoPreco = malloc(lidos * sizeof(char));
                    idArtigo = atoi(palavras[1]); //ler o id
                    if(!isNumber(palavras[1]) || !isNumber(palavras[numPalavrasInput-1]) || wcArtigos-1 < idArtigo) break;
                    tamNome = vectorToString(palavras, novoPreco, 2, numPalavrasInput-1); //a variavel tamNome é reutilizada no mesmo contexto
                    sprintf(bitsCampoLinha,"%15s", novoPreco); // guardar só o campo preço com o novo valor
                    lseek(fdArtigos,16+(47*idArtigo),SEEK_SET); //mudar o off_set para o inicio da linhabits dos outros campos deste artigo
                    write(fdArtigos,bitsCampoLinha,14); //escrever só o campo
                    lseek(fdArtigos,0,SEEK_END); //repor o off_set no fim
                    free(novoPreco);
                    break;

                case 'a':
                    if(lidos > 2) break;
                    //write(2,"A iniciar agregacao",4);
                    fdqueue=open(nomeFifo, O_WRONLY);
                    write(fdqueue, "a", 1);
                    close(fdqueue);
                    //write(1, "Agregacao iniciada.",18);
                    write(1,"\n",1);
                    break;
                
                default:
                    break;
            }
        }

    }
    close(fdStrings);
    close(fdArtigos);
    free(buffRead);
    
    return 0;

}
