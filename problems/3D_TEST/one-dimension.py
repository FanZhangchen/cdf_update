import numpy as np
import matplotlib.pyplot as plt
import math
import os
import pandas as pd

C11     = 188997
C12     = 135312
C44     = 90191
p       = 0.3
q       = 1.3
de      = 1.9e-6
ds      = 2.8e-6
ke_b    = 52000
tau0    = 127
f0      = 286000
gdot0   = 1.0e6
rho_e0  = 2.2e6
Sfixed  = 0

theta   = 22 + 273
Ce      = 0.5
Cs      = 0.5; 
lamda   = 0.3; 
burgers = 0.257e-6
R       = 8.314
Ke      = ke_b*burgers
ks_b    = 2*ke_b
Ks      = ks_b*burgers
mu      = 90191
#calculate the material parameter, E & v
v       = 1/(1+C11/C12)
E       = C11*(1-3*v**2-2*v**3)/(1-v**2)

orientation = '100'
match orientation:
    case '100':  #[100] direction
        N           = 8 
        Schimid_mn  = 1/math.sqrt(6) 
        # N           = 1
        # Schimid_mn  = 1/math.sqrt(6) 
    case '110':  #[110]   direction
        N           = 4 
        Schimid_mn  = 1/math.sqrt(6)
    case '111':  #[111]  direction
        N           = 6  
        Schimid_mn  = 2/(3*math.sqrt(6))

#initialize the var.
Depsilon= 0
epsilon = 0
gamma   = 0
s       = 0
tau     = 0
#applied strain control
E_d     = 1e-3
h       = 0.001
Depsilon= E_d*h


rho_e_ini   = 2.2e6
rho_s_ini   = 2.2e6
rho_e       = 2.2e6
rho_s       = 2.2e6


strain  = [epsilon]
stress  = [s]
while epsilon <= 0.5:
    match orientation:
        case '100':  #[100] direction
            rho_st  = 1.3*(rho_e+rho_s)+1.5*(rho_e+rho_s)*7+(rho_e_ini+rho_s_ini)*4*1.5
            # rho_st  = 1.5*(rho_e+rho_s)
        case '110':  #[110]   direction
            rho_st  = 1.3*(rho_e+rho_s)+1.5*(rho_e+rho_s)*3+(rho_e_ini+rho_s_ini)*8*1.5
        case '111':  #[111]  direction
            rho_st  = 1.3*(rho_e+rho_s)+1.5*(rho_e+rho_s)*5+(rho_e_ini+rho_s_ini)*6*1.5

    ST      = lamda*mu*burgers*math.sqrt(rho_st)

    if ((abs(tau)-ST) < 0):
        gamma_dot=gdot0*math.exp(-f0/(R*theta))*np.sign(tau)
    else:
        gamma_dot=gdot0*math.exp(-f0/(R*theta)*(1-((abs(tau)-ST)/tau0)**p)**q)*np.sign(tau)
    
    match orientation:
        case '100':  #[100] direction
            rhoT    = (rho_e+rho_s)*8+(rho_e_ini+rho_s_ini)*4
            # rhoT    = rho_e+rho_s
        case '110':  #[110]   direction
            rhoT    = (rho_e+rho_s)*4+(rho_e_ini+rho_s_ini)*8
        case '111':  #[111]  direction
            rhoT    = (rho_e+rho_s)*6+(rho_e_ini+rho_s_ini)*6

    rhoe_d  = Ce/burgers*(Ke*math.sqrt(rhoT)-2*de*rho_e)*abs(gamma_dot)
    rhos_d  = Cs/burgers*(Ks*math.sqrt(rhoT)-rho_s*(math.pi*ds**2*Ks*math.sqrt(rhoT)+2*ds))*abs(gamma_dot)

    epsilon  = epsilon + Depsilon

    gamma   = gamma + gamma_dot*h
    dsig    = E*(Depsilon-N*gamma_dot*h*Schimid_mn)
    tau     = tau + dsig*Schimid_mn
    s       = s + dsig
    rho_e   = rho_e + h*rhoe_d
    rho_s   = rho_s + h*rhos_d

    strain.append(epsilon)
    stress.append(s)

path        = os.getcwd()
path_output = path + '/'

df = pd.read_csv('data_output_111.csv')

# Assuming the columns are named exactly as shown in the image
plt.plot(df['strain_xx'], df['stress_xx'])  
plt.plot(strain,stress)

# Customize the plot (add labels, title, etc.)
plt.xlabel('Strain XX')
plt.ylabel('Stress XX')
plt.title('Stress-Strain Curve')

plt.show()
plt.savefig(path_output+"oned-ss.png")

data = np.column_stack((strain,stress))
np.savetxt(path_output+"one-d-data.dat",data,delimiter=',',fmt='%10.5f')
