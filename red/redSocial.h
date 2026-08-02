#pragma once
#include "../estructuras/TablaHash.h"
#include "../estructuras/Grafo.h"
#include "../estructuras/ColaPrioridad.h"
#include "../estructuras/Lista.h"
#include "usuario.h"
#include "publicacion.h"

class RedSocial {
private:
    TablaHash<int, Usuario> usuariosPorId;
    Grafo grafoAmistades;
    Lista<Publicacion> publicaciones;

    int totalUsuarios;
    int totalPublicaciones;

public:
    RedSocial();

    // para el dataset snap y csv
    bool cargarGrafoSNAP(const char* rutaArchivo);
    bool cargarPublicacionesCSV(const char* rutaArchivo);

    // operaciones de la red
    bool registrarUsuario(int id, const char* nombre, const char* email, const char* fecha);
    void agregarAmistad(int id1, int id2);
    void crearPublicacion(int idPub, int idUsuario, const char* contenido, const char* fecha, int likes = 0);

    // Algoritmos y Consultas
    Usuario* buscarUsuarioPorId(int id);
    Lista<int> caminoAmistad(int idOrigen, int idDestino);
    Lista<Usuario> obtenerTopUsuariosActivos(int topK);

    int obtenerNumUsuarios() const { return totalUsuarios; }
    int obtenerNumPublicaciones() const { return totalPublicaciones; }
};