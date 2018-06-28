/*
    ---cliente---------------------------------
    Integrantes:
    Jesus Miguel Quinto Teran C.I = 25619081
    Edgar Joseph Rivero Matheus C.I = 20428267
*/

#include <stdio.h> 
#include <string.h>    
#include <sys/socket.h>  
#include <arpa/inet.h> 
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

pthread_t hilo;
int sock;
int estado = 1;

void uso()
{
  printf("Uso\n");
  printf("    cliente [port]\n");
  exit(0);
}

void strip_newline(char *s){
    while(*s != '\0')
    {
        if(*s == '\r' || *s == '\n')
        {
            *s = '\0';
        }
        s++;
    }
}

void *observar(){
    char buff_in[1024];
    int rlen;

    while((rlen = read(sock, buff_in, sizeof(buff_in)-1)) > 0)
    {
        buff_in[rlen] = '\0';
        strip_newline(buff_in);
        
        //Captura de opciones
        if(buff_in[0] == '#')
        {
            char *command;
            command = strtok(buff_in," ");

            if(!strcmp(command, "#SALIR"))
            {
                estado = 0;
                break;
            }
        }  

        puts(buff_in); 
    }
    pthread_exit((void*)(1));
}
 
int main(int argc , char *argv[])
{
    if (argc < 2)
    {
        uso();    
    }
    
        int port = (atoi(argv[1]));
        printf("%d\n",port);

        struct sockaddr_in server;
        char message[1000];
        system("clear");

        //Crear socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            printf("No se puede crear el socket");
        }
        puts("Socket Creado");
         
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
 
        if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
        {
            perror("Conexion Fallida. Error");
            return 1;
        }
         
        puts("Conectado\n");  
        
        pthread_create(&hilo,NULL,( void *)&observar,NULL) ;


        char name[32];
        printf("Ingrese su nombre:\n");
        fgets(name, 31, stdin);

        char newName[1000] = "#NOMBRE ";
        strcat(newName, name);

        //Enviar Nombre
        if( send(sock,newName,strlen(newName),0) < 0)
        {
            puts("Envio Fallido");
            return 1;
        }


        while(estado)
        {
            printf("\n");
            fgets(message, 1000, stdin);
             
            //Enviar Mensaje
            if( send(sock , message , strlen(message) , 0) < 0)
            {
                puts("Envio Fallido");
                return 1;
            }
            sleep(1);
        }

        close(sock);
        system("clear");
        printf("Adios\n");
         
        return 0;
}