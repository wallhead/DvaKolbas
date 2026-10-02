from pathlib import Path
import hashlib, json, struct, sys
root=Path(__file__).resolve().parents[2]
pd=Path('/mnt/data/aio18_nr_work/PDPerfPlugin.dll')
checks=[]
def ok(name,v): checks.append({'check':name,'passed':bool(v)})
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
ok('PD hash',pd.exists() and sha(pd)=='53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1')
b=pd.read_bytes()
# .rdata mapping for this exact PE
rd_va=0x180113000; rd_off=0x111a00
def readva(va,n): return b[rd_off+(va-rd_va):rd_off+(va-rd_va)+n]
ok('scale minimum 0.25', struct.unpack('<f',readva(0x1811aca8c,4))[0]==0.25)
ok('scale native 1.0', struct.unpack('<f',readva(0x1811acad0,4))[0]==1.0)
vals=struct.unpack('<4f',readva(0x1811ad3e0,16))
ok('ratio defaults 1 1 2 1', vals==(1.0,1.0,2.0,1.0))
# Exact opcode landmarks from original module, RVA->bytes
def at(rva,hexs):
    # .text RVA 0x1000, raw 0x400
    off=0x400+(rva-0x1000); return b[off:off+len(bytes.fromhex(hexs))]==bytes.fromhex(hexs)
for name,rva,h in [
 ('scale clamp maxss 0.25',0x9cfd5,'f30f5f05'),
 ('ratio selector compare method 2',0x9cff0,'83b91401000002'),
 ('pass count external +130',0x9f233,'418b8d30010000'),
 ('feature slot cap 16',0xa1b90,'83f810'),
 ('UI correction external +103 test',0x9f604,'4180bd0301000000'),
 ('private UI pipeline call',0x9f621,'e88adeffff')]: ok(name,at(rva,h))
text=(root/'AIO18_NR_RE_v3.md').read_text('utf-8')
for s in ['Auto is a selector','Lanczos3','luma ratio + OkLab','max_feature_slots_observed']:
    ok('report marker '+s, s in text or s in (root/'evidence/v3/recovered_reconstruction_contract.json').read_text())
out={'checks':checks,'passed':sum(x['passed'] for x in checks),'failed':sum(not x['passed'] for x in checks)}
(root/'evidence/v3/verification_v3.json').write_text(json.dumps(out,indent=2))
print(json.dumps(out,indent=2))
sys.exit(1 if out['failed'] else 0)
