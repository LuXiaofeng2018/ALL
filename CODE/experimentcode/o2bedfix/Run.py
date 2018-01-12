# -*- coding: utf-8 -*-
"""
Created on Wed Apr 22 10:09:21 2015

@author: jordan
"""
from Serre2 import *
from scipy import *
import csv
import os
from numpy.linalg import norm  
from matplotlib.pyplot import plot
import time
    
def copyarraytoC(a):
    n = len(a)
    b = mallocPy(n)
    for i in range(n):
        writetomem(b,i,a[i])
    return b
    
def copyarrayfromC(a,n):
    b = [0]*n
    for i in range(n):
        b[i] = readfrommem(a,i)
        
    return b
    
def makevar(sx,ex,dx,st,et,dt): 
    x = arange(sx, ex, dx)
    t = arange(st, et, dt)
    
    return x,t 

        
#gives exact up to linears, so is second order accurate huzzah    
def getGfromupy(h,u,bed,u0,u1,h0,h1,b0,b1,dx):
    idx = 1.0 / dx
    ithree = 1.0 / 3.0
        
    n = len(h)

    G = zeros(n)
        
    for i in range(1,n-1):
        th = h[i]
        thx = 0.5*idx*(h[i+1] - h[i-1])
        tbx = 0.5*idx*(bed[i+1] - bed[i-1])
        tbxx = idx*idx*(bed[i+1] -2*bed[i] + bed[i-1])
        
        D = th + th*thx*tbx + 0.5*th*th*tbxx + th*tbx*tbx
        
        ai = -ithree*idx*idx*th*th*th + 0.5*idx*th*th*thx
        bi = D + 2.0*ithree*idx*idx*th*th*th
        ci = -ithree*idx*idx*th*th*th - 0.5*idx*th*th*thx
        
        G[i] = ai*u[i-1] + bi*u[i] + ci*u[i+1]
        
    #boundary    
    #i=0
    i=0
    th = h[i]
    thx = 0.5*idx*(h[i+1] - h0)
    tbx = 0.5*idx*(bed[i+1] - b0)
    tbxx = idx*idx*(bed[i+1] -2*bed[i] + b0)
            
    D = th + th*thx*tbx + 0.5*th*th*tbxx + th*tbx*tbx
            
    ai = -ithree*idx*idx*th*th*th + 0.5*idx*th*th*thx
    bi = D + 2.0*ithree*idx*idx*th*th*th
    ci = -ithree*idx*idx*th*th*th - 0.5*idx*th*th*thx
 
    G[i] = ai*u0 + bi*u[i] + ci*u[i+1]
    
    #i = n-1
    i = n-1

    th = h[i]
    thx = 0.5*idx*(h1 - h[i-1])
    tbx = 0.5*idx*(b1 - bed[i-1])
    tbxx = idx*idx*(b1 -2*bed[i] + bed[i-1])
        
    D = th + th*thx*tbx + 0.5*th*th*tbxx + th*tbx*tbx
        
    ai = -ithree*idx*idx*th*th*th + 0.5*idx*th*th*thx
    bi = D + 2.0*ithree*idx*idx*th*th*th
    ci = -ithree*idx*idx*th*th*th - 0.5*idx*th*th*thx

    G[i] = ai*u[i-1] + bi*u[i] + ci*u1
            
    return G  
    

def dambreak(x,hf,hc,hl,bot,dx):
    n = len(x)
    h = zeros(n)
    u = zeros(n)
    bed = bot*ones(n)
    
    for i in range(n):
        if (x[i] < hc):
            h[i] = hf
        else:
            h[i] = hl
    
    G = getGfromupy(h,u,bed,0.0,0.0,h[0],h[-1],0.0,0.0,dx)
    return h,G,bed

       

def sech2 (x):
  a = 2./(exp(x) + exp(-x))
  return a*a

def soliton(x,t,g,a0,a1):
  c = sqrt(g*(a0 + a1))
  phi = x - c*t;
  k = sqrt(3.0*a1) / (2.0*a0 *sqrt(a0 + a1))
  return a0 + a1*sech2(k*phi)
  
def solitoninit(n,a0,a1,g,x,t0,bot,dx):
    h = zeros(n)
    bx = zeros(n)
    u = zeros(n)
    c = sqrt(g*(a0 + a1))
    for i in range(n):
        bx[i] = bot
        h[i] = soliton(x[i],t0,g,a0,a1)
        u[i] =  c* ((h[i] - a0) / h[i])
         
    G = getGfromupy(h,u,bx,0.0,0.0,a0,a0,0.0,0.0,dx)
    
    return h,G,bx 
    
