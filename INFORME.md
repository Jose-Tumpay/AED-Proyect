# Informe técnico — Red social con estructuras de datos propias

Proyecto Final, curso de Algoritmos y Estructuras de Datos (AED). Esqueleto y contenido
según el reparto de `PLAN-v2.pdf`: seis de las ocho secciones están firmadas; las otras dos
(Descripción de las estructuras, Complejidad computacional) quedan marcadas como
pendientes para Jose, y la parte de Diagramas de grafo/pseudocódigo para el Tercero
integrante.

Estado: **borrador en progreso**. Los números de la sección 6 son reales (se generaron
corriendo `./app --bench` sobre este repositorio); las capturas de pantalla de la sección 7
todavía no se tomaron.

> ⚠️ **Nota de Jose (J7), tras revisar el código actual:** la §6 se escribió _antes_ de que
> se integraran J4 (rehash en `TablaHash`) y T1 (se quitó el patrón `obtener(i)` en bucle de
> `obtenerTopUsuariosActivos` y del resto de sitios señalados en el plan). El §6.4 describe
> ambos defectos como si siguieran presentes en el código — ya no lo están. Hay que volver a
> correr `./app --bench` sobre el `main` actual y actualizar la tabla de §6.3 y el análisis de
> §6.4 antes de la entrega, o el informe va a contradecir al propio código que evalúan en la
> sustentación (riesgo directo para E9). No lo corrijo yo porque los números y el guion de esa
> sección son de C4/C5 (Cristhian) — esto es solo la alerta.

---

## 1 · Introducción

_(Cristhian — C5)_

El proyecto implementa una red social mínima en C++17, sin ninguna estructura de datos de
la STL (E1 del enunciado): toda lista, tabla hash, grafo, cola y cola de prioridad que usa
el sistema está escrita desde cero en `estructuras/`.

El sistema soporta las 13 funcionalidades pedidas en el enunciado (registrar, eliminar y
buscar usuarios; crear, eliminar y listar publicaciones; gestionar amistades; camino más
corto entre usuarios; amigos en común; sugerencias de amistad; ranking de usuarios activos
y de publicaciones por reacciones), organizadas en un menú de consola (`main.cpp`)
deliberadamente sobrio, ya que el enunciado aclara que el aspecto visual no se evalúa.

Como fuente de datos se usa una combinación de:

- Un dataset público de amistades (formato SNAP, `data/amistades_4039n_88234r.txt`):
  4 039 usuarios, 88 234 aristas.
- Un CSV de publicaciones de Kaggle (`data/publicaciones_interaciiones.csv`, 20 000 filas)
  para poblar contenido realista.
- Un **generador sintético propio** (`RedSocial::generarUsuariosSinteticos`, ver §6) capaz
  de producir cientos de miles de usuarios con su grafo de amistades, para poder medir
  escalabilidad más allá de lo que ofrece el dataset público (E6, E7).

El resto del informe describe la arquitectura del sistema, los resultados experimentales
de escalabilidad obtenidos con el generador sintético, y las conclusiones del equipo.

## 2 · Arquitectura

_(Cristhian — C5)_

El código está organizado en tres módulos (E3):

```
estructuras/    Contenedores genéricos, sin conocimiento del dominio "red social"
  lista.h         Lista doblemente enlazada con iterador (base de casi todo lo demás)
  tablaHash.h     Tabla hash generica <K,V>, encadenamiento con Lista<Par>, hash DJB2 para
                  strings y modulo para enteros
  grafo.h         Grafo no dirigido sobre TablaHash<int, Lista<int>>; BFS para camino mas
                  corto
  colaPrioridad.h Heap binario sobre arreglo, usado para rankings (top-K)
  cola.h, Pila.h  Cola y pila clasicas, usadas por el BFS y utilidades internas

red/            El dominio: usuarios, publicaciones, comentarios y la fachada RedSocial
  usuario.h/.cpp        Los 9 campos de Usuario (E4)
  publicacion.h/.cpp    Los 7 campos de Publicacion (E4), incluida su Lista<Comentario>
  comentario.h/.cpp     Comentario individual de una publicacion
  redSocial.h/.cpp      Fachada: une TablaHash<int,Usuario> + Grafo + Lista<Publicacion> y
                         expone las operaciones del enunciado
  redSocial_io.cpp      Generador sintetico, enlace preferencial y medicion de tiempos
                         (C2-C4 de este informe)

main.cpp        Menu de consola (13 opciones) + modo `--bench` (bateria de escalado)
```

