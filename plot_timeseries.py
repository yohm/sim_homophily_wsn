import sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

plt.plotfile(sys.argv[1], cols=(0,1), names=('time','degree'), delimiter=' ', linestyle='--')
plt.savefig('timeseries.png')