def experiment1(x,b,h0,h1,dx):
    n = len(x)
    u = zeros(n)
    bx = zeros(n)
    h = ones(n)*h1
    for i in range(n):
        if (x[i] <0 and x[i] > -2*b):
            h[i] = h0
    G = getGfromupy(h,u,bx,0.0,0.0,h1,h1,0.0,0.0,dx)

    return h,G,bx 

def dambreaksmooth(x,x0,base,eta0,diffuse,bot,dx):
    from numpy import tanh
    n = len(x)
    h = zeros(n)
    u = zeros(n)
    bed = bot*ones(n)
    
    for i in range(n):
        h[i] = base + 0.5*eta0*(1 + tanh(diffuse*(x0 - abs(x[i]))))
    
    G = getGfromupy(h,u,bed,0.0,0.0,h[0],h[-1],0.0,0.0,dx)
    return h,G,bed

def powerfunction(r,n):
    if ( r >= 0 and r<= 1):
        return (1-r)**n
    else:
        return 0
    
    
    
def flowoverbump(x,stage,center,width,height,vel,l):
    n = len(x)
    h = zeros(n)
    u = zeros(n)
    bed = zeros(n)
    
    for i in range(n): 
        r = abs(x[i] - center) / width
        bed[i] = height*(powerfunction(r,l + 2)*((l*l + 4*l + 3)*r*r*(1.0/3) + (l + 2)*r  + 1))
        h[i] = stage - bed[i]
        u[i] = vel
        
    G = getGfromupy(h,u,bed,u[0],u[-1],h[0],h[-1],bed[0],bed[-1],dx)    
    
    
    return h,G,bed
    
def soloverbump(x,a0,a1,solbeg,solend,t0,g,stage,center,width,height,vel,l):
    n = len(x)
    h = zeros(n)
    u = zeros(n)
    bed = zeros(n)
    
    c = sqrt(g*(a0 + a1))
    for i in range(n):
        
        if (x[i] > solbeg and x[i] < solend):
            r = abs(x[i] - center) / width
            bed[i] = height*(powerfunction(r,l + 2)*((l*l + 4*l + 3)*r*r*(1.0/3) + (l + 2)*r  + 1))
            h[i] = soliton(x[i],t0,g,a0,a1) - bed[i]
            u[i] =  c* ((h[i] - a0) / h[i])
        else:
            r = abs(x[i] - center) / width
            bed[i] = height*(powerfunction(r,l + 2)*((l*l + 4*l + 3)*r*r*(1.0/3) + (l + 2)*r  + 1))
            h[i] = stage - bed[i]
            u[i] =  vel
            
        
    G = getGfromupy(h,u,bed,u[0],u[-1],h[0],h[-1],bed[0],bed[-1],dx)    
    
    
    return h,G,bed
    
    
def soloverslope(x,a0,a1,solbeg,solend,slopbeg,slopend,g):
    
	"""
	soloverslope : set up initial conditions for the bed
	    
	Input:
		    
		x   :   array of cell centres
		a0  :   deep height, also still water level for soliton
		a1  :   soliton height above the still water
		solbeg  :  the beginning of the range over which the soliton scheme is defined
		solend  :  the end of the range over which the soliton scheme is defined
		slopbeg :  the beginning of the slope
		g       :  the acceleration due to gravity
	
	Output:
	    
		h    :   array of heights at cell centres
		G    :   array of G values at cell centres
		bed  :   array of bed heights at cell centres
	
	"""
	
	n = len(x)
	h = zeros(n)
	u = zeros(n)
	bed = zeros(n)
    
    
	#speed of the soliton
	c = sqrt(g*(a0 + a1))

	for i in range(n):
		
		#This is the range over which we define a soliton with a bed of 0 beneath it, which is smaller than the constant depth area
		if (x[i] > solbeg and x[i] < solend):
			bed[i] = 0
			h[i] = soliton(x[i],0,g,a0,a1)
			u[i] =  c* ((h[i] - a0) / h[i])

		#We still have a constant bed, but now the height is constant as well
		elif(x[i] <= slopbeg):
			bed[i] = 0
			h[i] = a0 - bed[i]
			u[i] = 0

		#This is the region in which the bed has a linear slope with a constant stage
		elif(x[i] > slopbeg and x[i] <= slopend):
			bed[i] = (x[i] - slopbeg)
			h[i] = a0 - bed[i]
			u[i] = 0

		#After the slope the bed is constant as is the stage
		elif(x[i] >= slopend):
			bed[i] = 0.99
			h[i] = a0 - bed[i]
			u[i] = 0

	#Calculate G from u,h and bed
	G = getGfromupy(h,u,bed,u[0],u[-1],h[0],h[-1],bed[0],bed[-1],dx)    
    
	#return h,G,bed
	return h,G,bed
    
