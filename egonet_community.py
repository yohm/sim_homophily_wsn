import sys
import math
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import infomap

def findCommunities(G):
    infomapWrapper = infomap.Infomap("-2")
    mapping = {}
    for n in sorted( G.nodes(), key=lambda x: G.degree(x) ):
        mapping[n] = len(mapping)
    G2 = nx.relabel_nodes(G, mapping, copy=True)
    for e in G2.edges_iter(data='weight'):
        infomapWrapper.addLink(*e)

    infomapWrapper.run();
    
    tree = infomapWrapper.tree
    
    inverse_mapping = { v:k for k,v in mapping.items() }
    communities = {}
    for node in tree.leafIter():
        n = node.originalLeafIndex
        org_idx = inverse_mapping[n]
        communities[org_idx] = node.moduleIndex()
    for n in G.nodes():
        if not n in communities:
            communities[n] = -1
    nx.set_node_attributes(G, 'community', communities)
    return tree.numTopModules()

def drawNetwork(G):
    # position map
    pos = nx.spring_layout(G, weight=None)
    communities = [ n[1]['community'] for n in G.nodes(data='community') ]
    numCommunities = max(communities) + 1

    # edges
    weights = [ math.log10(e[2]) for e in G.edges(data='weight')]
    nx.draw_networkx_edges(G, pos, edge_color=weights)

    # nodes
    nodeCollection = nx.draw_networkx_nodes(G,
        pos = pos,
        node_color = communities,
    )
    # set node border color to the darker shade

    # Print node labels separately instead
    for n in G.nodes_iter():
        plt.annotate(n,
            xy = pos[n],
            textcoords = 'offset points',
            horizontalalignment = 'center',
            verticalalignment = 'center',
            xytext = [0, 2],
        )
    plt.axis('off')
    plt.show()

if len(sys.argv) != 2:
    print("usage: python egonet_community.py net.edg 1")

infile = sys.argv[1]
g=nx.read_weighted_edgelist( infile, nodetype=int )

ego = int(sys.argv[2])
ego_net = nx.ego_graph(g, ego)
ego_net.remove_node(ego)


findCommunities(ego_net)
drawNetwork(ego_net)

