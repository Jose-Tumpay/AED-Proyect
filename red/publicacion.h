#pragma once
#include "comentario.h"
#include "../estructuras/lista.h"

class Publicacion {
private:
    char postId[40];
    char userId[40];
    char postContent[256];
    char postDate[16];
    int likes;
    int comments;
    int shares;
    Lista<Comentario> comentarios;

    void copiarTexto(char* destino, const char* origen, int tamMax);

public:
    Publicacion();
    Publicacion(const char* pId, const char* uId, const char* content, const char* date, int l, int c, int s);

    const char* getPostId() const;
    const char* getUserId() const;
    const char* getPostContent() const;
    const char* getPostDate() const;
    int getLikes() const;
    int getComments() const;
    int getShares() const;
    const Lista<Comentario>& getComentarios() const;

    void agregarComentario(const Comentario& c);

    // Métodos const con nombres de parámetro idénticos a la implementación
    bool operator>(const Publicacion& otra) const;
    bool operator<(const Publicacion& otra) const;
    bool operator==(const Publicacion& otra) const;
};