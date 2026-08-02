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

> TODO Cristhian: descripción del generador sintético y el enlace preferencial, metodología
> de medición con `<chrono>`, tabla de resultados (`./app --bench`) y análisis de los
> defectos que muestran los números.

## 7 · Capturas

*(Cristhian — C5, pendiente)*

> TODO: capturas de pantalla del menú (`./app`) ejecutando al menos: registrar usuario,
> buscar usuario, camino de amistad, amigos en común, sugerencias y usuarios más activos —
> más una corrida de `./app --bench` mostrando la tabla de la §6.3. El enunciado no evalúa
> el aspecto visual, así que basta con capturas de la terminal.

## 8 · Conclusiones

*(Los tres)*

> TODO: conclusiones del equipo tras cerrar J4 (rehash) y T1 (quitar `obtener(i)` en bucle).
