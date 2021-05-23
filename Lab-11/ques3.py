#extraction message from file
def message(filename):
	f = open(filename, "r")
	msg = f.read()
	f.close()
	
	m = ''
	i = 0
	while i < len(msg)-1:
		m += msg[i]
		i += 1
	return m

# extracting sign from file	 and converting into decimal
def signedMsg(filename):
	f = open(filename, "r")
	sign = f.read()
	f.close()
	
	s = ''
	i = 0
	while i < len(sign)-1:
		if sign[i] == ' ':
			break
		s += chr(int(sign[i]+sign[i+1]))
		i += 2
	s = int(s)
	return s

# function for extracting public key from file	
def publicKey(filename):
	f = open(filename, "r")
	pubA = f.read()
	f.close()
	
	nA = ''
	eA = ''
	i = 0
	while i < len(pubA)-1:
		if pubA[i] == ' ':
			break
		nA += chr(int(pubA[i]+pubA[i+1]))
		i += 2
	i += 2
	while i < len(pubA)-1:
		eA += chr(int(pubA[i]+pubA[i+1]))
		i += 2
	
	nA = int(nA)
	eA = int(eA)
	return nA, eA

# function for decryption of messages
# and converting result into ASCII
def decryption(s):
	i = 0
	res = 1
	while i < eA:
		res = (res * s) % nA
		i += 1
	
	res = str(res)
	i = 0
	dec = ''
	while i < len(res):
		dec += str(ord(res[i]))
		i += 1
	return dec

def actualMessage(msg):
		
if __name__ == '__main__':
	msg = message("message.txt")
	nA, eA = publicKey("A.pub")
	sign = signedMsg("secret.txt")
	res = decryption(sign)
	
	# comparing actual message and message after decryption
	if res == msg:
		print(msg)
	else:
		print("Message not verified")
	
