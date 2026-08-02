#include "redSocial.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

RedSocial::RedSocial() {
    totalUsuarios = 0;
    totalPublicaciones = 0;
}

bool RedSocial::registrarUsuario(int id, const char* nombre, const char* email, const char* fecha) {
    if (usuariosPorId.buscar(id) != nullptr) {
        return false;
    }

    Usuario nuevo(id, nombre, email, fecha);
    usuariosPorId.insertar(id, nuevo);
    grafoAmistades.agregarVertice(id);
    totalUsuarios++;
    return true;
}

bool RedSocial::agregarAmistad(int id1, int id2) {
    Usuario* u1 = usuariosPorId.buscar(id1);
    Usuario* u2 = usuariosPorId.buscar(id2);

    if (u1 == nullptr || u2 == nullptr) {
        return false;
    }

    grafoAmistades.agregarArista(id1, id2);
    u1->agregarAmigo(id2);
    u2->agregarAmigo(id1);

    return true;
}

void RedSocial::crearPublicacion(int idPub, int idUsuario, const char* contenido, const char* fecha, int likes) {
    char pIdStr[40];
    char uIdStr[40];

    snprintf(pIdStr, sizeof(pIdStr), "%d", idPub);
    snprintf(uIdStr, sizeof(uIdStr), "%d", idUsuario);

    // prepara para los 7 parámetros de Publicacion: pId, uId, content, date, likes, comments, shares
    Publicacion pub(pIdStr, uIdStr, contenido, fecha, likes, 0, 0);
    publicaciones.agregarFinal(pub);
    totalPublicaciones++;

    Usuario* u = usuariosPorId.buscar(idUsuario);
    if (u != nullptr) {
        u->agregarPublicacion(idPub);
        if (likes > 0) {
            u->incrementarReacciones(likes);
        }
    }
}

Usuario* RedSocial::buscarUsuarioPorId(int id) {
    return usuariosPorId.buscar(id);
}

Lista<int> RedSocial::caminoAmistad(int idOrigen, int idDestino) {
    return grafoAmistades.caminoMasCorto(idOrigen, idDestino);
}

// E10: top-K acotado. Antes se copiaban los N usuarios a una Lista
// (usuariosPorId.obtenerTodosLosValores(), O(n) memoria) y se armaba un heap
// de tamano N para sacar solo K. Ahora se recorre la TablaHash directamente
// (sin copia intermedia) y se mantiene un min-heap acotado a K elementos:
// cada usuario nuevo solo entra si supera al peor (el minimo) que ya esta
// adentro, que se descarta.
// @complejidad O(n log k) tiempo, O(k) memoria (n = usuarios totales, k = topK)
Lista<Usuario> RedSocial::obtenerTopUsuariosActivos(int topK) {
    Lista<Usuario> topUsuarios;
    if (topK <= 0) return topUsuarios;

    ColaPrioridad<Usuario> minHeap(topK, /*minHeap=*/true);

    for (Usuario& u : usuariosPorId) {
        if (minHeap.obtenerTamano() < topK) {
            minHeap.insertar(u);
        } else if (u > minHeap.tope()) {
            minHeap.extraerMaximo();
            minHeap.insertar(u);
        }
    }

    // extraerMaximo() en un min-heap saca primero al menor: se agrega al
    // inicio de la lista para que quede ordenado de mayor a menor (el orden
    // esperado de un "top").
    while (!minHeap.estaVacia()) {
        topUsuarios.agregarInicio(minHeap.extraerMaximo());
    }

    return topUsuarios;
}

Lista<int> RedSocial::amigosEnComun(int id1, int id2) {
    Lista<int> enComun;

    Usuario* u1 = usuariosPorId.buscar(id1);
    Usuario* u2 = usuariosPorId.buscar(id2);

    if (u1 == nullptr || u2 == nullptr) {
        return enComun;
    }

    const Lista<int>& amigos1 = u1->getAmigos();
    const Lista<int>& amigos2 = u2->getAmigos();

    // volcar amigos1 a una tabla hash: O(n), evita el .contiene() O(m) de
    // Lista dentro del bucle (que dejaba la funcion en O(n*m))
    TablaHash<int, bool> presentes(amigos1.obtenerTamano() * 2 + 7);
    for (int idAmigo : amigos1) {
        presentes.insertar(idAmigo, true);
    }

    // recorrer amigos2 una sola vez, sin bucle anidado: O(m)
    for (int idAmigo : amigos2) {
        if (presentes.buscar(idAmigo) != nullptr) {
            enComun.agregarFinal(idAmigo);
        }
    }

    return enComun;
}

