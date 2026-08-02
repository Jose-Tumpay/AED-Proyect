#pragma once
#include "estructuras/tablaHash.h"
#include "estructuras/lista.h"
#include "estructuras/cola.h"

class Grafo {
private:
    TablaHash<int, Lista<int>> adyacencia;

public:
    void agregarVertice(int id) {
        if (!adyacencia.buscar(id)) {
            adyacencia.insertar(id, Lista<int>());
        }
    }

    void agregarArista(int u, int v) {
        agregarVertice(u);
        agregarVertice(v);

        Lista<int>* listaU = adyacencia.buscar(u);
        if (listaU && !listaU->contiene(v)) {
            listaU->agregarFinal(v);
        }

        Lista<int>* listaV = adyacencia.buscar(v);
        if (listaV && !listaV->contiene(u)) {
            listaV->agregarFinal(u);
        }
    }

    void eliminarArista(int u, int v) {
        Lista<int>* listaU = adyacencia.buscar(u);
        if (listaU) listaU->eliminar(v);

        Lista<int>* listaV = adyacencia.buscar(v);
        if (listaV) listaV->eliminar(u);
    }

    Lista<int> obtenerVecinos(int u) {
        Lista<int>* lista = adyacencia.buscar(u);
        if (lista) return *lista;
        return Lista<int>();
    }

    // Busqueda en anchura (BFS) para hallar la ruta mas corta
    Lista<int> caminoMasCorto(int origen, int destino) {
        TablaHash<int, int> padre;
        TablaHash<int, bool> visitado;
        Cola<int> q;

        q.encolar(origen);
        visitado.insertar(origen, true);

        bool encontrado = false;
        while (!q.estaVacia()) {
            int actual = q.desencolar();
            if (actual == destino) {
                encontrado = true;
                break;
            }

            Lista<int> vecinos = obtenerVecinos(actual);
            for (int v : vecinos) {
                bool* vis = visitado.buscar(v);
                if (!vis || !(*vis)) {
                    visitado.insertar(v, true);
                    padre.insertar(v, actual);
                    q.encolar(v);
                }
            }
        }

        Lista<int> camino;
        if (!encontrado) return camino;

        int curr = destino;
        while (curr != origen) {
            camino.agregarInicio(curr);
            int* p = padre.buscar(curr);
            if (p) curr = *p;
            else break;
        }
        camino.agregarInicio(origen);
        return camino;
    }
};