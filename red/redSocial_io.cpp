#include "redSocial.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>

/*
 * Generador sintetico, enlace preferencial y medicion de tiempos.
 *
 * <chrono> y las llamadas a fopen/fprintf estan permitidas por el enunciado
 * (medicion de tiempos y E/S): no son estructuras de datos, no violan E1.
 */

namespace {

// Por encima de este N, obtenerTopUsuariosActivos (O(n^2) por el Defecto 2
// del plan) tarda minutos en vez de milisegundos: no se mide, se reporta -1.
const int LIMITE_MEDICION_TOPK = 20000;

using Reloj = std::chrono::steady_clock;

double milisegundosDesde(Reloj::time_point inicio) {
    return std::chrono::duration<double, std::milli>(Reloj::now() - inicio).count();
}

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

/*
 * Arma una RedSocial sintetica de tamano n y cronometra, con <chrono>, sus
 * operaciones principales: carga (generacion), insercion, busqueda, BFS
 * (camino de amistad), sugerencias y top-K. Pensada para correrse para
 * varios N y volcar la serie con exportarMedicionesCSV (bateria de escalado,
 * ver main.cpp --bench).
 */
MedicionTiempos RedSocial::medirOperaciones(int n, int enlacesPorUsuario, int usuariosPorComunidad) {
    MedicionTiempos m{};
    m.n = n;

    RedSocial red;

    Reloj::time_point t0 = Reloj::now();
    red.generarUsuariosSinteticos(n, enlacesPorUsuario, usuariosPorComunidad);
    m.msCarga = milisegundosDesde(t0);

    t0 = Reloj::now();
    red.registrarUsuario(n, "Bench_nuevo", "bench@red.local", "2026-01-01");
    for (int k = 0; k < enlacesPorUsuario && k < n; k++) {
        red.agregarAmistad(n, k);
    }
    m.msInsercion = milisegundosDesde(t0);

    int repeticionesBusqueda = (n < 1000) ? (n > 0 ? n : 1) : 1000;
    t0 = Reloj::now();
    for (int i = 0; i < repeticionesBusqueda; i++) {
        red.buscarUsuarioPorId(i);
    }
    m.msBusqueda = milisegundosDesde(t0) / repeticionesBusqueda;

    t0 = Reloj::now();
    red.caminoAmistad(0, n > 1 ? n - 1 : 0);
    m.msBFS = milisegundosDesde(t0);

    t0 = Reloj::now();
    red.obtenerSugerenciasAmistad(0);
    m.msSugerencias = milisegundosDesde(t0);

    // obtenerTopUsuariosActivos hace, para cada usuario, todos.obtener(i)
    // desde la cabeza de una Lista enlazada (Defecto 2 del plan de trabajo,
    // redSocial.cpp:67): el bucle entero es O(n^2). No es tarea de C4
    // arreglarlo (le toca a T1), pero medirlo sin limite colgaria la
    // bateria de escalado a partir de unos pocos miles de usuarios. Se
    // acota la medicion y se deja constancia del limite en el CSV con -1.
    if (n <= LIMITE_MEDICION_TOPK) {
        t0 = Reloj::now();
        red.obtenerTopUsuariosActivos(10);
        m.msTopK = milisegundosDesde(t0);
    } else {
        m.msTopK = -1.0;
    }

    return m;
}

/* @complejidad O(cantidad): una escritura por fila */
bool exportarMedicionesCSV(const char* rutaSalida, const MedicionTiempos* mediciones, int cantidad) {
    FILE* f = fopen(rutaSalida, "w");
    if (!f) {
        printf("Error al abrir '%s' para exportar mediciones.\n", rutaSalida);
        return false;
    }

    fprintf(f, "n,ms_carga,ms_insercion,ms_busqueda,ms_bfs,ms_sugerencias,ms_topk\n");
    for (int i = 0; i < cantidad; i++) {
        const MedicionTiempos& m = mediciones[i];
        fprintf(f, "%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                m.n, m.msCarga, m.msInsercion, m.msBusqueda, m.msBFS, m.msSugerencias, m.msTopK);
    }

    fclose(f);
    return true;
}
