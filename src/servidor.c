/*
//	##################################
//	#	Elaborado por:				 #
//	#								 #
//	#   Peralta Rosales Oscar Esaú   #
//	#   Ventura Mijangos Geovanni	 #
//	##################################
*/

#include "temas.c"
#include "main.c"
#define ADIVINADA 0
#define TOTAL_ADIVINADAS 1
#define PLAYER 2
#define READY 3
#define FIN 4

int *vectorRequest = NULL;
char *share_pal = NULL;
int id_R, id_RR;
int TEMA_ACTUAL;

void createVRequest();
void send_message( int nc, char *message );
void inicia_juego();
void adivina_la_palabra();
void get_pal_tip(char *s, char *tip);
void salir_juego();

int main(int argc, char** argv) {

	int pid;
	srand(time(NULL));

	//DEBUG = 0;

	/* Puerto alterno*/
	if( argc == 2 ) {
		PORT = atoi( argv[1] );
	}

	read_IP_file();

	if ( NUM_HOST < 2 ) {
		printf(" Lo siento, pero debes tener por lo menos un amigo más para poder jugar :( ...\n\n");
		exit( EXIT_SUCCESS );
	}

	createVRequest();

	/* Socket Servidor */
	if( (Socket_Servidor = Abrir_Socket_INET( NULL, 0 )) ==-1 ) {
		printf("Error al abrir socket Servidor\n");
		exit(EXIT_FAILURE);
	}

	puts("Iniciando conexiones...");
	/* Proceso que levanta sockets a la escucha para cada máquina que se conecte a este servidor */
	recibe_conexiones();

	/* Realiza la conexión con los demas servidores */
	inicia_conexiones();

	puts("Todas las conexiones han sido establecidas");

	/* Proceso para verificar la funcionalidad del Moderador */
	if( My_ID == COORDINADOR ) {
		lee_Temas();
		inicia_juego();
	}
	else 
		adivina_la_palabra();

	wait();

	cerrar_conexiones();
	
	shmctl (id_R, IPC_RMID,(struct shmid_ds *)NULL);
	shmctl (id_RR, IPC_RMID,(struct shmid_ds *)NULL);
	
	exit(EXIT_SUCCESS);
}

void inicia_juego() {
	int pid,i,j, Nip_player;
	int state_palabra[100];
	int pal;
	char salir;
	char str[200];
	
	vectorRequest[ PLAYER ] = search_Nip( 0 );

	while( 1 ) {

		salir = 'o';

		vectorRequest[ TOTAL_ADIVINADAS ] = 0;
		
		memset(state_palabra, 0, sizeof(int) * 100);

		printf("\tElige un tema para iniciar el juego:\n\n");
		
		for( i = 0; i < NUM_TEMAS; i++) {
			printf("\t\t%2d) %s\n", i+1, TEMAS[i].name );
		}

		printf("\t\t%2d) %s\n", i+1, "Agregar un tema" );

		printf("\n\t\t%2d) Salir\n",i+2);

		printf("\tNúmero del tema o opción: ");
		scanf("%d", &TEMA_ACTUAL);

		if( TEMA_ACTUAL == i + 2 )
			salir_juego();

		if( TEMA_ACTUAL == i+1 ) {
			system("clear");
			nuevo_tema();
		}
		
		TEMA_ACTUAL--;

		while( vectorRequest[ TOTAL_ADIVINADAS ] < TEMAS[TEMA_ACTUAL].numPalabras ) {



			Nip_player = search_Nip( vectorRequest[ PLAYER ] );
			
			do {
				pal = rand() % TEMAS[TEMA_ACTUAL].numPalabras;
			}while( state_palabra[pal] == 1);
			
			state_palabra[ pal ] = 1;
			
			//printf(" El jugador %2d inicia una nueva ronda\n\n", Nip_player +1 );
			printf(" Se inicia una nueva ronda\n\n");
			printf("\tPalabra: %s\n", TEMAS[ TEMA_ACTUAL ].palabra[ pal ]);
			printf("\tTip: %s\n", TEMAS[ TEMA_ACTUAL ].tip[ pal ]);

			sprintf( str,"%s-%s", TEMAS[ TEMA_ACTUAL ].palabra[ pal ], TEMAS[TEMA_ACTUAL].tip[ pal ]);

			send_message( Nip_player, str );

			while( vectorRequest[ ADIVINADA ] == -1 ) {
				sleep(1);
			}

			//puts("La palabra fue adivinada\n\n");
				
			vectorRequest[ ADIVINADA ] = -1;
		}

		puts(" Todas las palabras se han adivinado");

		puts(" ¿Deseas terminar el juego? [s/n] : ");

		while( salir != 's' && salir != 'S' && salir != 'n' && salir != 'N'  ) {
			scanf( "%c", &salir );
		}

		if( salir == 'n' || salir == 'N' )
			continue;

		salir_juego();
		break;

	}

}

void salir_juego() {
	int i;
	for( i = 0; i < NUM_HOST; i++ )
		send_message( i, "FIN DEL JUEGO" );

	puts("FIN DEL JUEGO");
}

void get_pal_tip(char *s, char *tip) {
	int i,j;
	for( i=0; share_pal[i] != '-'; i++ ){
		s[i] = share_pal[i];
	}
	s[i++] = '\0';
	for( j=0; share_pal[i] != '\0'; i++ )
		tip[j++] = share_pal[i];
	tip[j] = '\0';
}

