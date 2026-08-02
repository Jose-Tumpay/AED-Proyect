#include "RedSocial.h"
#include <cstdio>

RedSocial::RedSocial() {
    totalUsuarios = 0;
    totalPublicaciones = 0;
}

bool RedSocial::registrarUsuario(int id, const char* nombre, const char* email, const char* fecha) {
    if (usuariosPorId.buscar(id) != nullptr) {
        return false;
    }

    Usuario nuevo(id, nombre, email, fecha);
    usuariosPorId.insertar(id, nuevo);
    grafoAmistades.agregarVertice(id);
    totalUsuarios++;
    return true;
}

void RedSocial::agregarAmistad(int id1, int id2) {
    grafoAmistades.agregarArista(id1, id2);
}

void RedSocial::crearPublicacion(int idPub, int idUsuario, const char* contenido, const char* fecha, int likes) {
    char pIdStr[40];
    char uIdStr[40];

    snprintf(pIdStr, sizeof(pIdStr), "%d", idPub);
    snprintf(uIdStr, sizeof(uIdStr), "%d", idUsuario);

    // Pasa los 7 parámetros requeridos: pId, uId, contenido, fecha, likes, comments, shares
    Publicacion pub(pIdStr, uIdStr, contenido, fecha, likes, 0, 0);
    publicaciones.agregarFinal(pub);
    totalPublicaciones++;

    Usuario* u = usuariosPorId.buscar(idUsuario);
    if (u != nullptr) {
        u->incrementarPublicaciones();
    }
}

Usuario* RedSocial::buscarUsuarioPorId(int id) {
    return usuariosPorId.buscar(id);
}

Lista<int> RedSocial::caminoAmistad(int idOrigen, int idDestino) {
    return grafoAmistades.caminoMasCorto(idOrigen, idDestino);
}

Lista<Usuario> RedSocial::obtenerTopUsuariosActivos(int topK) {
    ColaPrioridad<Usuario> maxHeap;
    Lista<Usuario> todos = usuariosPorId.obtenerTodosLosValores();

    for (int i = 0; i < todos.obtenerTamano(); i++) {
        maxHeap.insertar(todos.obtener(i));
    }

    Lista<Usuario> topUsuarios;
    int contador = 0;

    while (!maxHeap.estaVacia() && contador < topK) {
        topUsuarios.agregarFinal(maxHeap.extraerMaximo());
        contador++;
    }

    return topUsuarios;
}

bool RedSocial::cargarGrafoSNAP(const char* rutaArchivo) {
    FILE* f = fopen(rutaArchivo, "r");
    if (!f) {
        printf("Error al abrir archivo de amistades\n");
        return false;
    }

    char buffer[256];
    int u, v;

    while (fgets(buffer, sizeof(buffer), f)) {
        if (buffer[0] == '#' || buffer[0] == '%') continue;

        if (sscanf(buffer, "%d %d", &u, &v) == 2 || sscanf(buffer, "%d,%d", &u, &v) == 2) {
            
            if (!usuariosPorId.buscar(u)) {
                char nom[32];
                snprintf(nom, sizeof(nom), "User_%d", u);
                registrarUsuario(u, nom, "user@mail.com", "2026-01-01");
            }
            if (!usuariosPorId.buscar(v)) {
                char nom[32];
                snprintf(nom, sizeof(nom), "User_%d", v);
                registrarUsuario(v, nom, "user@mail.com", "2026-01-01");
            }

            agregarAmistad(u, v);
        }
    }

    fclose(f);
    return true;
}

bool RedSocial::cargarPublicacionesCSV(const char* rutaArchivo) {
    FILE* f = fopen(rutaArchivo, "r");
    if (!f) {
        printf("Error al abrir archivo de publicaciones\n");
        return false;
    }

    char buffer[1024];
    // Saltar cabecera del CSV
    if (!fgets(buffer, sizeof(buffer), f)) {
        fclose(f);
        return false;
    }

    int idPub = 1;
    while (fgets(buffer, sizeof(buffer), f)) {
        int idUsuario, likes;
        char contenido[256];
        char fecha[16];

        if (sscanf(buffer, "%d,%255[^,],%15[^,],%d", &idUsuario, contenido, fecha, &likes) >= 3) {
            crearPublicacion(idPub++, idUsuario, contenido, fecha, likes);
        }
    }

    fclose(f);
    return true;
}