Diseño de la fachada `RedSocial`: por requisito (E4) cada `Usuario` guarda su propia
`Lista<int> amigos`, y por eficiencia de recorrido el `Grafo` mantiene además su propia
`TablaHash<int, Lista<int>> adyacencia`. Es una duplicación deliberada — el campo del
enunciado y el índice de recorrido no son la misma responsabilidad — sincronizada en la
misma operación (`agregarAmistad`, `eliminarAmistad`, `eliminarUsuario`).

Diagrama de módulos (versión mínima; el diagrama de clases completo y el del grafo generado
quedan para T7, ver §4):

```
        main.cpp
           |
           v
     red/redSocial.{h,cpp}  <---- red/redSocial_io.cpp (generador, mediciones)
       |        |       \
       v        v        v
  usuario   publicacion  (usa)
       \        |
        \       v
         \  comentario
          \     |
           v    v
      estructuras/{lista,tablaHash,grafo,colaPrioridad,cola,Pila}.h
```

## 3 · Descripción de las estructuras

_(Jose — J7)_

Todas las estructuras de `estructuras/` son plantillas (`template <typename T>` o `<K,V>`)
para reutilizarse en distintos tipos sin duplicar código, y ninguna usa contenedores de la
STL (E1): la memoria se gestiona a mano con `new`/`delete`.

**`Lista<T>` — doblemente enlazada, con punteros a cabeza y cola.**
Es la estructura base de la que dependen todas las demás (`TablaHash` la usa para
encadenamiento, `Grafo` para las listas de adyacencia, `Cola`/`Pila` son listas enlazadas
simples especializadas). Se eligió doblemente enlazada porque `eliminar(dato)` necesita
reconectar el nodo anterior y siguiente sin tener que volver a recorrer la lista desde la
cabeza; con un solo enlace, borrar un nodo intermedio requeriría guardar el nodo anterior
durante el recorrido. El costo es el sobrecoste de memoria por nodo (documentado y medido en
§7 del plan de trabajo: 24 bytes de nodo para guardar 4 bytes de `int`, seis veces el dato
útil), que el equipo decidió aceptar porque no está en el camino crítico de E6/E10 — el
cuello de botella medido fue el patrón `obtener(i)` en bucle (Defecto 2 del plan, corregido
en T1), no el tamaño del nodo en sí.
`Lista` expone un `Iterador` (`begin()`/`end()`) para permitir `for (T& x : lista)` en O(n)
total; `obtener(indice)` existe para acceso puntual por posición pero es O(n) por llamada —
usarlo dentro de un bucle `for (i=0..n)` es el error que causaba el Defecto 2.
Constructor de copia y `operator=` sí están implementados (a diferencia de las otras cuatro
estructuras, ver más abajo) porque `Lista` se retorna por valor en varias operaciones de
`RedSocial` (`caminoAmistad`, `amigosEnComun`, `obtenerVecinos`, etc.) y necesita una copia
profunda correcta para eso.

**`TablaHash<K,V>` — encadenamiento con `Lista<Par>` por cubeta.**
Cada cubeta es una `Lista` de pares clave-valor; colisionar no sobreescribe nada, solo
alarga la lista de esa cubeta. Se usan dos funciones hash: módulo directo para claves
enteras (los IDs de usuario/publicación son consecutivos, por lo que el módulo reparte bien
sin necesidad de mezclar bits) y DJB2 (`hash = hash*33 + c`) para claves de texto, elegida
por su buena distribución empírica sobre cadenas cortas y su simplicidad de implementar sin
librerías externas. La capacidad inicial es un primo (10 007) para reducir patrones de
colisión con claves que comparten factores comunes. Para que la carga no se acumule sin
control, `insertar` dispara `rehashear()` cuando el factor de carga supera 0.75 (J4):
duplica la capacidad (`capacidad*2+1`, se mantiene impar por la misma razón que el primo
inicial) y reinserta todo. El costo de un rehash individual es O(n), pero como solo se
dispara cuando `n` creció proporcional a la capacidad, el costo amortizado por inserción
sigue siendo O(1) (mismo argumento que el crecimiento geométrico de un `std::vector`, aplicado
aquí a mano). Constructor de copia y `operator=` están bloqueados con `= delete` (J5/N7):
`tabla` es un puntero a un arreglo de `Lista`, y una copia por defecto duplicaría el puntero,
no el arreglo — el destructor de ambas copias liberaría la misma memoria dos veces
(double free). Como ninguna operación del proyecto necesita copiar una `TablaHash` completa
(siempre se usa como miembro o variable local, nunca se pasa ni retorna por valor), bloquear
la copia convierte un bug potencial en tiempo de ejecución en un error de compilación.

