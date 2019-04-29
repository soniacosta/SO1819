#include "lib/auxiliar.h"

#define N 512

//Falta imprimir o que recebe do FIFO

int main(){

  //criar um FIFO identificado pelo pid do processo:
  int i=getpid();
  char nomefifo[10];
  sprintf(nomefifo, "./%07d", i );//converte o int i numa string para nomear o fifo
  printf("%s\n", nomefifo);
  int fifo=mkfifo(nomefifo, 0666);

  //fifo comum ao servidor e aos clientes
  char * nomeFifoGeral = "./queue";
  int fifoQ=mkfifo(nomeFifoGeral, 0666); //se existir nao faz nada

  int fdQueue, fdFifo;

  char* buffRead = malloc(N); //o buff dado ao read
  char* palavras[8]; 
  int numPalavrasInput = 0;
  size_t tamLinha = 48;
  char linha[tamLinha];

  while(1){
  //para ler varias linhas um readline:
    /// 1. Ler uma linha do input(0) e guardar no buf

      ssize_t sread = readln(0, buffRead, N);
      if(sread <=0){break;}

      numPalavrasInput = gatherArg(palavras,buffRead,sread);
      switch (numPalavrasInput){

        //instruçao que vem do ficheiro Artigos, para alterar stock
        case (1):
          sprintf( linha,"%s %s",nomefifo,palavras[0]);
          //definir linhas e preencher com espaços:
          break;

        case (2):
          sprintf(linha, "%s %s %s",nomefifo,palavras[0], palavras[1]);
          break;

        default:
          printf("erro!\n");
          break;

      }

    //escrever a linha
    /// 3. Enviar a linha para o fifo queue:
      fdQueue = open(nomeFifoGeral, O_WRONLY);
             
      write(fdQueue, linha, 48);
      write(1, linha, 48);
      close(fdQueue);
        
      //fdFifo = open(nomefifo,O_RDONLY);
      //read


  }

  return 0;
}
