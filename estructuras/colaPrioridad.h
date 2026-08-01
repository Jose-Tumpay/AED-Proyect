#pragma once
#include <stdexcept>

template <typename T>
class ColaPrioridad {
private:
    T* heap;
    int capacidad;
    int tamano;

    void redimensionar() {
        capacidad *= 2;
        T* nuevo = new T[capacidad];
        for (int i = 0; i < tamano; i++) nuevo[i] = heap[i];
        delete[] heap;
        heap = nuevo;
    }

    void hundir(int i) {
        int mayor = i;
        int izq = 2 * i + 1;
        int der = 2 * i + 2;

        if (izq < tamano && heap[izq] > heap[mayor]) mayor = izq;
        if (der < tamano && heap[der] > heap[mayor]) mayor = der;

        if (mayor != i) {
            T temp = heap[i];
            heap[i] = heap[mayor];
            heap[mayor] = temp;
            hundir(mayor);
        }
    }

    void flotar(int i) {
        while (i > 0 && heap[(i - 1) / 2] < heap[i]) {
            T temp = heap[i];
            heap[i] = heap[(i - 1) / 2];
            heap[(i - 1) / 2] = temp;
            i = (i - 1) / 2;
        }
    }
    