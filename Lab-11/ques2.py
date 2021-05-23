if __name__ == '__main__':
	f = open("message.txt", "r")
	msg = f.read()
	f.close()
	
	f = open("A.pri", "r")
	priA = f.read()
	f.close()
	
	nA = ''
	dA = ''
	i = 0
	while i < len(priA)-1:
		print(priA[i]+priA[i+1], end = " ")
		if priA[i] == ' ':
			break
		nA += chr(int(priA[i]+priA[i+1]))
		i += 2
	i += 2
	print()
	while i < len(priA)-1:
		print(priA[i]+priA[i+1], end = " ")
		dA += chr(int(priA[i]+priA[i+1]))
		i += 2
	nA = int(nA)
	dA = int(dA)
	print()
	print((nA, dA))
	
	i = 0
	res = 1
	while i < dA:
		res = (res * msg) % nA
		i += 1
	res %= nA
		
	sign = res % nA
	print(sign)
