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
static const char* RUTA_PUBLICACIONES_CSV = "data/publicaciones_interaciiones.csv";

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

/* @complejidad O(1) amortizado: se agrega al final de la lista de publicaciones */
static void opcionCrearPublicacion(RedSocial& red) {
    int idPub, idUsuario, likes;
    char contenido[256], fecha[16];

    if (!leerEntero("  ID de la publicacion: ", idPub)) return;
    if (!leerEntero("  ID del usuario autor: ", idUsuario)) return;

    if (!red.buscarUsuarioPorId(idUsuario)) {
        printf("  No existe un usuario con ID %d.\n", idUsuario);
        return;
    }

    if (!leerTexto("  Contenido: ", contenido, sizeof(contenido))) return;
    if (!leerTexto("  Fecha (YYYY-MM-DD): ", fecha, sizeof(fecha))) return;
    if (!leerEntero("  Likes iniciales: ", likes)) return;

    red.crearPublicacion(idPub, idUsuario, contenido, fecha, likes);
    printf("  Publicacion %d creada.\n", idPub);
}

/* @complejidad O(n) sobre las publicaciones: busca linealmente el ID */
static void opcionEliminarPublicacion(RedSocial& red) {
    int idPub;
    if (!leerEntero("  ID de publicacion a eliminar: ", idPub)) return;

    if (red.eliminarPublicacion(idPub)) {
        printf("  Publicacion %d eliminada.\n", idPub);
    } else {
        printf("  No existe una publicacion con ID %d.\n", idPub);
    }
}

/* @complejidad O(publicaciones del usuario + publicaciones totales), ver T2 */
static void opcionMostrarPublicacionesUsuario(RedSocial& red) {
    int id;
    if (!leerEntero("  ID del usuario: ", id)) return;

    if (!red.buscarUsuarioPorId(id)) {
        printf("  No existe un usuario con ID %d.\n", id);
        return;
    }

    Lista<Publicacion> pubs = red.obtenerPublicacionesDeUsuario(id);
    if (pubs.estaVacia()) {
        printf("  El usuario %d no tiene publicaciones.\n", id);
        return;
    }
    printf("  Publicaciones de %d (%d):\n", id, pubs.obtenerTamano());
    for (Publicacion& p : pubs) {
        printf("    [%s] %s (likes: %d, comentarios: %d)\n",
               p.getPostId(), p.getPostContent(), p.getLikes(), p.getComments());
    }
}

/* @complejidad O(1) promedio: dos busquedas en la tabla hash */
static void opcionAgregarAmigo(RedSocial& red) {
    int id1, id2;
    if (!leerEntero("  ID del primer usuario: ", id1)) return;
    if (!leerEntero("  ID del segundo usuario: ", id2)) return;

    if (red.agregarAmistad(id1, id2)) {
        printf("  Amistad %d <-> %d creada.\n", id1, id2);
    } else {
        printf("  No se pudo crear la amistad (algun ID no existe).\n");
    }
}

/* @complejidad O(1) promedio: dos busquedas en la tabla hash */
static void opcionEliminarAmigo(RedSocial& red) {
    int id1, id2;
    if (!leerEntero("  ID del primer usuario: ", id1)) return;
    if (!leerEntero("  ID del segundo usuario: ", id2)) return;

    if (red.eliminarAmistad(id1, id2)) {
        printf("  Amistad %d <-> %d eliminada.\n", id1, id2);
    } else {
        printf("  No se pudo eliminar la amistad (algun ID no existe).\n");
    }
}

/* @complejidad O(usuarios + amistades): BFS sobre el grafo */
static void opcionCaminoAmistad(RedSocial& red) {
    int origen, destino;
    if (!leerEntero("  ID origen: ", origen)) return;
    if (!leerEntero("  ID destino: ", destino)) return;

    Lista<int> camino = red.caminoAmistad(origen, destino);
    if (camino.estaVacia()) {
        printf("  No hay camino entre %d y %d.\n", origen, destino);
        return;
    }
    printf("  Camino (%d saltos): ", camino.obtenerTamano() - 1);
    for (int i = 0; i < camino.obtenerTamano(); i++) {
        printf("%d", camino.obtener(i));
        if (i + 1 < camino.obtenerTamano()) printf(" -> ");
    }
    printf("\n");
}