**`Grafo` — no dirigido, sobre `TablaHash<int, Lista<int>>`.**
Cada vértice es una clave de la tabla hash; su valor es la `Lista<int>` de sus vecinos. Esto
da acceso a la lista de adyacencia de un vértice en O(1) promedio, en vez de buscarlo
linealmente en un arreglo de vértices como haría una matriz o lista de adyacencia clásica
sobre un arreglo. El costo es que `Usuario::amigos` y `Grafo::adyacencia` guardan
información parcialmente redundante (la lista de amigos de un usuario existe en los dos
lados) — una decisión deliberada, no un descuido: `Usuario::amigos` es el campo que exige el
enunciado (E4), y `Grafo::adyacencia` es el índice que necesita el BFS para ser eficiente.
La sincronización entre ambos ocurre siempre en la misma operación (`agregarAmistad`,
`eliminarAmistad`, `eliminarUsuario`), nunca por separado, para que no puedan desincronizarse
a mitad de una llamada. `caminoMasCorto` usa BFS clásico con una `Cola<int>` para el
recorrido y dos `TablaHash` auxiliares (`visitado`, `padre`) en vez de arreglos, porque los
IDs de usuario no son necesariamente un rango denso desde 0 — un arreglo indexado por ID
desperdiciaría memoria o requeriría un mapeo adicional.

**`ColaPrioridad<T>` — heap binario sobre un arreglo (`T*`), sin nodos ni punteros.**
Se eligió arreglo en vez de una estructura enlazada de árbol porque un heap binario completo
se indexa aritméticamente (hijos de `i` en `2i+1` y `2i+2`) sin necesidad de punteros
padre/hijo, lo que evita el sobrecoste por nodo que sí tiene `Lista` y hace que
`insertar`/`extraerMaximo` sean O(log n) con muy poca sobrecarga de memoria por elemento.
Cuando el arreglo se llena, `redimensionar()` lo duplica (mismo patrón de crecimiento
geométrico que el rehash de `TablaHash`, mismo argumento de costo amortizado O(1)). Se usa
para los dos rankings del proyecto: usuarios más activos (por `Usuario::operator>`, que
compara `contadorPublicaciones`) y publicaciones con más reacciones (por
`Publicacion::operator>`, que compara `likes`) — reutilizando la misma estructura genérica
para ambos casos en vez de escribir un heap especializado por tipo. Copia bloqueada con
`= delete` por el mismo motivo que `TablaHash`: `heap` es un puntero crudo a un arreglo.

**`Cola<T>` y `Pila<T>` — lista enlazada simple especializada, sin usar `Lista<T>` genérica.**
Ambas son la estructura clásica de libro de texto (nodo con un solo puntero `siguiente`),
implementadas por separado en vez de reutilizar `Lista<T>` porque sus invariantes son más
estrictas y más baratas de mantener: `Cola` solo necesita insertar por un extremo y quitar
por el otro (no requiere doble enlace para eso), y `Pila` solo apila/desapila por un extremo.
Usar la `Lista` doblemente enlazada genérica para estos dos casos habría funcionado, pero
hubiera cargado con el puntero `anterior` que ninguna de las dos operaciones necesita.
`Cola` es la estructura auxiliar del BFS de `Grafo`; `Pila` no se usa todavía en el flujo
principal del proyecto, pero se deja completa y documentada por si una futura funcionalidad
(por ejemplo, deshacer/rehacer una acción) la necesita. Copia bloqueada en ambas por la misma
razón que en `TablaHash`/`ColaPrioridad`: son punteros crudos a nodos, y una copia por
defecto compartiría los mismos nodos entre dos instancias que luego los liberarían dos veces.

