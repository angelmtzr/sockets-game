/*

   Lectura remota de una palabra para devolver el numero de vocales usando sockets pertenecientes
   a la familia TCP, en modo conexion.
   Codigo del servidor

   Nombre Archivo: tcpserver.c
   Archivos relacionados: num_vocales.h tcpclient.c 
   Fecha: Febrero 2023

   Compilacion: cc tcpserver.c -lnsl -o server
   Ejecución: ./server
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>

#define  DIRSIZE   2048      /* longitud maxima parametro entrada/salida */
#define  PUERTO   5006		/* numero puerto arbitrario */
#define  jsonSIZE  10000
#define  msgSIZE   2048      /* longitud maxima parametro entrada/salida */
   

int                  sd, sd_actual;  /* descriptores de sockets */
int                  addrlen;        /* longitud direcciones */
struct sockaddr_in   sind, pin;      /* direcciones sockets cliente u servidor */


/*  procedimiento de aborte del servidor, si llega una senal SIGINT */
/* ( <ctrl> <c> ) se cierra el socket y se aborta el programa       */
void aborta_handler(int sig){
   printf("....abortando el proceso servidor %d\n",sig);
   close(sd);  
   close(sd_actual); 
   exit(1);
}


int main(){

	char usernameOne[] = "player1";
	char passwordOne[] = "password1";
	char usernameTwo[] = "player2";
	char passwordTwo[] = "password2";

	
	int clientsLimit = 2; // Limite de clientes que acepta el servidor.
	int contClients = 0;  // Clientes conectados al momento.
	pid_t child_pid;
	int mainProcess = getpid(); //ID del proceso principal.
	char  msg[msgSIZE];	     /* parametro entrada y salida */

	char  json[jsonSIZE];	

	/*
	When the user presses <Ctrl + C>, the aborta_handler function will be called, 
	and such a message will be printed. 
	Note that the signal function returns SIG_ERR if it is unable to set the 
	signal handler, executing line 54.
	*/	
   if(signal(SIGINT, aborta_handler) == SIG_ERR){
   	perror("Could not set signal handler");
      return 1;
   }
       //signal(SIGINT, aborta);      /* activando la senal SIGINT */

/* obtencion de un socket tipo internet */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

/* asignar direcciones en la estructura de direcciones */
	sind.sin_family = AF_INET;
	sind.sin_addr.s_addr = INADDR_ANY;   /* INADDR_ANY=0x000000 = yo mismo */
	sind.sin_port = htons(PUERTO);       /*  convirtiendo a formato red */

/* asociando el socket al numero de puerto */
	if (bind(sd, (struct sockaddr *)&sind, sizeof(sind)) == -1) {
		perror("bind");
		exit(1);
	}
			

/* ponerse a escuchar a traves del socket */
	if (listen(sd, 5) == -1) {
		perror("listen");
		exit(1);
	}		

/* esperando que un cliente solicite un servicio */

	for(;;){
			if ((sd_actual = accept(sd, (struct sockaddr *)&pin, &addrlen)) == -1) {
				perror("accept");
				exit(1);
			}else if(mainProcess == getpid() && contClients < clientsLimit){
				child_pid = fork();
				contClients++;	
			}
		
		if(child_pid == 0){
			int authFlag = 0;
			char delimiter[] = " ";
			char userReceived[msgSIZE];
			char passwordReceived[msgSIZE];
			char sendAuth[msgSIZE];

			char sigue='S';
			char msgReceived[1000];

			strcpy(json," ");

			while(!authFlag){
				if (recv(sd_actual, msg, sizeof(msg), 0) == -1){
					perror("recv");
					exit(1);
				}
				char *ptr = strtok(msg, delimiter);

				if(ptr != NULL){
					strcpy(userReceived, ptr);
					ptr = strtok(NULL, delimiter);
					strcpy(passwordReceived, ptr);
				}

				printf("Auth Flag: %d\n", authFlag);
				printf("%s\n%s\n", userReceived, passwordReceived);
				
				if(!strcmp(usernameOne, userReceived) && !strcmp(passwordOne, passwordReceived)){
					strcpy(sendAuth, "1 1");
					if (send(sd_actual, sendAuth, strlen(sendAuth), 0) == -1) {
						perror("send");
						exit(1);
					}
					authFlag = 1;
				}else if(!strcmp(usernameTwo, userReceived) && !strcmp(passwordTwo, passwordReceived)){
					strcpy(sendAuth, "1 2");
					if (send(sd_actual, sendAuth, strlen(sendAuth), 0) == -1) {
						perror("send");
						exit(1);
					}
					authFlag = 1;
				}else{
					strcpy(sendAuth, "0 0");
					printf("Cliente ingreso datos incorrectos.\n");
					if (send(sd_actual, sendAuth, strlen(sendAuth), 0) == -1) {
						perror("send");
						exit(1);
					}
				}
			}
			
			while(sigue=='S'){				
				/* tomar un mensaje del cliente */

				int n = recv(sd_actual, msg, sizeof(msg), 0);
				if (n == -1) {
					perror("recv");
					exit(1);
				}		
				//msg[n] = '\0';
				printf("Client sent: %s \n", msg);

				if((strcmp(msg,"close")==0)){ //it means that the conversation must be closed
					sigue='N';
				}else{
					//convert msg received to json format
					strcpy(msgReceived,"{'");
					strcat(msgReceived,msg);
					strcat(msgReceived,"' : '");
					strcat(msgReceived,msg);
					strcat(msgReceived,"'},");
					strcat(json,msgReceived);
					//----------------------------------
				}
					
			}

			if (send(sd_actual, json, strlen(json), 0) == -1) {
				perror("send");
				exit(1);
			}
		}
	}

	close(sd_actual);  
   	close(sd);
   	printf("Conexion cerrada\n");
	return 0;
}
