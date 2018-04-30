#
#   y = x1*a1 + x2*a2 + x3*a3 +...+ xn*an
#   Ax = b
#
import numpy as np
from numpy import *

import sys

BLOCK_SIZE = 32
def generateData(sampleNumber,paraOfNumber):
	
	s = np.random.normal(0,1,paraOfNumber)
	s = mat(s).T
	np.savetxt("../data/solution.dat",s)

	for i in range(BLOCK_SIZE):
		Astr = "../data/lineardataA_%d.dat"%i;bstr = "../data/lineardatab_%d.dat"%i
		#a = np.random.uniform(-0.375111355247,0.322563975491,paraOfNumber * sampleOfNumber)	
		#a = a.reshape((sampleOfNumber,paraOfNumber))
		a = np.random.normal(size=(sampleNumber,paraOfNumber))
		np.savetxt(Astr,a)
		#noise = np.random.normal(0,0.1,5000)
		#noise = mat(noise).T
		b = a * s 
		np.savetxt(bstr,b)
		print "the %d block data"%i
		
if __name__ == "__main__":
	if len(sys.argv) < 3:
		print "argv must two praraments likes (samepleNubmer,paraNumber)!"
	sampleOfNumber = int(sys.argv[1])
	paraOfNumber = int(sys.argv[2])
	generateData(sampleOfNumber,paraOfNumber)