## 4 · Diagramas

_(Tercero + Cristhian — T7)_

> TODO Tercero: diagrama de clases completo y diagrama del grafo generado (una visualización
> del grafo sintético de §6, por ejemplo con las comunidades coloreadas). Ver la tarea T7 en
> `PLAN-v2.pdf`.

## 5 · Complejidad computacional

_(Jose — J7)_

`n` = usuarios totales, `P` = publicaciones totales, `grado(u)` = cantidad de amigos de `u`,
`A` = aristas del grafo de amistad, `V` = vértices. Las anotaciones `/// @complejidad` están
además puestas en el código fuente, método por método, en `estructuras/*.h`.

### 5.1 · Estructuras base

| Estructura            | Operación                          | Complejidad          | Nota                                                              |
| --------------------- | ---------------------------------- | -------------------- | ----------------------------------------------------------------- |
| `Lista<T>`            | `agregarInicio`, `agregarFinal`    | O(1)                 | punteros a cabeza y cola                                          |
|                       | `eliminar(dato)`, `contiene(dato)` | O(n)                 | búsqueda lineal                                                   |
|                       | `obtener(indice)`                  | O(n)                 | recorre desde la cabeza; O(n²) si se llama en bucle               |
|                       | copia (ctor / `operator=`)         | O(n)                 | única de las 5 estructuras que copia (se retorna por valor)       |
| `TablaHash<K,V>`      | `insertar`, `buscar`, `eliminar`   | O(1) promedio        | O(long. de cubeta) peor caso; amortizado O(1) con el rehash       |
|                       | `rehashear`                        | O(n)                 | se dispara al superar 75% de carga; amortizado O(1) por inserción |
|                       | `obtenerTodosLosValores`           | O(capacidad + n)     | recorre todas las cubetas                                         |
| `Grafo`               | `agregarVertice`                   | O(1) promedio        | `TablaHash::buscar` + `insertar`                                  |
|                       | `agregarArista`, `eliminarArista`  | O(grado(u)+grado(v)) | `Lista::contiene`/`eliminar` en cada lado                         |
|                       | `eliminarVertice`                  | O(grado(u))          | solo recorre los vecinos directos (J1)                            |
|                       | `caminoMasCorto` (BFS)             | O(V + A)             | cada vértice se encola una vez, cada arista se examina una vez    |
| `ColaPrioridad<T>`    | `insertar`, `extraerMaximo`        | O(log n)             | `flotar`/`hundir` acotados por la altura del heap                 |
| `Cola<T>` / `Pila<T>` | todas sus operaciones              | O(1)                 | listas enlazadas simples, un extremo fijo                         |

### 5.2 · Operaciones de `RedSocial` (composición de las anteriores)

| Operación                                    | Complejidad                                          | Justificación                                                                                                                                                                                                     |
| -------------------------------------------- | ---------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `registrarUsuario`                           | O(1) promedio amortizado                             | 1 `buscar` + 1 `insertar` en `TablaHash` + `agregarVertice`                                                                                                                                                       |
| `agregarAmistad` / `eliminarAmistad`         | O(grado(u1)+grado(u2))                               | domina `Grafo::agregarArista`/`eliminarArista`                                                                                                                                                                    |
| `crearPublicacion`                           | O(1) promedio amortizado                             | `agregarFinal` en `Lista` + `agregarPublicacion` en `Usuario`                                                                                                                                                     |
| `buscarUsuarioPorId`                         | O(1) promedio                                        | `TablaHash::buscar` directo                                                                                                                                                                                       |
| `caminoAmistad`                              | O(V + A)                                             | delega en `Grafo::caminoMasCorto` (BFS)                                                                                                                                                                           |
| `eliminarUsuario`                            | O(grado(u))                                          | domina `Grafo::eliminarVertice` (J1)                                                                                                                                                                              |
| `darLike`                                    | O(P)                                                 | recorre `publicaciones` buscando el `postId` (no está indexada por ID)                                                                                                                                            |
| `obtenerTopUsuariosActivos(k)`               | O(n log n)                                           | tras el fix de T1: `n` inserciones al heap O(log n) cada una, luego `k` extracciones                                                                                                                              |
| `obtenerPublicacionesConMasReacciones(k)`    | O(P log P)                                           | mismo patrón que el anterior, sobre publicaciones                                                                                                                                                                 |
| `amigosEnComun`                              | O(grado(u1) · grado(u2))                             | por cada amigo de `u1`, `Lista::contiene` sobre los de `u2` es O(grado(u2)); **T4 en el plan de trabajo propone bajar esto a O(grado(u1)+grado(u2)) volcando un lado a una `TablaHash`, todavía no implementado** |
| `obtenerSugerenciasAmistad`                  | O(Σ grado(amigo) · (grado(u)+tamaño de sugerencias)) | recorrido amigos-de-amigos con `contiene` lineal en cada paso; **T5 propone rankearlo con `TablaHash`+`ColaPrioridad`, todavía no implementado**                                                                  |
| `obtenerPublicacionesDeUsuario`              | O(P)                                                 | recorre todas las publicaciones filtrando por autor (no indexado)                                                                                                                                                 |
| `cargarGrafoSNAP` / `cargarPublicacionesCSV` | O(A) / O(P)                                          | una pasada por el archivo, cada línea es O(1) amortizado                                                                                                                                                          |

