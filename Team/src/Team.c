
#include "Team.h"


int main(int argc, char *argv[]) {

	logger = iniciar_logger();

	config = leerConfigDesde("src/team.config");
	iniciarVariables();


	//obtiene la lista de entrenadores desde el config
	entrenadores = getEntrenadoresDesde("src/team.config");
	t_list* entrena = entrenadores;
	quickLog("$-Ya fueron todos los entrenadores cargados con sus posiciones, objetivos y atrapados");

	objetivosTotales = getObjetivosTotalesDesde(entrenadores); //son pokes copias
	objetivosAtrapados = getTotalAtrapadosDesde(entrenadores);
	objetivosGlobales = getObjetivosGlobalesDesde(objetivosTotales, objetivosAtrapados);

	cantidadDeEspeciesTotales = list_size(objetivosGlobales);


	//Lanzar hilo para concetarme a Broker
	pthread_t hilosEscuchaBroker;
	pthread_create(&hilosEscuchaBroker, NULL, (void*) crearHilosDeEscucha, NULL);

	crearHilosDeEntrenadores();
	quickLog("$-Se crea un hilo por cada entrenador");

	//Lanzar hilo para escuchar a GameBoy
	pthread_t hiloEscuchaGameBoy;
	pthread_create(&hiloEscuchaGameBoy, NULL, (void*) escucharGameBoy, NULL);

	planificarEntrenadores();


//	pthread_join(hilosEscuchaBroker, NULL);
//	pthread_join(hiloEscuchaGameBoy, NULL);

	return 0;
}



