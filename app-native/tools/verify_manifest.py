#!/usr/bin/env python3
import struct, sys
from pathlib import Path
NO=0xffffffff
p=Path(sys.argv[1]); d=p.read_bytes(); pos=8; strings=[]; resmap=[]; service=None

def rlen(buf,p):
    b=buf[p]
    if b<0x80:return b,p+1
    return ((b&0x7f)<<8)|buf[p+1],p+2
while pos < len(d):
    t,hs,sz=struct.unpack_from('<HHI',d,pos)
    if t==1:
        sc,_,flags,ss,_=struct.unpack_from('<IIIII',d,pos+8)
        offs=struct.unpack_from('<'+'I'*sc,d,pos+hs); base=pos+ss
        for o in offs:
            q=base+o; _,q=rlen(d,q); bl,q=rlen(d,q); strings.append(d[q:q+bl].decode())
    elif t==0x180:
        n=(sz-8)//4;resmap=list(struct.unpack_from('<'+'I'*n,d,pos+8))
    elif t==0x102:
        _,name=struct.unpack_from('<II',d,pos+16)
        astart,asz,acount,_,_,_=struct.unpack_from('<HHHHHH',d,pos+24)
        tag=strings[name]; ap=pos+16+astart; attrs=[]
        for i in range(acount):
            ns,ni,raw=struct.unpack_from('<III',d,ap+i*asz)
            _,_,typ,val=struct.unpack_from('<HBBI',d,ap+i*asz+12)
            nm=strings[ni]; rid=resmap[ni] if ni < len(resmap) else None
            rendered=strings[raw] if raw!=NO else val
            attrs.append((nm,rid,rendered,typ))
        if tag=='service': service=attrs
    pos += sz
if service is None: raise SystemExit('FAIL: service tag missing')
a={n:(rid,v,t) for n,rid,v,t in service}
if a.get('process',(None,None,None))[1] != ':trust': raise SystemExit(f"FAIL: service process={a.get('process')}")
if a.get('foregroundServiceType',(None,None,None))[1] != 0x40000000: raise SystemExit('FAIL: specialUse FGS type missing')
rids=[rid for _,rid,_,_ in service if rid is not None]
if rids != sorted(rids): raise SystemExit(f'FAIL: service attrs not sorted by resource id: {[hex(x) for x in rids]}')
print('manifest verify: OK process=:trust, FGS=specialUse, attrs sorted')
