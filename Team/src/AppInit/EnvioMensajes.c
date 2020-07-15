
#include "EnvioMensajes.h"

static void agregarPokemonSiLoNecesita(char* nombrePokemon, t_posicion posicion);
static void agregarComoIdCorrelativoLocalized(int idCorrelativo);
static void agregarComoIdCorrelativoCaught(int idCorrelativo, Entrenador* entrenadorEsperando);
static void recibirIdCatch(Entrenador* entrenador);
static int puedeSeguirRecibiendo();
static char* charResultado(uint32_t ok);
//static int tieneComoIdCorrelativoLocalized(int idBuscado);
static int tieneComoIdCorrelativoCaught(int idBuscado);
static Entrenador* entrenadorQueTieneId(int idCatchQueResponde);
static void procesarEspera(Entrenador*  entrenador, uint32_t atrapo);

typedef bool(*erasedTypeFilter)(void*);

//////////GET-LOCALIZED//////////////
void enviarGetDesde(int socketGet){
	quickLog("$-Esta enviando los get por cada pokemon objetivo necesario");
	pthread_mutex_lock(&mutexObjetivosGlobales);
	for(int index=0; index<list_size(objetivosGlobales);index++){
		PokemonEnElMapa* poke = list_get(objetivosGlobales, index);
		t_get_pokemon* getPoke = crearEstructuraGetDesde(poke);
		enviar_get_pokemon(getPoke, socketGet, -1, -1);
		//recibirIdGet(socketGet);

		//free(getPoke); ->DOUBLE FREE
	}

	pthread_mutex_unlock(&mutexObjetivosGlobales);
}

//TODO: verificar que si no lo recibe por un tiempo es que el broker no funciona
void recibirIdGet(int socketGet) {
	t_paquete* paqueteIdRecibido = recibir_mensaje(socketGet);
	t_respuesta_id* idGet = paqueteIdRecibido->buffer->stream;
	agregarComoIdCorrelativoLocalized(idGet->idCorrelativo);
}

void agregarComoIdCorrelativoLocalized(int idCorrelativo){
	//lista de ids correlativos globales que se mandaron
	//recien se agregan cuando recibo la respuesta del broker

	list_add(idsCorrelativosLocalized, &idCorrelativo);
}

t_paquete* recibirLocalizedYGuardalos(int socketLocalized) {

	t_paquete* paqueteLocalized = recibir_mensaje(socketLocalized);
	t_localized_pokemon* localized = paqueteLocalized->buffer->stream;
	while(puedeSeguirRecibiendo()) {
		//TODO: Hacer list_iterate para poder mostrar por consola la lista de posiciones que recibe localized
		log_info(LO, "Se recibio el Localized | Pokemon: %s | Cantidad de posiciones: %d", localized->pokemon, localized->cantidadPosiciones);

		quickLog("$-Se recibio un localized");
		//si el id correlativo del localized recibido coincide con algunos de los que tengo en mi lista de correlativos mandados
		//if(tieneComoIdCorrelativoLocalized(paqueteLocalized->ID_CORRELATIVO) == 1) {
		int cantidad = (int) (localized->cantidadPosiciones);
		for(int index=0; index<cantidad; index++){
			//cada posicion recibida en el localized del poke que necesito cazar la agrego en la lista de pokemonesLibres
			t_posicion* posicion = list_get(localized->listaPosiciones,index);

			agregarPokemonSiLoNecesita(localized->pokemon, *posicion);

		}


		log_info(logger, "$-Se recibieron el localized | Pokemon: %s | Cantidad de posiciones: %d ", localized->pokemon, localized->cantidadPosiciones);

		return paqueteLocalized;
	}
	return NULL;
}

//si ya no hay mas objetivos globales quiere decir que todos fueron encontrados a traves de un localized o un appeared
int puedeSeguirRecibiendo() {
	pthread_mutex_lock(&mutexPokemonesRecibidos);
	t_list* pokesR = pokemonesRecibidos;
	int cantidadRecibidos = list_size(pokemonesRecibidos);
	pthread_mutex_unlock(&mutexPokemonesRecibidos);
	log_info(logger, "La cantidad total es: %d y la cantidad recibida es: %d", cantidadDeEspeciesTotales, cantidadRecibidos);
	return cantidadRecibidos <= cantidadDeEspeciesTotales;
}

