#ifndef GAMECARD_H_
#define GAMECARD_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<commons/collections/list.h>
//UTILS
#include <utils.h>
//THREADS
#include<pthread.h>


typedef struct
{
	int ID;
	int size;
	int posX;
	int posY;
	int cantidad;
	char* pokemon;
} t_new_pokemon_args;


//agrego las variables que se agregan en el archivo de conf ademas del ip y puerto
int TIEMPO_DE_REINTENTO_CONEXION;
int TIEMPO_DE_REINTENTO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;
char* IP_BROKER;
char* PUERTO_BROKER;
t_log* logger;
t_config* config;

pthread_mutex_t semaforoOpen;

void* procesarGetPokemon(void);
void* procesarCatchPokemon(void);
void* procesarNewPokemon(void* args);
void* checkingOpenFile(void* filePath);
void leer_config(void);

void

#endif /* GAMECARD_H_ */
