import sys

in_dir = sys.argv[1]
net_file = in_dir + '/net.edg'
trait_file = in_dir + '/traits.txt'

nodes = set()
with open(trait_file) as f:
    for (i,line) in enumerate(f):
        f0 = int(line.split(' ')[0])
        if f0 == 0:
            nodes.add(i)

# print(nodes, len(nodes))

with open(net_file) as f:
    for line in f:
        a = line.split(' ')
        n0 = int(a[0])
        n1 = int(a[1])
        if n0 in nodes or n1 in nodes:
            sys.stdout.write(line)
        
