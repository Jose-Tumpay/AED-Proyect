#pragma once

class Publicacion {
private:
    int id;
    int idUsuario;
    char contenido[256];
    char fecha[16];
    int likes;

    void copiarTexto(char* destino, const char* origen, int tamMax);

public:
    // constructores
    Publicacion();
    Publicacion(int id, int idUsuario, const char* cont, const char* f, int likes = 0);

    // getters
    int getId() const;
    int getIdUsuario() const;
    const char* getContenido() const;
    const char* getFecha() const;
    int getLikes() const;

    void darLike();

    // sobrecarga maxheap
    bool operator>(const Publicacion& otra) const;
    bool operator<(const Publicacion& otra) const;
    bool operator==(const Publicacion& otra) const;
};