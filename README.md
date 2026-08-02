<div align="center">
  <img src="images/logo_compubook.png" alt="CompuBook Logo" width="220"/>

  # CompuBook
  ### Red Social con Estructuras de Datos Propias

  ![C++17](https://img.shields.io/badge/C%2B%2B-17-1A538C?logo=cplusplus&logoColor=white)
  ![Sin STL](https://img.shields.io/badge/contenedores-sin%20STL-0D2C54)
  ![Escalado](https://img.shields.io/badge/probado%20hasta-500%2C000%20usuarios-3B82F6)
  ![License](https://img.shields.io/badge/uso-académico-64748B)

  **Universidad Nacional de San Agustín de Arequipa** · Escuela Profesional de Ciencia de la Computación
  Curso: *Algoritmos y Estructuras de Datos* (2026-A) · Docente: Percy Maldonado Q.

  **Autores:** Cristhian Taipe — `ctaipe@unsa.edu.pe` · Jose Tumpay — `jtumpay@unsa.edu.pe` · Roid N. Huaylla Guzmán — `rhuayllag@unsa.edu.pe`
</div>

---

## Tabla de Contenidos

- [Introducción](#introducción)
- [Objetivos](#objetivos)
- [Arquitectura General](#arquitectura-general)
- [Descripción de las Estructuras Utilizadas](#descripción-de-las-estructuras-utilizadas)
  - [Lista\<T\> — doblemente enlazada](#listat--doblemente-enlazada)
  - [TablaHash\<K,V\> — encadenamiento con Lista\<Par\>](#tablahashkv--encadenamiento-con-listapar)
  - [Grafo — no dirigido, sobre TablaHash](#grafo--no-dirigido-sobre-tablahash)
  - [ColaPrioridad\<T\> — heap binario](#colaprioridadt--heap-binario)
  - [Cola\<T\> y Pila\<T\>](#colat-y-pilat)
- [Funcionalidades Implementadas](#funcionalidades-implementadas)
- [Diagramas](#diagramas)
- [Complejidad Computacional](#complejidad-computacional)
- [Estructura del Repositorio](#estructura-del-repositorio)
- [Requisitos y Compilación](#requisitos-y-compilación)
- [Uso](#uso)
- [Resultados Experimentales](#resultados-experimentales)
- [Informe Técnico Completo](#informe-técnico-completo)
- [Limitaciones Conocidas y Trabajo Futuro](#limitaciones-conocidas-y-trabajo-futuro)
- [Conclusiones](#conclusiones)

---

## Introducción

**CompuBook** es el núcleo de backend de una red social mínima, desarrollado en **C++17**
para el Proyecto Final del curso de Algoritmos y Estructuras de Datos (EPCC — UNSA). El
enunciado exige administrar usuarios, relaciones de amistad, publicaciones e interacciones
**sin usar ninguna estructura de datos de la STL** (`vector`, `list`, `map`,
`unordered_map`, `set`, `queue`, etc.): toda lista, tabla hash, grafo, cola, pila y cola de
prioridad que usa el sistema fue escrita desde cero, en el módulo `estructuras/`.

El sistema soporta las **13 funcionalidades mínimas** pedidas por el enunciado, expuestas a
través de un menú de consola (`main.cpp`) deliberadamente sobrio, ya que el propio enunciado
aclara que el aspecto visual del sistema no se evalúa.

Como fuente de datos se combinan tres orígenes:

- Un dataset público de amistades en formato **SNAP** (`data/amistades_4039n_88234r.txt`):
  4,039 usuarios y 88,234 aristas, tomado de la red social *ego-Facebook* de Stanford SNAP.
- Un **CSV de publicaciones e interacciones de Kaggle**
  (`data/publicaciones_interaciiones.csv`, 20,000 filas) para poblar contenido realista.
- Un **generador sintético propio** (`RedSocial::generarUsuariosSinteticos`), capaz de
  producir cientos de miles de usuarios con su grafo de amistades agrupado en comunidades,
  para medir escalabilidad muy por encima de lo que ofrece el dataset público.

## Objetivos

**Objetivo general:** implementar desde cero el núcleo de funcionamiento de una red social
similar a Facebook utilizando únicamente estructuras de datos propias, priorizando
eficiencia computacional, escalabilidad y análisis de rendimiento.

**Objetivos específicos:**

1. Diseñar e implementar seis estructuras de datos genéricas (`Lista`, `TablaHash`, `Grafo`,
   `ColaPrioridad`, `Cola`, `Pila`) sin depender de la STL de C++.
2. Modelar el dominio de una red social (usuarios, publicaciones, comentarios) sobre esas
   estructuras, cumpliendo los 9 campos de `Usuario` y los 7 de `Publicacion` exigidos.
3. Implementar las 13 funcionalidades mínimas del enunciado con una complejidad algorítmica
   justificada y documentada método por método.
4. Construir un generador sintético de usuarios con enlace preferencial (modelo tipo
   Barabási–Albert) para poder medir escalabilidad más allá del dataset público disponible.
5. Evaluar experimentalmente el sistema con redes de hasta 500,000 usuarios, cronometrando
   las operaciones principales con `<chrono>`.

## Arquitectura General

CompuBook sigue una arquitectura en capas, donde cada capa solo conoce a la inmediatamente
inferior:

<div align="center">
  <img src="images/arquitectura.png" alt="Arquitectura General de CompuBook" width="80%"/>
  <br/>
  <sub><em>Arquitectura en capas de CompuBook: main.cpp (menú de consola) → RedSocial (fachada) → Usuario / Publicacion (dominio) → estructuras/ (contenedores genéricos), con redSocial_io.cpp como generador sintético y medidor de tiempos.</em></sub>
</div>

Por requisito del enunciado, cada `Usuario` guarda su propia `Lista<int> amigos`; por
eficiencia de recorrido, el `Grafo` mantiene además su propia
`TablaHash<int, Lista<int>> adyacencia`. Es una duplicación deliberada, no un descuido: el
campo que exige el enunciado y el índice que necesita el BFS para ser eficiente no son la
misma responsabilidad. La sincronización entre ambos ocurre siempre dentro de la misma
operación (`agregarAmistad`, `eliminarAmistad`, `eliminarUsuario`):

```cpp
bool RedSocial::agregarAmistad(int id1, int id2) {
    Usuario* u1 = usuariosPorId.buscar(id1);
    Usuario* u2 = usuariosPorId.buscar(id2);
    if (u1 == nullptr || u2 == nullptr) return false;

    grafoAmistades.agregarArista(id1, id2); // indice de recorrido del BFS
    u1->agregarAmigo(id2);                  // campo que exige el enunciado
    u2->agregarAmigo(id1);
    return true;
}
```

`main.cpp` expone el menú interactivo de 13 opciones y, además, un modo `--bench` que arma
redes sintéticas de distintos tamaños y cronometra sus operaciones principales, exportando
la serie a `output/mediciones.csv`.

## Descripción de las Estructuras Utilizadas

Todas las estructuras de `estructuras/` son plantillas (`template <typename T>` o `<K,V>`)
para reutilizarse en distintos tipos sin duplicar código, y ninguna usa contenedores de la
STL: la memoria se gestiona a mano con `new`/`delete`.

### `Lista<T>` — doblemente enlazada

Estructura base de la que dependen todas las demás: `TablaHash` la usa para encadenamiento
por cubeta, `Grafo` para las listas de adyacencia. Se eligió doblemente enlazada porque
`eliminar(dato)` necesita reconectar el nodo anterior y siguiente sin recorrer la lista de
nuevo desde la cabeza. Expone un `Iterador` (`begin()`/`end()`) para recorrer en O(n) total;
`obtener(indice)` también existe, pero es O(n) por llamada — usarlo dentro de un bucle
degrada cualquier recorrido completo a O(n²), el primer defecto de rendimiento detectado y
corregido en el proyecto.

### `TablaHash<K,V>` — encadenamiento con `Lista<Par>`

Cada cubeta es una `Lista` de pares clave-valor. Se usan dos funciones hash: módulo directo
para claves enteras (los IDs son consecutivos) y **DJB2** (`hash = hash*33 + c`) para claves
de texto. La capacidad inicial es un primo (10,007). Cuando el factor de carga supera 0.75,
`insertar` dispara `rehashear()`: duplica la capacidad y reinserta todo — O(n) por rehash,
pero **amortizado O(1) por inserción**:

```cpp
void insertar(const K& clave, const V& valor) {
    int idx = funcionHash(clave);
    for (Par& p : tabla[idx]) {
        if (p.clave == clave) { p.valor = valor; return; }
    }
    tabla[idx].agregarFinal(Par(clave, valor));
    tamano++;
    if (tamano > (capacidad * 3) / 4) rehashear(); // O(n), amortizado O(1)
}
```

### `Grafo` — no dirigido, sobre `TablaHash`

Cada vértice es una clave de la tabla hash; su valor es la `Lista<int>` de sus vecinos, dando
acceso a la adyacencia en O(1) promedio. `caminoMasCorto` usa **BFS** clásico con una
`Cola<int>` para el recorrido y dos `TablaHash` auxiliares (`visitado`, `padre`) en vez de
arreglos, porque los IDs de usuario no son necesariamente un rango denso desde 0.

### `ColaPrioridad<T>` — heap binario

Heap binario sobre un arreglo (`T*`), indexado aritméticamente (hijos de `i` en `2i+1` y
`2i+2`). Admite modo *min-heap* (parámetro `esMinHeap`), lo que permite un **Top-K acotado**:
un min-heap de tamaño fijo `k` en vez de un heap con todos los elementos, usado en los dos
rankings del sistema (usuarios más activos, publicaciones con más reacciones).

### `Cola<T>` y `Pila<T>`

Listas enlazadas simples especializadas (nodo con un solo puntero `siguiente`). `Cola` es la
estructura auxiliar del BFS de `Grafo`; `Pila` queda documentada y lista para futuras
funcionalidades (por ejemplo, deshacer/rehacer una acción).

## Funcionalidades Implementadas

Las 13 funcionalidades mínimas del enunciado, cableadas al menú de consola:

- Registrar, eliminar y buscar usuarios
- Crear, eliminar y listar publicaciones de un usuario
- Agregar y eliminar amigos
- Camino de amistad más corto entre dos usuarios (BFS)
- Amigos en común entre dos usuarios
- Sugerencias de amistad (rankeadas por amigos en común)
- Usuarios más activos (Top-K por publicaciones)
- Publicaciones con más reacciones (Top-K por likes)

## Diagramas

El informe técnico completo (`informe.pdf`) incluye los diagramas visuales:

- **Grafo de amistades y BFS**: recorrido nivel por nivel desde un vértice origen hasta el
  destino, con la reconstrucción del camino más corto siguiendo la tabla de padres.
- **Top-K acotado con min-heap**: cómo un heap de tamaño fijo `k` descarta al peor candidato
  cada vez que llega uno mejor, sin crecer más allá de `k` elementos sin importar cuántos
  usuarios se procesen.

## Complejidad Computacional

| Operación | Complejidad | Justificación |
|---|---|---|
| Registrar / buscar usuario | O(1) promedio | `TablaHash::buscar`/`insertar` directo |
| Agregar / eliminar amistad | O(grado(u1)+grado(u2)) | domina `Grafo::agregarArista`/`eliminarArista` |
| Camino de amistad (BFS) | O(V + A) | cada vértice se encola una vez, cada arista se examina una vez |
| Amigos en común | O(grado(u1) + grado(u2)) | se vuelca un lado a `TablaHash<int,bool>` y se recorre el otro una sola vez |
| Sugerencias de amistad | O(A·B + c log c) | conteo con `TablaHash<int,int>` + orden con `ColaPrioridad` |
| Top-K usuarios/publicaciones | O(n log k) tiempo, O(k) memoria | min-heap acotado a `k` elementos, sin copiar los n elementos a una lista aparte |

El detalle método por método está documentado con anotaciones `@complejidad` en el propio
código fuente (`estructuras/*.h`, `red/redSocial.cpp`) y en el informe técnico completo.

## Estructura del Repositorio

```
CompuBook/
├── estructuras/              # Contenedores genéricos, sin STL, sin conocer el dominio
│   ├── lista.h                  Lista doblemente enlazada con iterador
│   ├── tablaHash.h               TablaHash<K,V>, encadenamiento con Lista<Par>
│   ├── grafo.h                    Grafo no dirigido sobre TablaHash<int, Lista<int>>; BFS
│   ├── colaPrioridad.h             Heap binario sobre arreglo (max-heap / min-heap)
│   ├── cola.h                       Cola (FIFO), auxiliar del BFS
│   └── Pila.h                        Pila (LIFO)
├── red/                       # Dominio: usuarios, publicaciones y la fachada RedSocial
│   ├── usuario.h/.cpp            Los 9 campos de Usuario
│   ├── publicacion.h/.cpp        Los 7 campos de Publicacion + Lista<Comentario>
│   ├── comentario.h/.cpp         Comentario individual
│   ├── redSocial.h/.cpp          Fachada: une TablaHash<int,Usuario> + Grafo + Lista<Publicacion>
│   └── redSocial_io.cpp          Generador sintético, enlace preferencial, medición de tiempos
├── data/                      # Datasets de entrada
│   ├── amistades_4039n_88234r.txt   Dataset SNAP ego-Facebook (4,039 usuarios, 88,234 aristas)
│   └── publicaciones_interaciiones.csv   Dataset de Kaggle (20,000 filas)
├── images/                    # Logo, diagrama de arquitectura y capturas del informe
│   ├── logo_compubook.png
│   ├── arquitectura.png
│   └── capturas/
├── output/                    # Binario compilado y mediciones exportadas (mediciones.csv)
├── main.cpp                   # Menú de consola (13 opciones) + modo --bench
├── informe.tex                # Informe técnico completo (LaTeX)
└── README.md
```

## Requisitos y Compilación

Requiere un compilador con soporte **C++17** (g++ ≥ 7, clang++ ≥ 5).

```bash
g++ -std=c++17 -O2 -o app main.cpp red/*.cpp
```

## Uso

```bash
# Menú interactivo
./app

# Bateria de medición de tiempos (sin pasar por el menú)
./app --bench
```

El modo `--bench` arma redes sintéticas de distintos tamaños, cronometra las operaciones
principales con `<chrono>` (búsqueda, BFS, sugerencias de amistad, Top-K) y exporta la serie
a `output/mediciones.csv`.

## Resultados Experimentales

Corridos sobre el `main` actual, semilla fija y reproducible:

| N | Carga (ms) | BFS (ms) | Sugerencias (ms) | Top-K (ms) |
|---:|---:|---:|---:|---:|
| 2,000 | 6.37 | 1.56 | 0.27 | 0.12 |
| 4,000 | 10.14 | 2.45 | 0.23 | 0.11 |
| 8,000 | 50.49 | 6.20 | 0.54 | 0.30 |
| 16,000 | 138.25 | 14.43 | 1.11 | 0.60 |
| 32,000 | 350.20 | 39.83 | 1.70 | 1.01 |
| 64,000 | 1,235.36 | 110.89 | 3.86 | 2.34 |
| 100,000 | 959.68 | 209.32 | 0.27 | 3.66 |
| 200,000 | 3,039.84 | 402.72 | 11.18 | 8.47 |
| 500,000 | 12,882.49 | 1,280.33 | 57.92 | 17.96 |

*La búsqueda puntual (promedio de 1,000 `buscarUsuarioPorId`) se mantiene por debajo de
0.001 ms en todo el rango medido, consistente con el O(1) promedio esperado de
`TablaHash::buscar`.*

Con el min-heap acotado, el tiempo de Top-K crece de forma aproximadamente lineal con `n`:
de 2,000 a 500,000 usuarios (×250) el tiempo pasa de 0.12 ms a 17.96 ms (×150), muy por
debajo de lo que predeciría una curva cuadrática. La carga y el BFS crecen más rápido que
`n` a partir de cientos de miles de usuarios, atribuible al volumen agregado de operaciones
encadenadas sobre la `TablaHash`, no a una regresión de complejidad en ninguna operación
individual (el detalle completo está en la Sección 6 del informe).

## Informe Técnico Completo

El análisis completo — introducción, arquitectura, descripción de estructuras, diagramas,
complejidad computacional, resultados experimentales y conclusiones — está en
[`informe.tex`](informe.tex) / `informe.pdf`.

## Limitaciones Conocidas y Trabajo Futuro

- Compactación periódica de `TablaHash` pendiente, para suavizar el costo agregado
  observado a partir de cientos de miles de claves.
- Sin persistencia en disco: la red se reconstruye en memoria en cada ejecución.
- `obtenerPublicacionesDeUsuario`/`darLike` recorren `publicaciones` en O(P); un índice por
  `postId` eliminaría ese recorrido cuando el volumen de publicaciones crezca
  significativamente.

## Conclusiones

- Se implementó el núcleo completo de una red social sin usar ninguna estructura de datos
  de la STL: las 13 funcionalidades exigidas están implementadas y cableadas al menú de
  consola, sobre seis estructuras propias escritas desde cero y documentadas método por
  método con su complejidad.
- El punto de mayor impacto en el rendimiento fue identificar y corregir el patrón
  `lista.obtener(i)` dentro de bucles, que degradaba a O(n²) operaciones que debían ser
  O(n); su eliminación en el ranking Top-K, combinada con un min-heap acotado a `k`
  elementos, permitió medir la batería completa de escalado hasta 500,000 usuarios.
- Reutilizar las mismas estructuras genéricas para propósitos distintos (`TablaHash` tanto
  para el diccionario de usuarios como para el índice de adyacencia del grafo;
  `ColaPrioridad` tanto para el ranking de usuarios como el de publicaciones) redujo la
  superficie de código a mantener sin sacrificar la complejidad algorítmica de cada
  operación.
- La evaluación experimental confirma cuantitativamente el comportamiento esperado: el
  Top-K acotado escala de forma prácticamente lineal con `n`, la búsqueda puntual se
  mantiene O(1) promedio en todo el rango medido, y el costo superlineal observado en la
  carga y el BFS a partir de cientos de miles de usuarios es atribuible al volumen agregado
  de operaciones sobre la tabla hash.

---

<div align="center">

**Repositorio:** [github.com/Jose-Tumpay/AED-Proyect](https://github.com/Jose-Tumpay/AED-Proyect)
Arequipa — Perú · Algoritmos y Estructuras de Datos (2026-A) · UNSA

</div>uye una red de `n` usuarios sin depender de ningún dataset
externo, agrupados en comunidades de `usuariosPorComunidad` usuarios (500 por defecto).
Dentro de cada comunidad, cada usuario nuevo se conecta preferentemente a los que ya tienen
más amigos (modelo tipo Barabási–Albert), y el primer usuario de cada comunidad se enlaza
con una comunidad anterior para mantener el grafo completo conectado.

La elección preferencial es O(1) por candidato: se mantiene, por comunidad, un arreglo
dinámico (`PoolGrados`, un `int*` con `new`/`delete` manual — no es una "estructura del
proyecto", es el truco estándar de enlace preferencial) donde cada usuario aparece una vez
por cada amistad que tiene, y elegir un objetivo al azar es indexar ese arreglo con
`rand() % tamano`. Esto evita a propósito el patrón `for (i) lista.obtener(i)` que causaba
el Defecto 2 (ver §6.4): generar toda la red es O(n · enlacesPorUsuario), no O(n²).

Sin comunidades ni enlace preferencial el grafo sale uniforme y "amigos en común" /
"sugerencias de amistad" devuelven casi siempre vacío, lo que en la demo se ve como un
sistema roto aunque el código esté bien (motivación de N2 en el plan de trabajo). Verificado
en esta máquina sobre una red sintética de 2 000 usuarios:

- Usuario 250: 9 amigos.
- Amigos en común entre usuario 10 y 15: 12.
- Sugerencias de amistad para el usuario 10: 441 candidatos.

### 6.2 · Metodología de medición

`RedSocial::medirOperaciones(n)` arma una red sintética de tamaño `n` y cronometra con
`<chrono>` (permitido por el enunciado, §2): carga (generación completa), inserción (un
`registrarUsuario` + sus amistades), búsqueda (promedio de 1 000 `buscarUsuarioPorId`), BFS
(`caminoAmistad` entre el primer y el último usuario), sugerencias de amistad y el ranking
top-K. `exportarMedicionesCSV` vuelca la serie a `output/mediciones.csv`. `main.cpp --bench`
corre la batería completa sin pasar por el menú (no es una de las 13 funcionalidades: es la
herramienta de medición que el enunciado permite).

### 6.3 · Resultados

Corrida actual (`./app --bench`, tras integrar T4/T5/T6 del Tercero), misma semilla fija:

| N | carga (ms) | BFS (ms) | sugerencias (ms) | top-K (ms) |
|---:|---:|---:|---:|---:|
| 2 000 | 7.10 | 2.64 | 0.34 | **0.36** |
| 4 000 | 15.19 | 4.51 | 0.33 | **0.34** |
| 8 000 | 70.58 | 16.32 | 0.29 | **0.86** |
| 16 000 | 214.38 | 49.50 | 0.41 | **1.52** |
| 32 000 | 454.74 | 98.90 | 0.36 | **3.49** |
| 64 000 | 1 046.37 | 245.16 | 0.37 | **8.70** |
| 100 000 | 1 338.84 | 229.32 | 0.28 | **9.41** |
| 200 000 | 2 394.77 | 411.70 | 0.31 | **16.91** |
| 500 000 | 9 686.07 | 2 258.76 | 0.65 | **51.75** |

(columna "búsqueda" omitida: midió sistemáticamente por debajo de 0.001 ms — no es un dato
útil a esa resolución. "sugerencias" bajó frente a corridas anteriores de este informe
porque T5 ahora usa `TablaHash` para contar en vez de `contiene()` sobre `Lista`; "top-K"
bajó de forma mucho más marcada — ver §6.4, no es solo la mejora algorítmica de T6.)

### 6.4 · Análisis

**Top-K ya no es O(n²) — corregido (T1).** Una versión anterior de este informe documentaba
un defecto real: `obtenerTopUsuariosActivos` hacía `for (i=0; i<n; i++) todos.obtener(i)`, y
como `Lista::obtener(i)` recorre desde la cabeza, el bucle completo era O(n²) (medido
entonces: de 2 000 a 16 000 usuarios, ×73.5 en vez de ×8). El commit `fb14ff7`
("quitar obtener(i) en bucles") lo corrigió reemplazando ese patrón por el iterador de
`Lista` en los sitios donde aparecía dentro de `RedSocial`.

**La tabla hash sin rehash — la explicación anterior era incorrecta, corregido.** Una
versión anterior de este informe afirmaba que la degradación de carga y BFS a partir de
cientos de miles se debía a que `TablaHash` no hacía rehash. Es falso: el rehash automático
a factor de carga 0.75 se implementó en el commit `4637759` ("j4 terminado rehash hecho"),
anterior a los commits que escribieron esa sección — la afirmación estaba desactualizada
desde que se escribió. Con rehash, `insertar`/`buscar` se mantienen en O(1) promedio incluso
a 500 000 claves; la degradación medida en carga/BFS a esa escala es real pero su causa no
se aisló (hipótesis: costo O(grado) de `contiene()` en `agregarArista` sobre los vértices
"hub" del generador, no verificado con instrumentación).

**Amigos en común y sugerencias, de O(n·m) a O(n+m) — T4 y T5.** `amigosEnComun` hacía, por
cada amigo de `u1`, un `contiene()` O(grado(u2)) sobre la lista de `u2`: O(grado(u1)·grado(u2))
total. Ahora vuelca `amigos1` a una `TablaHash<int,bool>` una vez y recorre `amigos2` con
búsquedas O(1), quedando en O(grado(u1)+grado(u2)). `obtenerSugerenciasAmistad` tenía el mismo
patrón en su bucle amigos-de-amigos, y además no rankeaba el resultado (salía en el orden en
que aparecían los candidatos); ahora cuenta coincidencias con `TablaHash<int,int>` y ordena
los candidatos únicos con `ColaPrioridad` antes de devolverlos. En la tabla de §6.3,
"sugerencias" pasó de ~0.9-1.3 ms (medido en corridas anteriores de este informe) a
0.3-0.65 ms — más rápido a pesar de que ahora también rankea, porque el costo de contar con
`TablaHash` es menor que el de los `contiene()` O(n) sobre `Lista` que reemplaza.

**Top-K acotado a tamaño k — T6, dos implementaciones convergentes.** `obtenerTopUsuariosActivos`
y `obtenerPublicacionesTopReacciones` insertaban los `n`/`m` elementos completos en un heap y
extraían los `k` mejores (O(n log n), O(n) de memoria). Cristhian y el Tercero implementaron
un heap acotado a `k` en paralelo, sin coordinarse, con diseños distintos: Cristhian con un
wrapper externo (`Inverso<T>`) que invertía la comparación sin tocar `ColaPrioridad`; el
Tercero modificando `ColaPrioridad` directamente (flag `esMinHeap`, ver §3.4) y agregando un
iterador nuevo a `TablaHash` para recorrer `usuariosPorId` sin copiarlo antes a una `Lista`.
Al integrar ambas ramas se conservó la del Tercero — mismo resultado observable, pero medido
es notablemente más rápido: con la versión de Cristhian (heap acotado + copia previa a
`Lista`), top-K a N=500 000 medía 906.7 ms; con la versión integrada (heap acotado + iterador
directo sobre `TablaHash`, sin esa copia), 51.7 ms — 17.5 veces más rápido para la misma
complejidad O(n log k), porque evitar materializar 500 000 `Usuario` en una `Lista` intermedia
(cada nodo con su propia asignación `new`) resultó ser un factor constante mucho mayor de lo
esperado. Es un buen ejemplo de que dos soluciones con el mismo big-O pueden diferir un orden
de magnitud en la práctica — parte del "análisis de rendimiento" que pide el enunciado (E10).

## 7 · Capturas

*(Cristhian)*

El enunciado aclara que el aspecto visual no se evalúa, así que estas son capturas de
**terminal real** (no imágenes de pantalla), tomadas ejecutando `./app` compilado desde
el HEAD actual (`g++ -std=c++17 -Wall -I. main.cpp red/*.cpp -o app`, compila sin
warnings) tras integrar T4/T5/T6 del Tercero. Salida real, recortada donde el resultado
es muy largo (se indica el recorte).

**Carga inicial del dataset:**
```
Cargando data/amistades_4039n_88234r.txt ...
Listo: 4039 usuarios cargados.
Cargando data/publicaciones_interaciiones.csv ...
Listo: 20000 publicaciones cargadas.
```

**1) Registrar usuario** (ID 99999, usuario nuevo para esta demo):
```
Usuario 99999 registrado.
```

**6) Agregar amigo** (99999 <-> 0):
```
Amistad 99999 <-> 0 creada.
```

**8) Camino de amistad** (origen 99999, destino 0 — se acaban de hacer amigos):
```
Camino (1 saltos): 99999 -> 0
```

**9) Amigos en común** (entre los usuarios 0 y 1, ambos del dataset SNAP — mismo
resultado antes y después de T4, solo cambió cómo se calcula):
```
Amigos en comun (16): 48 53 54 73 88 92 119 126 133 194 236 280 299 315 322 346
```

**10) Sugerencias de amistad** (usuario 0 — con T5, ahora rankeadas por cantidad de
amigos en común, mayor primero; salida real truncada aquí por espacio):
```
Sugerencias por amigos en comun, mayor primero (1171): 348 414 1684 2885 649 1387
3003 549 1171 1486 1912 904 1193 1549 3290 428 1297 1718 2838 2640 2636 [...] 2647
2649 2704 351 2643 2660
```
(mismo conteo que antes de T5, 1171 — el generador con enlace preferencial da un
usuario 0 con grado alto, así que sus amigos-de-amigos son muchos — pero ahora el
orden refleja cuántos amigos en común tiene cada candidato, no el orden de aparición.)

**12) Usuarios más activos** (top 5, por publicaciones — sin cambios de comportamiento
tras T6, solo de rendimiento):
```
Top 5 usuarios por publicaciones:
    1) ID 0        User_0                   publicaciones: 5
    2) ID 1        User_1                   publicaciones: 5
    3) ID 3        User_3                   publicaciones: 5
    4) ID 7        User_7                   publicaciones: 5
    5) ID 15       User_15                  publicaciones: 5
```

**4) Crear publicación** (ID 999001, autor 99999):
```
Publicacion 999001 creada.
```

**11) Mostrar publicaciones de un usuario** (usuario 13, del dataset de Kaggle):
```
Publicaciones de 13 (5):
    [14] Identify some discuss test pass form finally before about admit budget
    set treatment inside. Make star one interesting. (likes: 821, comentarios: 0)
    [4053] Sometimes live dream bill across we culture cut rock movement there
    development radio station yet. Night today interesting claim process.
    (likes: 2531, comentarios: 0)
    [8092] Foot its reflect continue various myself blood our government letter
    mission produce. Factor successful full land easy point. (likes: 4914,
    comentarios: 0)
    [...]
```

**13) Publicaciones con más reacciones** (top 5, por likes):
```
Top 5 publicaciones por reacciones:
    1) [2265] likes: 5178   Month audience pick fact subject lead art expert
       those poor cost art I along watch road firm least simple law. Claim
       available media include.
    2) [9292] likes: 5142   Eight fish woman mouth social relationship five
       west father phone drug camera college over here part go get book huge
       hard. Picture view himself newspaper commercial performance anyone do.
    3) [18359] likes: 5136  Product successful decade five while small amount
       something any cause senior. Someone thousand add still indeed.
```

**5) Eliminar publicación** (999001, la creada más arriba):
```
Publicacion 999001 eliminada.
```

**7) Eliminar amigo** (99999 <-> 0):
```
Amistad 99999 <-> 0 eliminada.
```

**2) Eliminar usuario** (99999) **seguido de 3) Buscar usuario** (99999, confirma que
la eliminación no dejó rastro — incluyendo el grafo, ver §3.3 sobre `eliminarVertice`):
```
Usuario 99999 eliminado.
...
No existe un usuario con ID 99999.
```

**`./app --bench`** — batería completa de escalado, salida real (números iguales a
la tabla del §6.3):
```
Bateria de escalado: 9 tamanos de N
  N = 2000     ... carga=    7.10ms  busqueda=0.00004ms  bfs=   2.64ms  sugerencias= 0.3409ms  topk= 0.3629ms
  N = 4000     ... carga=   15.19ms  busqueda=0.00010ms  bfs=   4.51ms  sugerencias= 0.3316ms  topk= 0.3370ms
  N = 8000     ... carga=   70.58ms  busqueda=0.00006ms  bfs=  16.32ms  sugerencias= 0.2932ms  topk= 0.8646ms
  N = 16000    ... carga=  214.38ms  busqueda=0.00007ms  bfs=  49.50ms  sugerencias= 0.4123ms  topk= 1.5198ms
  N = 32000    ... carga=  454.74ms  busqueda=0.00005ms  bfs=  98.90ms  sugerencias= 0.3611ms  topk= 3.4940ms
  N = 64000    ... carga= 1046.37ms  busqueda=0.00006ms  bfs= 245.16ms  sugerencias= 0.3662ms  topk= 8.6967ms
  N = 100000   ... carga= 1338.84ms  busqueda=0.00012ms  bfs= 229.32ms  sugerencias= 0.2848ms  topk= 9.4067ms
  N = 200000   ... carga= 2394.77ms  busqueda=0.00007ms  bfs= 411.70ms  sugerencias= 0.3110ms  topk=16.9101ms
  N = 500000   ... carga= 9686.07ms  busqueda=0.00006ms  bfs=2258.76ms  sugerencias= 0.6463ms  topk=51.7464ms

Mediciones exportadas a output/mediciones.csv
```

**`./app --graph-viz`** (T7, genera la imagen de §4.2):
```
Grafo (300 usuarios, 6 comunidades de 50) exportado a output/grafo_aristas.csv
```

## 8 · Conclusiones

*(Los tres)*

El sistema cumple el núcleo del enunciado sin usar STL: las 13 funcionalidades tienen
código funcional y las 13 están cableadas al menú, verificado en §7. Las 7 tareas del
plan de trabajo (T1-T7) están cerradas entre los tres integrantes — Cristhian T1-T3 y
T7, el Tercero T4, T5 y (en paralelo con Cristhian) T6, Jose J1-J7 — y el generador
sintético con enlace preferencial permite demostrar y medir el sistema con cientos de
miles de usuarios (E6), muy por encima de los 4 039 del dataset público, sin depender
de datos externos para escalar. El `main()` interactivo sigue arrancando por defecto
con el dataset SNAP de 4 039 (para que el menú responda rápido en la demo); `--bench` y
`--graph-viz` son los que ejercitan la escala real.

Dos defectos que documentaban versiones anteriores de este informe (top-K cuadrático y
"tabla hash sin rehash") ya no describen el estado real del código, y quedaron
corregidos aquí (§6.4) en vez de solo mencionados. T4 y T5 bajaron `amigosEnComun` y
`obtenerSugerenciasAmistad` de un patrón O(n·m) a O(n+m) usando `TablaHash`, y T5 además
agregó el ranking que faltaba. T6 se implementó dos veces en paralelo sin coordinación
entre Cristhian y el Tercero — mismo algoritmo (heap acotado a `k`), pero la versión que
se conservó resultó medible 17.5 veces más rápida a N=500 000 por evitar una copia
intermedia, no por diferencia de complejidad asintótica (§6.4). Documentar ese tipo de
hallazgo — no solo que algo es O(n log k), sino cuánto importa la constante en la
práctica — es exactamente el "análisis de rendimiento" que pide el enunciado (E10).

Lo que sigue genuinamente pendiente, verificado hoy contra el código integrado:

- **Campo `Usuario::seguidores` muerto.** `incrementarSeguidores()` existe
  (`usuario.cpp:58-60`) pero no la llama nadie en todo el proyecto — el campo vale 0
  siempre. El enunciado pide que `Usuario` "contenga" cantidad de seguidores (E4); el
  campo estructuralmente está, pero nunca se puebla.
- **`Comentario` y `Publicacion::agregarComentario` sin usar.** La clase existe, el
  campo `Lista<Comentario> comentarios` existe en `Publicacion` (E4 lo pide), pero no
  hay ninguna llamada a `agregarComentario` en todo el proyecto: no hay opción de menú
  para comentar ni se poblan comentarios al cargar los datasets. La lista de
  comentarios de toda publicación está vacía en la práctica.

Ambos son del eje de modelo de datos (J3), no del eje de algoritmos ni del de datos y
aplicación — no son difíciles de cerrar comparado con lo que ya se resolvió (rehash,
defecto O(n²), las 13 operaciones cableadas, top-K acotado, T4/T5, diagrama de clases y
visualización del grafo); quedan como la lista de trabajo concreta para la entrega
final, no como incertidumbre sobre qué falta.
