#include "lib/auxiliar.h"

#define N 48
#define tamLinhaArtigos 48
volatile  int flag_ctrl_c;

void int_handler(){
    flag_ctrl_c = 0;
    write(1,"\nA terminar",11);
}

int main(int argc, char* argv[]){
    //printf("111");

    signal(SIGINT,int_handler);
    flag_ctrl_c = 1;


    char* palavras[5]; //o maximo de palavras lidas é 5por convençao
    int numPalavrasInput = 0;
    //unlink("./queue");
    char * nomeFifoGeral = "./queue";
    //criar fifo e abrir:
    int fifoQ=mkfifo(nomeFifoGeral, 0777);
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
    int id, quantidade, absQuantidade = 0;
    int preco = 0;
    int stock = 0;
    char linha2[tamLinhaStocks];
   
    char nomeFifoCv[8];
    char auxStock[15] , auxpreco[15];

    //variaveis para verificar id:
    int numlseek=0;
    int idmax=0;

    //variaveis especificas para o case 3: (declarei aqui para o codigo ser mais legivel)
    int novaquantidade=0;
    int tamLinhaQuantidade=15;
    char linhaquantidade[tamLinhaQuantidade];
    //quando quantidade < 0
    int tamLinhaVendas=48;
    char linhavendas[tamLinhaVendas];
    int montante=0;

while(flag_ctrl_c){
        fdqueue = open(nomeFifoGeral, O_RDONLY); 
        if(fdqueue == -1){
            
            write(1,"error! fdqueue open sv\n",18);
            perror(0);
            _exit(errno);
        }

   while(flag_ctrl_c){
        memset(buffRead, ' ',N);
        memset(linhavendas, ' ',tamLinhaVendas);
        memset(linhaquantidade, ' ',tamLinhaQuantidade);
        memset(auxpreco , ' ',15);
        memset(auxStock , ' ',15);
        memset(linha2 , ' ',tamLinhaStocks);
        memset(linhastock, ' ',tamLinhaStocks);
        quantidade = 0;
        id = 0;
        fdFifoCv = 0;

        //ler do fifo geral
        lidos = readln(fdqueue,buffRead, N);
        if(lidos == -1) {
            write(1,"error! fdqueue read sv\n",22);
            perror(0); 
            break;
         }
            // fechar a queue para que os clientes possam escrever
        int numPalavrasInput = gatherArg(palavras,buffRead,lidos);
        if(numPalavrasInput > 3){ break; }
        //if(!fork()){
        //printf("numpalavras: %d\n", numPalavrasInput);
        switch (numPalavrasInput){

            
            case (1): //instruçao que vem do ficheiro Artigo, para criar o stock
                //write(1,"case1",5);
                if(isNumber(buffRead)){
                    sscanf(buffRead, "%d", &id);
                    fdStock=open("./stocks.txt", O_RDWR | O_CREAT, 0666);

                    quantidade = rand() % 100;

                    sprintf(linhastock, "%15d %15d\n", id, quantidade);
                    //printf("%d\n", id);
                    lseek(fdStock,0,SEEK_END); 
                    write(fdStock,linhastock,32);
                    close(fdStock);
                }else{
                    if(buffRead[0] == 'a'){
                        write(2,"Agregacao iniciada. ",20);
                        if(!fork()){ //filho

                            execl("./ag","ag",(char*)0); //mandamos executar o ag que trata da agregaçao sozinho
                            _exit(0);
                        }
                    }


                }
                break;

            
            case(2): //instrucao que vem do cv com o nome do fifo e o id para consulta
                sscanf(buffRead,"%s %d", nomeFifoCv,&id);
                
                if(!isNumber(palavras[1])){ 
                    
                    escreverFifo(nomeFifoCv, "o id para consulta não é válido.");
                    break;
                }
                fdStock = open("./stocks.txt", O_RDONLY , 0666);
                fdArtigos = open("./artigos.txt", O_RDONLY, 0666);
                if(fdArtigos == -1 || fdStock == -1){
                    write(1,"error! fdartigos fdstock open sv\n",18);
                    perror(0);
                    break;
                }
                
                //verificar se o id existe:
                numlseek = lseek(fdStock, 0, SEEK_END);
                idmax = numlseek/tamLinhaStocks;
                
                if(idmax < id){ 
                    close(fdStock); close(fdArtigos);
                    write(1,"error! id nao existe",20);
                    //enviar o erro para o cliente
                    sprintf(nomeFifoCv,"%s",palavras[0]);
                    fdFifoCv = open(nomeFifoCv,O_WRONLY);
                    if(fdFifoCv == -1){
                        write(1,"error! fdfifocv open sv\n",18);
                        perror(0);
                        break;
                    }
                    write(fdFifoCv,"error! nao existe id",20);
                    close(fdFifoCv);
                    break;
                }

                // TODO : verificar se o id existe
                lseek(fdStock, id*tamLinhaStocks+16,SEEK_SET);
                
                lidos = readln(fdStock,auxStock,15);
                close(fdStock);
                sscanf(auxStock,"%d",&stock);
                //write(1,auxStock,lidos);
                //printf("offset : %d \n", 16+(47*id));

                lseek(fdArtigos, 16+(47*id),SEEK_SET);
                readln(fdArtigos,auxpreco,15);
                close(fdArtigos);

                //printf("stock %d\n", stock);
                sprintf(linha2,"%s %s", auxStock, auxpreco);  
                //printf("linha %s\n", linha2);    

                sprintf(nomeFifoCv,"%s",palavras[0]);
                fdFifoCv = open(nomeFifoCv,O_WRONLY);
                if(fdFifoCv == -1){
                    write(1,"error! fdfifocv open sv\n",18);
                    perror(0);
                    break;
                }
                write(fdFifoCv,linha2,32);
                close(fdFifoCv);
                break;

            case(3):
                sscanf(buffRead, "%s %d %d", nomeFifoCv, &id, &quantidade);
                if(!isNumber(palavras[1])){ 
                    write(2,"id is not number",16);
                    escreverFifo(nomeFifoCv, "um dos campos não é válido.");
                    break;
                }
                fdStock = open("./stocks.txt", O_RDWR , 0666);
                if(fdStock == -1){
                    perror(0);
                    break;
                }
                
                //verificar se o id existe:
                numlseek = lseek(fdStock, 0, SEEK_END);
                idmax = numlseek/tamLinhaStocks;
                
                if(idmax < id){ 
                    close(fdStock); 
                    //write(1,"error!",6);
                    //enviar o erro para o cliente
                    sprintf(nomeFifoCv,"%s",palavras[0]);
                    escreverFifo(nomeFifoCv, "error! id nao existe"); 
                    /*fdFifoCv = open(nomeFifoCv,O_WRONLY);
                    if(fdFifoCv == -1){

                        perror(0);
                        break;
                    }
                    write(fdFifoCv,"error! id nao existe",20);
                    close(fdFifoCv);*/
                    break;
                }

                //ler a quantidade em stock daquele id:
                lseek(fdStock, id*tamLinhaStocks+16,SEEK_SET);
                lidos = readln(fdStock,auxStock,15);

                //guardar na variavel stock o stock atual(antes da ediçao):
                sscanf(auxStock,"%d",&stock);
                //verificar se é venda e se há stock
                if(quantidade < 0 && stock < abs(quantidade)){
            
                    close(fdStock); 
                     //enviar o erro para o cliente
                    sprintf(nomeFifoCv,"%s",palavras[0]);
                    escreverFifo(nomeFifoCv, "error! stock nao existe");
                     
                }else{
                
                    novaquantidade = stock + quantidade;

                    sprintf(linhaquantidade, "%15d", novaquantidade);
                    
                    lseek(fdStock, id*tamLinhaStocks+16,SEEK_SET);
                    write(fdStock, linhaquantidade,15);
                    
                    close(fdStock);

                    //1.determinar o montante total através do ficheiro Artigos:
                    int fdArtigos = open("./artigos.txt", O_RDONLY , 0666);
                    if(fdArtigos == -1){
                        write(1,"erro fdartigos",14);
                        perror(0);
                        break;
                    }

                    lseek(fdArtigos, 16+(47*id),SEEK_SET);
                    readln(fdArtigos,auxpreco,15);
                    close(fdArtigos);
                    preco=atoi(auxpreco);

                    //enviar para o Cliente a quantidade atual:
                    sprintf(nomeFifoCv,"%s",palavras[0]);
                    escreverFifo(nomeFifoCv, linhaquantidade);
                    
                    if(quantidade < 0){
                        absQuantidade = abs(quantidade);
                        montante = absQuantidade*preco;

                        //2.criar linhavendas:

                        sprintf(linhavendas, "%15d %15d %15d\n", id, absQuantidade,montante);
                        int fdVendas = open("./vendas.txt", O_RDWR | O_CREAT , 0666);
                        if(fdVendas == -1){
                              write(2,"erro fdvendas",13);
                              perror(0);
                              break;
                        }
                        lseek(fdArtigos,0,SEEK_END);
                        write(fdVendas,linhavendas,tamLinhaVendas);
                        close(fdVendas);

                    }

                }
                break;
            /*
            default:
                printf("default 1: \n");
                break;*/

        }
    /*    _exit(0);
    }*/
        
   }

    close(fdqueue);
}
   write(1," \n",2);
   unlink(nomeFifoGeral);
   perror(strerror(errno)); 
    //break;
   free(buffRead);
   return 0;
}
