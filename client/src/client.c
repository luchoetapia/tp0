#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	// Loggeamos el valor de config
	if (config_has_property(config, "CLAVE")) {
		valor = config_get_string_value(config, "CLAVE");
		log_info(logger, "la clave es: %s", valor);
	}
	if(config_has_property (config, "PUERTO")){
        puerto = config_get_string_value(config, "PUERTO");
        log_info(logger,"El valor de PUERTO es: %s", puerto);
    }
    if(config_has_property (config, "IP")){
        ip = config_get_string_value(config, "IP");
        log_info(logger,"El valor de IP es: %s", ip);
    }

	else log_error(logger, "Error al leer el archivo de configuracion");


	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);
	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	
	// Enviamos al servidor el valor de CLAVE como mensaje
	bool res = realizar_handshake(logger, conexion);

	if (res) {
		enviar_mensaje(valor, conexion);
	}

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "TP0", true, LOG_LEVEL_INFO);

	if (nuevo_logger != NULL)
		log_info(nuevo_logger, "Hola! Soy un logger!!");

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");

	if (nuevo_config == NULL) abort();

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	
	while(true) {
		// La primera te la dejo de yapa
		leido = readline("> ");
		log_info(logger, leido);

		// El resto, las vamos leyendo y logueando hasta recibir un string vacío	
		if (!strcmp(leido, "")) {
			free(leido);
			break;
		}

		free(leido);
	}

	// ¡No te olvides de liberar las lineas antes de regresar!

}

void paquete(int conexion)
{
	char* leido;
    t_paquete* paquete = crear_paquete();

    while (true) {
        leido = readline("> ");

        if (!strcmp(leido, "")) {
            free(leido);
            break;
        }

        agregar_a_paquete(paquete, leido, strlen(leido) + 1);
        free(leido);
    }

    enviar_paquete(paquete, conexion); 
    eliminar_paquete(paquete);
}

bool realizar_handshake(t_log* logger, int conexion) {
	size_t bytes;
	int32_t handshake = 1;
	int32_t result;

	bytes = send(conexion, &handshake, sizeof(int32_t), 0);
	bytes = recv(conexion, &result, sizeof(int32_t), MSG_WAITALL);

	if (result == 0) {
		log_info(logger, "Handshake exitoso");
		return true;
	}

	else {
		log_error(logger, "Error en el handshake");
		return false;
	}
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	config_destroy(config);
	log_destroy(logger);
	liberar_conexion(conexion);
}
