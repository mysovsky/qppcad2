import pyqpp

def multmom(geom, Lmax):
    found = False
    iq=0
    hd=geom.header()
    for k in hd:
        if k=="charge" or k=="q":
            found = True
            break
        iq +=1

    if not found:
        raise TypeError("Target geometry has no field for charges")
       
    C = 0e0
    D = pyqpp.vector3f(0e0)
    Q = pyqpp.matrix3f(0e0)

    for i in range(geom.nat()):
        q = geom.field[iq,i]
        r = geom.pos(i)
        C += q
        D += q*r
        for j in [0,1,2]:
            for k in [0,1,2]:
                Q[j,k] += q*r[j]*r[k] - ( q*r.norm()**2/3 if j==k else 0e0)

    s = "Total charge : " + str(C) + "\nDipole moment : " +\
        str(D) + "\nQuadruple moment :\n" + str(Q)
    return s