// E5#10: el enunciado (§4) pide "calcular sugerencias", no solo listarlas
// en el orden arbitrario en que aparecen al recorrer amigos-de-amigos.
// Ahora se cuenta, por candidato, cuantos amigos en comun tiene con el
// usuario usando una TablaHash<int,int>, y se ordena a los candidatos por
// ese conteo (mayor primero) con una ColaPrioridad.
// @complejidad O(A * B) tiempo para contar (A = mis amigos, B = amigos por
// amigo) + O(c log c) para ordenar los c candidatos unicos encontrados.
Lista<int> RedSocial::obtenerSugerenciasAmistad(int idUsuario) {
    Lista<int> sugerencias;

    Usuario* u = usuariosPorId.buscar(idUsuario);
    if (u == nullptr) {
        return sugerencias;
    }

    const Lista<int>& misAmigos = u->getAmigos();

    // marcar mis amigos actuales para descartarlos como candidatos: O(1)
    // por busqueda en vez del .contiene() O(n) de Lista
    TablaHash<int, bool> yaSoyAmigo(misAmigos.obtenerTamano() * 2 + 7);
    for (int idAmigo : misAmigos) {
        yaSoyAmigo.insertar(idAmigo, true);
    }

    // conteo de amigos en comun por candidato, y el orden en que aparecio
    // cada candidato por primera vez (para recorrerlos sin duplicados)
    TablaHash<int, int> conteoComun;
    Lista<int> candidatosUnicos;

    for (int idAmigo : misAmigos) {
        Usuario* amigo = usuariosPorId.buscar(idAmigo);
        if (amigo == nullptr) continue;

        for (int candidato : amigo->getAmigos()) {
            if (candidato == idUsuario || yaSoyAmigo.buscar(candidato) != nullptr) {
                continue;
            }

            int* conteoActual = conteoComun.buscar(candidato);
            if (conteoActual != nullptr) {
                (*conteoActual)++;
            } else {
                conteoComun.insertar(candidato, 1);
                candidatosUnicos.agregarFinal(candidato);
            }
        }
    }

    // pareja (id, conteo) solo para poder ordenar candidatos con la
    // ColaPrioridad generica, que necesita operator> / operator<
    struct CandidatoConConteo {
        int id;
        int conteo;
        CandidatoConConteo() {}
        bool operator>(const CandidatoConConteo& o) const { return conteo > o.conteo; }
        bool operator<(const CandidatoConConteo& o) const { return conteo < o.conteo; }
    };

    ColaPrioridad<CandidatoConConteo> ranking;
    for (int candidato : candidatosUnicos) {
        int* conteo = conteoComun.buscar(candidato);
        CandidatoConConteo cc;
        cc.id = candidato;
        cc.conteo = (conteo != nullptr) ? *conteo : 0;
        ranking.insertar(cc);
    }

    while (!ranking.estaVacia()) {
        sugerencias.agregarFinal(ranking.extraerMaximo().id);
    }

    return sugerencias;
}

