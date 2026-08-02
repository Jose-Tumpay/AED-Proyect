#pragma once
#include "../estructuras/tablaHash.h"
#include "../estructuras/grafo.h"
#include "../estructuras/colaPrioridad.h"
#include "../estructuras/lista.h"
#include "usuario.h"
#include "publicacion.h"

// Tiempos en milisegundos de las operaciones principales, para un N dado.
// Ver red/redSocial_io.cpp (medirOperaciones / exportarMedicionesCSV).
struct MedicionTiempos {
    int n;
    double msCarga;
    double msInsercion;
    double msBusqueda;
    double msBFS;
    double msSugerencias;
    double msTopK;
};

bool exportarMedicionesCSV(const char* rutaSalida, const MedicionTiempos* mediciones, int cantidad);

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

    // generador sintetico: usuarios agrupados en comunidades, conectados por
    // enlace preferencial (mas grado => mas probable recibir la nueva amistad).
    // Ver red/redSocial_io.cpp
    void generarUsuariosSinteticos(int cantidadUsuarios, int enlacesPorUsuario = 8,
                                    int usuariosPorComunidad = 500, unsigned semilla = 12345);

    // arma una RedSocial sintetica de tamano n y cronometra sus operaciones
    // principales con <chrono>. Ver red/redSocial_io.cpp
    static MedicionTiempos medirOperaciones(int n, int enlacesPorUsuario = 8,
                                             int usuariosPorComunidad = 500);

    // operaciones de la red
    bool registrarUsuario(int id, const char* nombre, const char* email, const char* fecha);
    bool agregarAmistad(int id1, int id2);
    void crearPublicacion(int idPub, int idUsuario, const char* contenido, const char* fecha, int likes = 0);

    Usuario* buscarUsuarioPorId(int id);
    Lista<int> caminoAmistad(int idOrigen, int idDestino);
    Lista<Usuario> obtenerTopUsuariosActivos(int topK);
    Lista<int> amigosEnComun(int id1, int id2);
    Lista<int> obtenerSugerenciasAmistad(int idUsuario);
    Lista<Publicacion> obtenerPublicacionesDeUsuario(int idUsuario);
    Lista<Publicacion> obtenerPublicacionesConMasReacciones(int topK);

    bool darLike(int idPub);

    int obtenerNumUsuarios() const { return totalUsuarios; }
    int obtenerNumPublicaciones() const { return totalPublicaciones; }

    bool eliminarAmistad(int id1, int id2);
    bool eliminarPublicacion(int idPub);
    bool eliminarUsuario(int idUsuario);
};