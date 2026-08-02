#include "redSocial.h"
#include <cstdio>
#include <cstdlib>

/*
 * Generador sintetico, enlace preferencial y medicion de tiempos.
 *
 * <chrono> y las llamadas a fopen/fprintf estan permitidas por el enunciado
 * (medicion de tiempos y E/S): no son estructuras de datos, no violan E1.
 */

/*
 * Genera cantidadUsuarios usuarios sinteticos con IDs 0..cantidadUsuarios-1.
 * Version basica: cada usuario nuevo se conecta con enlacesPorUsuario
 * usuarios existentes elegidos uniformemente al azar (sin comunidades ni
 * enlace preferencial todavia).
 *
 * @complejidad O(cantidadUsuarios * enlacesPorUsuario)
 */
void RedSocial::generarUsuariosSinteticos(int cantidadUsuarios, int enlacesPorUsuario,
                                           int usuariosPorComunidad, unsigned semilla) {
    if (cantidadUsuarios <= 0) return;
    if (enlacesPorUsuario < 1) enlacesPorUsuario = 1;
    (void)usuariosPorComunidad; // sin uso todavia: llega con la version de comunidades

    srand(semilla);

    char nombre[32];
    char email[48];
    const char* fecha = "2026-01-01";

    for (int id = 0; id < cantidadUsuarios; id++) {
        snprintf(nombre, sizeof(nombre), "Sintetico_%d", id);
        snprintf(email, sizeof(email), "sint%d@red.local", id);
        registrarUsuario(id, nombre, email, fecha);

        int enlaces = (id < enlacesPorUsuario) ? id : enlacesPorUsuario;
        for (int k = 0; k < enlaces; k++) {
            int objetivo = rand() % id;
            agregarAmistad(id, objetivo);
        }
    }
}
