#pragma once

class Usuario {
private:
    int id;
    char nombre[64];
    char email[64];
    char fechaRegistro[16];
    int contadorPublicaciones;

    void copiarTexto(char* destino, const char* origen, int tamMax);

public:
    // constructores
    Usuario();
    Usuario(int id, const char* nom, const char* em, const char* fecha);

    // getters
    int getId() const;
    const char* getNombre() const;
    const char* getEmail() const;
    const char* getFechaRegistro() const;
    int getContadorPublicaciones() const;

    void incrementarPublicaciones();

    // para ordenar maxheap
    bool operator>(const Usuario& otro) const;
    bool operator<(const Usuario& otro) const;
    bool operator==(const Usuario& otro) const;
};