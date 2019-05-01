#include "lib/auxiliar.h"

#define N 512

//Falta imprimir o que recebe do FIFO

int main(){

  char newline[1]; newline[0] = '\n';
  //criar um FIFO identificado pelo pid do processo:
  int i=getpid();
  char nomefifo[10];
  sprintf(nomefifo, "./%07d", i );//converte o int i numa string para nomear o fifo
  //printf("%s\n", nomefifo);
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

  int flagErro;

  while(1){
      flagErro = 0;
      memset(buffRead, ' ',N);
  //para ler varias linhas um readline:
    /// 1. Ler uma linha do input(0) e guardar no buf

      ssize_t sread = readln(0, buffRead, N);
      if(sread <=0){unlink(nomefifo); break;}

      numPalavrasInput = gatherArg(palavras,buffRead,sread);
      buffRead[0] = 0;
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
          flagErro = 1;
          printf("erro!\n");
          break;

      }

      if(flagErro != 1){
    //escrever a linha
    /// 3. Enviar a linha para o fifo queue:
      fdQueue = open(nomeFifoGeral, O_WRONLY);
             
      write(fdQueue, linha, strlen(linha));
      //write(1, linha, strlen(linha));
      close(fdQueue);
      linha[0] = 0;
      
    //4. receber do fifo a resposta
      fdFifo = open(nomefifo,O_RDONLY);
      int lidos = readln(fdFifo,buffRead,N);
      close(fdFifo);
      write(1,buffRead,lidos);
      
      write(1,newline,1);
      buffRead[0] = 0;
      }
  }
  free(buffRead);
  return 0;
}
