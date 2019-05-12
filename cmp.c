#include "lib/auxiliar.h"


int main(){

    //0.abrir ficheiros
    int fdArtigos = open("./artigos", O_CREAT | O_RDWR, 0666);
    int fdStrings = open("./strings", O_CREAT | O_RDWR, 0666);
    int fdStringsaux = open("./stringsaux.txt", O_CREAT | O_RDWR, 0666);
    if(fdArtigos == -1 || fdStrings == -1 || fdStringsaux == -1){
        perror(0);
        _exit(errno);
    }

    // 1.determinar numero de linhas de artigos
    int tam = 128;
    char buff[15];
    char buffRead[tam];
    int i=0;
    int N, lidos, lidos2;
    int ref;
    int fim = 0;
    N = (lseek(fdArtigos,0,SEEK_END))/47;
    int pos;

 
    //variaveis para redefinir ref:
    char bitsCampoLinha[16];
    bitsCampoLinha[15] = '\0';
    int numOff_setStrings=0;
    int lista[N+1];

    //1.Guardar o conteudo que nao é lixo de strings em string aux
    for(i = 0; i<N; i++){

        memset(buff, ' ',15);   // iniciar o buff a espaço
        memset(buffRead,' ', tam);
        
        lseek(fdArtigos,32+(47*i),SEEK_SET); //posiciona lseek na referencia de strings
        readln(fdArtigos, buff, 15);
        ref=atoi(buff);
        
        //vai a strings, copia para stringsaux
        lseek(fdStrings,ref,SEEK_SET);
        lidos=readln(fdStrings, buffRead,tam);
        int pos=lseek(fdStringsaux,0,SEEK_END);
        lista[i]=pos;
        write(fdStringsaux,buffRead,lidos);

    }

    //Ler o que esta no Stringsaux e passar p buffFinal
    fim =lseek(fdStringsaux,0,SEEK_END);
    char buffFinal[fim];
    lseek(fdStringsaux,0,SEEK_SET);
    lidos2 = read(fdStringsaux,buffFinal,fim);
    
   
    //Apagar conteudo Strings
    close(fdStrings);
    fdStrings = open("./strings", O_TRUNC, 0666);
    close(fdStrings);

    //escrever no fdStrings o novo conteudo
    fdStrings = open("./strings", O_RDWR , 0666);
    //write(2,buffFinal,lidos2);
    fim = write(fdStrings,buffFinal,lidos2);

    //redifinir referencias no fdArtigos:
    for(i=0; i<N; i++){
        lseek(fdArtigos,32+(47*i),SEEK_SET);
        sprintf(bitsCampoLinha,"%14d\n", lista[i]);
        write(fdArtigos, bitsCampoLinha, 14);
    }


    close(fdArtigos);
    close(fdStrings);

    //apagar fdStringaux
    //close(fdStringsaux);
    close(fdStringsaux);
    remove("./stringsaux.txt");
    //fdStringsaux = open("./stringsaux.txt", O_TRUNC, 0666);
    memset(buffFinal,' ', fim);

    //passar a variavel para string para imprimir no 1(ou seja imprimir no dup)
    char tmp[16]={0x0};
    sprintf(tmp,"%0d\n", fim);
    write(2,tmp,16);
    write(1,tmp,16);

    //return lidos2;
}