from Crypto.Util import number  
import random

# function to find GCD
def gcd(a, b):
   while a != 0:
      a, b = b % a, a
   return b

# function to find modulo inverse
def findModInverse(a, m):
   if gcd(a, m) != 1:
      return None
   u1, u2, u3 = 1, 0, a
   v1, v2, v3 = 0, 1, m
   
   while v3 != 0:
      q = u3 // v3
      v1, v2, v3, u1, u2, u3 = (u1 - q * v1), (u2 - q * v2), (u3 - q * v3), v1, v2, v3
   return u1 % m

# ASCII conversion function
def asciiConversion(eA, eB, nA, nB, dA, dB):
	aeA = ''
	anA = ''
	adA = ''
	for i in range(len(eA)):
		aeA += str(ord(eA[i]))
		
	for i in range(len(nA)):
		anA += str(ord(nA[i]))
	
	for i in range(len(dA)):
		adA += str(ord(dA[i]))
	
	aeB = ''
	anB = ''
	adB = ''
	for i in range(len(eB)):
		aeB += str(ord(eB[i]))
		
	for i in range(len(nB)):
		anB += str(ord(nB[i]))
	
	for i in range(len(dB)):
		adB += str(ord(dB[i]))
	
	return (aeA, aeB, anA, anB, adA, adB)

# function to write ASCII value of private and public keys into respective files
def fileWrite(aeA, aeB, anA, anB, adA, adB):
	f = open("A.pub", "w")
	f.write(anA)
	f.write("  ")
	f.write(aeA)
	f.close()
	
	f = open("A.pri", "w")
	f.write(anA)
	f.write("  ")
	f.write(adA)
	f.close()
	
	f = open("B.pub", "w")
	f.write(anB)
	f.write("  ")
	f.write(aeB)
	f.close()
	
	f = open("B.pri", "w")
	f.write(anB)
	f.write("  ")
	f.write(adB)
	f.close()
	
if __name__ == '__main__':
	# Generating random 4 big prime numbers for both users
	lst = [10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30];
	num = random.choice(lst);
	pA, qA, pB, qB = number.getPrime(num), number.getPrime(num), number.getPrime(num), number.getPrime(num);
	while(1):
		if(pA != qA != pB != qB):
			break
		pB = number.getPrime(num)
		qA = number.getPrime(num)
		qB = number.getPrime(num)
	
	nA = pA * qA
	nB = pB * qB
	phiA = (pA-1) * (qA-1)
	phiB = (pB-1) * (qB-1)
	
	# Public key (A) = (nA, eA) and Private key (A) = (nA, dA)
	for eA in range(2, phiA):
		if gcd(eA, phiA) == 1:
			break
	dA = findModInverse(eA, phiA)
	eA = str(eA)
	dA = str(dA)
	nA = str(nA)
	print(nA, dA)
	
	# Public key (B) = (nB, eB) and Private key (B) = (nB, dB)
	for eB in range(2, phiB):
		if gcd(eB, phiB) == 1:
			break		
	dB = findModInverse(eB, phiB)
	eB = str(eB)
	dB = str(dB)
	nB = str(nB)
	
	# converting results into ASCII and writing into files
	aeA, aeB, anA, anB, adA, adB = asciiConversion(eA, eB, nA, nB, dA, dB)
	fileWrite(aeA, aeB, anA, anB, adA, adB)
	
