#include "redSocial.h"
#include <cstdio>
#include <cstdlib>

/*
 * Generador sintetico, enlace preferencial y medicion de tiempos.
 *
 * <chrono> y las llamadas a fopen/fprintf estan permitidas por el enunciado
 * (medicion de tiempos y E/S): no son estructuras de datos, no violan E1.
 */

namespace {

// Arreglo dinamico de enteros (no es una "estructura del proyecto": es el
// truco clasico de enlace preferencial, un pool donde cada usuario aparece
// una vez por cada amistad que tiene). Se usa un arreglo crudo, no Lista,
// para que elegir un elemento al azar sea O(1) en vez de O(n) como
// Lista::obtener -- justo lo que el Defecto 2 del plan pide evitar.
struct PoolGrados {
    int* datos = nullptr;
    int tamano = 0;
    int capacidad = 0;

    void agregar(int id) {
        if (tamano == capacidad) {
            capacidad = (capacidad == 0) ? 16 : capacidad * 2;
            int* nuevo = new int[capacidad];
            for (int i = 0; i < tamano; i++) nuevo[i] = datos[i];
            delete[] datos;
            datos = nuevo;
        }
        datos[tamano++] = id;
    }

    // @complejidad O(1)
    int elegirAlAzar() const {
        return datos[rand() % tamano];
    }

    ~PoolGrados() { delete[] datos; }
};

} // namespace

/*
 * Genera cantidadUsuarios usuarios sinteticos con IDs 0..cantidadUsuarios-1,
 * agrupados en comunidades de usuariosPorComunidad usuarios. Dentro de cada
 * comunidad, cada usuario nuevo se conecta preferentemente a los que ya
 * tienen mas amigos (enlace preferencial / modelo Barabasi-Albert), y la
 * primera persona de cada comunidad (salvo la primera) se enlaza con una
 * comunidad anterior para que el grafo completo quede conectado.
 *
 * Sin esto el grafo sale uniforme: "amigos en comun" y "sugerencias" dan casi
 * siempre vacio y las capturas de la demo parecen un sistema roto (N2 del
 * plan de trabajo).
 *
 * @complejidad O(cantidadUsuarios * enlacesPorUsuario) -- cada eleccion
 * preferencial es O(1) gracias al PoolGrados, asi que nunca se repite el
 * patron cuadratico de Lista::obtener(i) en un bucle.
 */
void RedSocial::generarUsuariosSinteticos(int cantidadUsuarios, int enlacesPorUsuario,
                                           int usuariosPorComunidad, unsigned semilla) {
    if (cantidadUsuarios <= 0) return;
    if (enlacesPorUsuario < 1) enlacesPorUsuario = 1;
    if (enlacesPorUsuario > 200) enlacesPorUsuario = 200;
    if (usuariosPorComunidad < enlacesPorUsuario + 1) usuariosPorComunidad = enlacesPorUsuario + 1;

    srand(semilla);

    int numComunidades = (cantidadUsuarios + usuariosPorComunidad - 1) / usuariosPorComunidad;
    PoolGrados* pools = new PoolGrados[numComunidades];

    char nombre[32];
    char email[48];
    const char* fecha = "2026-01-01";

    for (int id = 0; id < cantidadUsuarios; id++) {
        int comunidad = id / usuariosPorComunidad;
        int idPrimeroComunidad = comunidad * usuariosPorComunidad;
        int disponibles = id - idPrimeroComunidad;

        snprintf(nombre, sizeof(nombre), "Sintetico_%d", id);
        snprintf(email, sizeof(email), "sint%d@red.local", id);
        registrarUsuario(id, nombre, email, fecha);

        PoolGrados& pool = pools[comunidad];
        int objetivos[256];
        int cuenta = 0;

        if (disponibles == 0) {
            // primer usuario de una comunidad nueva: nadie propio a quien
            // conectarse todavia. Se enlaza con una comunidad anterior para
            // no dejar islas desconectadas.
            if (comunidad > 0) {
                int otraComunidad = rand() % comunidad;
                if (pools[otraComunidad].tamano > 0) {
                    objetivos[cuenta++] = pools[otraComunidad].elegirAlAzar();
                }
            }
        } else if (disponibles <= enlacesPorUsuario) {
            // nucleo de la comunidad: todavia no hay suficientes miembros
            // para elegir al azar, asi que se conecta con todos los previos.
            for (int j = idPrimeroComunidad; j < id; j++) {
                objetivos[cuenta++] = j;
            }
        } else {
            int intentos = 0;
            while (cuenta < enlacesPorUsuario && intentos < enlacesPorUsuario * 30) {
                int candidato = pool.elegirAlAzar();
                bool repetido = false;
                for (int k = 0; k < cuenta; k++) {
                    if (objetivos[k] == candidato) { repetido = true; break; }
                }
                if (!repetido) objetivos[cuenta++] = candidato;
                intentos++;
            }

            // puente ocasional extra hacia otra comunidad, para que la
            // demo tenga sugerencias y caminos que crucen comunidades
            if (comunidad > 0 && id % 37 == 0 && cuenta < 256) {
                int otraComunidad = rand() % comunidad;
                if (pools[otraComunidad].tamano > 0) {
                    objetivos[cuenta++] = pools[otraComunidad].elegirAlAzar();
                }
            }
        }

        for (int k = 0; k < cuenta; k++) {
            agregarAmistad(id, objetivos[k]);
            pool.agregar(objetivos[k]);
            pool.agregar(id);
        }
    }

    delete[] pools;
}
