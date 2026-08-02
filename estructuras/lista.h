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

public:
    class Iterador {
        Nodo* actual;
    public:
        explicit Iterador(Nodo* n) : actual(n) {}
        T& operator*() { return actual->dato; }
        Iterador& operator++() { actual = actual->siguiente; return *this; }
        bool operator!=(const Iterador& o) const { return actual != o.actual; }
    };

    Iterador begin() const { return Iterador(cabeza); }
    Iterador end() const { return Iterador(nullptr); }

    Lista() : cabeza(nullptr), cola(nullptr), tamano(0) {}

    ~Lista() {
        limpiar();
    }

    Lista(const Lista& otro) : cabeza(nullptr), cola(nullptr), tamano(0) {
        Nodo* actual = otro.cabeza;
        while (actual) {
            agregarFinal(actual->dato);
            actual = actual->siguiente;
        }
    }

    Lista& operator=(const Lista& otro) {
        if (this != &otro) {
            limpiar();
            Nodo* actual = otro.cabeza;
            while (actual) {
                agregarFinal(actual->dato);
                actual = actual->siguiente;
            }
        }
        return *this;
    }

    void agregarInicio(const T& dato) {
        Nodo* nuevo = new Nodo(dato);
        if (!cabeza) {
            cabeza = cola = nuevo;
        } else {
            nuevo->siguiente = cabeza;
            cabeza->anterior = nuevo;
            cabeza = nuevo;
        }
        tamano++;
    }

    void agregarFinal(const T& dato) {
        Nodo* nuevo = new Nodo(dato);
        if (!cola) {
            cabeza = cola = nuevo;
        } else {
            nuevo->anterior = cola;
            cola->siguiente = nuevo;
            cola = nuevo;
        }
        tamano++;
    }

    bool eliminar(const T& dato) {
        Nodo* actual = cabeza;
        while (actual) {
            if (actual->dato == dato) {
                if (actual->anterior) actual->anterior->siguiente = actual->siguiente;
                else cabeza = actual->siguiente;

                if (actual->siguiente) actual->siguiente->anterior = actual->anterior;
                else cola = actual->anterior;

                delete actual;
                tamano--;
                return true;
            }
            actual = actual->siguiente;
        }
        return false;
    }

    bool contiene(const T& dato) const {
        Nodo* actual = cabeza;
        while (actual) {
            if (actual->dato == dato) return true;
            actual = actual->siguiente;
        }
        return false;
    }

    T& obtener(int indice) const {
        if (indice < 0 || indice >= tamano) throw std::out_of_range("Indice fuera de rango");
        Nodo* actual = cabeza;
        for (int i = 0; i < indice; i++) actual = actual->siguiente;
        return actual->dato;
    }

    void limpiar() {
        Nodo* actual = cabeza;
        while (actual) {
            Nodo* sig = actual->siguiente;
            delete actual;
            actual = sig;
        }
        cabeza = cola = nullptr;
        tamano = 0;
    }

    int obtenerTamano() const { return tamano; }
    bool estaVacia() const { return tamano == 0; }

    T& frente() const {
        if (!cabeza) throw std::underflow_error("Lista vacia");
        return cabeza->dato;
    }

    T& atras() const {
        if (!cola) throw std::underflow_error("Lista vacia");
        return cola->dato;
    }

    T extraerInicio() {
        if (!cabeza) throw std::underflow_error("Lista vacia");
        T dato = cabeza->dato;
        Nodo* borrar = cabeza;
        cabeza = cabeza->siguiente;
        if (cabeza) cabeza->anterior = nullptr;
        else cola = nullptr;
        delete borrar;
        tamano--;
        return dato;
    }
};