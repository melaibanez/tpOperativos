/*
 * utils.c
 *
 *
 *      Author: fritsplank
 */

#include "utils.h"
#include "errno.h"

void* serializar_paquete(t_paquete* paquete, int *bytes)
{
	int size_serializado = sizeof(op_code) + 3*sizeof(int) + (paquete->buffer->size);

	void* streamFinal = malloc(size_serializado);
	int offset = 0;

	memcpy(streamFinal + offset, &paquete->codigo_operacion, sizeof(op_code));
	offset += sizeof(op_code);
	memcpy(streamFinal + offset, &paquete->ID, sizeof(int));
	offset += sizeof(int);
	memcpy(streamFinal + offset, &paquete->ID_CORRELATIVO, sizeof(int));
	offset += sizeof(int);
	memcpy(streamFinal + offset, &paquete->buffer->size, sizeof(int));
	offset += sizeof(int);

	/* Serializo dependiendo de tipo de mensaje */
	switch(paquete->codigo_operacion){
	case NEW_POKEMON:
		serializar_new_pokemon(&streamFinal, offset, paquete->buffer->stream, bytes);
		break;
	case APPEARED_POKEMON:
		serializar_appeared_pokemon(&streamFinal, offset, paquete->buffer->stream, bytes);
		break;
	case CATCH_POKEMON:
		serializar_catch_pokemon(&streamFinal, offset, paquete->buffer->stream, bytes);
		break;
	case CAUGHT_POKEMON:
		serializar_caught_pokemon(&streamFinal, offset, paquete->buffer->stream, bytes);
		break;
	case GET_POKEMON:
		serializar_get_pokemon(&streamFinal, offset, paquete->buffer->stream, bytes);
		break;
	case LOCALIZED_POKEMON:
		serializar_localized_pokemon(&streamFinal, offset, paquete->buffer->stream, bytes);
		break;
	case RESPUESTA_ID:
		serializar_respuesta_id(&streamFinal, offset, paquete->buffer->stream, bytes);
		break;
	case ACK:
		*bytes = offset;
		break;
	case GAMEBOYSUSCRIBE:
		serializar_gameboy_suscribe(&streamFinal, offset, paquete->buffer->stream, bytes);
		break;
	}
	return streamFinal;
}

int enviar(t_paquete* paquete, int socket_cliente)
{
	int size_serializado;

	void* mensajeAEnviar = serializar_paquete(paquete, &size_serializado);

	int resultadoEnvio = send(socket_cliente, mensajeAEnviar, size_serializado, 0);
//	fprintf(stderr, "socket() failed %s\n", strerror(errno));

	free(mensajeAEnviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	return resultadoEnvio;
}

t_paquete* crearPaqueteCon(void* datos, int sizeOfStream, int Id, int IdCorrelativo, op_code op_code) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = op_code;
	paquete->ID = Id;
	paquete->ID_CORRELATIVO = IdCorrelativo;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = sizeOfStream;
	paquete->buffer->stream = datos;
	return paquete;
}

