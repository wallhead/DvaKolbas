#!/usr/bin/env python3
"""Recheck both passes against the original files. Never executes a target DLL.
Python standard library + GNU objdump; extraction separately uses libarchive.
"""
from __future__ import annotations
import argparse, collections, hashlib, json, pathlib, platform, shutil, struct
import subprocess, sys, tempfile
from pe_index import PE
from disasm_index import Disasm
from pdb_index import PDB
from pdb_types import Types
from pdb_class_layout import ClassTypes
from verify_evidence import require, sha256


def read_json(p: pathlib.Path):
    return json.loads(p.read_text(encoding='utf-8'))


def main() -> int:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('extracted_root',type=pathlib.Path)
    ap.add_argument('--archive',type=pathlib.Path)
    ap.add_argument('--output',type=pathlib.Path,default=pathlib.Path('verification_v2.json'))
    args=ap.parse_args()
    root=pathlib.Path(__file__).resolve().parent.parent
    ev=root/'evidence'; new=ev/'v2'
    objdump=shutil.which('objdump')
    require(bool(objdump),'GNU objdump must be installed')
    with tempfile.TemporaryDirectory(prefix='aio18-verify-') as work:
        tmp=pathlib.Path(work)
        cmd=[sys.executable,str(root/'scripts/verify_evidence.py'),str(args.extracted_root),
             '--output',str(tmp/'v1.json')]
        if args.archive: cmd += ['--archive',str(args.archive)]
        run=subprocess.run(cmd,text=True,capture_output=True,check=True)
        v1=read_json(tmp/'v1.json')
        manifest=read_json(ev/'manifest.json')
        filemap={pathlib.PurePosixPath(x['path']).name:x['path'] for x in manifest['files']}
        modules={}
        def module(name):
            if name not in modules: modules[name]=PE(args.extracted_root/filemap[name])
            return modules[name]
        marks=read_json(new/'landmarks_v2.json')
        groups=collections.defaultdict(list)
        for mark in marks:
            pe=module(mark['module']); expected=bytes.fromhex(mark['bytes']); r=int(mark['rva'],0)
            require(hashlib.sha256(pe.data).hexdigest()==mark['sha256'],'Module identity changed')
            require(pe.read(r,len(expected))==expected,f'Byte mismatch {mark["module"]}+{r:X}')
            groups[mark['module']].append(mark)
        decoded=0
        for name, group in groups.items():
            pe=module(name); start=min(int(x['rva'],0) for x in group)
            end=max(int(x['rva'],0)+len(bytes.fromhex(x['bytes'])) for x in group)
            out=tmp/(name+'.asm')
            with out.open('w') as fp:
                subprocess.run([objdump,'-d','-j','.text','-M','intel','--no-show-raw-insn',
                                f'--start-address={pe.base+start}',f'--stop-address={pe.base+end}',
                                str(pe.path)],stdout=fp,stderr=subprocess.PIPE,text=True,check=True)
            dis=Disasm(pe,out)
            for mark in group:
                r=int(mark['rva'],0); ins=dis.byaddr.get(r)
                require(ins is not None,f'No decoded instruction at {name}+{r:X}')
                for field in ('mnemonic','operands','refs'):
                    require(ins[field]==mark[field],f'Disassembly mismatch {name}+{r:X}: {field}')
                decoded += 1
        spans=read_json(new/'excerpt_manifest_v2.json')
        for span in spans:
            pe=module(span['module']); s=span['start_rva']; e=span['end_rva']
            require(sha256(root/span['file'])==span['file_sha256'],'Evidence excerpt changed')
            require(hashlib.sha256(pe.read(s,e-s)).hexdigest()==span['byte_range_sha256'],
                    f'Excerpt code changed: {span["file"]}')
        fg=module('amd_fidelityfx_framegeneration_dx12.dll')
        vtables=read_json(new/'amd_swapchain_vtable.json')
        for v in vtables:
            require(fg.ptr(v['vtable_rva']+v['offset'])-fg.base==v['target_rva'],
                    f'AMD vtable mismatch +{v["offset"]:x}')
        pdb=PDB(args.extracted_root/'UpscalerBasePlugin/amd_fidelityfx_framegeneration_dx12.pdb')
        classes=ClassTypes(pdb); layouts=read_json(new/'amd_internal_class_layouts.json')
        for stored in layouts:
            actual=classes.layout(stored['type_index'])
            require(actual==stored,f'Class layout changed: {stored["name"]}')
        # Regression control: the richer reader agrees on every old plain-struct field.
        oldtypes=read_json(ev/'amd_fg_struct_layouts.json')
        old=Types(pdb)
        for record in oldtypes:
            idx=record['type_index']; info=old.struct_info(idx)
            plain=old.fields(info['field_list'])
            rich=[x for x in classes.class_fields(info['field_list']) if x['kind']=='member']
            require(len(plain)==len(rich),'Old/new field count mismatch')
            for a,b in zip(plain,rich):
                for key in ('name','offset','type_index','type'):
                    require(a[key]==b[key],f'Old/new class parser mismatch: {key}')
        # Real rejection controls mutate expected data in memory; no input is modified.
        first=marks[0]; raw=module(first['module']).read(int(first['rva'],0),len(bytes.fromhex(first['bytes'])))
        mutated=bytearray(bytes.fromhex(first['bytes'])); mutated[0]^=1
        mismatch_rejected=False
        try: require(raw==bytes(mutated),'deliberate altered landmark')
        except ValueError: mismatch_rejected=True
        require(mismatch_rejected,'Negative byte test did not fail')
        classes.rec[0x7ffffffe]=(0x1203,b'\x11\x11\x00\x00')
        unknown_rejected=False
        try: classes.class_fields(0x7ffffffe)
        except ValueError: unknown_rejected=True
        require(unknown_rejected,'Unsupported CodeView leaf did not fail closed')
        oldmarks=read_json(ev/'landmarks.json')
        result={
            'status':'PASS','revision':2,
            'scope':'Static evidence reproducibility, not runtime/GPU validation or a proof of all semantic conclusions',
            'v1':v1,
            'new_instruction_records_checked':len(marks),
            'new_instructions_independently_redecoded':decoded,
            'total_instruction_records_checked':len(marks)+len(oldmarks),
            'distinct_instruction_addresses':len({(m['module'],int(m['rva'],0)) for m in marks+oldmarks}),
            'new_full_code_ranges_checked':len(spans),
            'new_class_layouts_reparsed':len(layouts),
            'new_direct_data_fields_reparsed':sum(len(x['fields']) for x in layouts),
            'new_vtable_slot_checks':len(vtables),
            'plain_struct_parser_regressions_checked':len(oldtypes),
            'mutated_instruction_rejected':mismatch_rejected,
            'unknown_codeview_leaf_rejected':unknown_rejected,
            'python':platform.python_version(),
            'objdump':subprocess.run([objdump,'--version'],text=True,capture_output=True,check=True).stdout.splitlines()[0],
        }
        args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
        print(json.dumps(result,indent=2))
    return 0

if __name__=='__main__':
    try: raise SystemExit(main())
    except (OSError,ValueError,KeyError,struct.error,subprocess.CalledProcessError) as exc:
        print(f'VERIFICATION FAILED: {exc}',file=sys.stderr)
        if isinstance(exc,subprocess.CalledProcessError) and exc.stderr: print(exc.stderr,file=sys.stderr)
        raise SystemExit(1)