/* @complejidad O(amigos del usuario 1) con busqueda O(amigos del usuario 2) por elemento */
static void opcionAmigosEnComun(RedSocial& red) {
    int id1, id2;
    if (!leerEntero("  ID del primer usuario: ", id1)) return;
    if (!leerEntero("  ID del segundo usuario: ", id2)) return;

    Lista<int> comunes = red.amigosEnComun(id1, id2);
    if (comunes.estaVacia()) {
        printf("  No tienen amigos en comun.\n");
        return;
    }
    printf("  Amigos en comun (%d): ", comunes.obtenerTamano());
    for (int i = 0; i < comunes.obtenerTamano(); i++) {
        printf("%d ", comunes.obtener(i));
    }
    printf("\n");
}

/* @complejidad O(amigos * amigos-de-amigos + c log c): c = candidatos unicos,
   rankeados por amigos en comun (mayor primero) */
static void opcionSugerenciasAmistad(RedSocial& red) {
    int id;
    if (!leerEntero("  ID del usuario: ", id)) return;

    Lista<int> sugerencias = red.obtenerSugerenciasAmistad(id);
    if (sugerencias.estaVacia()) {
        printf("  No hay sugerencias para el usuario %d.\n", id);
        return;
    }
    printf("  Sugerencias por amigos en comun, mayor primero (%d): ", sugerencias.obtenerTamano());
    for (int i = 0; i < sugerencias.obtenerTamano(); i++) {
        printf("%d ", sugerencias.obtener(i));
    }
    printf("\n");
}

/* @complejidad O(n log k): min-heap acotado a k elementos (n = usuarios, k = topK) */
static void opcionUsuariosMasActivos(RedSocial& red) {
    int topK;
    if (!leerEntero("  Cuantos usuarios mostrar: ", topK)) return;

    Lista<Usuario> top = red.obtenerTopUsuariosActivos(topK);
    if (top.estaVacia()) {
        printf("  No hay usuarios registrados.\n");
        return;
    }
    printf("  Top %d usuarios por publicaciones:\n", top.obtenerTamano());
    for (int i = 0; i < top.obtenerTamano(); i++) {
        Usuario& u = top.obtener(i);
        printf("    %d) ID %-8d %-24s publicaciones: %d\n",
               i + 1, u.getId(), u.getNombre(), u.getContadorPublicaciones());
    }
}

/* @complejidad O(m log k): min-heap acotado a k elementos (m = publicaciones, k = topK) */
static void opcionPublicacionesTopReacciones(RedSocial& red) {
    int topK;
    if (!leerEntero("  Cuantas publicaciones mostrar: ", topK)) return;

    Lista<Publicacion> top = red.obtenerPublicacionesTopReacciones(topK);
    if (top.estaVacia()) {
        printf("  No hay publicaciones registradas.\n");
        return;
    }
    printf("  Top %d publicaciones por reacciones:\n", top.obtenerTamano());
    int puesto = 1;
    for (Publicacion& p : top) {
        printf("    %d) [%s] likes: %-6d %s\n", puesto++, p.getPostId(), p.getLikes(), p.getPostContent());
    }
}

/*
 * Bateria de escalado: mide carga, insercion, busqueda, BFS, sugerencias
 * y top-K para una serie creciente de N y vuelca el resultado a CSV. Se usa
 * "cientos de miles" (E6) como la mayor N, no todos los tamanos: por encima
 * de 20 000 medirOperaciones ya no cronometra el top-K porque hoy es O(n^2)
 * (Defecto 2, redSocial.cpp:67) y se colgaria la bateria entera.
 * No es una de las 13 funcionalidades del menu: es la herramienta de
 * medicion permitida por el enunciado (§2), asi que vive fuera del menu,
 * detras de un flag de linea de comandos.
 */
