def setcharges(geom, species, charges):

    found = False
    q=0
    hd=geom.header()
    for k in hd:
        if k=="charge" or k=="q":
            found = True
            break
        q +=1

    if not found:
        raise TypeError("Target geometry has no field for charges")
    
    for i in range(geom.nat()):
        try:
            j = species.index(geom.atom[i])
            geom.field[q,i] = charges[j]
        except ValueError:
            pass
        
