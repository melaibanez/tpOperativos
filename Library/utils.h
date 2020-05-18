#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<pthread.h>

typedef enum
{
	SUBSCRIBE = 0,
	NEW_POKEMON = 1,
	APPEARED_POKEMON = 2,
	CATCH_POKEMON = 3,
	CAUGHT_POKEMON = 4,
	GET_POKEMON = 5,
	LOCALIZED_POKEMON = 6,
}op_code;

typedef struct {
	op_code cola;
} t_subscribe;

typedef struct
{
	int lengthOfPokemon;
	char* pokemon;
	int posicionX;
	int posicionY;
	int cantidad;
} t_new_pokemon;

typedef struct
{
	int lengthOfPokemon;
	char* pokemon;
	int posicionX;
	int posicionY;
} t_appeared_pokemon;

typedef struct
{
	int lengthOfPokemon;
	char* pokemon;
	int posicionX;
	int posicionY;
} t_catch_pokemon;

typedef struct
{
	bool ok;
} t_caught_pokemon;

typedef struct
{
	int lengthOfPokemon;
	char* pokemon;
} t_get_pokemon;

typedef struct // No se bien como resolver esto TODO
{
	int lengthOfPokemon;
	char* pokemon;
	t_list* listaPosiciones;
} t_localized_pokemon;


typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	int ID;
	t_buffer* buffer;
} t_paquete;

int crear_conexion(char* ip, char* puerto);

void enviar_subscribe(t_subscribe* subscribe, int socket_cliente, int ID);
void enviar_new_pokemon(t_new_pokemon* new_pokemon, int socket_cliente, int ID);
void enviar_appeared_pokemon(t_appeared_pokemon* appeared_pokemon, int socket_cliente, int ID);
void enviar_catch_pokemon(t_catch_pokemon* catch_pokemon, int socket_cliente, int ID);
void enviar_caught_pokemon(t_caught_pokemon* caught_pokemon, int socket_cliente, int ID);
void enviar_get_pokemon(t_get_pokemon* get_pokemon, int socket_cliente, int ID);
void enviar_localized_pokemon(t_localized_pokemon* localized_pokemon, int socket_cliente, int ID);

t_subscribe* des_serializar_subscribe(int socket);
t_new_pokemon* des_serializar_new_pokemon(int socket);
t_appeared_pokemon* des_serializar_appeared_pokemon(int socket);
t_catch_pokemon* des_serializar_catch_pokemon(int socket);
t_caught_pokemon* des_serializar_caught_pokemon(int socket);
t_get_pokemon* des_serializar_get_pokemon(int socket);
t_localized_pokemon* des_serializar_localized_pokemon(int socket); //TODO


void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
t_log* iniciar_log(char* nombreProceso);

#endif /* UTILS_H_ */
