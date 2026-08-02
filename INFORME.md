# Informe técnico — Red social con estructuras de datos propias

Proyecto Final, curso de Algoritmos y Estructuras de Datos (AED). Estructura según el
reparto de `PLAN-v2.pdf` (E8: informe con 8 secciones). Esqueleto inicial: los títulos y
los responsables ya están fijados; el contenido se completa en los commits siguientes.

---

## 1 · Introducción

*(Cristhian — C5)*

> TODO Cristhian: alcance del proyecto, fuentes de datos (SNAP, CSV de Kaggle, generador
> sintético propio) y resumen de las 13 funcionalidades pedidas por el enunciado.

## 2 · Arquitectura

*(Cristhian — C5)*

> TODO Cristhian: módulos del proyecto (`estructuras/`, `red/`, `main.cpp`), diseño de la
> fachada `RedSocial` y diagrama de módulos.

## 3 · Descripción de las estructuras

*(Jose — J7, pendiente)*

> TODO Jose: describir el diseño interno de cada estructura de `estructuras/` (invariantes,
> por qué doblemente enlazada en `Lista`, por qué encadenamiento en `TablaHash`, por qué
> heap binario en `ColaPrioridad`, la función hash DJB2). Ver la tarea J7 en `PLAN-v2.pdf`.

## 4 · Diagramas

*(Tercero + Cristhian — T7)*

> TODO Tercero: diagrama de clases completo y diagrama del grafo generado (una visualización
> del grafo sintético de §6, por ejemplo con las comunidades coloreadas). Ver la tarea T7 en
> `PLAN-v2.pdf`.

## 5 · Complejidad computacional

*(Jose — J7, pendiente)*

> TODO Jose: complejidad de cada método público de `estructuras/` y de las operaciones
> principales de `RedSocial` (ya hay anotaciones `@complejidad` sueltas en `main.cpp` y
> `redSocial_io.cpp` para usar como base). Ver la tarea J7 en `PLAN-v2.pdf`.

## 6 · Resultados experimentales

*(Cristhian — C5)*

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

| N | carga (ms) | búsqueda (µs, promedio) | BFS (ms) | sugerencias (ms) | top-K (ms) |
|---:|---:|---:|---:|---:|---:|
| 2 000 | 6.44 | 0.03 | 2.20 | 0.93 | 12.14 |
| 4 000 | 10.99 | 0.03 | 3.02 | 0.92 | 41.26 |
| 8 000 | 20.79 | 0.03 | 8.84 | 0.98 | 147.58 |
| 16 000 | 44.91 | 0.04 | 20.99 | 1.15 | 891.77 |
| 32 000 | 102.11 | 0.04 | 64.15 | 1.25 | no medido¹ |
| 64 000 | 268.71 | 0.04 | 187.69 | 1.25 | no medido¹ |
| 100 000 | 492.38 | 0.05 | 260.52 | 1.35 | no medido¹ |
| 200 000 | 1 499.31 | 0.04 | 730.67 | 1.36 | no medido¹ |
| 500 000 | 8 913.31 | 0.05 | 7 319.58 | 1.56 | no medido¹ |

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

*(Cristhian — C5, pendiente)*

> TODO: capturas de pantalla del menú (`./app`) ejecutando al menos: registrar usuario,
> buscar usuario, camino de amistad, amigos en común, sugerencias y usuarios más activos —
> más una corrida de `./app --bench` mostrando la tabla de la §6.3. El enunciado no evalúa
> el aspecto visual, así que basta con capturas de la terminal.

## 8 · Conclusiones

*(Los tres)*

> TODO: conclusiones del equipo tras cerrar J4 (rehash) y T1 (quitar `obtener(i)` en bucle).
