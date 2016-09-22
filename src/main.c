/*
//	##################################
//	#	Elaborado por:				 #
//	#								 #
//	#   Peralta Rosales Oscar Esaú   #
//	#   Ventura Mijangos Geovanni	 #
//	##################################
*/


#include "main.h"

int NEXTHOST = -1;

int Abrir_Socket_INET(char * ip, int nc) {

	int fd;

	if( ( fd = socket(AF_INET, SOCK_STREAM, 0) ) == -1 )
		return -1;

	if( ip == NULL ) {
		Servidor.sin_family = AF_INET;
		Servidor.sin_port = htons(PORT);
		Servidor.sin_addr.s_addr = INADDR_ANY;
		bzero(&(Servidor.sin_zero),8);

		/* Asociar el socket con el puerto */ 
		if( ( bind( fd, (struct sockaddr *)&Servidor, sizeof(struct sockaddr) ) ) == -1 ) {
			printf("Puerto no disponible\n");
			close( fd );
			return -1;
		}
		/* Escuchando conexiones entrantes*/
		if( listen( fd, BACKLOG ) == -1 ) {
			close( fd );
			return -1;
		}
	}
	else {
		
		if( ( Host_tmp = gethostbyname(ip) ) == NULL )
    	  	return -1;

		Host[nc].sin_family = AF_INET;
		Host[nc].sin_port = htons(PORT);
		Host[nc].sin_addr = *((struct in_addr *)Host_tmp->h_addr);	
		bzero(&(Host[nc].sin_zero),8);
		printf("Conectando con: %s\n", ip );
		if( connect(fd, (struct sockaddr *)&Host[nc], sizeof (Host[nc])) == -1 ) {
			printf("Error al conectarse con el Servidor : %s\n",ip);
			return -1;
		}
		printf("Conectado a : %s\n", ip );
	}

	return fd;
}

int Aceptar_Conexiones(int fd_Servidor, int nc) {
	int fd;
	unsigned int sin_size = sizeof(struct sockaddr_in);
	if( ( fd = accept( fd_Servidor, (struct sockaddr *)&(Host[nc]), &sin_size ) ) == -1 )
		return -1;
	printf("Conexión establecida desde: %s\n", inet_ntoa(Host[nc].sin_addr));
	
	return fd;
}

void recibe_conexiones() {

	int nc = 0, pid;

	if( (pid = fork()) == -1 ) {
		 		printf("Error al bifurcar proceso\n");
		 		exit(EXIT_FAILURE);
	}
	
	if( pid == 0 ) {
		// if( DEBUG == 0 ) {
		// 	close(STD_INPUT);
		// 	close(STD_OUTPUT);
		// }
		printf("Esperando Conexiones de los demás servidores...\n");
		for( nc = 0; nc < NUM_HOST ; nc++ ) {

			if( (pid = fork()) == -1 ) {

			 		printf("Error al bifurcar proceso\n");
			 		exit(EXIT_FAILURE);
			}

			if( pid == 0)  {
				// if( DEBUG == 0 ) {
				// 	close(STD_INPUT);
				// 	close(STD_OUTPUT);
				// }
				
				if( ( socket_Host[nc] = Aceptar_Conexiones(Socket_Servidor, nc) ) == -1 ) {
		 			printf("No se puede abrir socket Cliente\n");
		 			exit(EXIT_FAILURE);
		 		}

				escuchar( socket_Host[nc], nc );
			
				close(socket_Host[nc]);
			
				exit(EXIT_SUCCESS);
			}

		}

		for( nc = 0; nc < NUM_HOST ; nc++ )
			wait();

		exit(EXIT_SUCCESS);
	}
}

void inicia_conexiones() {
	int nc;
	/* Tiempo de espera para levantar las demas máquinas*/
	sleep(7);
	printf("Estableciendo conexión con los demas servidores...\n");
	/* Abre los sockets para conectarse como cliente con las demás máquinas*/
	
	for( nc=0; nc < NUM_HOST ; nc++)
		if( (socket_Host_ip[nc] = Abrir_Socket_INET( host_ip[nc], nc )) ==-1 ) {
			printf("Error al abrir socket con %s\n", host_ip[nc]);
			exit(EXIT_FAILURE);
		}

	sleep(4);
}

