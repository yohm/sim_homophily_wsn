import sys
import collections
import networkx as nx
import matplotlib.pyplot as plt

for f in sys.argv[1::]:
    g = nx.read_weighted_edgelist(f)
    degree_sequence = sorted([d for n, d in g.degree()], reverse=True)
    degreeCount = collections.Counter(degree_sequence)
    deg, cnt = zip(*degreeCount.items())
    print(deg,cnt)
    plt.plot(deg,cnt, label=f)
#plt.legend()
plt.xlabel('k')
plt.ylabel('degree distribution')
plt.show()
