import sys,re,json,os.path
import matplotlib.pyplot as plt
import numpy as np

infile = open(sys.argv[1])
pattern = re.compile("^#")

communities = {}
nodes = {}

for line in infile:
    if pattern.match(line):
        continue
    s = line.split(' ')
    n = int(s[0])
    c = int(s[1])
    if not c in communities:
        communities[c] = []
    communities[c].append(n)
    if not n in nodes:
        nodes[n] = []
    nodes[n].append(c)

# merge json
def merge_output_json( merged ):
    out_json = '_output.json'
    if os.path.isfile(out_json):
        l = json.load( open(out_json) )
        for k,v in merged.items():
            l[k] = v
        fp = open(out_json, 'w')
        json.dump(l, fp )
        fp.flush()
        fp.close()

def calculate_avg_num_coms_per_node():
    num_coms = [ len(coms) for n,coms in nodes.items() ]
    return sum(num_coms) / len(num_coms)

def calculate_avg_com_size():
    com_sizes = [len(ns) for c,ns in communities.items() ]
    return sum(com_sizes) / len(com_sizes)

merge_output_json( {"AvgNumComsPerNode": calculate_avg_num_coms_per_node(), "AvgComSize": calculate_avg_com_size() } )

# print community size distribution
def num_communities_distribution():
    num_coms = [ len(coms) for n,coms in nodes.items() ]
    num_coms_dat = 'num_communities.dat'
    counts = {}
    for num_com in num_coms:
        if not num_com in counts:
            counts[num_com] = 0
        counts[num_com] += 1
    f = open(num_coms_dat, 'w')
    for k in sorted(counts.keys()):
        f.write("%d %d\n" % (k,counts[k]) )
    f.close()
    dat = np.loadtxt(num_coms_dat)
    plt.yscale('log')
    plt.xlabel('# of communities')
    plt.ylabel('frequency')
    plt.plot( dat[:,0], dat[:,1], 'o-' )
    plt.savefig('num_communities.png')
    plt.clf()

num_communities_distribution()

def communit_size_distribution():
    com_sizes = [ len(nodes) for c,nodes in communities.items() ]
    com_size_dat = 'community_size.dat'
    counts = {}
    for s in com_sizes:
        if not s in counts:
            counts[s] = 0
        counts[s] += 1
    f = open(com_size_dat, 'w')
    for k in sorted(counts.keys()):
        f.write("%d %d\n" % (k,counts[k]) )
    f.close()
    dat = np.loadtxt(com_size_dat)
    plt.yscale('log')
    plt.xlabel('community size')
    plt.ylabel('frequency')
    plt.plot( dat[:,0], dat[:,1], 'o-' )
    plt.savefig('community_size.png')
    plt.clf()

communit_size_distribution()

