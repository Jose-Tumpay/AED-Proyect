#include "publicacion.h"

void Publicacion::copiarTexto(char* destino, const char* origen, int tamMax) {
    if (!destino || !origen || tamMax <= 0) return;
    int i = 0;
    while (origen[i] != '\0' && i < tamMax - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

Publicacion::Publicacion() : likes(0), comments(0), shares(0) {
    postId[0] = '\0';
    userId[0] = '\0';
    postContent[0] = '\0';
    postDate[0] = '\0';
}

Publicacion::Publicacion(const char* pId, const char* uId, const char* content, const char* date, int l, int c, int s)
    : likes(l), comments(c), shares(s) {
    copiarTexto(postId, pId, 40);
    copiarTexto(userId, uId, 40);
    copiarTexto(postContent, content, 256);
    copiarTexto(postDate, date, 16);
}

const char* Publicacion::getPostId() const {
    return postId;
}

const char* Publicacion::getUserId() const {
    return userId;
}

const char* Publicacion::getPostContent() const {
    return postContent;
}

const char* Publicacion::getPostDate() const {
    return postDate;
}

int Publicacion::getLikes() const {
    return likes;
}

int Publicacion::getComments() const {
    return comments;
}

int Publicacion::getShares() const {
    return shares;
}

const Lista<Comentario>& Publicacion::getComentarios() const {
    return comentarios;
}

void Publicacion::agregarComentario(const Comentario& c) {
    comentarios.agregarFinal(c);
    comments++;
}

bool Publicacion::operator>(const Publicacion& otra) const {
    return likes > otra.likes;
}

bool Publicacion::operator<(const Publicacion& otra) const {
    return likes < otra.likes;
}

bool Publicacion::operator==(const Publicacion& otra) const {
    int i = 0;
    while (postId[i] != '\0' && otra.postId[i] != '\0') {
        if (postId[i] != otra.postId[i]) return false;
        i++;
    }
    return postId[i] == otra.postId[i];
}