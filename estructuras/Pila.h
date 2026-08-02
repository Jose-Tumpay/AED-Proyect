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
    /// @complejidad O(1)
    Pila() : topeNodo(nullptr), tamano(0) {}

    // Copiar una Pila haria un shallow-copy de los nodos: el destructor de
    // ambas copias borraria los mismos punteros -> double free. Se bloquea
    // en compilacion en vez de dejarlo como bug en tiempo de ejecucion.
    Pila(const Pila&) = delete;
    Pila& operator=(const Pila&) = delete;

    /// @complejidad O(n) — desapila cada nodo restante
    ~Pila() {
        while (!estaVacia()) desapilar();
    }

    /// @complejidad O(1)
    void apilar(const T& dato) {
        Nodo* nuevo = new Nodo(dato);
        nuevo->siguiente = topeNodo;
        topeNodo = nuevo;
        tamano++;
    }

    /// @complejidad O(1)
    T desapilar() {
        if (estaVacia()) throw std::underflow_error("Pila vacia");
        Nodo* temp = topeNodo;
        T dato = temp->dato;
        topeNodo = topeNodo->siguiente;
        delete temp;
        tamano--;
        return dato;
    }

    /// @complejidad O(1)
    T tope() const {
        if (estaVacia()) throw std::underflow_error("Pila vacia");
        return topeNodo->dato;
    }

    /// @complejidad O(1)
    bool estaVacia() const { return tamano == 0; }
    /// @complejidad O(1)
    int obtenerTamano() const { return tamano; }
};