//int tieneComoIdCorrelativoLocalized(int idBuscado) {
//	typedef bool(*erasedTypeFind)(void*);
//
//	int existe(int idExistente) {
//		return idExistente == idBuscado;
//	}
//	if(list_is_empty(idsCorrelativosLocalized) == 1) {
//		return 0;
//	}
//	//me fijo de la lista de idsCorrelativos que mande como get, si coincide con el id del que recien llego
//	return list_find(idsCorrelativosLocalized, (erasedTypeFind)existe) != NULL;
//}

//////////APPEARED//////////////
t_paquete* recibirAppearedYGuardarlos(int socketAppeared) {

	quickLog("$-Esta por recibir el appeared");

	//sudo strace -s 255 -p 4299

	t_paquete* paqueteAppeared = recibir_mensaje(socketAppeared);
	//quickLog("Recibe el appeared");
	if (paqueteAppeared != NULL) {
		t_appeared_pokemon* appeared = paqueteAppeared->buffer->stream;


			agregarPokemonSiLoNecesita(appeared->pokemon, *(appeared->posicion));

			log_info(LO, "Se recibio el Appeared | Pokemon: %s - Posicion X: %d - Posicion Y: %d", appeared->pokemon, appeared->posicion->posicionX, appeared->posicion->posicionY);

		//	free(appeared->pokemon);
		//	free(appeared->posicion);
		//	free(appeared);
		//	free(paqueteAppeared->buffer);
		//	free(paqueteAppeared);
			log_info(logger, "$-Se recibio el appeared | Pokemon: %s - Posicion X: %d - Posicion Y: %d", appeared->pokemon, appeared->posicion->posicionX, appeared->posicion->posicionY);

			free(appeared);
			free(paqueteAppeared->buffer);
			free(paqueteAppeared);
			return paqueteAppeared;
	}

	return NULL;

}

//si tengo ese pokemon como objetivo lo agregego en la lista de pokemones libres
void agregarPokemonSiLoNecesita(char* nombreNuevoPoke, t_posicion posicionNuevoPoke){

	//si ese pokemon lo tengo como objetivo
	pthread_mutex_lock(&mutexObjetivosGlobales);
	t_list* pokemonesAAtrapar = objetivosGlobales;
	//en el nombre de los globales hay cualquier cosa
	if(buscarPorNombre(nombreNuevoPoke, pokemonesAAtrapar) != NULL) {

		//ya tengo uno de esos pokes libres en el mapa y esta en la misma posicion
		pthread_mutex_lock(&mutexPokemonesLibres);
		if(buscarPorNombre(nombreNuevoPoke, pokemonesLibres) != NULL && sonLaMismaPosicion(buscarPorNombre(nombreNuevoPoke, pokemonesLibres)->posicion, posicionNuevoPoke)) {
			PokemonEnElMapa* pokeExistente = buscarPorNombre(nombreNuevoPoke, pokemonesLibres);

			pokeExistente->cantidad ++;
			pthread_mutex_unlock(&mutexPokemonesLibres);
		} else {
			//solo lo agrego a la lista
			PokemonEnElMapa* pokemonNuevo = newPokemon();
			setPosicionTo(pokemonNuevo, posicionNuevoPoke);
			setNombreTo(pokemonNuevo, nombreNuevoPoke);
			setCantidadTo(pokemonNuevo, 1);
			list_add(pokemonesLibres, pokemonNuevo);
			pthread_mutex_lock(&mutexPokemonesRecibidos);
			list_add(pokemonesRecibidos, pokemonNuevo);
			pthread_mutex_unlock(&mutexPokemonesRecibidos);
			pthread_mutex_unlock(&mutexPokemonesLibres);
		}

	}
	pthread_mutex_unlock(&mutexObjetivosGlobales);

//	pthread_mutex_lock(&mutexPokemonesLibres);
//	log_info(logger, "$-Ahora la cantidad de pokemones libres es: %d", list_size(pokemonesLibres));
//	t_list* libres = pokemonesLibres;
//	pthread_mutex_unlock(&mutexPokemonesLibres);

}