// E5#11: usa la lista de IDs de publicaciones que ya guarda el propio Usuario
// (u->getPublicaciones(), poblada en crearPublicacion) en vez de recorrer
// TODAS las publicaciones del sistema comparando pub.getUserId() como cadena.
// Se vuelca esa lista propia a una TablaHash<int,bool> (O(k), k = publicaciones
// del usuario) y se hace un unico recorrido sobre publicaciones comparando
// enteros por hash, no cadenas.
// @complejidad O(k + m): k = publicaciones del usuario, m = publicaciones totales
Lista<Publicacion> RedSocial::obtenerPublicacionesDeUsuario(int idUsuario) {
    Lista<Publicacion> resultado;

    Usuario* u = usuariosPorId.buscar(idUsuario);
    if (u == nullptr) {
        return resultado;
    }

    const Lista<int>& misPublicaciones = u->getPublicaciones();

    TablaHash<int, bool> misIds(misPublicaciones.obtenerTamano() * 2 + 7);
    for (int idPub : misPublicaciones) {
        misIds.insertar(idPub, true);
    }

    for (Publicacion& pub : publicaciones) {
        int idPub = atoi(pub.getPostId());
        if (misIds.buscar(idPub) != nullptr) {
            resultado.agregarFinal(pub);
        }
    }

    return resultado;
}

// E5#13 + E10: Publicacion::operator> ya compara por likes (publicacion.cpp:69).
// Antes se armaba un heap con las m publicaciones completas para sacar solo
// K (O(m) memoria extra, O(m log m) tiempo). Ahora, igual que en
// obtenerTopUsuariosActivos, se usa un min-heap acotado a K: cada publicacion
// solo entra si supera a la peor (la de menos likes) que ya esta adentro.
// @complejidad O(m log k) tiempo, O(k) memoria extra (m = publicaciones totales, k = topK)
Lista<Publicacion> RedSocial::obtenerPublicacionesTopReacciones(int topK) {
    Lista<Publicacion> topPublicaciones;
    if (topK <= 0) return topPublicaciones;

    ColaPrioridad<Publicacion> minHeap(topK, /*minHeap=*/true);

    for (Publicacion& pub : publicaciones) {
        if (minHeap.obtenerTamano() < topK) {
            minHeap.insertar(pub);
        } else if (pub > minHeap.tope()) {
            minHeap.extraerMaximo();
            minHeap.insertar(pub);
        }
    }

    // extraerMaximo() en un min-heap saca primero al de menos likes: se
    // agrega al inicio para dejar el orden esperado de mayor a menor.
    while (!minHeap.estaVacia()) {
        topPublicaciones.agregarInicio(minHeap.extraerMaximo());
    }

    return topPublicaciones;
}

bool RedSocial::cargarGrafoSNAP(const char* rutaArchivo) {
    FILE* f = fopen(rutaArchivo, "r");
    if (!f) {
        printf("Error al abrir archivo de amistades: %s\n", rutaArchivo);
        return false;
    }

    char buffer[256];
    int u, v;

    while (fgets(buffer, sizeof(buffer), f)) {
        if (buffer[0] == '#' || buffer[0] == '%') continue;

        if (sscanf(buffer, "%d %d", &u, &v) == 2 || sscanf(buffer, "%d,%d", &u, &v) == 2) {
            
            if (!usuariosPorId.buscar(u)) {
                char nom[32];
                snprintf(nom, sizeof(nom), "User_%d", u);
                registrarUsuario(u, nom, "user@mail.com", "2026-01-01");
            }
            if (!usuariosPorId.buscar(v)) {
                char nom[32];
                snprintf(nom, sizeof(nom), "User_%d", v);
                registrarUsuario(v, nom, "user@mail.com", "2026-01-01");
            }

            agregarAmistad(u, v);
        }
    }

    fclose(f);
    return true;
}

/*
 * Exporta las aristas de amistad a CSV (origen,destino), una fila por
 * arista sin duplicar (el grafo no es dirigido, solo se escribe si
 * origen < vecino). Pensado para un script externo (Python/networkx) que
 * dibuje el grafo -- ver T7, scripts/visualizar_grafo.py.
 *
 * @complejidad O(usuarios + amistades)
 */
bool RedSocial::exportarGrafoCSV(const char* rutaSalida) {
    FILE* f = fopen(rutaSalida, "w");
    if (!f) {
        printf("Error al abrir '%s' para exportar el grafo.\n", rutaSalida);
        return false;
    }

    fprintf(f, "origen,destino\n");

    Lista<Usuario> todos = usuariosPorId.obtenerTodosLosValores();
    for (auto& u : todos) {
        int origen = u.getId();
        Lista<int> vecinos = grafoAmistades.obtenerVecinos(origen);
        for (int vecino : vecinos) {
            if (origen < vecino) {
                fprintf(f, "%d,%d\n", origen, vecino);
            }
        }
    }

    fclose(f);
    return true;
}

