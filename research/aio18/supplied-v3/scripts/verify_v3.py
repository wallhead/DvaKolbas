#!/usr/bin/env python3
"""Hash/byte/decoder checks for AIO18 RE v3. Read-only; never runs a target DLL.
Requires Python stdlib, GNU objdump, LLVM llvm-objdump. Calls the v2/v1 verifier.
"""
from __future__ import annotations
import argparse, collections, hashlib, json, pathlib, platform, re, shutil, struct
import subprocess, sys, tempfile, unittest
from pe_index import PE
from disasm_index import Disasm
from verify_evidence import require, sha256
from reference_models_v3 import RecoveredContractTests

def read(p): return json.loads(p.read_text(encoding='utf-8'))
def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('extracted_root',type=pathlib.Path)
    ap.add_argument('--archive',type=pathlib.Path)
    ap.add_argument('--output',type=pathlib.Path,default=pathlib.Path('verification_v3.json'))
    ap.add_argument('--llvm-objdump',default=shutil.which('llvm-objdump') or '/usr/local/swift/usr/bin/llvm-objdump')
    args=ap.parse_args(); root=pathlib.Path(__file__).resolve().parent.parent; ev=root/'evidence/v3'
    gnu=shutil.which('objdump'); require(bool(gnu),'GNU objdump missing')
    require(pathlib.Path(args.llvm_objdump).is_file(),'LLVM objdump missing; pass --llvm-objdump PATH')
    with tempfile.TemporaryDirectory(prefix='aio18-v3-') as tmpdir:
        tmp=pathlib.Path(tmpdir)
        cmd=[sys.executable,str(root/'scripts/verify_v2.py'),str(args.extracted_root),'--output',str(tmp/'v2.json')]
        if args.archive:cmd+=['--archive',str(args.archive)]
        subprocess.run(cmd,check=True,capture_output=True,text=True)
        previous=read(tmp/'v2.json')
        files=read(root/'evidence/manifest.json')['files']
        paths={pathlib.PurePosixPath(x['path']).name:x['path'] for x in files}
        marks=read(ev/'landmarks_v3.json'); spans=read(ev/'excerpt_manifest_v3.json')
        modules={n:PE(args.extracted_root/paths[n]) for n in {m['module'] for m in marks}}
        groups=collections.defaultdict(list)
        for m in marks:
            pe=modules[m['module']]; b=bytes.fromhex(m['bytes']); r=int(m['rva'],0)
            require(hashlib.sha256(pe.data).hexdigest()==m['sha256'],'input hash mismatch')
            require(pe.read(r,len(b))==b,f'landmark byte mismatch {m["module"]}+{r:x}')
            groups[m['module']].append(m)
        gnu_count=llvm_count=excerpt_lines=0
        for name,group in groups.items():
            pe=modules[name]; out=tmp/(name+'.asm')
            with out.open('w') as fp:
                subprocess.run([gnu,'-d','-j','.text','-M','intel','--no-show-raw-insn',str(pe.path)],
                               stdout=fp,stderr=subprocess.PIPE,text=True,check=True)
            dis=Disasm(pe,out)
            for m in group:
                ins=dis.byaddr.get(int(m['rva'],0)); require(ins is not None,'missing GNU instruction')
                for k in ('mnemonic','operands','refs'):require(ins[k]==m[k],f'GNU {k} mismatch {m["rva"]}')
                gnu_count+=1
            # Confirm the mnemonic/operand text of every v3 excerpt line against a fresh decoder pass.
            for span in [s for s in spans if s['module']==name]:
                for line in (root/span['file']).read_text().splitlines():
                    match=re.match(r'^([0-9A-F]{8}):\s+(\S+)\s*(.*)$',line)
                    if not match:continue
                    addr=int(match[1],16); ins=dis.byaddr.get(addr)
                    require(ins is not None,'excerpt start is not decoded boundary')
                    require(ins['mnemonic']==match[2],'excerpt mnemonic differs')
                    # Annotations after ';' are evidence comments, not decoded machine operands.
                    require(ins['operands']==match[3].split(' ; ',1)[0].rstrip(),'excerpt operands differ')
                    excerpt_lines+=1
            llvmout=tmp/(name+'.llvm.asm')
            with llvmout.open('w') as fp:
                subprocess.run([args.llvm_objdump,'-d','--x86-asm-syntax=intel',str(pe.path)],
                               stdout=fp,stderr=subprocess.PIPE,text=True,check=True)
            llvm={}
            for line in llvmout.read_text().splitlines():
                match=re.match(r'^\s*([0-9a-fA-F]+):\s+((?:[0-9a-fA-F]{2}\s+)+)\s*(\S+)',line)
                if match:llvm[int(match[1],16)-pe.base]=(bytes.fromhex(match[2]),match[3])
            for m in group:
                record=llvm.get(int(m['rva'],0)); require(record is not None,'LLVM boundary missing')
                require(record[0]==bytes.fromhex(m['bytes']),f'LLVM length/bytes mismatch {m["rva"]}')
                require(record[1] not in ('<unknown>','(bad)'),f'LLVM cannot decode {m["rva"]}')
                llvm_count+=1
        for s in spans:
            pe=modules[s['module']]; start,end=s['start_rva'],s['end_rva']
            require(sha256(root/s['file'])==s['file_sha256'],'excerpt file hash mismatch')
            require(hashlib.sha256(pe.read(start,end-start)).hexdigest()==s['byte_range_sha256'],'code span mismatch')
        constants=read(ev/'constants_and_imports.json')
        for x in constants:
            b=bytes.fromhex(x['bytes']); require(modules[x['module']].read(x['rva'],len(b))==b,'constant mismatch')
        vtable=read(ev/'pd_api_dimension_vtable.json'); pd=modules['PDPerfPlugin.dll']
        for x in vtable['entries']:
            require(pd.ptr(vtable['vtable_rva']+x['offset'])-pd.base==x['target_rva'],'PD vtable mismatch')
        # Negative control proves a wrong expected byte is rejected.
        first=marks[0]; expected=bytearray.fromhex(first['bytes']); expected[0]^=1
        rejected=False
        try:require(modules[first['module']].read(int(first['rva'],0),len(expected))==expected,'deliberate corruption')
        except ValueError:rejected=True
        require(rejected,'negative byte test failed')
        tests=unittest.defaultTestLoader.loadTestsFromTestCase(RecoveredContractTests)
        result=unittest.TextTestRunner(verbosity=0).run(tests)
        require(result.wasSuccessful(),'reference model tests failed')
        old=read(root/'evidence/landmarks.json')+read(root/'evidence/v2/landmarks_v2.json')
        output={'status':'PASS','revision':3,
          'scope':'Static evidence reproducibility and reference-model tests only. No DLL, game, GPU, or live debugger execution.',
          'previous_pass_reverified':previous,'new_instruction_records_checked':len(marks),
          'new_landmarks_GNU_redecoded':gnu_count,'new_landmarks_LLVM_boundary_and_byte_agreement':llvm_count,
          'new_excerpts_code_and_file_hashes_checked':len(spans),'new_excerpt_instruction_lines_GNU_redecoded':excerpt_lines,
          'new_constants_checked':len(constants),'additional_PD_vtable_entries_checked':len(vtable['entries']),
          'reference_model_tests':result.testsRun,'reference_model_failures':len(result.failures)+len(result.errors),
          'mutated_instruction_rejected':rejected,'total_instruction_records':len(old)+len(marks),
          'total_distinct_instruction_addresses':len({(m['module'],int(m['rva'],0)) for m in old+marks}),
          'total_disassembly_excerpts':len(list((root/'evidence').rglob('*.asm'))),
          'python':platform.python_version(),
          'llvm':subprocess.run([args.llvm_objdump,'--version'],capture_output=True,text=True,check=True).stdout.splitlines()[0]}
        args.output.write_text(json.dumps(output,indent=2)+'\n',encoding='utf-8');print(json.dumps(output,indent=2))
    return 0
if __name__=='__main__':
    try:raise SystemExit(main())
    except (OSError,ValueError,KeyError,struct.error,subprocess.CalledProcessError) as exc:
        print(f'VERIFICATION FAILED: {exc}',file=sys.stderr)
        if isinstance(exc,subprocess.CalledProcessError) and exc.stderr:print(exc.stderr,file=sys.stderr)
        raise SystemExit(1)