void cerrar_conexiones() {
	
	int nc;

	for( nc=0; nc < NUM_HOST ; nc++)
		close(socket_Host_ip[nc]);

	close(Socket_Servidor);
}

int read_IP_file() {

	printf(" Leyendo archivo de configuración...\n\n");

	FILE *file_conf = NULL;
	int length, i=0, j=0, k=0;
	char bf[20];
	char id[20];

	file_conf = fopen("conf", "r");
	
	if( file_conf == NULL ) {
		printf("No se pudo leer del archivo de configuración\n");
		exit(EXIT_FAILURE);
	}

	/* Leer Numero de host*/
	while( ( bf[i] = getc(file_conf) ) != EOF && bf[i] != '\n' ) {
		if(bf[i]!=' ') i++;
	}
	bf[i] = '\0';
	NUM_HOST = atoi(bf);
	NUM_HOST--;

	/*Leer ip del servidor*/
	i=0;
	while( ( bf[i] = getc(file_conf) ) != EOF && bf[i] != '\n' ) {
		if(bf[i]!=' ') i++;
	}
	bf[i] = '\0';

	for( i = 0; bf[i] != '\0'; i++)
		if( bf[i+1] == '-' )
			bf[i+1] = '\0';

	for( i = i+1; bf[i] != '\0'; i++)
		id[j++] = bf[i];
	id[j] = '\0';

	strcpy( My_ip, bf );
	My_ID = atoi(id);

	if( My_ID > COORDINADOR ) {
		COORDINADOR = My_ID;
		COORDINADOR_POS = -1;
	}
			

	/* Leer ips restantes */
	for (i = 0; i < NUM_HOST; i++) {
		j=0;
		while( ( host_ip[i][j] = getc(file_conf) ) != EOF && host_ip[i][j] != '\n' ) {
			if( host_ip[i][j] !=' ') j++;
		}
		host_ip[i][j] = '\0';

		for( j = 0; host_ip[i][j] != '\0'; j++)
			if( host_ip[i][j+1] == '-' )
				host_ip[i][j+1] = '\0';
		k = 0;
		for( j = j+1; host_ip[i][j] != '\0'; j++)
			id[k++] = host_ip[i][j];
		id[k] = '\0';

		host_ID[i] = atoi(id);

		if( host_ID[i] > COORDINADOR ) {
			COORDINADOR = host_ID[i];
			COORDINADOR_POS = i;
		}
		
	}

	
	printf(" Número de Servidores: %d\n", NUM_HOST+1);

	if( My_ID == COORDINADOR )
		printf(" IP Servidor Local: %s | ID = %d  =>  COORDINADOR\n\n", My_ip, My_ID);
	else
		printf(" IP Servidor Local: %s | ID = %d \n\n", My_ip, My_ID);

	for (i = 0; i < NUM_HOST; i++) {
		if( host_ID[i] != COORDINADOR )
			printf(" Servidor %2d: %s | ID = %d\n", i+1, host_ip[i], host_ID[i] );
		else
			printf(" Servidor %2d: %s | ID = %d  =>  COORDINADOR\n", i+1, host_ip[i], host_ID[i] );
	}

	puts("");

	fclose(file_conf);

	NEXTHOST = ( My_ID + 1 ) % NUM_HOST;
	NEXTHOST = search_Nip( NEXTHOST );

	if( COORDINADOR_POS == NEXTHOST ) {
		NEXTHOST = ( NEXTHOST + 1 ) % NUM_HOST;
		NEXTHOST = search_Nip( NEXTHOST );
	}

}

int search_IP( char * ip ) {
	int i;
	for( i = 0 ; i < NUM_HOST; i++ )
		if( strcmp( host_ip[ i ], ip ) == 0 )
			return i;
	return -1;
}

int search_Nip( int id ) {
	int i;
	for( i = 0 ; i < NUM_HOST; i++ )
		if( host_ID[i] == id  )
			return i;
	return -1;
}
