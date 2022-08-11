import sys
import matplotlib
matplotlib.use('Agg')
import numpy as np
import matplotlib.pyplot as plt

x,y = np.loadtxt(sys.argv[1], usecols=(0,1), unpack=True)
plt.plot(x, y, linestyle='--', label=sys.argv[1])
plt.xlabel('time')
plt.ylabel('degree')
plt.savefig('timeseries.png')

