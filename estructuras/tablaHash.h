#pragma once
#include "Lista.h"
#include <string>
#include <stdexcept>

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

    int funcionHash(int clave) const {
        int h = clave % capacidad;
        return h < 0 ? h + capacidad : h;
    }

    int funcionHash(const std::string& clave) const {
        unsigned long hash = 5381;
        for (char c : clave) {
            hash = ((hash << 5) + hash) + c;
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
};