import networkx as nx

fh = open("edges.txt", "rb")
fw = open("output_cycles.txt", "w")

n = (int) (fh.readline().strip())
print(n, file = fw)

G = nx.read_edgelist(fh, nodetype = int, create_using = nx.DiGraph())

for c in nx.simple_cycles(G):
    print(len(c) + 1, file = fw)
    print(*c, end = ' ', file = fw)
    print(c[0], file = fw)

fh.close()
fw.close()
