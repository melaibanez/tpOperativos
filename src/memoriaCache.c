/*
 * memoriaCache.c
 *
 *  Created on: 20 jun. 2020
 *      Author: utnso
 */
#include "memoriaCache.h"

void cachearMensaje(t_metadata * meta, void * mensaje) {

	void * mensajeACachear = malloc(meta->tamanioMensaje);

	switch (meta->tipoMensaje) {
	case NEW_POKEMON:
		cachearNewPokemon(meta, mensaje, &mensajeACachear);
		break;
	case LOCALIZED_POKEMON:
		cachearLocalizedPokemon(meta, mensaje,&mensajeACachear);
		break;
	case GET_POKEMON:
		cachearGetPokemon(meta, mensaje, &mensajeACachear);
		break;
	case APPEARED_POKEMON:
		cachearAppearedPokemon(meta, mensaje,&mensajeACachear);
		break;
	case CATCH_POKEMON:
		cachearCatchPokemon(meta, mensaje, &mensajeACachear);
		break;
	case CAUGHT_POKEMON:
		cachearCaughtPokemon(meta, mensaje, &mensajeACachear);
		break;
	default:
		log_error(logger, "No se puede cachear el mensaje. No coincide el tipo mensaje.");
	}

	meta->posicion = particionLibre(meta->tamanioMensajeEnMemoria);

	while(meta->posicion == -1){
		meta->posicion = particionLibre(meta->tamanioMensajeEnMemoria);
	}

	escribirMemoria(mensajeACachear, meta);
	free(mensajeACachear);
}

