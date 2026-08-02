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

public:
    Pila() : topeNodo(nullptr), tamano(0) {}

    // Copiar una Pila haria un shallow-copy de los nodos: el destructor de
    // ambas copias borraria los mismos punteros -> double free. Se bloquea
    // en compilacion en vez de dejarlo como bug en tiempo de ejecucion.
    Pila(const Pila&) = delete;
    Pila& operator=(const Pila&) = delete;

    ~Pila() {
        while (!estaVacia()) desapilar();
    }

    void apilar(const T& dato) {
        Nodo* nuevo = new Nodo(dato);
        nuevo->siguiente = topeNodo;
        topeNodo = nuevo;
        tamano++;
    }

    T desapilar() {
        if (estaVacia()) throw std::underflow_error("Pila vacia");
        Nodo* temp = topeNodo;
        T dato = temp->dato;
        topeNodo = topeNodo->siguiente;
        delete temp;
        tamano--;
        return dato;
    }

    T tope() const {
        if (estaVacia()) throw std::underflow_error("Pila vacia");
        return topeNodo->dato;
    }

    bool estaVacia() const { return tamano == 0; }
    int obtenerTamano() const { return tamano; }
};