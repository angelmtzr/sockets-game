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

void initGameConnect4(int ,int );
   

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

	int client1,client2;
	char usernameOne[] = "player1";
	char passwordOne[] = "password1";
	char usernameTwo[] = "player2";
	char passwordTwo[] = "password2";

	
	int clientsLimit = 2; // Limite de clientes que acepta el servidor.
	int contClients = 0;  // Clientes conectados al momento.
	pid_t child_pid;
	
	int playerOneFlag = 0;
	int playerTwoFlag = 0;

	int mainProcess = getpid(); //ID del proceso principal.
	char  msg[msgSIZE];	     /* parametro entrada y salida */

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
		perror("[-] Could not create socket\n");
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
	if (listen(sd, 6) != 0) {
		perror("[-]Error in binding.\n");
		exit(1);
	}else{
		printf("[+]Listening...\n");
	}

	
/* esperando que un cliente solicite un servicio */


	while(1){
	 	client1 = accept(sd, (struct sockaddr *)&pin,&addrlen);
	 	printf("Player 1 joined ...\n");
	 	client2 = accept(sd, (struct sockaddr *)&pin,&addrlen);
	 	printf("Player 2 joined ...\n");
		if(fork()==0)
			initGameConnect4(client1, client2);
	 }

	close(sd_actual);  
   	close(sd);
   	printf("Conexion cerrada\n");
	return 0;
}


void initGameConnect4(int client1, int client2){
	char  msg[msgSIZE];	     /* parametro entrada y salida */

	int authFlagOne = 0;
	int authFlagTwo = 0;

	char delimiter[] = " ";
	char userReceived[msgSIZE];
	char passwordReceived[msgSIZE];
	char sendAuth[msgSIZE];
	char initGame[] = "1";
	int playerOneFlag = 0;
	int playerTwoFlag = 0;

	char usernameOne[] = "edgar";
	char passwordOne[] = "contra1";
	char usernameTwo[] = "anapau";
	char passwordTwo[] = "contra2";

	if (send(client1, "Jugador 2 ingreso.", 18, 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(client2, "Jugador 1 ya ha ingresado.", 26, 0) == -1) {
		perror("send");
		exit(1);
	}

	while(!authFlagOne){
		if (recv(client1, msg, sizeof(msg), 0) == -1){
				perror("recv");
				exit(1);
		}
		char *ptr = strtok(msg, delimiter);

		if(ptr != NULL){
			strcpy(userReceived, ptr);
			ptr = strtok(NULL, delimiter);
			strcpy(passwordReceived, ptr);
		}

		if(!strcmp(usernameOne, userReceived) && !strcmp(passwordOne, passwordReceived)){
			strcpy(sendAuth, "1");
			if (send(client1, sendAuth, strlen(sendAuth), 0) == -1) {
				perror("send");
				exit(1);
			}
			printf("Jugador uno autenticado. \n");
			authFlagOne = 1;
			playerOneFlag = 1;

		}else if(!strcmp(usernameTwo, userReceived) && !strcmp(passwordTwo, passwordReceived)){
			strcpy(sendAuth, "1");
			if (send(client1, sendAuth, strlen(sendAuth), 0) == -1) {
				perror("send");
				exit(1);
			}
			printf("Jugador uno autenticado. \n");
			authFlagOne = 1;
			playerOneFlag = 1;

		}else{
			strcpy(sendAuth, "0");
			printf("Cliente ingreso datos incorrectos.\n");
			if (send(client1, sendAuth, strlen(sendAuth), 0) == -1) {
				perror("send");
				exit(1);
			}
		}

	}
	
	while(!authFlagTwo){
		if (recv(client2, msg, sizeof(msg), 0) == -1){
				perror("recv");
				exit(1);
		}
		char *ptr = strtok(msg, delimiter);

		if(ptr != NULL){
			strcpy(userReceived, ptr);
			ptr = strtok(NULL, delimiter);
			strcpy(passwordReceived, ptr);
		}

		if(!strcmp(usernameOne, userReceived) && !strcmp(passwordOne, passwordReceived)){
			strcpy(sendAuth, "1");
			if (send(client2, sendAuth, strlen(sendAuth), 0) == -1) {
				perror("send");
				exit(1);
			}
			printf("Jugador dos autenticado. \n");
			authFlagTwo = 1;
			playerTwoFlag = 1;

		}else if(!strcmp(usernameTwo, userReceived) && !strcmp(passwordTwo, passwordReceived)){
			strcpy(sendAuth, "1");
			if (send(client2, sendAuth, strlen(sendAuth), 0) == -1) {
				perror("send");
				exit(1);
			}
			printf("Jugador dos autenticado. \n");
			authFlagTwo = 1;
			playerTwoFlag = 1;

		}else{
			strcpy(sendAuth, "0");
			printf("Cliente ingreso datos incorrectos.\n");
			if (send(client2, sendAuth, strlen(sendAuth), 0) == -1) {
				perror("send");
				exit(1);
			}
		}

	}

	if(playerOneFlag == 1 && playerTwoFlag == 1){
		printf("Inicia juego.\n");
		if (send(client1, initGame, strlen(initGame), 0) == -1) {
			perror("send");
			exit(1);
		}
		if (send(client2, initGame, strlen(initGame), 0) == -1) {
			perror("send");
			exit(1);
		}
	}
}
