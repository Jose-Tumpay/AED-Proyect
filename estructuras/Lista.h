#pragma once
#include <stdexcept>

template <typename T>
class Lista {
private:
    struct Nodo {
        T dato;
        Nodo* siguiente;
        Nodo* anterior;

        explicit Nodo(const T& d) : dato(d), siguiente(nullptr), anterior(nullptr) {}
    };

    Nodo* cabeza;
    Nodo* cola;
    int tamano;


};