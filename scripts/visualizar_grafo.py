#!/usr/bin/env python3
"""
Lee output/grafo_aristas.csv (generado por `./app --graph-viz`) y dibuja el
grafo con networkx + matplotlib, coloreando cada nodo por comunidad.

La formula de comunidad (id // USUARIOS_POR_COMUNIDAD) es la MISMA que usa
generarUsuariosSinteticos en red/redSocial_io.cpp para agrupar usuarios: no
se guarda en ningun lado como campo de Usuario, hay que recalcularla aca
igual que alla.

Uso: python3 scripts/visualizar_grafo.py
"""
import csv
import matplotlib
import matplotlib.pyplot as plt
import networkx as nx

N_USUARIOS = 300
USUARIOS_POR_COMUNIDAD = 50
RUTA_CSV = "output/grafo_aristas.csv"
RUTA_PNG = "output/grafo_sintetico.png"


def main():
    g = nx.Graph()
    g.add_nodes_from(range(N_USUARIOS))  # por si algun nodo queda sin aristas

    with open(RUTA_CSV, newline="") as f:
        lector = csv.DictReader(f)
        for fila in lector:
            g.add_edge(int(fila["origen"]), int(fila["destino"]))

    num_comunidades = N_USUARIOS // USUARIOS_POR_COMUNIDAD
    # matplotlib.cm.get_cmap fue removido; matplotlib.colormaps es la API
    # vigente (verificado en esta maquina, matplotlib 3.11).
    paleta = matplotlib.colormaps["tab10"]
    colores = [paleta((nodo // USUARIOS_POR_COMUNIDAD) / max(1, num_comunidades - 1))
               for nodo in g.nodes()]

    pos = nx.spring_layout(g, seed=12345, k=0.35, iterations=200)

    plt.figure(figsize=(10, 10))
    nx.draw_networkx_nodes(g, pos, node_size=60, node_color=colores)
    nx.draw_networkx_edges(g, pos, alpha=0.25, width=0.5)
    plt.title(f"Red sintetica: {N_USUARIOS} usuarios, {num_comunidades} comunidades")
    plt.axis("off")
    plt.tight_layout()
    plt.savefig(RUTA_PNG, dpi=150)
    print(f"Imagen guardada en {RUTA_PNG}")


if __name__ == "__main__":
    main()
