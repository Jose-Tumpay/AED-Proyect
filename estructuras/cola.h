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

public:
    /// @complejidad O(1)
    Cola() : frenteNodo(nullptr), finalNodo(nullptr), tamano(0) {}

    Cola(const Cola&) = delete;
    Cola& operator=(const Cola&) = delete;

    /// @complejidad O(n) — desencola cada nodo restante
    ~Cola() {
        while (!estaVacia()) encolar_pop();
    }

    /// @complejidad O(1) — se mantiene puntero al final
    void encolar(const T& dato) {
        Nodo* nuevo = new Nodo(dato);
        if (!finalNodo) {
            frenteNodo = finalNodo = nuevo;
        } else {
            finalNodo->siguiente = nuevo;
            finalNodo = nuevo;
        }
        tamano++;
    }

    /// @complejidad O(1)
    T desencolar() {
        if (estaVacia()) throw std::underflow_error("Cola vacia");
        Nodo* temp = frenteNodo;
        T dato = temp->dato;
        frenteNodo = frenteNodo->siguiente;
        if (!frenteNodo) finalNodo = nullptr;
        delete temp;
        tamano--;
        return dato;
    }

    /// @complejidad O(1)
    void encolar_pop() {
        if (!estaVacia()) desencolar();
    }

    /// @complejidad O(1)
    T frente() const {
        if (estaVacia()) throw std::underflow_error("Cola vacia");
        return frenteNodo->dato;
    }

    /// @complejidad O(1)
    bool estaVacia() const { return tamano == 0; }
    /// @complejidad O(1)
    int obtenerTamano() const { return tamano; }
};