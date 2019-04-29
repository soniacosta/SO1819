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
   
    char nomeFifoCv[8];

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
        printf("num de plavras lidas: %d %s \n", numPalavrasInput, buffRead);
        switch (numPalavrasInput){

            //instruçao que vem do ficheiro Artigo, para criar o stock
            case (1):
                fdStock=open("./stocks.txt", O_RDWR | O_CREAT, 0666);
                sscanf(buffRead, "%d",&id);

                sprintf(linhastock, "%15d %15d\n", id, quantidade);
                //printf("%d\n", id);
                lseek(fdStock,0,SEEK_END); 
                write(fdStock,linhastock,32);
                close(fdStock);

                break;

            //quando quer devolver o :
            case(2):
                    printf("case 2: %s %s \n", palavras[0], palavras[1]);
                    //fdFifoCv = open()
                    id = atoi(palavras[0]);
                    
                    fdStock = open("./stocks", O_RDONLY , 0666);
                    fdArtigos = open("./artigos.txt", O_RDONLY, 0666);
                    if(fdArtigos == -1 || fdStock == -1){
                        perror(0);
                        _exit(errno);
                    }
                    
                    // TODO : verificar se o id existe
                    lseek(fdStock, id*tamLinhaStocks+16,SEEK_SET);
                    read(fdStock,&stock,15);
                    close(fdStock);
                    printf("stock : %d", stock);

                    lseek(fdArtigos, id*tamLinhaArtigos+15,SEEK_SET);
                    read(fdArtigos,&preco,15);
                    close(fdArtigos);
                    printf("preco : %f\n",preco);
                    //INCOMPLETO
                    //sscanf(buffRead, "%s %d", &fifoQ, &id);
                    //sprintf(linha2,"%15d %15d\n", id2, )  
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
        fdFifoCv = open(nomeFifoCv,O_WRONLY);
        
        //write(fdFifoCv,)
   }
}
