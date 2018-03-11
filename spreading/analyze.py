import os,sys,json,subprocess

fp = open( '_input.json' )
params = json.load( fp )

simulator = os.path.abspath(os.path.dirname(__file__)) + "/../spreading_main.out"
N = params['simulation_parameters']['N']
cmd = "%s %d _input/net.edg 1234 > infection.dat" % (simulator, N)
sys.stderr.write("Running: %s\n" % cmd)
subprocess.run(cmd, shell=True, check=True)
sys.stderr.write("Successfully finished")

# make plot
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

dat = np.loadtxt("infection.dat")
plt.plot(dat[:,0],dat[:,1]/N)
plt.xlabel("t")
plt.ylabel("infected")
plt.savefig("infection.png")

t_half = dat[:,0][int(N/2)]
fp = open( '_input/_output.json' )
sim_result = json.load( fp )
w = sim_result['AverageEdgeWeight']
out_fp = open('_output.json', 'w')

json.dump( {"t_half": t_half, "rescaled_t_half":t_half*w}, out_fp )
out_fp.close()
