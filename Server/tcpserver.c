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
int check_winner(int board[6][7]);
   

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
	char tokenPositionOne[msgSIZE];
	char tokenPositionTwo[msgSIZE];
	char opMove[msgSIZE];
	char turnStr[msgSIZE];

	char usernameOne[] = "cliente1";
	char passwordOne[] = "contra1";
	
	char usernameTwo[] = "cliente2";
	char passwordTwo[] = "contra2";

	char delimiter[] = " ";
	char userReceived[msgSIZE];
	char passwordReceived[msgSIZE];
	char sendAuth[msgSIZE];
	char initGame[] = "1";
	int authFlagOne = 0;
	int authFlagTwo = 0;
	int playerOneFlag = 0;
	int playerTwoFlag = 0;

	int board[6][7];
	int winner;
	char columnReceived[msgSIZE];
	char rowReceived[msgSIZE];
	int row = 0;
	int column = 0;
	memset(board, 0, sizeof(board));

	int turn = 1;


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

	while(1){
		sprintf(turnStr, "%d", turn);
		printf("\nTurn %d\n",turn);
		if(turn%2 != 0){
			sleep(1);
			//send tokenPositionTwo only if turn > 1
			if (send(client1, turnStr, strlen(turnStr), 0) == -1) {
				perror("send");
				exit(1);
			}
			if(turn > 1){
				sleep(1);
				if (send(client1, opMove, strlen(opMove), 0) == -1) {
					perror("send");
					exit(1);
				}
			}
			if (recv(client1, tokenPositionOne, sizeof(tokenPositionOne), 0) == -1){
				perror("recv");
				exit(1);
			}
			strcpy(opMove, tokenPositionOne);

			char *ptr = strtok(tokenPositionOne, delimiter);

			if(ptr != NULL){
				strcpy(columnReceived, ptr);
				ptr = strtok(NULL, delimiter);
				strcpy(rowReceived, ptr);
			}
			row = atoi(rowReceived);
			column = atoi(columnReceived);

			printf("Player 1 pressed: Row %d Column %d\n", row+1, column+1);

			board[5 - row][column] = 1;
		}else{
			sleep(1);	
			// send tokenPositionOne
			if (send(client2, turnStr, strlen(turnStr), 0) == -1) {
				perror("send");
				exit(1);
			}
			sleep(1);
			if (send(client2, opMove, strlen(opMove), 0) == -1) {
				perror("send");
				exit(1);
			}
			if (recv(client2, tokenPositionTwo, sizeof(tokenPositionTwo), 0) == -1){
				perror("recv");
				exit(1);
			}
			strcpy(opMove, tokenPositionTwo);
			
			char *ptr = strtok(tokenPositionTwo, delimiter);

			if(ptr != NULL){
				strcpy(columnReceived, ptr);
				ptr = strtok(NULL, delimiter);
				strcpy(rowReceived, ptr);
			}
			row = atoi(rowReceived);
			column = atoi(columnReceived);

			printf("Player 2 pressed: Row %d Column %d\n", row+1, column+1);

			board[5 - row][column] = 2;
		}

		printf("Board: \n");
		for (int i = 0; i < 6; i++) {
    		for (int j = 0; j < 7; j++) {
      			printf("%d ", board[i][j]);
    		}
    		printf("\n");
  		}
		winner = check_winner(board);
		if(winner == 1)
			printf("Player one with red tokens wins!\n");

		else if(winner == 2)
			printf("Player two with yellow tokens wins!\n");

		turn++;
	}
}

int check_winner(int board[6][7]){
  // Check for horizontal wins
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 4; j++) {
      if (board[i][j] == board[i][j+1] &&
          board[i][j] == board[i][j+2] &&
          board[i][j] == board[i][j+3] &&
          board[i][j] != 0) {
        return board[i][j];
      }
    }
  }

  // Check for vertical wins
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 7; j++) {
      if (board[i][j] == board[i+1][j] &&
          board[i][j] == board[i+2][j] &&
          board[i][j] == board[i+3][j] &&
          board[i][j] != 0) {
        return board[i][j];
      }
    }
  }

  // Check for diagonal wins (top-left to bottom-right)
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      if (board[i][j] == board[i+1][j+1] &&
          board[i][j] == board[i+2][j+2] &&
          board[i][j] == board[i+3][j+3] &&
          board[i][j] != 0) {
        return board[i][j];
      }
    }
  }

  // Check for diagonal wins (bottom-left to top-right)
  for (int i = 3; i < 6; i++) {
    for (int j = 0; j < 4; j++) {
      if (board[i][j] == board[i-1][j+1] &&
          board[i][j] == board[i-2][j+2] &&
          board[i][j] == board[i-3][j+3] &&
          board[i][j] != 0) {
        return board[i][j];
      }
    }
  }

  // No winner found
  return 0;
}