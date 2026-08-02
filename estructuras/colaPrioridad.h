#pragma once
#include <stdexcept>

template <typename T>
class ColaPrioridad {
private:
    T* heap;
    int capacidad;
    int tamano;
    // false: max-heap (la raiz es el mayor, extraerMaximo() saca el mayor).
    // true:  min-heap (la raiz es el menor, extraerMaximo() saca el menor).
    // Sirve para armar un top-K acotado: min-heap de tamano K donde solo
    // sobrevive el elemento si supera al menor actual (ver tope()).
    bool esMinHeap;

    // @complejidad O(1) — compara segun el modo del heap: en un max-heap
    // "mejor" significa mayor; en un min-heap, menor.
    bool esMejor(const T& a, const T& b) const {
        return esMinHeap ? (a < b) : (a > b);
    }

    // @complejidad O(n) — copia todo el heap a un arreglo mas grande;
    // amortizado O(1) por insercion ya que la capacidad se duplica
    void redimensionar() {
        capacidad *= 2;
        T* nuevo = new T[capacidad];
        for (int i = 0; i < tamano; i++) nuevo[i] = heap[i];
        delete[] heap;
        heap = nuevo;
    }

    // @complejidad O(log n) — desciende como maximo la altura del heap
    void hundir(int i) {
        int mejor = i;
        int izq = 2 * i + 1;
        int der = 2 * i + 2;

        if (izq < tamano && esMejor(heap[izq], heap[mejor])) mejor = izq;
        if (der < tamano && esMejor(heap[der], heap[mejor])) mejor = der;

        if (mejor != i) {
            T temp = heap[i];
            heap[i] = heap[mejor];
            heap[mejor] = temp;
            hundir(mejor);
        }
    }

    // @complejidad O(log n) — asciende como maximo la altura del heap
    void flotar(int i) {
        while (i > 0 && esMejor(heap[i], heap[(i - 1) / 2])) {
            T temp = heap[i];
            heap[i] = heap[(i - 1) / 2];
            heap[(i - 1) / 2] = temp;
            i = (i - 1) / 2;
        }
    }

public:
    /// @complejidad O(capacidad)
    /// @param minHeap si es true, la raiz es el minimo (util para un top-K
    /// acotado con heap de tamano K); si es false (default), la raiz es el
    /// maximo, como antes.
    explicit ColaPrioridad(int cap = 16, bool minHeap = false)
        : capacidad(cap), tamano(0), esMinHeap(minHeap) {
        heap = new T[capacidad];
    }

    ColaPrioridad(const ColaPrioridad&) = delete;
    ColaPrioridad& operator=(const ColaPrioridad&) = delete;

    /// @complejidad O(1)
    ~ColaPrioridad() {
        delete[] heap;
    }

    /// @complejidad O(log n) amortizado — flotar() es O(log n);
    /// redimensionar() solo se dispara ocasionalmente
    void insertar(const T& elemento) {
        if (tamano == capacidad) redimensionar();
        heap[tamano] = elemento;
        flotar(tamano);
        tamano++;
    }

    /// @complejidad O(log n) — hundir() es O(log n)
    T extraerMaximo() {
        if (tamano == 0) throw std::underflow_error("Heap vacio");
        T maximo = heap[0];
        heap[0] = heap[tamano - 1];
        tamano--;
        hundir(0);
        return maximo;
    }

    /// @complejidad O(1) — mira la raiz sin sacarla (el maximo en un
    /// max-heap, el minimo en un min-heap). Util para un top-K acotado:
    /// comparar un candidato nuevo contra el peor elemento que ya esta
    /// adentro, sin gastar un extraerMaximo()+insertar() cuando no conviene.
    const T& tope() const {
        if (tamano == 0) throw std::underflow_error("Heap vacio");
        return heap[0];
    }

    /// @complejidad O(1)
    bool estaVacia() const { return tamano == 0; }
    /// @complejidad O(1)
    int obtenerTamano() const { return tamano; }
};