void adivina_la_palabra() {

	int pid, i, band=1, j;
	char s[100];
	char s2[100];
	char s3[100];
	char tip[200];

	puts(" === ADIVINA PALABRAS ===\n");
	printf(" === JUGADOR %2d ===\n", My_ID + 1 );

	while( 1 ) {
		sleep(1);
		if( vectorRequest[ FIN ] == 1 ){
			for( i = 0; i < NUM_HOST; i++ )
				send_message( i, "FIN DEL JUEGO" );
			puts("Todas las palabras se han adivinado");
			puts("FIN DEL JUEGO");
			return;
		}

		//system("clear");
		// if( vectorRequest[ ADIVINADA ] == 1 ) {
		// 		vectorRequest[ ADIVINADA ] = 0;
		// 		puts("\n ¡Rayos!  Te ganaron la palabra\n\n");
		// }

		if( vectorRequest[ READY ] == 1 ) {
			vectorRequest[ READY ] = 0;

			for( i=0; i<NUM_HOST; i++ )
				send_message(i,"ES MI TURNO");

			system("clear");
			puts(" ¡ ES TU TURNO !\n");

			printf("\tTu palabra a adivinar es:\n\n");

			get_pal_tip(s, tip);

			strcpy(s3,s);
			revolver(s3);
			printf("\t\t%s\n\n", s3 );
			printf(" TIP : %s\n\n", tip );

			to_lowerCase(s);

			if( ( pid = fork() ) == 0 ) {
				
				while(1) {
					
					printf("Escribe la palabra: ");
					scanf("%s", s2);
					to_lowerCase(s2);
					
					if( strcmp( s2, s ) == 0 ) {
						
						puts(" ¡ ADIVINASTE ! \n");
						
						for( i=0; i<NUM_HOST; i++ )
							send_message(i,"YO ADIVINE");
						
						vectorRequest[ ADIVINADA ] = 1;
						exit( EXIT_SUCCESS );
					}
					else
						puts("Intenta de nuevo...");
				}
			}

			for( j = 0; j < 15; j++ ) {
				if( vectorRequest[ ADIVINADA ] == 1 )
					break;
				sleep(1);
			}
			
			if( vectorRequest[ ADIVINADA ] == 1 ) {
				vectorRequest[ ADIVINADA ] = 0;
			}
			else {
				puts("\nSe te acabó el tiempo");
				send_message( NEXTHOST, share_pal );
				kill( pid, 1 );
			}
			band = 1;
		}
		else if( band == 1 ) {
			puts("\n Espera que sea tu turno :( ...\n\n");
			band = 0;
		}
	}
}

/* Escucha peticiones de un socket asociado */
int escuchar( int fd_Host, int nc) {

	int size, Nip;
	Nip = search_IP( inet_ntoa(Host[nc].sin_addr) );
	char palabra[100];

	while( 1 ) {

		size = recv( fd_Host, buffer, MAXSIZE, 0);
		buffer[size] = '\0';

		if( strcmp( buffer, "" ) == 0 ) {
			continue;
		}
			
		//printf("Mensaje recibido de: %s => %s\n", inet_ntoa(Host[nc].sin_addr), buffer );

		if( strcmp(buffer, "FIN DEL JUEGO") == 0 ) {
			vectorRequest[ FIN ] = 1;
			break;
		}

		if( strcmp(buffer, "ES MI TURNO") == 0 ) {
			printf("\n Ahora es el turno del jugador %2d\n", host_ID[ Nip ] + 1 );
			continue;
		}

		if( strcmp(buffer, "YO ADIVINE") == 0 ) {

			if( My_ID != COORDINADOR ) {
				printf(" ¡Rayos! Han adivinado la palabra\n");
			}
			else {
				printf(" La palabra fue adivinada\n");
				vectorRequest[ PLAYER ] = ( host_ID[ Nip ] + 1) % NUM_HOST;
				vectorRequest[ ADIVINADA ] = 1;	
			}

			vectorRequest[ TOTAL_ADIVINADAS ] ++;
			continue;
		}

		strcpy( share_pal, buffer );
		vectorRequest[ READY ] = 1;

	}

	return 1;
}

void createVRequest() {
	key_t Clave, Clave2;
	
	Clave = ftok ("/bin/ls", 33);
	Clave2 = ftok ("/bin/cp", 97);

	if ( Clave == -1 || Clave2 == -1 ) {
		printf("Error al obtener clave para memoria compartida.\n");
		exit(EXIT_FAILURE);
	}

	id_R = shmget (Clave, sizeof(int)*10, 0777 | IPC_CREAT);
	id_RR = shmget (Clave2, sizeof(char)*100, 0777 | IPC_CREAT);

	if ( id_R == -1 || id_RR == -1 ) {
		printf("Error al obtener identificador para memoria compartida\n");
		exit(EXIT_FAILURE);
	}

	vectorRequest = (int *)shmat (id_R, (char *)0, 0);
	share_pal = (char *)shmat(id_RR, (char*)0, 0);

	if ( vectorRequest == NULL || share_pal == NULL ) {
		printf("Error de acceso a memoria compartida");
		exit(EXIT_FAILURE);
	}

	memset(vectorRequest, -1, sizeof(int) * 10);
}

void send_message( int nc, char *message ) {
	send(socket_Host_ip[ nc ], message, strlen(message), 0);
}

