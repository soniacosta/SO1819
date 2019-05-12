#include "lib/auxiliar.h"

int contaVendas(int tamBuff,char* buff , int totArtigos) { //recebe o buff lido do ficheiro e o numero total de artigos
    
    int id, qtd;
    int preco;
    int count_id = 0, count_qtd = 0; 
    int count_preco = 0;
    int info[totArtigos][2];
    for(int i = 0; i < totArtigos;i++){
        info[i][0]= 0;
        info[i][1]= 0;
    }

    int lidos = 0;
    int tamLinha = 48;
    int index = 0;
    
    while(1){
        sscanf(&buff[index], "%d %d %d", &id, &qtd, &preco);
        
        info[id][0] = info[id][0] + qtd;
        info[id][1] = info[id][1] + preco;
        
        index += tamLinha ;
        if(index >= tamBuff){
            break;
        }
        
    }
    
    char* buffer = malloc(sizeof(char)*22);;
    formatTime(buffer);
    
    int fdAgreg = open(buffer, O_CREAT | O_WRONLY, 0666);
    int tamLinhaVendas=48;
    char linhavendas[tamLinhaVendas];

    for(int i=0; i<totArtigos; i++) {
        if(info[i][0] !=0 ){
            sprintf(linhavendas, "%15d %15d %15d\n", i, info[i][0],info[i][1]);
            
            write(fdAgreg,linhavendas,tamLinhaVendas);
        }
            
    }
    close(fdAgreg);
    return 0;
}


int main(){   
    int lidos = 0;
    char* buff;
    int numBits;
    int offset_init = 0 , offset_end = 0;

    //ver qual o fim da ultima agregaçao se existente
    int fdAgregs = open("./agregs.txt", O_CREAT | O_RDWR, 0666);
    numBits = lseek(fdAgregs, 0 , SEEK_END);
    
    
    if (numBits > 0){

        buff = malloc(sizeof(char)*numBits);
        lseek(fdAgregs, 0 , SEEK_SET);
        lidos = read(fdAgregs, buff,numBits);

        if(lidos < 0){
            perror(0);
            _exit(errno);
        }
        offset_init = atoi(buff); free(buff);
    }

    //abrir o ficheiro de vendas, e ler desde o offset da ultima agregacao ate ao fim
    int fdVendas = open("./vendas",O_RDONLY | O_CREAT);
    offset_end = lseek(fdVendas, 0, SEEK_END);
    
    numBits = offset_end - offset_init;
    
    if(numBits > 0){
        
        buff = malloc(numBits);
        lseek(fdVendas,offset_init,SEEK_SET);
        lidos = read(fdVendas,buff,numBits);
        close(fdVendas);

        /*escrever o novo ultimo offset lido no agregs.txt*/
        lseek(fdAgregs, 0, SEEK_SET);
        char offset_endStr[16];
        sprintf(offset_endStr,"%d",offset_end);
        write(fdAgregs, offset_endStr,strlen(offset_endStr));
        
        /****************************************************************************************************
        verificar "wc -l" do ficheiro artigos para obter o numero de ids existentes
        e verificar "wc -c" para obter o numero de bits do ficheiros strings para usar como offset*/
        int fd[2];
        pipe(fd);
        if(!fork()){ //filho
            close(fd[0]); //fechamos o 0 (leitura)
            dup2(fd[1],1);
            close(fd[1]);
            execlp("wc","wc", "-l", "artigos",(char*)0); //queremos saber quantas linhas o ficheiro tem
            _exit(0);
        }

        close(fd[1]);//0 e de leitura
        char buf[10];
        int n = read(fd[0],buf, sizeof(buf)); //lê do pipe para o buf
        if(n <=0){
            perror(0);
            _exit(errno);
        }
        close(fd[0]);

        int wcArtigos = 0;
        sscanf(buf, "%d",&wcArtigos); //do buf queremos só a primeira parte (ver wc -l)
        
        /***************************************************************************************************/
        contaVendas(numBits, buff, wcArtigos);

    }else{close(fdVendas);}
    write(1,"Agregacao terminada!. ", 22);
    return 0; 
}