def DBsmoothoverbump(x,hadd,dbstart, diffuse,g,stage,center,width,height,l):
    n = len(x)
    h = zeros(n)
    u = zeros(n)
    bed = zeros(n)

    for i in range(n):
        
        if (x[i] < dbstart):
            r = abs(x[i] - center) / width
            bed[i] = height*(powerfunction(r,l + 2)*((l*l + 4*l + 3)*r*r*(1.0/3) + (l + 2)*r  + 1))
            h[i] = stage - bed[i] + 0.5*hadd*(1 + tanh(diffuse*(dbstart - x[i])))
        else:
            r = abs(x[i] - center) / width
            bed[i] = height*(powerfunction(r,l + 2)*((l*l + 4*l + 3)*r*r*(1.0/3) + (l + 2)*r  + 1))
            h[i] = stage - bed[i]
            
        
    G = getGfromupy(h,u,bed,u[0],u[-1],h[0],h[-1],bed[0],bed[-1],dx)    
    
    
    return h,G,bed
    
def flatlake(x,dx,x0,stage):
    
    n = len(x)
    u = zeros(n)
    h = zeros(n)
    bed = zeros(n)
    for i in range(n):
        if(x[i] > x0):
            bed[i] = 5.0
            h[i] = stage - bed[i]
        else:
            h[i] = stage
    
    G = getGfromupy(h,u,bed,u[0],u[-1],h[0],h[-1],bed[0],bed[-1],dx)    
    
    
    return h,G,bed  

def Dambreak(x,x0,h0,h1):
    n = len(x)
    u = zeros(n)
    G = zeros(n)
    h = zeros(n)
    b = zeros(n)
    
    for i in range(n):
        if (x[i] < x0):
            h[i] = h0
        else:
            h[i] = h1
            
    return h,u,b,G      


BEGT = time.time()
#Dambreak
h1 = 1.0
h0 = 1.8
x0 = 500

g = 9.81
dx = 0.1
l =  0.01
dt = l*dx
startx = 300
endx = 700 + 0.9*dx
startt = 0.0
endt = 30 + (dt*0.9)  
        
    
x,t = makevar(startx,endx,dx,startt,endt,dt)
n = len(x)
m = len(t)

gap = int(10.0/dt)

theta = 1.2
    
#h,G,bed = flowoverbump(x,stage,center,width,height,vel,el)
h,u,bed,G = Dambreak(x,x0,h0,h1)  
    
nBC = 3
nBCs = 4
b0 = bed[0]*ones(nBCs)
b1 = bed[-1]*ones(nBCs)
u0 = u[0]*ones(nBCs)
u1 = u[-1]*ones(nBCs)   
h0 = h[0]*ones(nBCs)
h1 = h[-1]*ones(nBCs)
    
h_c = copyarraytoC(h)
G_c = copyarraytoC(G)
bed_c = copyarraytoC(bed)
h0_c  = copyarraytoC(h0)
h1_c  = copyarraytoC(h1)
u0_c  = copyarraytoC(u0)
u1_c  = copyarraytoC(u1)
b0_c  = copyarraytoC(b0)
b1_c  = copyarraytoC(b1)
u_c = mallocPy(n)

xbeg = arange(startx - nBC*dx,startx,dx)
xend = arange(endx + dx,endx + (nBC+1)*dx) 

xbc =  concatenate([xbeg,x,xend]) 

xbc_c = copyarraytoC(xbc)
hbc_c = mallocPy(n + 2*nBC)
ubc_c = mallocPy(n + 2*nBC)
bedbc_c = mallocPy(n + 2*nBC)
conc(b0_c , bed_c,b1_c,nBC,n ,nBC , bedbc_c)

tBC = 2
#initial conditions for time steps
    
for i in range(1,len(t)):        
    evolvewrap(G_c,h_c,bed_c,h0_c,h1_c,u0_c,u1_c,b0_c,b1_c,g,dx,dt,n,nBCs,theta)
    print (t[i])
        
getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
u = copyarrayfromC(u_c,n)
G = copyarrayfromC(G_c,n)
h = copyarrayfromC(h_c,n)
    




deallocPy(u_c)   
deallocPy(h_c)
deallocPy(G_c)
deallocPy(h0_c)
deallocPy(h1_c)
deallocPy(u0_c)
deallocPy(u1_c) 
ENDT = time.time()
print(ENDT - BEGT)

