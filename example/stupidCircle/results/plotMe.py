#!/usr/bin/python3 

import numpy as np 
import matplotlib.pyplot as plt

filename = 'micValues'

temp = np.loadtxt(filename, dtype='str', delimiter=' ', skiprows=3)
nMics = temp.shape[1]-2# because it counts the last ' ' at the end of the line as a new column
#print('your file contains {} mics'.format(nMics))

rawData = np.loadtxt(filename, dtype='f', delimiter=' ', skiprows=1, usecols=np.arange(0,nMics+1))
f = rawData[:,0];

plt.figure(1)
for iMic in np.arange(1, nMics+1):
	plt.plot(f, 20*np.log10(rawData[:,iMic]/(2e-5)), '*-', label='mic {}'.format(iMic))

plt.xlabel('frequency (Hz)')
plt.ylabel('SPL')
plt.title('SPL at micPoints')
plt.legend(loc='upper right')
plt.grid(visible=True)

plt.show()