static void modoEscalado(const char* rutaSalida) {
    const int tamanos[] = {2000, 4000, 8000, 16000, 32000, 64000, 100000, 200000, 500000};
    const int cantidadTamanos = static_cast<int>(sizeof(tamanos) / sizeof(tamanos[0]));

    MedicionTiempos mediciones[cantidadTamanos];

    printf("Bateria de escalado: %d tamanos de N\n", cantidadTamanos);
    for (int i = 0; i < cantidadTamanos; i++) {
        printf("  N = %-8d ... ", tamanos[i]);
        fflush(stdout);

        mediciones[i] = RedSocial::medirOperaciones(tamanos[i]);

        printf("carga=%8.2fms  busqueda=%7.5fms  bfs=%7.2fms  sugerencias=%7.4fms  topk=%7.4fms\n",
               mediciones[i].msCarga, mediciones[i].msBusqueda, mediciones[i].msBFS,
               mediciones[i].msSugerencias, mediciones[i].msTopK);
    }

    if (exportarMedicionesCSV(rutaSalida, mediciones, cantidadTamanos)) {
        printf("\nMediciones exportadas a %s\n", rutaSalida);
    }
}

/*
 * Genera una red sintetica chica (legible en una imagen) y vuelca sus
 * aristas a CSV para que un script externo (Python + networkx) dibuje el
 * grafo con comunidades coloreadas -- ver INFORME.md §4.2 (T7). No es una
 * de las 13 funcionalidades del menu: vive detras de un flag de linea de
 * comandos, igual que --bench.
 */
static void modoVisualizacionGrafo(const char* rutaSalida) {
    const int N = 300;
    const int USUARIOS_POR_COMUNIDAD = 50;

    RedSocial red;
    red.generarUsuariosSinteticos(N, /*enlacesPorUsuario=*/6, USUARIOS_POR_COMUNIDAD);

    if (red.exportarGrafoCSV(rutaSalida)) {
        printf("Grafo (%d usuarios, %d comunidades de %d) exportado a %s\n",
               N, N / USUARIOS_POR_COMUNIDAD, USUARIOS_POR_COMUNIDAD, rutaSalida);
    }
}

int main(int argc, char** argv) {
    if (argc > 1 && strcmp(argv[1], "--bench") == 0) {
        modoEscalado("output/mediciones.csv");
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "--graph-viz") == 0) {
        modoVisualizacionGrafo("output/grafo_aristas.csv");
        return 0;
    }

    RedSocial red;

    printf("Cargando %s ...\n", RUTA_SNAP);
    if (red.cargarGrafoSNAP(RUTA_SNAP)) {
        printf("Listo: %d usuarios cargados.\n", red.obtenerNumUsuarios());
    } else {
        printf("Aviso: no se pudo cargar el dataset. Se arranca vacio.\n");
    }

    printf("Cargando %s ...\n", RUTA_PUBLICACIONES_CSV);
    if (red.cargarPublicacionesCSV(RUTA_PUBLICACIONES_CSV)) {
        printf("Listo: %d publicaciones cargadas.\n", red.obtenerNumPublicaciones());
    } else {
        printf("Aviso: no se pudieron cargar publicaciones.\n");
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
            case 4: opcionCrearPublicacion(red); break;
            case 5: opcionEliminarPublicacion(red); break;
            case 6: opcionAgregarAmigo(red); break;
            case 7: opcionEliminarAmigo(red); break;
            case 8: opcionCaminoAmistad(red); break;
            case 9: opcionAmigosEnComun(red); break;
            case 10: opcionSugerenciasAmistad(red); break;
            case 12: opcionUsuariosMasActivos(red); break;
            case 11: opcionMostrarPublicacionesUsuario(red); break;
            case 13: opcionPublicacionesTopReacciones(red); break;
            default:
                printf("  Opcion fuera de rango.\n");
                break;
        }
    }

    return 0;
}
