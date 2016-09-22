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
#include <time.h>

#define NUM_MAX_TEMAS 20
FILE *file = NULL;

int NUM_TEMAS = -1;

struct tema
{
	char name[100];
	int numPalabras;
	char palabra[100][100];
	char tip[100][200];
};

typedef struct tema TEMA;

TEMA TEMAS[ NUM_MAX_TEMAS ];

void lee_Temas();
void to_next_token();
void get_token( char *bf );
void to_lowerCase( char *bf );
void revolver( char *bf );
void nuevo_tema();

void to_next_token(  ) {
	char C;
	while( ( C = getc( file ) )  != EOF && C != ':' );
}

void get_token( char *bf ){
	to_next_token();
	int i=0;
	while( ( bf[i] = getc(file) ) != EOF && bf[i] != '\n' ) {
		if( bf[i] != ' ' ) i++;
	}
	bf[i] = '\0';
}

void get_frase( char *bf ){
	to_next_token();
	int i=0;
	while( ( bf[i] = getc(file) ) != EOF && bf[i++] != '\n' );
	bf[i] = '\0';
}

void print_temas() {
	int i, j;

	for( i=0; i < NUM_TEMAS; i++ ) {
		printf("TEMAS %d : %s\n", i+1, TEMAS[i].name );

		for( j=0; j < TEMAS[i].numPalabras; j++ ) {
			printf("\tPalabra %d : %s\n", j+1, TEMAS[i].palabra[j] );
			printf("\tTip %d : %s\n", j+1, TEMAS[i].tip[j] );
		}

	}

}

void lee_Temas(){

	printf(" Leyendo Base de Datos...\n\n");

	int length, i=0, j=0, k=0;
	char bf[200];
	char id[20];

	file = fopen("temas.txt", "r");
	
	if( file == NULL ) {
		printf("No se pudo leer del archivo de configuración\n");
		exit(EXIT_FAILURE);
	}

	/* Leer Numero de temas */
	get_token( bf );
	NUM_TEMAS = atoi( bf );

	printf("Número de Temas : %d\n", NUM_TEMAS);

	for( i = 0; i < NUM_TEMAS; i++ ) {

		/* Leer nombre del tema */
		get_frase( bf );
		strcpy( TEMAS[i].name, bf );

		/* Leer número de palabras por temas */
		get_token( bf );
		TEMAS[i].numPalabras = atoi( bf );

		for( j = 0; j < TEMAS[i].numPalabras ; j++ ) {
			/* Leer Palabra */
			get_token( bf );
			strcpy( TEMAS[i].palabra[j], bf );
			/* Leer Tip */
			get_frase( bf );
			strcpy( TEMAS[i].tip[j], bf );
		}
	}

	//print_temas();

	fclose( file );
}

char lowerCase( char C ) {
	
	if( C > 64 && C < 91 )
		return C + 32;
	return C;
}

void to_lowerCase( char *bf ) {
	int i;
	int length = strlen( bf );

	for( i=0; i < length; i++ ) {
		bf[i] = lowerCase( bf[i] );
	}
}

void revolver( char * bf ) {

	int i, pos, length = strlen( bf );
	char c;

	for( i = 0; i < length; i++ ) {
		pos = rand() % length;
		c = bf[ pos ];
		bf[pos] = bf[i];
		bf[i] = c;
	}

}

void elimina_salto(char *str) {
	int l = strlen( str );
	str[l-1] = '\0';
}

void nuevo_tema(){

	int i,c;

	NUM_TEMAS ++;
	getchar();

	printf(" Escribe el nombre de tu tema: ");
	//scanf("%s", TEMAS[ NUM_TEMAS -1 ].name );
	fgets(TEMAS[ NUM_TEMAS -1 ].name, 200, stdin);
	elimina_salto( TEMAS[ NUM_TEMAS -1 ].name );

	printf(" Escribe el numero de palabras que tiene el tema: ");
	scanf("%d", &TEMAS[ NUM_TEMAS -1 ].numPalabras);
	getchar();

	for( i=0; i<TEMAS[ NUM_TEMAS -1 ].numPalabras; i++) {
		
		printf(" Escribe la palabra %2d: ", i+1);
		//scanf("%s", TEMAS[ NUM_TEMAS -1 ].palabra[i]);
		fgets( TEMAS[ NUM_TEMAS -1 ].palabra[i], 100, stdin);
		elimina_salto( TEMAS[ NUM_TEMAS -1 ].palabra[i] );

		printf(" Escribe un tip de la palabra: ");
		//scanf("%s", TEMAS[ NUM_TEMAS -1 ].tip[i]);
		fgets( TEMAS[ NUM_TEMAS -1 ].tip[i], 100, stdin);
		//elimina_salto( TEMAS[ NUM_TEMAS -1 ].tip[i] );
	}

	//while ((c = getchar()) != '\n' && c != EOF);
}

// int main(int argc, char const *argv[]) {

// 	lee_Temas();

// 	nuevo_tema();

// 	print_temas();

// 	return 0;
// }