### 5.3 · Lectura de la tabla

Dos filas quedan marcadas explícitamente como pendientes (`amigosEnComun`,
`obtenerSugerenciasAmistad`): el análisis de complejidad describe el código tal como está
hoy, no el objetivo final del plan de trabajo. Es intencional dejarlo así en vez de reportar
la complejidad _deseada_: el enunciado pide "análisis de rendimiento" (E10) sobre el sistema
real, y ocultar que dos operaciones todavía son más lentas de lo necesario contradice ese
objetivo. Si T4/T5 se completan antes de la entrega, esta tabla debe actualizarse.

## 6 · Resultados experimentales

_(Cristhian — C5)_

### 6.1 · Generador sintético y enlace preferencial

`RedSocial::generarUsuariosSinteticos(n, enlacesPorUsuario, usuariosPorComunidad)`
(`red/redSocial_io.cpp`) construye una red de `n` usuarios sin depender de ningún dataset
externo, agrupados en comunidades de `usuariosPorComunidad` usuarios (500 por defecto).
Dentro de cada comunidad, cada usuario nuevo se conecta preferentemente a los que ya tienen
más amigos (modelo tipo Barabási–Albert), y el primer usuario de cada comunidad se enlaza
con una comunidad anterior para mantener el grafo completo conectado.

