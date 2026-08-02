#pragma once
#include "estructuras/lista.h"

template <typename K, typename V>
class TablaHash {
private:
    struct Par {
        K clave;
        V valor;
        Par() : clave(K()), valor(V()) {}
        Par(K c, V v) : clave(c), valor(v) {}
        bool operator==(const Par& o) const { return clave == o.clave; }
    };

    Lista<Par>* tabla;
    int capacidad;
    int tamano;

    // funcion hash para enteros
    int funcionHash(int clave) const {
        int h = clave % capacidad;
        return h < 0 ? h + capacidad : h;
    }

    // fucnion hashpara strings
    int funcionHash(const char* clave) const {
        if (!clave) return 0;
        unsigned long hash = 5381;
        int i = 0;
        while (clave[i] != '\0') {
            hash = ((hash << 5) + hash) + clave[i];
            i++;
        }
        return hash % capacidad;
    }

public:
    explicit TablaHash(int cap = 10007) : capacidad(cap), tamano(0) {
        tabla = new Lista<Par>[capacidad];
    }

    ~TablaHash() {
        delete[] tabla;
    }

    void insertar(const K& clave, const V& valor) {
        int idx = funcionHash(clave);
        for (Par& p : tabla[idx]) {
            if (p.clave == clave) {
                p.valor = valor;
                return;
            }
        }
        tabla[idx].agregarFinal(Par(clave, valor));
        tamano++;
    }

    V* buscar(const K& clave) {
        int idx = funcionHash(clave);
        for (Par& p : tabla[idx]) {
            if (p.clave == clave) {
                return &p.valor;
            }
        }
        return nullptr;
    }

    bool eliminar(const K& clave) {
        int idx = funcionHash(clave);
        V* val = buscar(clave);
        if (val) {
            Par temp(clave, *val);
            if (tabla[idx].eliminar(temp)) {
                tamano--;
                return true;
            }
        }
        return false;
    }

    int obtenerTamano() const { return tamano; }

    // reusar el iterador para tener los valores de la tabla hash
    Lista<V> obtenerTodosLosValores() const {
        Lista<V> resultado;
        for (int i = 0; i < capacidad; i++) {
            for (const Par& p : tabla[i]) {
                resultado.agregarFinal(p.valor);
            }
        }
        return resultado;
    }
};