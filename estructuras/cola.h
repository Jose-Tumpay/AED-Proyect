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
    Cola() : frenteNodo(nullptr), finalNodo(nullptr), tamano(0) {}

    ~Cola() {
        while (!estaVacia()) encolar_pop();
    }

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

    void encolar_pop() {
        if (!estaVacia()) desencolar();
    }

    T frente() const {
        if (estaVacia()) throw std::underflow_error("Cola vacia");
        return frenteNodo->dato;
    }

    bool estaVacia() const { return tamano == 0; }
    int obtenerTamano() const { return tamano; }
};