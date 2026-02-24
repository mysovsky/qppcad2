import pyqpp as pq

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
    c1 = pq.vector3f(color1)
    c2 = pq.vector3f(color2)
    d = [x.norm() for x in vec.vectors]
    d0 = max(d)
    for i in range(len(geom)):
        c = colmix(c1,c2,scheme,xaux,d[i]/d0)
        geom.field[ir,i]=c[0]
        geom.field[ig,i]=c[1]
        geom.field[ib,i]=c[2]

        
    