void cachearNewPokemon(t_metadata * meta, void * mensaje,void ** mensajeACachear) {

	int offset = 0;

	t_new_pokemon * new_pokemon = mensaje;

	memcpy(*mensajeACachear + offset, &new_pokemon->lengthOfPokemon, offset);
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, new_pokemon->pokemon, strlen(new_pokemon->pokemon));
	offset += strlen(new_pokemon->pokemon);
	memcpy(*mensajeACachear + offset, &new_pokemon->cantidad, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, &new_pokemon->posicion->posicionX, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, &new_pokemon->posicion->posicionY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	meta->tamanioMensajeEnMemoria = offset;
}
void cachearLocalizedPokemon(t_metadata * meta, void * mensaje, void ** mensajeACachear) {

	int offset = 0;

	t_localized_pokemon * localized_pokemon = mensaje;

	memcpy(*mensajeACachear + offset, &localized_pokemon->lengthOfPokemon, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, localized_pokemon->pokemon, strlen(localized_pokemon->pokemon));
	offset += strlen(localized_pokemon->pokemon);
	memcpy(*mensajeACachear + offset, &localized_pokemon->cantidadPosiciones, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (int i = 0; i < localized_pokemon->cantidadPosiciones; i++) {
		t_posicion * posiciones = list_get(localized_pokemon->listaPosiciones, i);
		memcpy(*mensajeACachear + offset, &posiciones->posicionX, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(*mensajeACachear + offset, &posiciones->posicionY, sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}
	meta->tamanioMensajeEnMemoria = offset;
}

void cachearGetPokemon(t_metadata * meta, void * mensaje, void ** mensajeACachear) {

	int offset = 0;

	t_get_pokemon * get_pokemon = mensaje;

	memcpy(*mensajeACachear + offset, &get_pokemon->lengthOfPokemon, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, get_pokemon->pokemon, strlen(get_pokemon->pokemon));
	offset += strlen(get_pokemon->pokemon);

	meta->tamanioMensajeEnMemoria = offset;
}

void cachearAppearedPokemon(t_metadata * meta, void * mensaje, void ** mensajeACachear) {

	int offset = 0;

	t_appeared_pokemon * appeared_pokemon = mensaje;

	memcpy(*mensajeACachear + offset, &appeared_pokemon->lengthOfPokemon, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, appeared_pokemon->pokemon, strlen(appeared_pokemon->pokemon));
	offset += strlen(appeared_pokemon->pokemon);
	memcpy(*mensajeACachear + offset, &appeared_pokemon->posicion->posicionX, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, &appeared_pokemon->posicion->posicionY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	meta->tamanioMensajeEnMemoria = offset;
}

void cachearCatchPokemon(t_metadata * meta, void * mensaje, void ** mensajeACachear) {

	int offset = 0;

	t_catch_pokemon * catch_pokemon = mensaje;

	memcpy(*mensajeACachear + offset, &catch_pokemon->lengthOfPokemon, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, catch_pokemon->pokemon, strlen(catch_pokemon->pokemon));
	offset += strlen(catch_pokemon->pokemon);
	memcpy(*mensajeACachear + offset, &catch_pokemon->posicion->posicionX, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(*mensajeACachear + offset, &catch_pokemon->posicion->posicionY, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	meta->tamanioMensajeEnMemoria = offset;
}

void cachearCaughtPokemon(t_metadata * meta, void * mensaje, void ** mensajeACachear) {

	int offset = 0;

	t_caught_pokemon * caught_pokemon = mensaje;

	memcpy(*mensajeACachear + offset, &caught_pokemon->ok, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	meta->tamanioMensajeEnMemoria = offset;
}

/* ----------------------------------------------*/
//SI VAS A ENVIAR UN MENSAJE DEBERAS CREAR EN TU FUNCION UN VOID * MENSAJE = MALLOC(meta->tamanioMensaje);
void * descachearMensaje(void * mensajeEnMemoria, t_metadata * meta) {
	void * estructuraPokemonReturn;
	switch (meta->tipoMensaje) {
	case NEW_POKEMON:
		estructuraPokemonReturn = descachearNewPokemon(mensajeEnMemoria);
		break;
	case LOCALIZED_POKEMON:
		estructuraPokemonReturn = descachearLocalizedPokemon(mensajeEnMemoria);
		break;
	case GET_POKEMON:
		estructuraPokemonReturn = descachearGetPokemon(mensajeEnMemoria);
		break;
	case APPEARED_POKEMON:
		estructuraPokemonReturn = descachearAppearedPokemon(mensajeEnMemoria);
		break;
	case CATCH_POKEMON:
		estructuraPokemonReturn = descachearCatchPokemon(mensajeEnMemoria);
		break;
	case CAUGHT_POKEMON:
		estructuraPokemonReturn = descachearCaughtPokemon(mensajeEnMemoria);
		break;
	default:
		log_error(logger,
				"No se puede descachear el mensaje. No coincide el tipo mensaje.");
	}
	return estructuraPokemonReturn;
}

void * descachearNewPokemon(void* mensajeEnMemoria) {
	void* recorrerStream = mensajeEnMemoria;
	t_new_pokemon * new_pokemon = malloc(sizeof(t_new_pokemon));
	memcpy(&(new_pokemon->lengthOfPokemon), recorrerStream, sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);

	new_pokemon->pokemon = malloc(new_pokemon->lengthOfPokemon + 1);
	memcpy(new_pokemon->pokemon, recorrerStream, new_pokemon->lengthOfPokemon);
	recorrerStream += new_pokemon->lengthOfPokemon;
	*(new_pokemon->pokemon + new_pokemon->lengthOfPokemon) = '\0';
	new_pokemon->posicion = malloc(sizeof(t_posicion));
	memcpy(&(new_pokemon->posicion->posicionX), recorrerStream,
			sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);
	memcpy(&(new_pokemon->posicion->posicionY), recorrerStream,
			sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);
	memcpy(&(new_pokemon->cantidad), recorrerStream, sizeof(uint32_t));

	return new_pokemon;

}
void * descachearGetPokemon(void* mensajeEnMemoria) {
	void* recorrerStream = mensajeEnMemoria;
	t_get_pokemon* get_pokemon = malloc(sizeof(t_get_pokemon));

	memcpy(&(get_pokemon->lengthOfPokemon), recorrerStream, sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);
	get_pokemon->pokemon = malloc(get_pokemon->lengthOfPokemon + 1);
	memcpy(get_pokemon->pokemon, recorrerStream, get_pokemon->lengthOfPokemon);
	*(get_pokemon->pokemon + get_pokemon->lengthOfPokemon) = '\0';
	return get_pokemon;

}
void * descachearLocalizedPokemon(void* mensajeEnMemoria) {
	void* recorrerStream = mensajeEnMemoria;
	t_localized_pokemon* localized_pokemon = malloc(
			sizeof(t_localized_pokemon));

	memcpy(&(localized_pokemon->lengthOfPokemon), recorrerStream,
			sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);
	localized_pokemon->pokemon = malloc(localized_pokemon->lengthOfPokemon + 1);
	memcpy(localized_pokemon->pokemon, recorrerStream,
			localized_pokemon->lengthOfPokemon);
	recorrerStream += localized_pokemon->lengthOfPokemon;
	*(localized_pokemon->pokemon + localized_pokemon->lengthOfPokemon) = '\0';
	memcpy(&(localized_pokemon->cantidadPosiciones), recorrerStream,
			sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);

	localized_pokemon->listaPosiciones = list_create();

	int cantidadPosiciones = (int) localized_pokemon->cantidadPosiciones;

	for (int i = 0; i < cantidadPosiciones; i++) {
		t_posicion* posicion = malloc(sizeof(t_posicion));

		memcpy(&(posicion->posicionX), recorrerStream, sizeof(uint32_t));
		recorrerStream += sizeof(uint32_t);
		memcpy(&(posicion->posicionY), recorrerStream, sizeof(uint32_t));
		recorrerStream += sizeof(uint32_t);

		list_add(localized_pokemon->listaPosiciones, posicion);
	}
	return localized_pokemon;
}
void * descachearAppearedPokemon(void* mensajeEnMemoria) {
	void* recorrerStream = mensajeEnMemoria;
	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));

	memcpy(&(appeared_pokemon->lengthOfPokemon), recorrerStream,
			sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);
	appeared_pokemon->pokemon = malloc(appeared_pokemon->lengthOfPokemon + 1);
	memcpy(appeared_pokemon->pokemon, recorrerStream,
			appeared_pokemon->lengthOfPokemon);
	recorrerStream += appeared_pokemon->lengthOfPokemon;
	*(appeared_pokemon->pokemon + appeared_pokemon->lengthOfPokemon) = '\0';
	appeared_pokemon->posicion = malloc(sizeof(t_posicion));
	memcpy(&(appeared_pokemon->posicion->posicionX), recorrerStream,
			sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);
	memcpy(&(appeared_pokemon->posicion->posicionY), recorrerStream,
			sizeof(uint32_t));
	return appeared_pokemon;
}
void * descachearCatchPokemon(void* mensajeEnMemoria) {
	void* recorrerStream = mensajeEnMemoria;
	t_catch_pokemon* catch_pokemon = malloc(sizeof(t_catch_pokemon));

	memcpy(&(catch_pokemon->lengthOfPokemon), recorrerStream, sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);
	catch_pokemon->pokemon = malloc(catch_pokemon->lengthOfPokemon + 1);
	memcpy(catch_pokemon->pokemon, recorrerStream,
			catch_pokemon->lengthOfPokemon);
	recorrerStream += catch_pokemon->lengthOfPokemon;
	*(catch_pokemon->pokemon + catch_pokemon->lengthOfPokemon) = '\0';
	catch_pokemon->posicion = malloc(sizeof(t_posicion));
	memcpy(&(catch_pokemon->posicion->posicionX), recorrerStream,
			sizeof(uint32_t));
	recorrerStream += sizeof(uint32_t);
	memcpy(&(catch_pokemon->posicion->posicionY), recorrerStream,
			sizeof(uint32_t));

	return catch_pokemon;
}
void * descachearCaughtPokemon(void* mensajeEnMemoria) {
	void* recorrerStream = mensajeEnMemoria;
	t_caught_pokemon* caught_pokemon = malloc(sizeof(t_caught_pokemon));

	memcpy(&(caught_pokemon->ok), recorrerStream, sizeof(uint32_t));

	return caught_pokemon;
}

void escribirMemoria(void * mensaje, t_metadata * meta) { //OK
	/*if (!mensaje) {
	 //log_info(logger, "No se agregó ningun mensaje en la memCache");
	 return;
	 }*/
	memcpy((memoriaCache + meta->posicion), mensaje,meta->tamanioMensajeEnMemoria);
	log_info(logger, "Se cacheo el mensaje de tamanio: %d", meta->tamanioMensajeEnMemoria); //NO OBLIGATORIO
	log_info(logger, "Se almacena un mensaje en la posicion [%d].", meta->posicion); //OBLIGATORIO (6)
	modificarUltimaReferencia(meta, 'X');
}

void * leerMemoria(t_metadata * meta) { //OK
	modificarUltimaReferencia(meta, 'L');
	return descachearMensaje(memoriaCache + meta->posicion, meta);
}
void modificarUltimaReferencia(t_metadata * meta, char tipoReferencia) {
	meta->flagLRU += 1;
	meta->ultimaReferencia = tipoReferencia;
}

/* el operando >> mueve los bits del numero en la izquierda hacia la izquierda
 * utilizandolo con los valores de abajo logramos que el valor se incremente en potencias de dos
 * sacando la minima potencia de 2 que sea mayor al numero enviado por parametro
 */
int potenciaDeDosProxima(uint32_t tamanio) {
    tamanio -= 1;
    tamanio = tamanio | (tamanio >> 1);
    tamanio = tamanio | (tamanio >> 2);
    tamanio = tamanio | (tamanio >> 4);
    tamanio = tamanio | (tamanio >> 8);
    tamanio = tamanio | (tamanio >> 16);
    return tamanio + 1;
}

int esPotenciaDeDos(int numero){
	if(!(numero & (numero - 1))){
		return 1;
	}
	else{
		return 0;
	}
}
