#pragma once

class Comentario {
private:
    int id;
    int autorId;
    char texto[256];
    char fecha[16];

    void copiarTexto(char* destino, const char* origen, int tamMax);

public:
    Comentario();
    Comentario(int id, int autorId, const char* texto, const char* fecha);

    int getId() const;
    int getAutorId() const;
    const char* getTexto() const;
    const char* getFecha() const;

    bool operator==(const Comentario& otro) const;
};