/*
  Cliente del programa numero 1, de la materia Cómputo Distribuido
  

   Lectura remota de un directorio usando sockets pertenecientes
   a la familia TCP, en modo conexion.
   Codigo del cliente.

   Nombre Archivo: tcpclient.c
   Archivos relacionados: tcpserver.c num_vocales.h
   Fecha: Febrero 2023

   Compilacion: cc tcpclient.c -lnsl -o client

   Ejecucion: tcpclient <host> <palabra>   
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <netdb.h>

#define  MSGSIZE    2048   /* longitud maxima parametro entrada/salida */
#define  PUERTO    5006   /* numero puerto arbitrario */
#define  MSGSIZE    2048   /* longitud de los mensajes */

int main(argc, argv)
   int    argc;  
   char  *argv[];
{
	char                dir[MSGSIZE];  /* parametro entrada y salida */
	char				dirResponse[MSGSIZE];

	char 				username[MSGSIZE];
	char 				password[MSGSIZE];
	char				auth[MSGSIZE];
	int					authFlag = 0;
	char 				authResponse[MSGSIZE];
	char 				authCode[MSGSIZE];
	char 				playerCode[MSGSIZE];
	char                delimiter[] = " ";
	int 				gameOption;

	int                 sd;		   /* descriptores de sockets    */
	struct hostent 	   *hp;		   /* estructura del host        */
	struct sockaddr_in sin, pin; 	   /* direcciones socket        */
    int                *status;        /* regreso llamada sistema */
    char               *host;          /* nombre del host */

/* verificando el paso de parametros */

    if ( argc != 2) {
        fprintf(stderr,"Error uso: %s <host> \n",argv[0]);
        exit(1);
    } 
    

/* encontrando todo lo referente acerca de la maquina host */
	host = argv[1];
	if ( (hp = gethostbyname(host)) == 0) {
		perror("gethosbyname");
		exit(1);
	}
		
/* llenar la estructura de direcciones con la informacion del host */
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *) (hp->h_addr))->s_addr;
	pin.sin_port = htons(PUERTO);                    

/* obtencion de un socket tipo internet */
	if ( (sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

/* conectandose al PUERTO en el HOST  */
	if ( connect(sd, (struct sockaddr *)&pin, sizeof(pin)) == -1) {
		perror("connect");
		exit(1);
	}

	while(gameOption != 2){
		printf("Elija una opción:\n1.Instrucciones de uso y juego.\n2.Autentificar e iniciar juego.\n");
		scanf("%d", &gameOption);

		if(gameOption == 1){
			printf( "Para empezar el juego tienes que presionar la tecla '2' en el menú principal."
					" Primero tendrás que ingresar un usuario y una contraseña que sean correctas. Una vez"
					" ingresadas deberás esperar a que otro jugador ingrese al juego. Una vez que ambos jugadores"
					" se hayan autenticado exitosamente comenzará el juego. El servidor asignará aleatoriamente quien"
					" empieza el juego y que color de ficha tendrá cada jugador. El objetivo del juego es ser el primero"
					" en colocar cuatro fichas del mismo color en línea, ya sea vertical, horizontal o diagonalmente."
					" Cada jugador coloca una ficha en su turno en una columna vacía, y la ficha cae hasta el lugar"
					" más bajo posible en esa columna. Para esto, en el tablero habrá un boton por cada columna, deberás"
					" hacer clic en la columna donde desees colocar tu ficha. Una vez terminado el juego se cerrará el tablero"
					" y mostrará al jugador ganador.");
		}
	}
	
	while(!authFlag){
		printf("Introduzca su usario: ");
		scanf("%s", username);
		printf("Introduzca la contraseña: ");
		scanf("%s", password);
		strcpy(auth, username);
		strcat(auth, " ");
		strcat(auth, password);

		if ( send(sd, auth, sizeof(auth), 0) == -1 ) {
				perror("send");
				exit(1);
		}

		if ( recv(sd, authResponse, sizeof(authResponse), 0) == -1 ) {
			perror("recv");
			exit(1);
		}
		
		char *ptr = strtok(authResponse, delimiter);

		if(ptr != NULL){
			strcpy(authCode, ptr);
			ptr = strtok(NULL, delimiter);
			strcpy(playerCode, ptr);
		}

		if(!strcmp(authCode, "0")){
			printf("Usuario o contraseña incorrecta. Favor de intentar otra vez.\n");
		}else{
			authFlag = 1;
		}
	}
	
/* enviar mensaje al PUERTO del  servidor en la maquina HOST */
	/* while(strcmp(dir, "close") != 0){
        scanf("%s", dir);
	    if ( send(sd, dir, sizeof(dir), 0) == -1 ) {
		    perror("send");
		    exit(1);
	    }
    } */

/* esperar por la respuesta */
	

/* imprimimos el resultado y cerramos la conexion del socket */
	printf("El resultado de la expresion es: %s \n", dirResponse);
	close(sd);
	return 0;
}
