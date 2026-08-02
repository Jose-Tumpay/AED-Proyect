#include <iostream>
#include "red/redSocial.h"

int main() {
    RedSocial red;
    red.cargarGrafoSNAP("data/amistades_4039n_88234r.txt");
    std::cout << red.obtenerNumUsuarios() << std::endl;
    return 0;
}