from matplotlib import pyplot as plt
import numpy as np

def main():
	n = int(input("Enter number of users per slot : "))
	nL = int(input("Enter number of lembda to generate : "))
	nS = int(input("Enter number of slots : "))
	
	S = []
	L = np.linspace(0,1,nL)	# generating 100 random lembda in [0,1] and storing in list L
	y = L*(np.exp(-L))		#function for theotitical prediction

	
	# simulation throughout
	# for each user finding frames for a given lembda
	# if sum of frames is 1, means there is no collision. Otherwise yes
	# throughput for a given lembda = (no. of collision free slots / total slots)
	# cnt is successfully frame generated count
	for lembda in L:
		cnt = 0
		
		for slot in range(nS):
		    frames = 0
		    P = lembda/n
		    
		    for user in range(n):
		        frames += np.random.choice([0,1], 1, p=[1-P, P])[0]
		        
		    if frames == 1:
		        cnt += 1
		
		# throughput  = (no. of successfully generated user frame / total slots)
		throughput = float(cnt/nS)
		S.append(throughput)

	# plotting
	plt.plot(L, y, label = "Theoritical")		# Plotting theoritical prediction
	plt.plot(L, S, label = "Simulation")		# Plotting simulation result
	
	plt.title("Comparison between Theoritical and Simulation result of Throughput")
	plt.xlabel("Lembda")			#labelling x-axis as Lembda and y-axis as Throughput
	plt.ylabel("Throughput")
	
	plt.legend()
	plt.show()
	
if __name__ == "__main__":
	main()
