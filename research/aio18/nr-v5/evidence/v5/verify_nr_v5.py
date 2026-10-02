#!/usr/bin/env python3
from pathlib import Path
import hashlib, json, subprocess, sys, zipfile
root=Path(__file__).resolve().parents[2]
checks=[]
def ok(name,v,detail=None):
    d={'check':name,'passed':bool(v)}
    if detail is not None:d['detail']=detail
    checks.append(d)
def load(rel): return json.loads((root/rel).read_text('utf-8'))
# Prior fresh verification records (re-generated immediately before this pass).
v2=load('evidence/v2/verification_v2.json'); v3=load('evidence/v3/verification_v3.json'); v4=load('evidence/v4/verification_v4.json')
ok('Pass 2 verifier fresh PASS',v2.get('result')=='PASS' and v2.get('failures')==0)
ok('Pass 3 verifier fresh PASS',v3.get('failed')==0 and v3.get('passed')==len(v3.get('checks',[])))
ok('Pass 4 verifier fresh PASS',v4.get('failed')==0 and v4.get('passed')==len(v4.get('checks',[])))
# Identity
expected={
'SkyrimUpscaler.dll':'5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81',
'PDPerfPlugin.dll':'53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1',
'nvngx_dlssnr.dll':'8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206'}
for n,h in expected.items():
 p=Path('/mnt/data/aio18_nr_work')/n
 got=hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else 'missing'
 ok(n+' exact hash',got==h,got)
archive=Path('/mnt/data/SkyrimUpscalerAIOBuild18-Hotfix1(1).7z')
got=hashlib.sha256(archive.read_bytes()).hexdigest()
ok('archive exact hash',got=='136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8',got)
# Cross maps
ext=load('evidence/v2/external_payload_map.json'); ngx=load('evidence/v2/ngx_parameter_map.json'); rec=load('evidence/v3/recovered_reconstruction_contract.json'); life=load('evidence/v4/recovered_lifecycle_contract.json'); order=load('evidence/v4/integration_order.json'); cross=load('evidence/v5/cross_pass_review.json'); dva=load('evidence/v5/dvakolbas_current_head.json')
ok('external payload remains 0x138',ext.get('size_bytes')==0x138)
ok('internal NGX block remains 0x110',ngx.get('size_bytes')==0x110)
ov=ngx.get('observed_core_overrides',{})
ok('AIO core NGX UI contract stable',ov.get('UI') in (None,'null') and ov.get('UIAlpha') in (None,'null') and ov.get('UICorrection')==0)
ok('reconstruction UI contract agrees with v2',rec.get('ngx_ui_in_core_path',{}).get('UICorrection')==0 and rec.get('private_ui_composition') is True)
ok('lifecycle UI contract agrees with v2/v3',life.get('ui_lifetime',{}).get('ngx_ui') is None and life.get('ui_lifetime',{}).get('private_post_nr_ui_composition') is True)
ok('Auto selector stable',rec.get('resolve_methods',{}).get('0',{}).get('native_scale')=='Direct' and rec.get('resolve_methods',{}).get('0',{}).get('reduced_scale')=='Residual')
ok('minimum reduced scale stable',rec.get('input_scale',{}).get('minimum_reduced_scale')==0.25)
ok('feature pool bound stable',rec.get('max_feature_slots_observed')==16 and life.get('feature_pool',{}).get('maximum_slots')==16)
ok('primary Reset=0 stable',life.get('primary_reset',{}).get('common_evaluation_value')==0)
ok('normal integration order stable',order.get('normal_late_after_upscale',{}).get('order')==['scene','SR','late NR','FG prepare/presentation'])
ok('before-upscale order stable',order.get('before_upscale',{}).get('order')==['scene/guides','NR','SR'])
ok('no cross-pass contradictions recorded',cross.get('contradictions')==[])
ok('five retained corrections',len(cross.get('corrections_retained',[]))==5)
# Current Dva identity
ok('Dva current head recorded',dva.get('current_head')=='f6a29f75db0029acf013227627a52e2e490733c6')
ok('Dva NR blobs unchanged since Pass 4',dva.get('nr_blobs_unchanged_since_pass4') is True and len(dva.get('files',{}))==6)
# Documents
log=(root/'NR_RE_LOG.md').read_text('utf-8'); p5=(root/'AIO18_NR_RE_v5.md').read_text('utf-8')
for marker in ['0x138','0x110','UI=null','Reset = 0','16 slots','Pass 1: 13','f6a29f75db0029acf013227627a52e2e490733c6','PARITY MISMATCH']:
    ok('combined log marker '+marker,marker in log)
for marker in ['no contradiction','runtime-only','A/B','f6a29f75db0029acf013227627a52e2e490733c6']:
    ok('Pass 5 marker '+marker,marker.lower() in p5.lower())
# Package must not contain redistributed binaries.
forbidden=[]
for p in root.rglob('*'):
    if p.is_file() and p.suffix.lower() in {'.dll','.exe','.pdb','.lib'}: forbidden.append(str(p.relative_to(root)))
ok('no vendor binaries/PDBs in package',not forbidden,forbidden)
result={'scope':'Pass-5 static cross-review verification; original binaries hashed/read only; no DLL/GPU/Skyrim execution.','checks':checks,'passed':sum(x['passed'] for x in checks),'failed':sum(not x['passed'] for x in checks)}
(root/'evidence/v5/verification_v5.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
print(json.dumps(result,indent=2)); sys.exit(1 if result['failed'] else 0)
