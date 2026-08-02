#include <cstdio>
#include <cstring>
#include "red/redSocial.h"

/*
 * Menu de consola de la red social.
 *
 * El enunciado dice que el aspecto visual no se evalua, asi que esto es
 * deliberadamente sobrio: su unico trabajo es dar acceso a las 13
 * funcionalidades del punto 4 para poder demostrarlas y capturarlas.
 *
 * Se usa <cstdio> en vez de <iostream> para no arrastrar std::string, igual
 * que hace el resto del proyecto.
 */

static const char* RUTA_SNAP = "data/amistades_4039n_88234r.txt";

/*
 * Lee una linea completa y la interpreta como entero.
 * Devuelve false si se acabo la entrada (EOF), para que el menu pueda salir
 * en vez de quedarse en un bucle infinito.
 * @complejidad O(1)
 */
static bool leerEntero(const char* etiqueta, int& destino) {
    char linea[128];
    printf("%s", etiqueta);
    fflush(stdout);

    if (!fgets(linea, sizeof(linea), stdin)) {
        return false;                   // EOF: se acabo la entrada
    }
    if (sscanf(linea, "%d", &destino) != 1) {
        printf("  Entrada no valida.\n");
        return leerEntero(etiqueta, destino);
    }
    return true;
}

/*
 * Lee una linea completa como texto libre (nombre, email, contenido...).
 * Recorta el salto de linea final a mano en vez de con strcspn+STL.
 * Devuelve false si se acabo la entrada.
 * @complejidad O(longitud de la linea)
 */
static bool leerTexto(const char* etiqueta, char* destino, int tamMax) {
    printf("%s", etiqueta);
    fflush(stdout);

    if (!fgets(destino, tamMax, stdin)) {
        return false;
    }
    int i = 0;
    while (destino[i] != '\0') {
        if (destino[i] == '\n') { destino[i] = '\0'; break; }
        i++;
    }
    return true;
}

/* @complejidad O(1) */
static void mostrarMenu(const RedSocial& red) {
    printf("\n");
    printf("==========================================================\n");
    printf("  RED SOCIAL - Algoritmos y Estructuras de Datos\n");
    printf("  usuarios: %d    publicaciones: %d\n",
           red.obtenerNumUsuarios(), red.obtenerNumPublicaciones());
    printf("==========================================================\n");
    printf("  --- Usuarios ---\n");
    printf("   1) Registrar usuario\n");
    printf("   2) Eliminar usuario\n");
    printf("   3) Buscar usuario\n");
    printf("  --- Publicaciones ---\n");
    printf("   4) Crear publicacion\n");
    printf("   5) Eliminar publicacion\n");
    printf("  11) Mostrar publicaciones de un usuario\n");
    printf("  13) Publicaciones con mas reacciones\n");
    printf("  --- Amistades ---\n");
    printf("   6) Agregar amigo\n");
    printf("   7) Eliminar amigo\n");
    printf("   8) Camino de amistad entre dos usuarios\n");
    printf("   9) Amigos en comun\n");
    printf("  10) Sugerencias de amistad\n");
    printf("  --- Rankings ---\n");
    printf("  12) Usuarios mas activos\n");
    printf("----------------------------------------------------------\n");
    printf("   0) Salir\n");
    printf("==========================================================\n");
}

/* @complejidad O(1) */
static void pendiente(int opcion) {
    printf("  [opcion %d todavia no conectada]\n", opcion);
}

/* @complejidad O(1): una insercion en la tabla hash + un vertice en el grafo */
static void opcionRegistrarUsuario(RedSocial& red) {
    int id;
    char nombre[64], email[64], fecha[16];

    if (!leerEntero("  ID: ", id)) return;
    if (!leerTexto("  Nombre: ", nombre, sizeof(nombre))) return;
    if (!leerTexto("  Email: ", email, sizeof(email))) return;
    if (!leerTexto("  Fecha de registro (YYYY-MM-DD): ", fecha, sizeof(fecha))) return;

    if (red.registrarUsuario(id, nombre, email, fecha)) {
        printf("  Usuario %d registrado.\n", id);
    } else {
        printf("  Ya existe un usuario con ID %d.\n", id);
    }
}

/* @complejidad O(grado del usuario) por las aristas que hay que desconectar */
static void opcionEliminarUsuario(RedSocial& red) {
    int id;
    if (!leerEntero("  ID a eliminar: ", id)) return;

    if (red.eliminarUsuario(id)) {
        printf("  Usuario %d eliminado.\n", id);
    } else {
        printf("  No existe un usuario con ID %d.\n", id);
    }
}

/* @complejidad O(1) promedio: busqueda en la tabla hash */
static void opcionBuscarUsuario(RedSocial& red) {
    int id;
    if (!leerEntero("  ID a buscar: ", id)) return;

    Usuario* u = red.buscarUsuarioPorId(id);
    if (!u) {
        printf("  No existe un usuario con ID %d.\n", id);
        return;
    }
    printf("  ID: %d\n", u->getId());
    printf("  Nombre: %s\n", u->getNombre());
    printf("  Email: %s\n", u->getEmail());
    printf("  Registrado: %s\n", u->getFechaRegistro());
    printf("  Amigos: %d\n", u->getContadorAmigos());
    printf("  Publicaciones: %d\n", u->getContadorPublicaciones());
    printf("  Seguidores: %d\n", u->getSeguidores());
    printf("  Reacciones recibidas: %d\n", u->getReacciones());
}

int main() {
    RedSocial red;

    printf("Cargando %s ...\n", RUTA_SNAP);
    if (red.cargarGrafoSNAP(RUTA_SNAP)) {
        printf("Listo: %d usuarios cargados.\n", red.obtenerNumUsuarios());
    } else {
        printf("Aviso: no se pudo cargar el dataset. Se arranca vacio.\n");
    }

    int opcion = -1;
    while (opcion != 0) {
        mostrarMenu(red);

        if (!leerEntero("  Opcion: ", opcion)) {
            printf("\n  Fin de la entrada. Saliendo.\n");
            break;
        }

        switch (opcion) {
            case 0:
                printf("  Hasta luego.\n");
                break;
            case 1: opcionRegistrarUsuario(red); break;
            case 2: opcionEliminarUsuario(red); break;
            case 3: opcionBuscarUsuario(red); break;
            case 4: case 5:
            case 6: case 7: case 8: case 9: case 10:
            case 11: case 12: case 13:
                pendiente(opcion);
                break;
            default:
                printf("  Opcion fuera de rango.\n");
                break;
        }
    }

    return 0;
}
