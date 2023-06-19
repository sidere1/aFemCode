#!/usr/bin/python3 

from math import pi 
import numpy as np 
import matplotlib.pyplot as plt


filename = "compEigenVsfMatrix"

rawData= np.loadtxt(filename, dtype='i', delimiter=' ')
#rawData = pd.readcsv(filename, header=None)
#print(rawData)
n = rawData[:,0]

plt.figure(1)
plt.title('Execution time')
plt.plot(n, rawData[:,1], label='fullPivLU')
plt.plot(n, rawData[:,2], label='partPivLU')
plt.plot(n, rawData[:,3], label='QR')
plt.plot(n, rawData[:,4], label='LDLt')
plt.plot(n, rawData[:,5], label='fMatrix')


plt.xlabel('Matrix size')
plt.ylabel('Resolution time (s)')
plt.xscale('log')
plt.yscale('log')
plt.legend(loc='upper left')
plt.grid(visible=True)

plt.show()

#for iFloe in range(speed.shape[1]):
#	lgd = 'floe {}'.format(iFloe)
#	plt.subplot(121)
#	plt.plot(time, speedNorm[:,iFloe], color=(0.5, 0.5, 0.5))
#	#plt.plot(time, speedNorm[:,iFloe], label=lgd)
## note : the foor loop is useless, it's possible to do it in 1 line. You moron. 	
## ex : 
#plt.subplot(122)
#plt.plot(time, xSpeed, color=(0.5,0.5,0.5))
#
#
#
#plt.subplot(121)
#plt.plot(time, meanSpeed, '-k', label='mean velocity')
#plt.legend()
#plt.ylabel('Speed (m/s)')
#plt.xlabel('Time (s)')
#
#plt.subplot(122)
#plt.plot(time, meanXSpeed, '-k', label='mean x velocity')
#plt.legend()
#plt.ylabel('X Speed (m/s)')
#plt.xlabel('Time (s)')
#
#
#plt.show()
#plt.legend(loc='upper left')

#x.shape
#print(x.shape)




