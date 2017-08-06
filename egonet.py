import sys
import networkx as nx
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print("usage: python egonet.py net.edg 1")

infile = sys.argv[1]
G=nx.read_weighted_edgelist( infile )

ego = sys.argv[2]
ego_net = nx.ego_graph(G, ego )
pos=nx.spring_layout(ego_net)
nx.draw(ego_net,pos,node_color='b',node_size=50,with_labels=True)
nx.draw_networkx_nodes(ego_net,pos,nodelist=[ego],node_size=300,node_color='r')
plt.show()


