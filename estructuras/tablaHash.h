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
    