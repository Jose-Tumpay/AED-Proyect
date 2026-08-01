#pragma once
#include "TablaHash.h"
#include "Lista.h"
#include "Cola.h"

class Grafo {
private:
    TablaHash<int, Lista<int>> adyacencia;
    