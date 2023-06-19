#!/usr/bin/python3 

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




x = rawData[:,1]
