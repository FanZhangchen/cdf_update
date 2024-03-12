#this file for run the moose job in current path

import os
import re

AppName = 'cdf_update'
ifileName = 'BLP_AL_L1e-1_ds_60120.i'
# ifileName = 'single_crystal_one_element_backstress.i'

path = os.getcwd()
path_up = re.sub(r'/problems.*$','',path)

#normal mode
# os.chdir(path_up)
# os.system("make -j8")
os.chdir(path)
os.system("mpiexec -n 16 " + "../../" + AppName +"-opt -i " + ifileName + " -> record.log &")


#debug mode
#os.chdir(path_up)
#os.system("METHOD=dbg make -j8")
#os.chdir(path)
#os.system("gdb --args ../" + AppName +"-dbg -i " + ifileName)

# then run "b MPI_Abort << r << bt"
# "p variablename" for print out the variable of interest
