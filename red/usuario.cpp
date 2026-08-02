#pragma once
#include "../estructuras/lista.h"

class Usuario {
private:
    int id;
    char nombre[64];
    char email[64];
    char fechaRegistro[16];
    int contadorPublicaciones;
    
    // nuevos 
    Lista<int> amigos;       
    Lista<int> publicaciones;
    int seguidores;          
    int reacciones;          
    void copiarTexto(char* destino, const char* origen, int tamMax);

public:
    // constructores
    Usuario();
    Usuario(int id, const char* nom, const char* em, const char* fecha);

    int getId() const;
    const char* getNombre() const;
    const char* getEmail() const;
    const char* getFechaRegistro() const;
    int getContadorPublicaciones() const;

    int getSeguidores() const;
    int getReacciones() const;
    int getContadorAmigos() const;  
    const Lista<int>& getAmigos() const;
    const Lista<int>& getPublicaciones() const;

    // metodos modificadores
    void agregarPublicacion(int idPub);
    void agregarAmigo(int idAmigo);
    void incrementarSeguidores();
    void incrementarReacciones(int cantidad = 1);

    bool operator>(const Usuario& otro) const;
    bool operator<(const Usuario& otro) const;
    bool operator==(const Usuario& otro) const;

    void eliminarAmigo(int idAmigo);
};