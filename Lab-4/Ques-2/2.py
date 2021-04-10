# use python3
# to compile used command : $:~ python3 < compile > out.txt

import random
class Graph:
	def __init__(self, n, cost):
		self.V = n
		self.cost = cost
		
	# this method is defining the Distance Vector Routing algorithm
	# this is used to update the routing table
	# with the help of adjacent node distance vector table
	def DVR(self, node, table):
		for i in range(self.V):
			u = node[i]
			nextDV = []
			nextVert = []
			
			# assigning the adjacent vertices to all nodes
			for v in range(self.V):
				m = float("Inf")
				if self.cost[u][v] is not 0:
					nextDV.append(table[v][1])
					nextVert.append(v)
			
			# if adjacent nodes are more than 1, then finding the minimum DV
			# among them and adding into cost of edges
			for v in range(self.V):
				if u == v:
					table[u][1][v] = 0
					table[u][2][v] = v+1
				else:
				 	m = float("Inf")
				 	if len(nextDV) > 1:
				 		minVertex = '-'
				 		for i in range(len(nextDV)):
				 			if m > nextDV[i][v] + self.cost[u][nextVert[i]]:
				 				m = nextDV[i][v] + self.cost[u][nextVert[i]]
				 				minVertex = table[nextVert[i]][2][v]
				 		
				 		if m != float("Inf"):
					 		table[u][1][v] = self.cost[u][v] + m
					 		table[u][2][v] = minVertex
					 	else:
					 		table[u][1][v] = float("Inf")
					 		table[u][2][v] = '-'
					 		
				 	elif len(nextDV) == 1:
				 		if nextDV[0][v] != float("Inf"):
					 		table[u][1][v] = self.cost[u][nextVert[0]] + nextDV[0][v]
				 			table[u][2][v] = table[nextVert[0]][2][v]
					 	else:
					 		table[u][1][v] = float("Inf")
					 		table[u][2][v] = '-'
					 		
		return table
				

class RoutingTable:
	def __init__(self, n, cost):
		self.V = n
		self.cost = cost
	
	# this method defines the initial routing table
	def table(self):
		Dest = [x+1 for x in range(self.V)]
		Table = [0]*self.V
		
		for u in range(self.V):
		
			DV = [float("Inf")]*self.V
			Next = ["-"]*self.V
			
			for v in range(self.V):
				if self.cost[u][v] is not 0 or u == v:
					DV[v] = self.cost[u][v]
					Next[v] = v+1
			Table[u] = Dest, DV,  Next
		return Table
			
def main():

	# 2 user inputs (number of nodes and cost matrix of graph)
	n = int(input())
	
	cost = []
	f = open(input())
	for i in range(n):
		cost.append(list(map(int, f.readline().split()[:n])))
	
	# Table is initial routing table of each nodes and printing the initial routing table
	RT = RoutingTable(n, cost)
	Table = RT.table()
	for u in range(n):
		print("Initial Routing Table for Vertex",u+1)
		print("Destination \t Distance \t Next Vertex")
		
		for v in range(n):
			print(' ', Table[u][0][v], '\t\t ', Table[u][1][v], '\t\t ', Table[u][2][v])
		print()
					
	A = Graph(n, cost)
	
	# shuffled the nodes and doing simulation
	node = [x for x in range(n)]
	random.shuffle(node)
	for simulation in range(n-1):
		Table = A.DVR(node, Table)

	print("-------------------------------------------------------\n")
	
	# printing the final routing table
	for u in range(n):
		print("Final Routing Table for Vertex",u+1)
		print("Destination \t Distance \t Next Vertex")
		for v in range(n):
			print(' ', Table[u][0][v], '\t\t ', Table[u][1][v], '\t\t ', Table[u][2][v])
		print()
		
if __name__ == '__main__':
	main()