"""    
###FLAT LAKE!

wdatadir = "../../../data/raw/flatlake/o2/"

if not os.path.exists(wdatadir):
    os.makedirs(wdatadir)


s = wdatadir +  "savenorms.txt"
with open(s,'a') as file3:
    writefile3 = csv.writer(file3, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)

    writefile3.writerow(['dx' ,'rel Eval'])



for k in range(14,18):
    stage = 1
    center = 50.0
    width = 25
    height = 0.5
    el = 4.0
    vel = 0
    
    
    a0 = 1.0
    a1 = 1.0
    
    g = 9.81
    dx = 100.0 / (2**k)
    Cr = 0.5
    l = Cr / (sqrt(g*10 ))
    dt = l*dx
    theta = 1.0
    startx = -200
    endx = 250.0 + dx
    startt = 0.0
    endt = 300 + dt  
    
    wdir = wdatadir + str(k) + "/"
    
    if not os.path.exists(wdir):
        os.makedirs(wdir)
        
    x,t = makevar(startx,endx,dx,startt,endt,dt)
    n = len(x)
    m = len(t)
    
    gap = int(10.0/dt)
        
    #h,G,bed = flowoverbump(x,stage,center,width,height,vel,el)
    h,G,bed = flatlake(x,dx,0.0,10.0)
        
    nBC = 3
    nBCs = 4
    b0 = bed[0]*ones(nBCs)
    b1 = bed[-1]*ones(nBCs)
    u0 = vel*ones(nBCs)
    u1 = vel*ones(nBCs)   
    h0 = h[0]*ones(nBCs)
    h1 = h[-1]*ones(nBCs)
        
    h_c = copyarraytoC(h)
    G_c = copyarraytoC(G)
    bed_c = copyarraytoC(bed)
    h0_c  = copyarraytoC(h0)
    h1_c  = copyarraytoC(h1)
    u0_c  = copyarraytoC(u0)
    u1_c  = copyarraytoC(u1)
    b0_c  = copyarraytoC(b0)
    b1_c  = copyarraytoC(b1)
    u_c = mallocPy(n)
    
    xbeg = arange(startx - nBC*dx,startx,dx)
    xend = arange(endx + dx,endx + (nBC+1)*dx) 
    
    xbc =  concatenate([xbeg,x,xend]) 
    
    xbc_c = copyarraytoC(xbc)
    hbc_c = mallocPy(n + 2*nBC)
    ubc_c = mallocPy(n + 2*nBC)
    bedbc_c = mallocPy(n + 2*nBC)
    conc(b0_c , bed_c,b1_c,nBC,n ,nBC , bedbc_c)
    #HEvals = []
    GNEvals = []
    Etimes = []
    
    tBC = 2
    #initial conditions for time steps
        
    for i in range(1,len(t)): 
    
        if (i == 1 or i%gap == 0):
            ki = i
            getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
            u = copyarrayfromC(u_c,n)
            G = copyarrayfromC(G_c,n)
            h = copyarrayfromC(h_c,n)
            
            conc(h0_c , h_c,h1_c,nBC,n ,nBC , hbc_c)
            conc(u0_c , u_c,u1_c,nBC,n ,nBC , ubc_c)  
            #HEval = HankEnergyall(xbc_c,hbc_c,ubc_c,g,n + 2*nBC,nBC,dx)
            GNEval = GNall(xbc_c,hbc_c,ubc_c,bedbc_c,g,n + 2*nBC,nBC,dx)
            
            
            
            #HEvals.append(HEval)
            GNEvals.append(GNEval)
            Etimes.append(t[i-1])
            
            s = wdir +  "out" + str(i)+".txt"
            with open(s,'a') as file2:
                writefile2 = csv.writer(file2, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            
                writefile2.writerow(['dx' ,'dt','time','Eval',"cell midpoint" ,'height(m)', 'G' , 'u(m/s)','bed' ])        
                           
                for j in range(n):
                    writefile2.writerow([str(dx),str(dt),str(t[i]), str(GNEval),str(x[j]), str(h[j]) , str(G[j]) , str(u[j]),str(bed[j])])
            
        evolvewrap(G_c,h_c,bed_c,h0_c,h1_c,u0_c,u1_c,b0_c,b1_c,g,dx,dt,n,nBCs,theta)
        print (t[i])
            
    getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
    u = copyarrayfromC(u_c,n)
    G = copyarrayfromC(G_c,n)
    h = copyarrayfromC(h_c,n)
    
    conc(h0_c , h_c,h1_c,nBC,n ,nBC , hbc_c)
    conc(u0_c , u_c,u1_c,nBC,n ,nBC , ubc_c)        
    #HEval = HankEnergyall(xbc_c,hbc_c,ubc_c,g,n + 2*nBC,nBC,dx)
    GNEval = GNall(xbc_c,hbc_c,ubc_c,bedbc_c,g,n + 2*nBC,nBC,dx)
    
    
    
    #HEvals.append(HEval)
    GNEvals.append(GNEval)
    Etimes.append(t[-1])
    s = wdir +  "outlast.txt"
    with open(s,'a') as file2:
         writefile2 = csv.writer(file2, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
        
         writefile2.writerow(['dx' ,'dt','time','Eval',"cell midpoint" ,'height(m)', 'G' , 'u(m/s)','bed' ])        
                       
         for j in range(n):
             writefile2.writerow([str(dx),str(dt),str(t[i]),str(GNEval),str(x[j]), str(h[j]) , str(G[j]) , str(u[j]),str(bed[j])])
    
    finE = GNEvals[-1]
    tfinE = Etimes[-1]
    begE = GNEvals[0]
    tbegE = Etimes[0]
    relerr = abs(finE - begE) / abs(begE)
    
    print(str(tbegE) + " || " + str(begE))
    print(str(tfinE) + " || " + str(finE))
    s = wdatadir +  "savenorms.txt"
    with open(s,'a') as file3:
        writefile3 = csv.writer(file3, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    
        writefile3.writerow([str(dx) ,str(relerr)])
    
    deallocPy(u_c)   
    deallocPy(h_c)
    deallocPy(G_c)
    deallocPy(h0_c)
    deallocPy(h1_c)
    deallocPy(u0_c)
    deallocPy(u1_c) 


    
"""
"""
####Soliton up slope

wdatadir = "../../../../data/raw/solslopelarger1p0R/o2/"

if not os.path.exists(wdatadir):
    os.makedirs(wdatadir)


a0 = 1.0
a1 = 0.01

g = 9.81
dx = 0.005
Cr = 0.5
l = 0.1
dt = l*dx
theta = 2.0
startx = -350
endx = 250.0 + dx
startt = 0.0
endt = 100 + dt  

wdir = wdatadir

if not os.path.exists(wdir):
    os.makedirs(wdir)
    
x,t = makevar(startx,endx,dx,startt,endt,dt)
n = len(x)
m = len(t)

gap = int(0.2/dt)
    
#h,G,bed = flowoverbump(x,stage,center,width,height,vel,el)
h,G,bed = soloverslope(x,a0,a1,-300,100,100,100.99,g)

   
nBC = 3
nBCs = 4
b0 = bed[0]*ones(nBCs)
b1 = bed[-1]*ones(nBCs)
u0 = 0*ones(nBCs)
u1 = 0*ones(nBCs)   
h0 = h[0]*ones(nBCs)
h1 = h[-1]*ones(nBCs)
    
h_c = copyarraytoC(h)
G_c = copyarraytoC(G)
bed_c = copyarraytoC(bed)
h0_c  = copyarraytoC(h0)
h1_c  = copyarraytoC(h1)
u0_c  = copyarraytoC(u0)
u1_c  = copyarraytoC(u1)
b0_c  = copyarraytoC(b0)
b1_c  = copyarraytoC(b1)
u_c = mallocPy(n)

xbeg = arange(startx - nBC*dx,startx,dx)
xend = arange(endx + dx,endx + (nBC+1)*dx) 

xbc =  concatenate([xbeg,x,xend]) 

xbc_c = copyarraytoC(xbc)
hbc_c = mallocPy(n + 2*nBC)
ubc_c = mallocPy(n + 2*nBC)
bedbc_c = mallocPy(n + 2*nBC)
conc(b0_c , bed_c,b1_c,nBC,n ,nBC , bedbc_c)

getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
u = copyarrayfromC(u_c,n)
G = copyarrayfromC(G_c,n)
h = copyarrayfromC(h_c,n)

conc(h0_c , h_c,h1_c,nBC,n ,nBC , hbc_c)
conc(u0_c , u_c,u1_c,nBC,n ,nBC , ubc_c)  
GNEvali = GNall(xbc_c,hbc_c,ubc_c,bedbc_c,g,n + 2*nBC,nBC,dx)

    
for i in range(1,len(t)): 

    if (i == 1 or i%gap == 0):
        getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
        u = copyarrayfromC(u_c,n)
        G = copyarrayfromC(G_c,n)
        h = copyarrayfromC(h_c,n)

        
        s = wdir +  "out" + str(i)+".txt"
        with open(s,'a') as file2:
            writefile2 = csv.writer(file2, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
        
            writefile2.writerow(['dx' ,'dt','time',"cell midpoint" ,'height(m)', 'G' , 'u(m/s)','bed' ])        
                       
            for j in range(n):
                writefile2.writerow([str(dx),str(dt),str(t[i]),str(x[j]), str(h[j]) , str(G[j]) , str(u[j]),str(bed[j])])
        
    evolvewrap(G_c,h_c,bed_c,h0_c,h1_c,u0_c,u1_c,b0_c,b1_c,g,dx,dt,n,nBCs,theta)
    print (t[i])
        
getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
u = copyarrayfromC(u_c,n)
G = copyarrayfromC(G_c,n)
h = copyarrayfromC(h_c,n)

conc(h0_c , h_c,h1_c,nBC,n ,nBC , hbc_c)
conc(u0_c , u_c,u1_c,nBC,n ,nBC , ubc_c)        
GNEval = GNall(xbc_c,hbc_c,ubc_c,bedbc_c,g,n + 2*nBC,nBC,dx)


s = wdir +  "outlast.txt"
with open(s,'a') as file2:
     writefile2 = csv.writer(file2, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    
     writefile2.writerow(['dx' ,'dt','time','Eval' , 'initial Eval',"cell midpoint" ,'height(m)', 'G' , 'u(m/s)','bed' ])        
                   
     for j in range(n):
         writefile2.writerow([str(dx),str(dt),str(t[-1]),str(GNEval),str(GNEvali),str(x[j]), str(h[j]) , str(G[j]) , str(u[j]),str(bed[j])])


deallocPy(u_c)   
deallocPy(h_c)
deallocPy(G_c)
deallocPy(h0_c)
deallocPy(h1_c)
deallocPy(u0_c)
deallocPy(u1_c) 
"""


