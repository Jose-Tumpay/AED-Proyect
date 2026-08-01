#include "Publicacion.h"

// copia de caracteres
void Publicacion::copiarTexto(char* destino, const char* origen, int tamMax) {
    if (!destino || !origen || tamMax <= 0) return;

    int i = 0;
    while (origen[i] != '\0' && i < tamMax - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

// constructor por defecto
Publicacion::Publicacion() : id(0), idUsuario(0), likes(0) {
    contenido[0] = '\0';
    fecha[0] = '\0';
}

// constructor parametrizado
Publicacion::Publicacion(int id, int idUsuario, const char* cont, const char* f, int likes)
    : id(id), idUsuario(idUsuario), likes(likes) {
    copiarTexto(contenido, cont, 256);
    copiarTexto(fecha, f, 16);
}

int Publicacion::getId() const {
    return id;
}

int Publicacion::getIdUsuario() const {
    return idUsuario;
}

const char* Publicacion::getContenido() const {
    return contenido;
}

const char* Publicacion::getFecha() const {
    return fecha;
}

int Publicacion::getLikes() const {
    return likes;
}

// modificador
void Publicacion::darLike() {
    likes++;
}

// comparadores 
bool Publicacion::operator>(const Publicacion& otra) const {
    return likes > otra.likes;
}

bool Publicacion::operator<(const Publicacion& otra) const {
    return likes < otra.likes;
}

bool Publicacion::operator==(const Publicacion& otra) const {
    return id == otra.id;
}