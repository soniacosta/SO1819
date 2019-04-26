#include "lib/auxiliar.h"


int main(){

  //criar um FIFO identificado pelo pid do processo:
  int i=getpid();
  char nomefifo[8];
  sprintf(nomefifo, "%d", i );//converte o int i numa string para nomear o fifo
  int fifo=mkfifo(nomefifo, 0666);


  while(1){
  //para ler varias linhas um readline:
    /// 1. Ler uma linha do input(0) e guardar no buff
    char buffRead[48]; //CONFIRMAR ESTES 48
    ssize_t sread = readln(0, buffRead, 48);
    if(sread <=0){break;}


    /// 2. Escrever o que foi lido numa linha

    //definir linhas e preencher com espaços:
    size_t tamLinha = 48;
    char linha[tamLinha];
    for(int i = 0; i<tamLinha-1;i++) {
       linha[i]=' ';
       linha[tamLinha-1] = '\n';
     }

    //escrever a linha

    /*
    passar o que foi lido para uma linha(tam48)
    [cv_pid(15) id(15) quantidade(15)]
     */
     sprintf(linha, "%s", nomefifo);


     char campo[48];
     int j=0;
     int k=1;
     for(int i=0; i<sread; i++) {

       //processa palavra <id> ou <quantidade>
       if (buffRead[i] == ' ' || buffRead[i] == '\n') {
         if (j>0 && k<3) {
           //campo ja existe
           memcpy(linha+16*k, campo, j);
           j=0;
         }
         k++;
       }
       else {
         campo[j]=buffRead[i];
         j++;
       }
     }

    /// 3. Enviar a linha para o fifo queue:
    int fd=open("./queue", O_RDWR, 0666);
          if(k<4){
            write(fd, linha, 48);
            write(1, linha, 48);
          }

    close(fd);
  }


  return 0;
}
