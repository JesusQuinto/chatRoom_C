#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>

#include<pthread.h>

pthread_t hilo;
int sock;
int state = 1;

/* Strip CRLF */
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
        puts(buff_in);

        /* Special options */
        if(buff_in[0] == '\\')
        {
            char *command, *param;
            command = strtok(buff_in," ");

            if(!strcmp(command, "\\QUIT"))
            {
                state = 0;
                break;
            }
            else
            {
                puts("Server reply :");
                puts(buff_in);
            }
        }   
    }
    pthread_exit((void*)(1));
}
 
int main(int argc , char *argv[])
{
        struct sockaddr_in server;
        char message[1000];
         
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            printf("Could not create socket");
        }
        puts("Socket created");
         
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_family = AF_INET;
        server.sin_port = htons( 5000 );
 
        //Connect to remote server
        if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
        {
                perror("connect failed. Error");
                return 1;
        }
         
        puts("Connected\n");
         
        
        pthread_create(&hilo,NULL,( void *)&observar,NULL) ;

        do
        {
            printf("Enter message : ");
            scanf("%s" , message);
             
            //Send some data
            if( send(sock , message , strlen(message) , 0) < 0)
            {
                puts("Send failed");
                return 1;
            }
            sleep(1);
        }
        while(state);

        printf("Adios\n");

         
        close(sock);
        return 0;
}