# use python3
# to compile used command : $:~ python3 < compile0/compile3 > out0/out3

class Graph():
	def __init__(self, n, cost, src):
		self.V = n
		self.cost = cost
		self.s = src
		
	
	# this method finds the vertex with minimum distance value,
	# from the set of unvisited vertex
	# initialize a minimum distance for next vertex
	def minDist(self, dist, Visited):
		Min = 10**4
		idx = 0
		
		# searching nearest vertex with minumum distance which is not visited yet
		for v in range(self.V):
			if dist[v] < Min and Visited[v] == False:
				Min = dist[v]
				idx = v
		
		return idx
	
	# this method implements Dijkstra's algorithm
	# for a graph represented using adjacency matrix representation
	def dijkstra(self):
		d = [10**4] * self.V
		d[self.s] = 0
		
		Visited = [False] * self.V
		for i in range(self.V):
		
			# u is the minimum distant vertex from the set of vertex which are unvisited
			u = self.minDist(d, Visited)
			Visited[u] = True
			
			# updating distance value of adjacent vertices of u,
			# only if the current distance is greater than new distance
			# and the vertex is unvisited
			for v in range(self.V):
				if self.cost[u][v] > 0 and Visited[v] == False and self.cost[u][v] + d[u] < d[v]:
					d[v] = self.cost[u][v] + d[u]
		return d
def main():
	# 3 user inputs 
	n = int(input())		# no.of vertices
	src = int(input())		# starting vertex
	
	# input file which have cost of every edges
	cost = []
	f = open(input())
	for i in range(n):
		cost.append(list(map(int, f.readline().split()[:n])))
	
	A = Graph(n, cost, src)
	
	dist = A.dijkstra()
	
	print("Vertex \t\t Distance from Vertex", src)
	for vertex in range(n):
		print(vertex, "\t\t\t", dist[vertex])
	
if __name__ == '__main__':
	main()