La elección preferencial es O(1) por candidato: se mantiene, por comunidad, un arreglo
dinámico (`PoolGrados`, un `int*` con `new`/`delete` manual — no es una "estructura del
proyecto", es el truco estándar de enlace preferencial) donde cada usuario aparece una vez
por cada amistad que tiene, y elegir un objetivo al azar es indexar ese arreglo con
`rand() % tamano`. Esto evita a propósito el patrón `for (i) lista.obtener(i)` que causa el
Defecto 2 (ver §6.4): generar toda la red es O(n · enlacesPorUsuario), no O(n²).

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

Corridos en esta máquina, `./app --bench`, semilla fija (reproducible):

|       N | carga (ms) | búsqueda (µs, promedio) | BFS (ms) | sugerencias (ms) | top-K (ms) |
| ------: | ---------: | ----------------------: | -------: | ---------------: | ---------: |
|   2 000 |       6.44 |                    0.03 |     2.20 |             0.93 |      12.14 |
|   4 000 |      10.99 |                    0.03 |     3.02 |             0.92 |      41.26 |
|   8 000 |      20.79 |                    0.03 |     8.84 |             0.98 |     147.58 |
|  16 000 |      44.91 |                    0.04 |    20.99 |             1.15 |     891.77 |
|  32 000 |     102.11 |                    0.04 |    64.15 |             1.25 | no medido¹ |
|  64 000 |     268.71 |                    0.04 |   187.69 |             1.25 | no medido¹ |
| 100 000 |     492.38 |                    0.05 |   260.52 |             1.35 | no medido¹ |
| 200 000 |   1 499.31 |                    0.04 |   730.67 |             1.36 | no medido¹ |
| 500 000 |   8 913.31 |                    0.05 | 7 319.58 |             1.56 | no medido¹ |

¹ Por encima de 20 000 usuarios el top-K no se mide: se colgaría la batería completa (ver
§6.4, Defecto 2). El límite está en `LIMITE_MEDICION_TOPK` (`redSocial_io.cpp`).

### 6.4 · Análisis — dos límites detectados

**Top-K es O(n²), no O(n log n).** `RedSocial::obtenerTopUsuariosActivos` (`redSocial.cpp:67`)
hace `for (i = 0; i < n; i++) todos.obtener(i)`: cada `obtener(i)` recorre la `Lista`
enlazada desde la cabeza, así que el bucle completo es O(n²). En la tabla se ve: de
2 000 a 16 000 usuarios (×8) el tiempo pasa de 12.1 ms a 891.8 ms (×73.5, no ×8, y cercano
al ×64 que predice una curva cuadrática). Con esa curva, medir a 100 000 hubiera tardado
varios minutos — por eso se acotó la medición en vez de reportar un número engañoso. Es el
mismo defecto que el plan de trabajo documenta como "Defecto 2" y asigna a la tarea T1
(Tercero): sustituir `obtener(i)` por el iterador de `Lista` en los seis sitios listados,
entre ellos esta misma línea.

**La tabla hash sin rehash degrada la carga y el BFS a partir de cientos de miles.** La
`TablaHash` tiene capacidad fija (10 007 cubetas por defecto, sin rehash — tarea J4 del
plan): con 500 000 claves eso da en promedio ~50 elementos por cubeta, así que cada
inserción y cada búsqueda de vecinos deja de ser O(1) para acercarse a O(50). El costo de
un lookup aislado sigue siendo pequeño en términos absolutos (por eso la columna "búsqueda"
casi no se mueve: de 0.03 µs a 2 000 usuarios a 0.05 µs a 500 000, una `buscarUsuarioPorId`
suelta es barata aunque la cadena tenga 50 elementos), pero el efecto se acumula en las
operaciones que hacen muchos lookups: de 100 000 a 200 000 usuarios (×2) la carga pasa de
492 ms a 1 499 ms (×3.0) y el BFS de 261 ms a 731 ms (×2.8); de 200 000 a 500 000 (×2.5) la
carga sube a 8 913 ms (×5.9) y el BFS a 7 320 ms (×10.0) — la degradación se acelera con N
en ambas, consistente con una tabla que ya no reparte bien sus claves. No se implementa el
rehash aquí (no es tarea de C4), pero el efecto es medible y consistente con la proyección
que hace el plan de trabajo en su §7 sobre la misma causa (capacidad fija sin rehash).

## 7 · Capturas

_(Cristhian — C5, pendiente)_

> TODO: capturas de pantalla del menú (`./app`) ejecutando al menos: registrar usuario,
> buscar usuario, camino de amistad, amigos en común, sugerencias y usuarios más activos —
> más una corrida de `./app --bench` mostrando la tabla de la §6.3. El enunciado no evalúa
> el aspecto visual, así que basta con capturas de la terminal.

## 8 · Conclusiones

_(Los tres — borrador de Cristhian, falta revisión conjunta)_

El sistema cumple el núcleo del enunciado sin usar STL: las 13 funcionalidades tienen
código funcional y 11 ya están cableadas al menú (las 2 restantes, mostrar publicaciones de
un usuario y publicaciones con más reacciones, son la tarea T2/T3 del plan). El generador
sintético con enlace preferencial permite demostrar y medir el sistema con cientos de miles
de usuarios (E6), muy por encima de los 4 039 del dataset público, sin depender de datos
externos para escalar.

Las mediciones de la §6 muestran, con números y no solo con intuición, los dos límites de
la implementación actual: el top-K cuadrático (Defecto 2 / T1) y la tabla hash sin rehash
(J4). Ambos son arreglos acotados y ya están identificados en el plan de trabajo; documentar
la degradación en vez de ocultarla es, según el propio enunciado, parte del "análisis de
rendimiento" que se evalúa (E10).

> TODO equipo: ampliar esta sección tras cerrar J4 (rehash) y T1 (quitar `obtener(i)` en
> bucle) — repetir la batería de §6 y contrastar antes/después.
