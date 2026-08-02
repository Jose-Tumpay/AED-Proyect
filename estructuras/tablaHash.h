#pragma once
#include "lista.h"

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

    // duplica la capacidad (mantiene impar para reducir colisiones con mod)
    // y reinserta todo. Se dispara desde insertar() al pasar el 75% de carga.
    void rehashear() {
        int capacidadVieja = capacidad;
        Lista<Par>* tablaVieja = tabla;

        capacidad = capacidad * 2 + 1;
        tabla = new Lista<Par>[capacidad];
        tamano = 0;

        for (int i = 0; i < capacidadVieja; i++) {
            for (Par& p : tablaVieja[i]) {
                insertar(p.clave, p.valor);
            }
        }

        delete[] tablaVieja;
    }

public:
    explicit TablaHash(int cap = 10007) : capacidad(cap), tamano(0) {
        tabla = new Lista<Par>[capacidad];
    }

    TablaHash(const TablaHash&) = delete;
    TablaHash& operator=(const TablaHash&) = delete;
    
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

        if (tamano > (capacidad * 3) / 4) {
            rehashear();
        }
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