#pragma once
#include <stdexcept>

template <typename T>
class Pila {
private:
    struct Nodo {
        T dato;
        Nodo* siguiente;
        Nodo(const T& d) : dato(d), siguiente(nullptr) {}
    };

    Nodo* topeNodo;
    int tamano;
    