"""
###Soliton over bump scenario
### Energy Test ####################

wdatadir = "../../../data/raw/solbumpEnergucontfullfix0m3/o2/"

if not os.path.exists(wdatadir):
    os.makedirs(wdatadir)


s = wdatadir +  "savenorms.txt"
with open(s,'a') as file3:
    writefile3 = csv.writer(file3, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)

    writefile3.writerow(['dx' ,'rel Eval'])



for k in range(6,19,3):
    stage = 1
    center = 50.0
    width = 25
    height = 0.5
    el = 4.0
    vel = 0
    
    
    a0 = 1.0
    a1 = 1.0
    
    g = 9.81
    dx = 100.0 / (2**k)
    Cr = 0.5
    l = Cr / (2.5 + sqrt(g*(a0 +a1) ))
    dt = l*dx
    theta = 1.0
    startx = -200
    endx = 250.0 + dx
    startt = 0.0
    endt = 50 + dt  
    
    wdir = wdatadir + str(k) + "/"
    
    if not os.path.exists(wdir):
        os.makedirs(wdir)
        
    x,t = makevar(startx,endx,dx,startt,endt,dt)
    n = len(x)
    m = len(t)
    
    gap = int(10.0/dt)
        
    #h,G,bed = flowoverbump(x,stage,center,width,height,vel,el)
    h,G,bed = soloverbump(x,a0,a1,-20,20,0.0,g,stage,center,width,height,vel,el)
        
    nBC = 3
    nBCs = 4
    b0 = bed[0]*ones(nBCs)
    b1 = bed[-1]*ones(nBCs)
    u0 = vel*ones(nBCs)
    u1 = vel*ones(nBCs)   
    h0 = h[0]*ones(nBCs)
    h1 = h[-1]*ones(nBCs)
        
    h_c = copyarraytoC(h)
    G_c = copyarraytoC(G)
    bed_c = copyarraytoC(bed)
    h0_c  = copyarraytoC(h0)
    h1_c  = copyarraytoC(h1)
    u0_c  = copyarraytoC(u0)
    u1_c  = copyarraytoC(u1)
    b0_c  = copyarraytoC(b0)
    b1_c  = copyarraytoC(b1)
    u_c = mallocPy(n)
    
    xbeg = arange(startx - nBC*dx,startx,dx)
    xend = arange(endx + dx,endx + (nBC+1)*dx) 
    
    xbc =  concatenate([xbeg,x,xend]) 
    
    xbc_c = copyarraytoC(xbc)
    hbc_c = mallocPy(n + 2*nBC)
    ubc_c = mallocPy(n + 2*nBC)
    bedbc_c = mallocPy(n + 2*nBC)
    conc(b0_c , bed_c,b1_c,nBC,n ,nBC , bedbc_c)
    #HEvals = []
    GNEvals = []
    Etimes = []
    
    tBC = 2
    #initial conditions for time steps
        
    for i in range(1,len(t)): 
    
        if (i == 1 or i%gap == 0):
            ki = i
            getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
            u = copyarrayfromC(u_c,n)
            G = copyarrayfromC(G_c,n)
            h = copyarrayfromC(h_c,n)
            
            conc(h0_c , h_c,h1_c,nBC,n ,nBC , hbc_c)
            conc(u0_c , u_c,u1_c,nBC,n ,nBC , ubc_c)  
            #HEval = HankEnergyall(xbc_c,hbc_c,ubc_c,g,n + 2*nBC,nBC,dx)
            GNEval = GNall(xbc_c,hbc_c,ubc_c,bedbc_c,g,n + 2*nBC,nBC,dx)
            
            
            
            #HEvals.append(HEval)
            GNEvals.append(GNEval)
            Etimes.append(t[i-1])
            
            s = wdir +  "out" + str(i)+".txt"
            with open(s,'a') as file2:
                writefile2 = csv.writer(file2, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            
                writefile2.writerow(['dx' ,'dt','time','Eval',"cell midpoint" ,'height(m)', 'G' , 'u(m/s)','bed' ])        
                           
                for j in range(n):
                    writefile2.writerow([str(dx),str(dt),str(t[i]), str(GNEval),str(x[j]), str(h[j]) , str(G[j]) , str(u[j]),str(bed[j])])
            
        evolvewrap(G_c,h_c,bed_c,h0_c,h1_c,u0_c,u1_c,b0_c,b1_c,g,dx,dt,n,nBCs,theta)
        print (t[i])
            
    getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
    u = copyarrayfromC(u_c,n)
    G = copyarrayfromC(G_c,n)
    h = copyarrayfromC(h_c,n)
    
    conc(h0_c , h_c,h1_c,nBC,n ,nBC , hbc_c)
    conc(u0_c , u_c,u1_c,nBC,n ,nBC , ubc_c)        
    #HEval = HankEnergyall(xbc_c,hbc_c,ubc_c,g,n + 2*nBC,nBC,dx)
    GNEval = GNall(xbc_c,hbc_c,ubc_c,bedbc_c,g,n + 2*nBC,nBC,dx)
    
    
    
    #HEvals.append(HEval)
    GNEvals.append(GNEval)
    Etimes.append(t[-1])
    s = wdir +  "outlast.txt"
    with open(s,'a') as file2:
         writefile2 = csv.writer(file2, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
        
         writefile2.writerow(['dx' ,'dt','time','Eval',"cell midpoint" ,'height(m)', 'G' , 'u(m/s)','bed' ])        
                       
         for j in range(n):
             writefile2.writerow([str(dx),str(dt),str(t[i]),str(GNEval),str(x[j]), str(h[j]) , str(G[j]) , str(u[j]),str(bed[j])])
    
    finE = GNEvals[-1]
    tfinE = Etimes[-1]
    begE = GNEvals[0]
    tbegE = Etimes[0]
    relerr = abs(finE - begE) / abs(begE)
    
    print(str(tbegE) + " || " + str(begE))
    print(str(tfinE) + " || " + str(finE))
    s = wdatadir +  "savenorms.txt"
    with open(s,'a') as file3:
        writefile3 = csv.writer(file3, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    
        writefile3.writerow([str(dx) ,str(relerr)])
    
    deallocPy(u_c)   
    deallocPy(h_c)
    deallocPy(G_c)
    deallocPy(h0_c)
    deallocPy(h1_c)
    deallocPy(u0_c)
    deallocPy(u1_c) 
"""




