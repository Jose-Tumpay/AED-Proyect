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
    // @complejidad O(n) — pero amortizado O(1) por insercion, ya que solo se
    // dispara cuando n crece proporcional a la capacidad.
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
    /// @complejidad O(capacidad) — inicializa el arreglo de listas vacias
    explicit TablaHash(int cap = 10007) : capacidad(cap), tamano(0) {
        tabla = new Lista<Par>[capacidad];
    }

    // tabla es un arreglo dinamico de Lista<Par>: copiar por defecto
    // duplicaria el puntero, no el arreglo -> doble delete[] al destruir.
    TablaHash(const TablaHash&) = delete;
    TablaHash& operator=(const TablaHash&) = delete;
    
    /// @complejidad O(capacidad + n) — libera el arreglo; cada Lista de
    /// cubeta libera sus propios nodos en cascada
    ~TablaHash() {
        delete[] tabla;
    }

    /// @complejidad O(1) promedio (distribucion uniforme de la funcion hash);
    /// O(n) en el peor caso si todas las claves colisionan en la misma
    /// cubeta. Amortizado O(1) incluyendo el rehash periodico.
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

    /// @complejidad O(1) promedio; O(longitud de la cubeta) en el peor caso
    V* buscar(const K& clave) {
        int idx = funcionHash(clave);
        for (Par& p : tabla[idx]) {
            if (p.clave == clave) {
                return &p.valor;
            }
        }
        return nullptr;
    }

    /// @complejidad O(1) promedio (buscar + Lista::eliminar en su cubeta);
    /// O(longitud de la cubeta) en el peor caso
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

    /// @complejidad O(1)
    int obtenerTamano() const { return tamano; }

    /// @complejidad O(capacidad + n) — recorre todas las cubetas
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