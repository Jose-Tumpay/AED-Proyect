#pragma once
#include <stdexcept>

template <typename T>
class Cola {
private:
    struct Nodo {
        T dato;
        Nodo* siguiente;
        Nodo(const T& d) : dato(d), siguiente(nullptr) {}
    };

    Nodo* frenteNodo;
    Nodo* finalNodo;
    int tamano;
    