//////////CATCH-CAUGHT/////////////
// cuando un entrenador manda este mensaje, ese poke ya no deberia estar en la lista de pokes libres
//funcion llamada con un semaforo -> solo 1 puede atrapar a la vez y agregar un id correlativo a la lista a la vez
void enviarCatchDesde(Entrenador* entrenadorEsperando){

	quickLog("$-Esta enviando el catch del pokemon que esta por ser atrapado");
	PokemonEnElMapa* pokemonPorAtrapar = entrenadorEsperando->movimientoEnExec->pokemonNecesitado;
	t_catch_pokemon* catchPoke = crearEstructuraCatchDesde(pokemonPorAtrapar);
	//entrenadorQueEspera = entrenadorEsperando;
	enviar_catch_pokemon(catchPoke, socketCatch, -1, -1);

	entrenadorEsperando->ciclosCPUConsumido += 1;
	log_info(LO, "El entrenador %d consumio %d ciclos de CPU", entrenadorEsperando->numeroEntrenador, entrenadorEsperando->ciclosCPUConsumido);

	//el entrenador que mando el catch de ese pokemon necesita guardarse el id de ese que mando
	//para saber que respuesta de caught es de el
	quickLog("$-Esta esperando recibir el id de su catch enviado");
	recibirIdCatch(entrenadorEsperando);


}

void recibirIdCatch(Entrenador* entrenador) {
	sem_wait(&semaforoCorrelativos);
	t_list* ids = idsCorrelativosCaught;
	sem_post(&semaforoCorrelativos);

	t_paquete* paqueteIdRecibido = recibir_mensaje(socketIdCatch);
	if(paqueteIdRecibido != NULL){
		t_respuesta_id* idCatch = paqueteIdRecibido->buffer->stream;

		agregarComoIdCorrelativoCaught(idCatch->idCorrelativo, entrenador);
		quickLog("$-Se agrego como id en el entrenador necesario");
		log_info(LO, "Se recibio el Id del Catch del entrenador %d | Id: %d", entrenador->numeroEntrenador, idCatch->idCorrelativo);

		log_info(LO, "El entrenador %d paso a block esperando la respuesta del catch mandado", entrenador->numeroEntrenador);
		pasarABlockEsperando(entrenador);
		quickLog("$-Se paso a estado bloqueado esperando respuesta");

		free(idCatch);
		free(paqueteIdRecibido->buffer);
		free(paqueteIdRecibido);
	} else {
		//si se corto la conexion
		log_info(LO, "Se corto la conexion con el Broker. Por default, el entrenador atrapo al pokemon");
		//agrego el pokemon atrapado y cambio al entrenador de estado
		agregarAtrapado(entrenador, entrenador->movimientoEnExec->pokemonNecesitado);
		estadoSiAtrapo(entrenador);
		free(paqueteIdRecibido);
	}

}

//no tengo como chequear que ese id sea el mio
void agregarComoIdCorrelativoCaught(int idCorrelativo, Entrenador* entrenadorEsperando){
	//lista de ids correlativos globales que se mandaron
	//recien se agregan cuando recibo la respuesta del broker

	pthread_mutex_lock(&entrenadorEsperando->mutexCorrelativo);
	entrenadorEsperando->idCorrelativoDeEspera = idCorrelativo;
	pthread_mutex_unlock(&entrenadorEsperando->mutexCorrelativo);
	//sem_post(&semaforoEntrenadorEsperando);


	sem_wait(&semaforoCorrelativos);
	list_add(idsCorrelativosCaught, (void*) idCorrelativo);
	log_info(logger, "$-Ahora la cantidad de ids correlativos esperando respuestas caught es: %d", list_size(idsCorrelativosCaught));
	sem_post(&semaforoCorrelativos);

	log_info(logger, "$-Se registro el id del catch que mando el entrenador %d como id: %d", entrenadorEsperando->numeroEntrenador, idCorrelativo);
}

t_paquete* recibirCaught(int socketCaught){
	t_paquete* paqueteCaught = recibir_mensaje(socketCaught);

	sem_wait(&semaforoCorrelativos);
	int noVacio = list_is_empty(idsCorrelativosCaught) != 1;
	sem_post(&semaforoCorrelativos);

	if(paqueteCaught != NULL){
		t_caught_pokemon* caught = paqueteCaught->buffer->stream;
		log_info(logger, "$-Se recibio un caught con id %d, la respuesta es %d", paqueteCaught->ID, caught->ok);



		if(noVacio && tieneComoIdCorrelativoCaught(paqueteCaught->ID_CORRELATIVO)) {
			//el entrenador que hizo el catch del caught respondido cambia de estado de acuerdo a la respuesta

			ejecutarRespuestaCaught(paqueteCaught->ID_CORRELATIVO, paqueteCaught);
			quickLog("$-El entrenador que esperaba el caught fue procesado");
			return paqueteCaught;
		}
	} else if (noVacio){ //se corto la conexion al escuchar respuestas caught
		//tomo todas las respuestas que faltaban como true
		log_info(LO, "Se corto la conexion con el Broker. Por default, los entrenadores esperando respuesta atrapan al pokemon");
		for(int index = 0; index < list_size(entrenadoresEsperandoRespuesta()); index ++) {
			Entrenador* entrenadorEsperando = list_get(entrenadoresEsperandoRespuesta(), index);
			//agrego el pokemon atrapado y cambio al entrenador de estado
			agregarAtrapado(entrenadorEsperando, entrenadorEsperando->movimientoEnExec->pokemonNecesitado);
			estadoSiAtrapo(entrenadorEsperando);
		}

	}

	return NULL;
}

