# use python3
# p-persistent CSMA

from matplotlib import pyplot as plt
import numpy as np

def main():
	n = int(input("Enter number of users per slot : "))
	nL = int(input("Enter number of lembda to generate : "))
	nS = int(input("Enter number of slots : "))

	pS = [0.5, 0.01]
	S = [[], []]		# S[0] stores throughput for p = 0.5 and S[1] for p = 0.01
	
	lembda = np.linspace(0, 1, nL)	# generating nL random lembda

	for k in range(2):
		queued = [0]*n	# no. of queued frame for each users
		idle = 0
		
		# simulation for throughput
		# cnt stores the total count of successfully generated frame
		for L in lembda:
			cnt = 0
			
			# sentUser stores the users who successfully sent the frame
			# frameCnt keep records of how many frame has been sent
			# P is probablity to generate a frame
			for slot in range(nS):
				sentUser = []
				frameCnt = 0
				P = float(L/100)
				
				# if frame is generated queued that user frame
				for user in range(n): 
					frameGen = np.random.choice([0,1], 1, p=[1-P, P])[0]
					queued[user] = queued[user] + frameGen
					
					# if channel is found idle and frames are queued, send frame with probablity p = pS[k]
					# if frame is sent, increase the frameCnt by 1 and append that user into the sentUser and decrease the queued value by 1
					if idle == 0 and queued[user] >= 1:
						frameSend = np.random.choice([0,1], 1, p=(1-pS[k], pS[k]))
						
						if frameSend == 1:
							frameCnt += 1
							sentUser.append(user)
							queued[user] -= 1
							
				# time when channel will became idle again is 3
				if frameCnt:
					idle = 3
				
				# if more than 1 or no frame sent, increase queued value by 1 for each sent users
				# if only one frame sent, increase the successful frame count by 1
				if frameCnt != 1:
					for user in sentUser:
						queued[user] += 1
				else:
					cnt += 1
				idle = max(0, idle-1)
			
			# throughput
			throughput = float(cnt/nS)
			S[k].append(throughput)

	# Plotting
	plt.plot(lembda, S[0], label = 'p = 0.5')	# plotting throughput graph for p = 0.5
	plt.plot(lembda, S[1], label = 'p = 0.01')	# plotting throughput graph for p = 0.01
	
	plt.title("Comparison between simulation result of throughput for p = 0.5 and p = 0.01")
	plt.xlabel('Lembda')
	plt.ylabel('Throughput')
	
	plt.legend()
	plt.show()

if __name__ == "__main__":
	main()
