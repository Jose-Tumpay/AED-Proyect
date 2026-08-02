#include "redSocial.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

// Envoltorio que invierte el orden de comparacion de T, para poder usar
// ColaPrioridad<Inverso<T>> como min-heap sin reimplementar la logica de
// heap (que ya vive, probada, en ColaPrioridad). Util local a este .cpp,
// no una estructura de dominio nueva en estructuras/.
template <typename T>
struct Inverso {
    T valor;

    Inverso() : valor() {}
    explicit Inverso(const T& v) : valor(v) {}

    bool operator>(const Inverso<T>& otro) const { return valor < otro.valor; }
    bool operator<(const Inverso<T>& otro) const { return valor > otro.valor; }
};

} // namespace

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

Lista<Usuario> RedSocial::obtenerTopUsuariosActivos(int topK) {
    Lista<Usuario> topUsuarios;

    Lista<Usuario> todos = usuariosPorId.obtenerTodosLosValores();
    int n = todos.obtenerTamano();
    if (topK <= 0 || n == 0) return topUsuarios;

    // heap acotado a min(topK, n): O(n log k) y O(k) de memoria, en vez de
    // meter los n elementos completos (O(n log n) y O(n))
    int capInicial = (topK < n) ? topK : n;
    ColaPrioridad<Inverso<Usuario>> minHeap(capInicial);

    for (auto& u : todos) {
        if (minHeap.obtenerTamano() < topK) {
            minHeap.insertar(Inverso<Usuario>(u));
        } else if (u > minHeap.verTope().valor) {
            minHeap.extraerMaximo();
            minHeap.insertar(Inverso<Usuario>(u));
        }
    }

    // extraerMaximo() del heap invertido da el minimo real primero;
    // agregarInicio() en cada paso deja el resultado en orden descendente
    while (!minHeap.estaVacia()) {
        topUsuarios.agregarInicio(minHeap.extraerMaximo().valor);
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

    for (int idAmigo : amigos1) {
        if (amigos2.contiene(idAmigo)) {
            enComun.agregarFinal(idAmigo);
        }
    }

    return enComun;
}

Lista<int> RedSocial::obtenerSugerenciasAmistad(int idUsuario) {
    Lista<int> sugerencias;

    Usuario* u = usuariosPorId.buscar(idUsuario);
    if (u == nullptr) {
        return sugerencias;
    }

    const Lista<int>& misAmigos = u->getAmigos();

    // ir por cada amigo y luego por sus amigos
    for (int idAmigo : misAmigos) {
        Usuario* amigo = usuariosPorId.buscar(idAmigo);

        if (amigo != nullptr) {
            const Lista<int>& amigosDelAmigo = amigo->getAmigos();

            // ir por cada amigo del amigo y validar si es una sugerencia
            for (int candidato : amigosDelAmigo) {
                // verificar qeu no sea
                if (candidato != idUsuario &&
                    !misAmigos.contiene(candidato) &&
                    !sugerencias.contiene(candidato)) {
                    sugerencias.agregarFinal(candidato);
                }
            }
        }
    }

    return sugerencias;
}

Lista<Publicacion> RedSocial::obtenerPublicacionesDeUsuario(int idUsuario) {
    Lista<Publicacion> resultado;
    for (auto& pub : publicaciones) {
        if (atoi(pub.getUserId()) == idUsuario) {
            resultado.agregarFinal(pub);
        }
    }
    return resultado;
}

Lista<Publicacion> RedSocial::obtenerPublicacionesConMasReacciones(int topK) {
    Lista<Publicacion> topPublicaciones;

    int n = publicaciones.obtenerTamano();
    if (topK <= 0 || n == 0) return topPublicaciones;

    int capInicial = (topK < n) ? topK : n;
    ColaPrioridad<Inverso<Publicacion>> minHeap(capInicial);

    for (auto& pub : publicaciones) {
        if (minHeap.obtenerTamano() < topK) {
            minHeap.insertar(Inverso<Publicacion>(pub));
        } else if (pub > minHeap.verTope().valor) {
            minHeap.extraerMaximo();
            minHeap.insertar(Inverso<Publicacion>(pub));
        }
    }

    while (!minHeap.estaVacia()) {
        topPublicaciones.agregarInicio(minHeap.extraerMaximo().valor);
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

    for (auto& pub : publicaciones) {
        const char* actualId = pub.getPostId();

        bool iguales = true;
        int j = 0;
        while (actualId[j] != '\0' || pIdStr[j] != '\0') {
            if (actualId[j] != pIdStr[j]) {
                iguales = false;
                break;
            }
            j++;
        }

        if (iguales) {
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

    for (auto& pub : publicaciones) {
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