int tieneComoIdCorrelativoCaught(int idBuscado) {
	typedef bool(*erasedType)(void*);

	quickLog("$-Fijandose si el caught es suyo");

	int existe(int idExistente) {
		return idExistente == idBuscado;
	}
	int cumple = 0;

	//si la lista no esta vacia
	sem_wait(&semaforoCorrelativos);
	if(list_is_empty(idsCorrelativosCaught) != 1) {
		//me fijo de la lista de idsCorrelativos que mande como catch, si coincide con el id del que recien llego
		cumple = list_any_satisfy(idsCorrelativosCaught, (erasedType)existe);
	}
	sem_post(&semaforoCorrelativos);

	return cumple;
}

void ejecutarRespuestaCaught(int idCatchQueResponde, t_paquete* paqueteCaught){
	typedef bool(*erasedType)(void*);
	t_caught_pokemon* caught = paqueteCaught->buffer->stream;

	int idQueResponde(int id) {
		//Entrenador* entrenador = entrenadorQueTieneId(idCatchQueResponde);

		return id == idCatchQueResponde;
	}

	char* charCaught = charResultado(caught->ok);


	//ya sabemos que algun entrenador tiene ese id como correlativo de espera
	Entrenador* entrenador = entrenadorQueTieneId(idCatchQueResponde);
	log_info(LO, "Se recibio el Caught del catch con id %d | Resultado: %s", idCatchQueResponde, charCaught);
	procesarEspera(entrenador, caught->ok);


	sem_wait(&semaforoCorrelativos);
	list_remove_by_condition(idsCorrelativosCaught, (erasedType) idQueResponde);
	log_info(logger, "$-Ahora la cantidad de ids correlativos esperando respuestas caught es: %d", list_size(idsCorrelativosCaught));
	sem_post(&semaforoCorrelativos);


}

char* charResultado(uint32_t ok) {
	char* resultado;
	if(ok == 1) {
		resultado = "TRUE";
	} else {
		resultado = "FALSE";
	}
	return resultado;
}


Entrenador* entrenadorQueTieneId(int idCatchQueResponde) {
	typedef bool(*erasedType)(void*);
	bool tieneIdCorrelativo(Entrenador* entrenador) {
		//sem_wait(&semaforoCorrelativos);
		pthread_mutex_lock(&entrenador->mutexCorrelativo);
		int cumple = entrenador->idCorrelativoDeEspera == idCatchQueResponde;
		pthread_mutex_unlock(&entrenador->mutexCorrelativo);
		return cumple;
		//sem_post(&semaforoCorrelativos);
	}

	pthread_mutex_lock(&mutexEntrenadores);
	Entrenador* entrenadorRespondido = list_find(entrenadores, (erasedType)tieneIdCorrelativo);
	pthread_mutex_unlock(&mutexEntrenadores);
	quickLog("$-Se encontro el entrenador que esperaba el caught");
	return entrenadorRespondido;
}

void procesarEspera(Entrenador*  entrenador, uint32_t atrapo){
	PokemonEnElMapa* pokemonAtrapado = entrenador->movimientoEnExec->pokemonNecesitado;

	log_info(logger, "$-Se esta procesando el entrenador en espera de la respuesta");
	//si lo atrapo
	if(atrapo){
		agregarAtrapado(entrenador, pokemonAtrapado);
		quickLog("$-Se atrapo el pokemon");
		estadoSiAtrapo(entrenador);

	}
	//no lo atrapo
	else {
		pthread_mutex_lock(&mutexObjetivosGlobales);
		//lo agrego devuelta a la lista de objetivos globales para que otro entrenador lo atrape
		setPokemonA(objetivosGlobales, pokemonAtrapado);
		pthread_mutex_unlock(&mutexObjetivosGlobales);

		pasarADormido(entrenador);
	}

	pthread_mutex_lock(&entrenador->mutexEstado);
	log_info(logger, "$-Luego de recibir la respuesta del caught el entrenador quedo en estado %d", entrenador->estado);
	pthread_mutex_unlock(&entrenador->mutexEstado);
}


