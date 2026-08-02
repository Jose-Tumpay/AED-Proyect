#include "comentario.h"

void Comentario::copiarTexto(char* destino, const char* origen, int tamMax) {
    if (!destino || !origen || tamMax <= 0) return;
    int i = 0;
    while (origen[i] != '\0' && i < tamMax - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

Comentario::Comentario() : id(0), autorId(0) {
    texto[0] = '\0';
    fecha[0] = '\0';
}

Comentario::Comentario(int id, int autorId, const char* texto, const char* fecha)
    : id(id), autorId(autorId) {
    copiarTexto(this->texto, texto, 256);
    copiarTexto(this->fecha, fecha, 16);
}

int Comentario::getId() const { return id; }
int Comentario::getAutorId() const { return autorId; }
const char* Comentario::getTexto() const { return texto; }
const char* Comentario::getFecha() const { return fecha; }

bool Comentario::operator==(const Comentario& otro) const {
    return id == otro.id;
}