// cargar el CSV
bool RedSocial::cargarPublicacionesCSV(const char* rutaArchivo) {
    FILE* f = fopen(rutaArchivo, "r");
    if (!f) {
        printf("Error al abrir archivo de publicaciones: %s\n", rutaArchivo);
        return false;
    }

    char buffer[1024];
    // saltar la cabecera del CSV
    if (!fgets(buffer, sizeof(buffer), f)) {
        fclose(f);
        return false;
    }

    int idPub = 1;
    char postId[40], userId[40], userName[64], gender[16];
    int age, followers, following;
    char creationDate[16], isVerified[10], location[64], topic[32], content[256];
    int contentLength;
    char hashtags[64], hasMedia[10], postDate[16], device[32], language[10];
    int likes, comments, shares;
    float engagementRate;

    const char* formato = 
        "%39[^,],%39[^,],%63[^,],%15[^,],%d,%d,%d,%15[^,],%9[^,],%63[^,],%31[^,],"
        "%255[^,],%d,%63[^,],%9[^,],%15[^,],%31[^,],%9[^,],%d,%d,%d,%f\n";

    while (fgets(buffer, sizeof(buffer), f)) {
        if (sscanf(buffer, formato,
            postId, userId, userName, gender, &age, &followers, &following,
            creationDate, isVerified, location, topic, content, &contentLength,
            hashtags, hasMedia, postDate, device, language, &likes, &comments, &shares, &engagementRate) >= 19) {
            
            // mapaear laspublicaciones
            int idUsuarioAsignado = (idPub - 1) % (totalUsuarios > 0 ? totalUsuarios : 4039);

            crearPublicacion(idPub, idUsuarioAsignado, content, postDate, likes);
            idPub++;
        }
    }

    fclose(f);
    return true;
}

bool RedSocial::eliminarAmistad(int id1, int id2) {
    Usuario* u1 = usuariosPorId.buscar(id1);
    Usuario* u2 = usuariosPorId.buscar(id2);

    if (u1 == nullptr || u2 == nullptr) {
        return false;
    }

    u1->eliminarAmigo(id2);
    u2->eliminarAmigo(id1);

    grafoAmistades.eliminarArista(id1, id2);

    return true;
}

bool RedSocial::eliminarPublicacion(int idPub) {
    char pIdStr[40];
    snprintf(pIdStr, sizeof(pIdStr), "%d", idPub);

    for (Publicacion& pub : publicaciones) {
        if (strcmp(pub.getPostId(), pIdStr) == 0) {
            publicaciones.eliminar(pub);
            totalPublicaciones--;
            return true;
        }
    }
    return false;
}

bool RedSocial::eliminarUsuario(int idUsuario) {
    Usuario* u = usuariosPorId.buscar(idUsuario);
    if (u == nullptr) {
        return false;
    }

    const Lista<int>& susAmigos = u->getAmigos();
    for (int idAmigo : susAmigos) {
        Usuario* amigo = usuariosPorId.buscar(idAmigo);
        if (amigo != nullptr) {
            amigo->eliminarAmigo(idUsuario);
        }
    }

    grafoAmistades.eliminarVertice(idUsuario);

    usuariosPorId.eliminar(idUsuario);
    totalUsuarios--;

    return true;
}

bool RedSocial::darLike(int idPub) {
    char pIdStr[40];
    snprintf(pIdStr, sizeof(pIdStr), "%d", idPub);

    for (Publicacion& pub : publicaciones) {
        if (strcmp(pub.getPostId(), pIdStr) == 0) {
            pub.agregarLike();

            int idAutor = atoi(pub.getUserId());
            Usuario* autor = usuariosPorId.buscar(idAutor);
            if (autor != nullptr) {
                autor->incrementarReacciones();
            }
            return true;
        }
    }
    return false;
}