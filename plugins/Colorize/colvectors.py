import pyqpp as pq
import numpy as np
from math import tanh,sqrt

def colmix(c1,c2,scheme,xaux,x):
    k = xaux[0]
    x0 = xaux[1]
    if scheme==0:
        return c1+x*(c2-c1)
    elif scheme==1:
         return c1+k*sqrt(x-x0)*(c2-c1)
    elif scheme==2:
        return c1+(c2-c1)*(tanh(k*(x-x0))+1)/2


def colvectors(geom, vec, color1, color2, scheme, xaux):
    #return geom, vec, color1, color2, scheme, xcol
    ir = 8
    ig = 9
    ib = 10
    ia = 11
    c1 = np.array(color1)
    c2 = np.array(color2)
    d = [x.norm() for x in vec.vectors]
    d0 = max(d)
    for i in range(len(geom)):
        c = colmix(c1,c2,scheme,xaux,d[i]/d0)
        geom.field[ir,i]=c[0]
        geom.field[ig,i]=c[1]
        geom.field[ib,i]=c[2]
        geom.field[ia,i]=c[3]
        vec.colors[i] = pq.vector3f(c[0],c[1],c[2])

        
    
