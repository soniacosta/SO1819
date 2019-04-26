#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#define N 48
#define tamLinhaArtigos 48

int main(int argc, char* argv[]){

  //criar fifo e abrir:
  int fifoq=mkfifo("queue", 0666);
  int fdqueue=open("./queue", O_RDWR, 0666);


   while(1){

    int lidos;
    char* buffRead;

    while(lidos=readln(fdqueue,buffRead, N){

          if(lidos <= 0) break;
          int numPalavrasInput = gatherArg(palavras,buffRead,lidos);
          if(numPalavrasInput>3){break;}


      switch (numPalavrasInput){

          //instruçao que vem do ficheiro Artigos, para alterar stock
          case (1):
              int  fdStock=open("./stocks", O_RDONLY | O_WRONLY | O_CREAT, 0666);
              int id, quantidade=0;
              sscanf(buffRead, "%d",&id);

              tamLinha=32;
              char linhastock[tamLinha];
              linhastock={0x0};
              sprintf(linhastock, "%15d %15d\n", id, quantidade);

              lseek(fdStock,0,SEEK_END);
              write(fdStock,linhastock,32);

              close(fdStock);
          break;

          //quando quer devolver o stock e preços:
          case(2):
            int id2;
            char fifo[8];
            tamLinha=32;
            char linhas2[tamLinha];
            linha2={0x0};

            int fdStock=open("./stocks", O_RDONLY | O_WRONLY | O_CREAT, 0666);
            int fdArtigos = open("./artigos.txt", O_CREAT | O_RDWR, 0666);
            if(fdArtigos == -1 || fdStock == -1){
                perror(0);
                _exit(errno);
            }

            lseek(fdArtigos, id2*tamLinhaArtigos+15,SEEK_END);
            //INCOMPLETO
            sscanf(buffRead, "%s %d", &fifo, &id2);
            sprintf(linha2,"%15d %15d\n", id2, )

          case(3):


            //quando quer adicionar artigos ao stock:
            if(quantidade>0){


            }

            //quando quer realizar uma venda:
            else{

            }

        }




      }










   }


}