"""
################################# SOLITON Accuracy ####################3
wdir = "../../../data/solcononesecguNucont/bo2/"

a0 = 1
a1 = 1
g = 9.81
Cr = 0.5

if not os.path.exists(wdir):
    os.makedirs(wdir)

s = wdir + "savenorms.txt"
with open(s,'a') as file1:
    writefile = csv.writer(file1, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)

    writefile.writerow(['dx','Normalised L1-norm Difference Height', ' Normalised L1-norm Difference Velocity', 'H err'])
    
for k in range(12,13):
    dx = 100.0 / (2**k)
    l = Cr / sqrt(g*(a0 + a1))
    dt = l*dx
    startx = -100.0
    endx = 200.0 + dx
    startt = 0
    endt = 20 + dt
    theta = 1.2
    
    wdatadir = wdir+ str(k) + "/"
    if not os.path.exists(wdatadir):
        os.makedirs(wdatadir)
    
    g = 9.81
    
    x,t = makevar(startx,endx,dx,startt,endt,dt)
    n = len(x)
    
    a0 = 1.0
    a1 = 1.0
    t0 = 0
    bot = 0
    gap = 0.5/dt
    
    h,G,bed = solitoninit(n,a0,a1,g,x,t0,bot,dx)
    
    nBC = 3
    nBCs = 4
    b0 = bot*ones(nBCs)
    b1 = bot*ones(nBCs)
    u0 = zeros(nBCs)
    u1 = zeros(nBCs)    
    h0 = h[0]*ones(nBCs)
    h1 = h[-1]*ones(nBCs)
    
    h_c = copyarraytoC(h)
    G_c = copyarraytoC(G)
    bed_c = copyarraytoC(bed)
    h0_c  = copyarraytoC(h0)
    h1_c  = copyarraytoC(h1)
    u0_c  = copyarraytoC(u0)
    u1_c  = copyarraytoC(u1)
    b0_c  = copyarraytoC(b0)
    b1_c  = copyarraytoC(b1)
    u_c = mallocPy(n)
    
    xbeg = arange(startx - nBC*dx,startx,dx)
    xend = arange(endx + dx,endx + (nBC+1)*dx) 
    
    xbc =  concatenate([xbeg,x,xend]) 
    
    xbc_c = copyarraytoC(xbc)
    hbc_c = mallocPy(n + 2*nBC)
    ubc_c = mallocPy(n + 2*nBC)
    bedbc_c = mallocPy(n + 2*nBC)
    conc(b0_c , bed_c,b1_c,nBC,n ,nBC , bedbc_c)
    
    Evals = []
    
    
    
    for i in range(1,len(t)):
        
        if(i % gap == 0 or i ==1):
            getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
            u = copyarrayfromC(u_c,n)
            G = copyarrayfromC(G_c,n)
            h = copyarrayfromC(h_c,n)
            

    
            conc(h0_c , h_c,h1_c,nBC,n ,nBC , hbc_c)
            conc(u0_c , u_c,u1_c,nBC,n ,nBC , ubc_c)        
            GNEval = GNall(xbc_c,hbc_c,ubc_c,bedbc_c,g,n + 2*nBC,nBC,dx)
            Evals.append(GNEval)
            
            
            
            
            c = sqrt(g*(a0 + a1))
            htrue = zeros(n)
            utrue = zeros(n)
            for j in range(n):             
                he = soliton(x[j],t[i],g,a0,a1)
                htrue[j] = he
                utrue[j] = c* ((he - a0) / he) 
                
            s = wdatadir + "saveoutputts" + str(i) + ".txt"
            
            print t[i]
            print(h[1],G[1]) 
            with open(s,'a') as file2:
                writefile2 = csv.writer(file2, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    
                writefile2.writerow(['dx' ,'dt','time','Eval', 'height(m)', 'G' , 'u(m/s)','bed','true height', 'true velocity' ])        
                   
                for j in range(n):
                    writefile2.writerow([str(dx),str(dt),str(t[i]),str(GNEval), str(h[j]) , str(G[j]) , str(u[j]),str(bed[j]) , str(htrue[j]), str(utrue[j])])  
                 
            
        evolvewrap(G_c,h_c,bed_c,h0_c,h1_c,u0_c,u1_c,b0_c,b1_c,g,dx,dt,n,nBCs,theta)
        print t[i]
        print(h[1],G[1]) 
        
        #print("##################### END OF STEP #####################################")
        #print("##################### END OF STEP #####################################")
        #print("##################### END OF STEP #####################################")


   
    getufromG(h_c,G_c,bed_c,u0[-1],u1[0],h0[-1],h1[0], b0[-1], b1[0], dx ,n,u_c)
    u = copyarrayfromC(u_c,n)
    G = copyarrayfromC(G_c,n)
    h = copyarrayfromC(h_c,n)
    
    conc(h0_c , h_c,h1_c,nBC,n ,nBC , hbc_c)
    conc(u0_c , u_c,u1_c,nBC,n ,nBC , ubc_c)        
    GNEval = GNall(xbc_c,hbc_c,ubc_c,bedbc_c,g,n + 2*nBC,nBC,dx)
    Evals.append(GNEval)
    
    
    c = sqrt(g*(a0 + a1))
    htrue = zeros(n)
    utrue = zeros(n)
    for j in range(n):             
        he = soliton(x[j],t[-1],g,a0,a1)
        htrue[j] = he
        utrue[j] = c* ((he - a0) / he) 
    
    s = wdatadir + "saveoutputtslast.txt"
    with open(s,'a') as file2:
         writefile2 = csv.writer(file2, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    
         writefile2.writerow(['dx' ,'dt','time', 'height(m)', 'G' , 'u(m/s)','bed','true height', 'true velocity'  ])        
                   
         for j in range(n):
             writefile2.writerow([str(dx),str(dt),str(t[-1]), str(h[j]) , str(G[j]) , str(u[j]),str(bed[j]), str(htrue[j]), str(utrue[j])])       
    
    normhdiffi = norm(h - htrue,ord=1) / norm(htrue,ord=1)
    normudiffi = norm(u -utrue,ord=1) / norm(utrue,ord=1)  
    Hconserr = abs(Evals[-1] - Evals[0]) / abs(Evals[0])

    s = wdir + "savenorms.txt"
    with open(s,'a') as file1:
        writefile = csv.writer(file1, delimiter = ',', quotechar='|', quoting=csv.QUOTE_MINIMAL)

        writefile.writerow([str(dx),str(normhdiffi), str(normudiffi),str(Hconserr)]) 

    deallocPy(u_c)   
    deallocPy(h_c)
    deallocPy(G_c)
    deallocPy(bed_c)
    deallocPy(h0_c)
    deallocPy(h1_c)
    deallocPy(u0_c)
    deallocPy(u1_c)
    deallocPy(b0_c)
    deallocPy(b1_c) 
    """
