/*
	---Servidor---------------------------------
	Integrantes:
	Jesus Miguel Quinto Teran C.I = 25619081
	Edgar Joseph Rivero Matheus C.I = 20428267
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>

#define limiteMaximo	50

//Variable de Seccion Critica
static unsigned int cli_count = 0;

static int uid = 10;
sem_t sem1;

//Estructura de Cliente
typedef struct {
	struct sockaddr_in addr;
	int connfd;		
	int uid;		
	char name[32];		
} client_t;

client_t *clients[limiteMaximo];

void uso()
{
  printf("Uso:\n");
  printf("    servidor [port]\n");
  exit(0);
}

void queue_add(client_t *cl)
{
	int i;
	for ( i=0; i<limiteMaximo; i++)
	{
		if (!clients[i])
		{
			clients[i] = cl;
			return;
		}
	}
}

void queue_delete(int uid)
{
	int i;
	for (i=0; i<limiteMaximo; i++)
	{
		if(clients[i])
		{
			if(clients[i]->uid == uid)
			{
				clients[i] = NULL;
				return;
			}
		}
	}
}

void send_message(char *s, int uid)
{
	int i;
	for ( i=0; i<limiteMaximo; i++)
	{
		if(clients[i])
		{
			if(clients[i]->uid != uid)
			{
				if( send(clients[i]->connfd , s , strlen(s) , 0) < 0)
		        {
		            puts("Fallo el envio");
		        }
			}
		}
	}
}

void send_message_all(char *s)
{
	int i;
	for(i=0;i<limiteMaximo;i++)
	{
		if(clients[i])
		{
			if( send(clients[i]->connfd , s , strlen(s) , 0) < 0)
	        {
	            puts("Fallo el envio");
	        }
		}
	}
}

void send_message_self(const char *s, int connfd)
{
	if( send(connfd , s , strlen(s) , 0) < 0)
	{
        puts("Fallo el envio");
    }
}

void send_message_client(char *s, int uid)
{
	int i;
	for(i=0;i<limiteMaximo;i++)
	{
		if(clients[i])
		{
			if(clients[i]->uid == uid)
			{
				if( send(clients[i]->connfd , s , strlen(s) , 0) < 0)
				{
					puts("Fallo el envio");
				}
			}
		}
	}
}

void send_active_clients(int connfd)
{
	int i;
	char s[64];
	for(i=0;i<limiteMaximo;i++)
	{
		if(clients[i])
		{
			sprintf(s, "<<CLIENT %d | %s\r\n", clients[i]->uid, clients[i]->name);
			send_message_self(s, connfd);
		}
	}
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


//Gestion de Cliente
void *gClient(void *arg)
{
	char buff_out[1024];
	char buff_in[1024];
	int rlen;

	//Seccion Critica controlada por un semaforo//////
		sem_wait(&sem1);							//
		cli_count++;    							//
		sem_post(&sem1);							//
	//////////////////////////////////////////////////

	client_t *cli = (client_t *)arg;

	printf("[%d] Se ha conectado al chat\n", cli->uid);

	/* Receive input from client */
	while((rlen = read(cli->connfd, buff_in, sizeof(buff_in)-1)) > 0)
	{
        buff_in[rlen] = '\0';
        buff_out[0] = '\0';
		strip_newline(buff_in);

		//Descartar mensaje vacio
		if(!strlen(buff_in))
		{
			continue;
		}
	
		//Opciones especiales
		if(buff_in[0] == '#')
		{
			char *command, *param;
			command = strtok(buff_in," ");
			if(!strcmp(command, "#SALIR"))
			{
				send_message_self("#SALIR", cli->connfd);
				break;
			}

			else if(!strcmp(command, "#NOMBRE"))
			{
				param = strtok(NULL, " ");
				if(param){
					strcpy(cli->name, param);
					
					sprintf(buff_out, "Hola %s!\r\n", cli->name);
					send_message_self(buff_out, cli->connfd);

					sprintf(buff_out, "[%s] Esta conectado al chat\r\n", cli->name);
					send_message(buff_out,cli->uid);
				}
				else
				{
					send_message_self("El nombre no debe ser vacio\r\n", cli->connfd);
				}
			}

			else if(!strcmp(command, "#HISTORIAL"))
			{
				sprintf(buff_out, "<<CLIENTS %d\r\n", cli_count);
				send_message_self(buff_out, cli->connfd);
				send_active_clients(cli->connfd);
			}

			
			else if(!strcmp(command, "#ACTIVOS"))
			{
				sprintf(buff_out, "Clientes: %d\r\n", cli_count);
				send_message_self(buff_out, cli->connfd);
				send_active_clients(cli->connfd);
			}

			else if(!strcmp(command, "#AYUDA"))
			{
				strcat(buff_out, "#SALIR     Salirce del chat\r\n");
				strcat(buff_out, "#HISTORIAL   Mostrar Historial\r\n");
				strcat(buff_out, "#NOMBRE     <name> Cambia el Nombre\r\n");
				strcat(buff_out, "#ACTIVOS   Mostrar los Cliente Activos\r\n");
				strcat(buff_out, "#AYUDA     Mostrar Ayuda\r\n");
				send_message_self(buff_out, cli->connfd);
			}
			
			else
			{
				send_message_self("No es un comando valido\r\n", cli->connfd);
			}
		}

		else
		{
			//Enviar Mensaje
			sprintf(buff_out, "[%s] %s\r\n", cli->name, buff_in);
			send_message(buff_out, cli->uid);
		}
	}

	//Cerrar Conexion
	close(cli->connfd);
	sprintf(buff_out, "[%s] Ha salido del chat\r\n", cli->name);
	send_message(buff_out, cli->uid);

	//Eliminar Cliente
	queue_delete(cli->uid);
	printf("[%d] ha abandonado\n", cli->uid);
	free(cli);

	//Seccion Critica controlada por un semaforo//////
		sem_wait(&sem1);							//
		cli_count--;    							//
		sem_post(&sem1);							//
	//////////////////////////////////////////////////

	pthread_detach(pthread_self());
	
	return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        uso();    
    }

    //Inicializacion del semaforo
    // sem_init(referencia, compartido entre proceosos, valor de inicializacion);
    sem_init(&sem1, 0, 1);

	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	pthread_t tid;

	int port = (atoi(argv[1]));

	//Configuracion del Sokect
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	printf("%d\n",atoi(argv[1]));

	//Remover Senal SIGPIPE
	signal(SIGPIPE, SIG_IGN);
	
	//Enlace
	if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Erro en bind");
		return 1;
	}

	//Listar
	if (listen(listenfd, 10) < 0)
	{
		perror("Error en listen");
		return 1;
	}

	printf("Servidor Iniciado\n");
	printf("Host: 127.0.0.1\n");
	printf("Port: %d\n",port);

	//Aceptar clientes
	while(1)
	{
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		//Solo permite limiteMaximo en una session
		if((cli_count+1) == limiteMaximo)
		{
			printf("Chat lleno!\n");
			printf("Notificaremos cuando un usuario salga\n");
			printf("para que vuelvas a intentar conectarte\n");
			close(connfd);
			continue;
		}

		//Configuracion del cliente
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->addr = cli_addr;
		cli->connfd = connfd;
		cli->uid = uid++;
		sprintf(cli->name, "%d", cli->uid);

		queue_add(cli);

		//Crea un hilo del clieente para ser manejado por el servido
		pthread_create(&tid, NULL, &gClient, (void*)cli);

		sleep(1);
	}
}
