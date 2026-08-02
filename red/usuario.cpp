#include "red/usuario.h"

// Copia para lso strings
void Usuario::copiarTexto(char* destino, const char* origen, int tamMax) {
    if (!destino || !origen || tamMax <= 0) return;

    int i = 0;
    while (origen[i] != '\0' && i < tamMax - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

// constructor
Usuario::Usuario() : id(0), contadorPublicaciones(0) {
    nombre[0] = '\0';
    email[0] = '\0';
    fechaRegistro[0] = '\0';
}

// constructor
Usuario::Usuario(int id, const char* nom, const char* em, const char* fecha) 
    : id(id), contadorPublicaciones(0) {
    copiarTexto(nombre, nom, 64);
    copiarTexto(email, em, 64);
    copiarTexto(fechaRegistro, fecha, 16);
}

//geters
int Usuario::getId() const { 
    return id; 
}

const char* Usuario::getNombre() const { 
    return nombre; 
}

const char* Usuario::getEmail() const { 
    return email; 
}

const char* Usuario::getFechaRegistro() const { 
    return fechaRegistro; 
}

int Usuario::getContadorPublicaciones() const { 
    return contadorPublicaciones; 
}

// modicador
void Usuario::incrementarPublicaciones() { 
    contadorPublicaciones++; 
}

// camparadores
bool Usuario::operator>(const Usuario& otro) const {
    return contadorPublicaciones > otro.contadorPublicaciones;
}

bool Usuario::operator<(const Usuario& otro) const {
    return contadorPublicaciones < otro.contadorPublicaciones;
}

bool Usuario::operator==(const Usuario& otro) const {
    return id == otro.id;
}