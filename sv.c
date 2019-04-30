#include "lib/auxiliar.h"

#define N 48
#define tamLinhaArtigos 48

int main(int argc, char* argv[]){
    //printf("111");

    char* palavras[5]; //o maximo de palavras lidas é 5por convençao
    int numPalavrasInput = 0;
    //unlink("./queue");
    char * nomeFifoGeral = "./queue";
    //criar fifo e abrir:
    int fifoQ=mkfifo(nomeFifoGeral, 0666);
    //if(fifoQ == -1){
    //        perror(0);
    //        _exit(errno);
    //    }

    int fdqueue;

    int lidos;
    char* buffRead = malloc(N);
    int tamLinhaStocks = 32;
    char linhastock[tamLinhaStocks];
    int  fdStock , fdArtigos, fdFifoCv;
    int id, quantidade = 1234;
    float preco = 0;
    int stock = 0;
    char linha2[tamLinhaStocks];
   
    char nomeFifoCv[8];
    char auxStock[15] , auxpreco[15];

   while(1){

        //ler do fifo geral
        fdqueue = open(nomeFifoGeral, O_RDONLY); 
        if(fdqueue == -1){
            write(1,"error!",6);
            perror(0);
            _exit(errno);
        }
        lidos = read(fdqueue,buffRead, N);
        close(fdqueue);
        if(lidos <= 0) { perror(0); _exit(errno); break; }
            // fechar a queue para que os clientes possam escrever
        int numPalavrasInput = gatherArg(palavras,buffRead,lidos);
        if(numPalavrasInput > 3){ break; }
        //printf("numpalavras: %d\n", numPalavrasInput);
        switch (numPalavrasInput){

            //instruçao que vem do ficheiro Artigo, para criar o stock
            case (1):
                //write(1,"case1",5);
                sscanf(buffRead, "%d", &id);
                fdStock=open("./stocks.txt", O_RDWR | O_CREAT, 0666);
                //atribuir uma quantidade random por enquanto
                   // Initialization, should only be called once.
                quantidade = rand() % 100;

                sprintf(linhastock, "%15d %15d\n", id, quantidade);
                //printf("%d\n", id);
                lseek(fdStock,0,SEEK_END); 
                write(fdStock,linhastock,32);
                close(fdStock);

                break;

            //quando quer devolver o :
            case(2):
                //write(1,"case2",5);
                //printf("case 2: %s %s \n", palavras[0], palavras[1]);
                //printf("palavras 1 %s\n",palavras[1]);
                //id = atoi(palavras[1]);
                sscanf(buffRead,"%s %d", nomeFifoCv,&id);
                //printf("id : %d nomeFifo %s numoffset %d\n",id, nomeFifoCv, id*tamLinhaStocks+16);
                
                fdStock = open("./stocks.txt", O_RDONLY , 0666);
                fdArtigos = open("./artigos.txt", O_RDONLY, 0666);
                if(fdArtigos == -1 || fdStock == -1){
                    perror(0);
                    _exit(errno);
                }
                
                // TODO : verificar se o id existe
                lseek(fdStock, id*tamLinhaStocks+16,SEEK_SET);
                
                lidos = read(fdStock,auxStock,15);
                close(fdStock);
                sscanf(auxStock,"%d",&stock);
                //write(1,auxStock,lidos);
                //printf("offset : %d \n", 16+(47*id));

                lseek(fdArtigos, 16+(47*id),SEEK_SET);
                read(fdArtigos,auxpreco,15);
                close(fdArtigos);

                //printf("stock %d\n", stock);
                sprintf(linha2,"%s %s", auxStock, auxpreco);  
                //printf("linha %s\n", linha2);    

                sprintf(nomeFifoCv,"%s",palavras[0]);
                fdFifoCv = open(nomeFifoCv,O_WRONLY);
                if(fdFifoCv == -1){
                    perror(0);
                    _exit(errno);
                }
                write(fdFifoCv,linha2,32);
                close(fdFifoCv);
                break;

            case(3):
            break;
                    /*case(3):
                    
        

            //quando quer adicionar artigos ao stock:
            if(quantidade>0){


            }

            //quando quer realizar uma venda:
            else{

            }
            }
            */
            default:
                printf("default 1: \n");
                break;

        }
       
        
   }
   free(buffRead);
   return 0;
}