int enviar_new_pokemon(t_new_pokemon* new_pokemon, int socket_cliente, int Id, int IdCorrelativo) {
	t_paquete* paquete = crearPaqueteCon(new_pokemon, 1 + new_pokemon->lengthOfPokemon + sizeof(uint32_t)*4, Id, IdCorrelativo, NEW_POKEMON);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

int enviar_appeared_pokemon(t_appeared_pokemon* appeared_pokemon, int socket_cliente, int Id, int IdCorrelativo) {
	t_paquete* paquete = crearPaqueteCon(appeared_pokemon, 1 + appeared_pokemon->lengthOfPokemon + sizeof(uint32_t)*3, Id, IdCorrelativo, APPEARED_POKEMON);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

int enviar_catch_pokemon(t_catch_pokemon* catch_pokemon, int socket_cliente, int Id, int IdCorrelativo) {
	t_paquete* paquete = crearPaqueteCon((void*) catch_pokemon, 1 + catch_pokemon->lengthOfPokemon + sizeof(uint32_t)*3, Id, IdCorrelativo, CATCH_POKEMON);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

int enviar_caught_pokemon(t_caught_pokemon* caught_pokemon, int socket_cliente, int Id, int IdCorrelativo) {
	t_paquete* paquete = crearPaqueteCon((void*) caught_pokemon, sizeof(uint32_t), Id, IdCorrelativo, CAUGHT_POKEMON);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

int enviar_get_pokemon(t_get_pokemon* get_pokemon, int socket_cliente, int Id, int IdCorrelativo) {
	t_paquete* paquete = crearPaqueteCon((void*) get_pokemon, 1 + get_pokemon->lengthOfPokemon + sizeof(uint32_t), Id, IdCorrelativo, GET_POKEMON);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

int enviar_localized_pokemon(t_localized_pokemon* localized_pokemon, int socket_cliente, int Id, int IdCorrelativo) {
	uint32_t sizeListaPosiciones = localized_pokemon->cantidadPosiciones * (2 * sizeof(uint32_t));
	t_paquete* paquete = crearPaqueteCon((void*) localized_pokemon, 1 + localized_pokemon->lengthOfPokemon + sizeof(uint32_t)*2 + sizeListaPosiciones, Id, IdCorrelativo, LOCALIZED_POKEMON);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

int enviar_respuesta_id(t_respuesta_id* respuesta_id, int socket_cliente, int Id, int IdCorrelativo){
	t_paquete* paquete = crearPaqueteCon((void*) respuesta_id, sizeof(int), Id, IdCorrelativo, RESPUESTA_ID);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

int enviar_ACK(int socket_cliente, int Id, int IdCorrelativo){
	t_paquete* paquete = crearPaqueteCon(NULL, 0, Id, IdCorrelativo, ACK);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

int enviar_gameboy_suscribe(t_gameboy_suscribe* gameboy_suscribe, int socket_cliente, int Id, int IdCorrelativo){
	t_paquete* paquete = crearPaqueteCon((void*) gameboy_suscribe, sizeof(op_code), Id, IdCorrelativo, GAMEBOYSUSCRIBE);
	int resultadoEnvio = enviar(paquete, socket_cliente);
	return resultadoEnvio;
}

/* Retorna el mensaje completo: Header y Payload ya serializado
 *
 * No me deja por alguna razón llevar esas funciones des_serializar a otro fuente
 * porque me tira que no reconoce t_paquete (idk, no intenté mucho para ser honesto)
 *
 */
t_paquete* recibir_mensaje(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));

	if(recv(socket_cliente, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL) < 1){
		//ACA ENTRA SI HAY ERROR
		free(paquete);
		return NULL;
	}
//	fprintf(stderr, "socket() failed %s\n", strerror(errno));

	recv(socket_cliente, &(paquete->ID), sizeof(int), MSG_WAITALL);
	recv(socket_cliente, &(paquete->ID_CORRELATIVO), sizeof(int), MSG_WAITALL);
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), MSG_WAITALL);

	switch(paquete->codigo_operacion){
		case NEW_POKEMON:
			paquete->buffer->stream = des_serializar_new_pokemon(socket_cliente, paquete->buffer->size);
			break;

		case APPEARED_POKEMON:;
			paquete->buffer->stream = des_serializar_appeared_pokemon(socket_cliente, paquete->buffer->size);
			break;

		case CATCH_POKEMON:;
			paquete->buffer->stream = des_serializar_catch_pokemon(socket_cliente, paquete->buffer->size);
			break;

		case CAUGHT_POKEMON:;
			paquete->buffer->stream = des_serializar_caught_pokemon(socket_cliente, paquete->buffer->size);
			break;

		case GET_POKEMON:;
			paquete->buffer->stream = des_serializar_get_pokemon(socket_cliente, paquete->buffer->size);
			break;

		case LOCALIZED_POKEMON:;
			paquete->buffer->stream = des_serializar_localized_pokemon(socket_cliente, paquete->buffer->size);
			break;

		case RESPUESTA_ID:;
			paquete->buffer->stream = des_serializar_respuesta_id(socket_cliente, paquete->buffer->size);
			break;

		case ACK:
			// No se hace nada porque no tiene payload
			break;

		case GAMEBOYSUSCRIBE:;
			paquete->buffer->stream = des_serializar_gameboy_suscribe(socket_cliente, paquete->buffer->size);
			break;
	}

	return paquete;
}


void liberar_conexion(int socket_cliente) {
	t_log* logger = iniciar_log();
	if (close(socket_cliente) == -1) {
		log_error(logger, "Error al cerrar la conexion");
	}
}

t_log* iniciar_log(void) {
	t_log* logger;
	if((logger = log_create("library.log", "LIBRARY", 1, log_level_from_string("INFO"))) == NULL){
		printf("No pude crear el logger\n");
		exit(1);
	}
	return logger;
}

t_log* iniciar_logger_modulo(char* nombreModulo) {
	t_log* logger;

	char* nombreArchivoLog = malloc(strlen(nombreModulo) + 1);
	strcpy(nombreArchivoLog, nombreModulo);
	strcat(nombreArchivoLog, ".log");
	string_to_upper(nombreModulo);

	if((logger = log_create(nombreArchivoLog, nombreModulo, 1, log_level_from_string("INFO"))) == NULL){
		printf("No pude crear el logger\n");

		free(nombreArchivoLog);
		free(nombreModulo);
		exit(1);
	}

	free(nombreArchivoLog);
	free(nombreModulo);
	return logger;
}
