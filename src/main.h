/*
//	##################################
//	#	Elaborado por:				 #
//	#								 #
//	#   Peralta Rosales Oscar Esaú   #
//	#   Ventura Mijangos Geovanni	 #
//	##################################
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <sys/un.h>
#include <sys/shm.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
#include <time.h>

#define STD_INPUT  0
#define STD_OUTPUT 1

#define MAXSIZE 1000
#define MAXHOST 10
#define BACKLOG MAXHOST /* El número de conexiones permitidas */


int NUM_HOST;  /* Número de máquinas en la red sin contar la maquina local*/
int PORT = 3550;

int DEBUG = 0;

struct sockaddr_in Servidor; /* Estructura Socket servidor principal */
struct sockaddr_in Host[MAXHOST]; /* Estructura sockets servidores secundarios */
struct hostent *Host_tmp; /* Temporal para conversiones de IP's */

int Socket_Servidor; 
int socket_Host[MAXHOST]; /* Sockets de servidores en espera de conexiones */
int socket_Host_ip[MAXHOST]; /* Sockets de servidores para realizar conexiones */

char host_ip[MAXHOST][20]; /* IP's leidas del archivo de configuración */
char buffer[MAXSIZE]; /* Buffer de uso general*/

int host_ID[MAXHOST]; /* ID's de los host*/

char My_ip[20]; /* IP de la máquina local */
int My_ID; /* Identificador de la Máquina Local */
int COORDINADOR = -1;
int COORDINADOR_POS = -1;

int read_IP_file(); /* Lee el archivo de configuración con las IP's */
int Abrir_Socket_INET(char *ip, int nc); /* Abre los sockets para máquina local y externas*/
int escuchar(int fd_Host, int nc); /* Escucha peticiones de un socket asociado */
int Aceptar_Conexiones(int fd_Servidor,int nc); /* Acepta una conexion entrante a un socket */

void abrir_socket_IP(); /* Sockets para enviar mensajes del recurso */
void recibe_conexiones(); /* Proceso que levanta sockets a la escucha para cada máquina que se conecte a este servidor */
void inicia_conexiones(); /* Realiza la conexión con los demas servidores */
void cerrar_conexiones(); /* Cierra los sockets creados */
int search_IP( char *ip );