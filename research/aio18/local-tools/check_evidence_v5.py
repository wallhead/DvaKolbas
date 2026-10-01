"""Read-only V5 excerpt bytes, selected boundaries and exact string offsets."""
import hashlib,json,pathlib,re
import capstone,pefile
def normalize(op):
    op = op.split(' ; ', 1)[0].split('#', 1)[0].strip().lower()
    def number(match):
        n = int(match[0], 16)
        if (1 << 31) <= n < (1 << 32):
            n -= 1 << 32
        elif n >= (1 << 63):
            n -= 1 << 64
        return str(n)
    op = re.sub(r'0x[0-9a-f]+', number, op)
    op = re.sub(r'\*1(?=[+\]])', '', re.sub(r'\s+', '', op))
    return re.sub(r'((?:cs|ds|es|fs|gs|ss):)\[([0-9]+)\]', r'\1\2', op).replace('+-', '-')

root=pathlib.Path(__file__).resolve().parents[1]
review=root/'evidence-review-v5'
paths={'host':root/'aio-build18/SKSE/Plugins/SkyrimUpscaler.dll',
       'pd':root/'aio-build18/UpscalerBasePlugin/PDPerfPlugin.dll'}
landmarks=json.loads((review/'evidence/v5/landmarks_v5.json').read_text(encoding='utf-8'))
images={m:pefile.PE(str(p)) for m,p in paths.items()}
for m,p in paths.items():
    assert hashlib.sha256(p.read_bytes()).hexdigest()==landmarks['module_sha256'][m]
decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64)
boundary_cache={}
def boundary(module,rva):
    pe=images[module]
    entries=[e.struct for e in pe.DIRECTORY_ENTRY_EXCEPTION if e.struct.BeginAddress<=rva<e.struct.EndAddress]
    if not entries:
        return None
    e=entries[0];key=(module,e.BeginAddress,e.EndAddress)
    if key not in boundary_cache:
        code=pe.get_data(e.BeginAddress,e.EndAddress-e.BeginAddress)
        boundary_cache[key]={i.address for i in decoder.disasm(code,e.BeginAddress)}
    return rva in boundary_cache[key]
# Verify all literal byte chunks, including objdump continuation and partial-edge rows.
files=[];chunks=0;byte_count=0;partial=[];starts=[];pd_instruction_count=0
for path in sorted((review/'evidence/v5/disassembly').glob('*.asm')):
    module='pd' if path.name.startswith('pd_') else 'host';pe=images[module]
    lines=path.read_text(encoding='utf-8').splitlines();local_count=0;first=None
    for line in lines:
        fields=line.split('\t')
        if len(fields)>=2 and re.fullmatch(r'\s*[0-9a-f]+:',fields[0]) and re.fullmatch(r'(?:[0-9a-f]{2}\s*)+',fields[1].strip()):
            va=int(fields[0].strip().removesuffix(':'),16);rva=va-pe.OPTIONAL_HEADER.ImageBase
            assert rva>=0
            raw=bytes.fromhex(fields[1]);assert pe.get_data(rva,len(raw))==raw,(path.name,hex(rva))
            chunks+=1;byte_count+=len(raw);local_count+=1
            if first is None:first=rva
            text=fields[2].strip() if len(fields)>2 else ''
            if text.startswith('.byte') or text in ('rex.W','rex.WR','rex.R'):
                partial.append({'file':path.name,'rva':hex(rva),'bytes':raw.hex(),'supplied_text':text})
        elif module=='pd':
            m=re.match(r'^([0-9A-F]{8}):\s+(\S+)\s*(.*)$',line)
            if not m:continue
            rva=int(m[1],16);ins=next(decoder.disasm(pe.get_data(rva,15),pe.OPTIONAL_HEADER.ImageBase+rva,count=1))
            expected={'movabs':'mov'}.get(m[2],m[2]);actual={'movabs':'mov'}.get(ins.mnemonic,ins.mnemonic)
            assert expected==actual,(path.name,hex(rva),m[2],ins.mnemonic)
            if expected!='nop':assert normalize(m[3])==normalize(ins.op_str),(path.name,hex(rva),m[3],ins.op_str)
            pd_instruction_count+=1;local_count+=1
            if first is None:first=rva
    assert local_count>0,path.name
    files.append({'file':path.name,'module':module,'records_checked':local_count})
    starts.append({'file':path.name,'first_rva':hex(first),'first_row_matches_unwind_function_instruction_boundary':boundary(module,first)})
assert len(files)==13
selected=[]
for rec in landmarks['records']:
    module=rec['module'];rva=int(rec['rva'],16);raw=bytes.fromhex(rec['bytes'])
    pe=images[module];assert pe.get_data(rva,len(raw))==raw
    decoded=list(decoder.disasm(raw,rva));assert sum(i.size for i in decoded)==len(raw)
    known=boundary(module,rva);assert known is not False,(module,hex(rva),'not a function-aligned instruction boundary')
    selected.append({**rec,'function_aligned_boundary_checked':known,'decoded':[f'{i.mnemonic} {i.op_str}' for i in decoded]})
strings=json.loads((review/'evidence/v5/strings_v5.json').read_text(encoding='utf-8'))
host_data=paths['host'].read_bytes()
for rec in strings['records']:
    off=rec['file_offset'];expected=rec['text'].encode('ascii')
    assert host_data[off:off+len(expected)]==expected,rec['text']
# The supplied file named cs_transform_helper covers capture code, not the callee.
helper=next(e.struct for e in images['host'].DIRECTORY_ENTRY_EXCEPTION if e.struct.BeginAddress==0x2a1240)
helper_code=images['host'].get_data(helper.BeginAddress,helper.EndAddress-helper.BeginAddress)
helper_instructions=list(decoder.disasm(helper_code,helper.BeginAddress))
assert sum(i.size for i in helper_instructions)==len(helper_code)
draw=next(i for i in helper_instructions if i.address==0x2a152b)
assert draw.bytes.hex()=='ff5068'
helper_text='; SkyrimUpscaler.dll SHA256='+landmarks['module_sha256']['host']+'\n; Local Capstone decode, RVAs in addresses and branch operands; unwind range 0x2a1240..0x2a15a4\n'
helper_text+='\n'.join(f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in helper_instructions)+'\n'
(review/'local-cs-transform-helper.asm').write_text(helper_text,encoding='utf-8')
helper_result={'module':'SkyrimUpscaler.dll','sha256':landmarks['module_sha256']['host'],
 'rva_begin':hex(helper.BeginAddress),'rva_end_exclusive':hex(helper.EndAddress),
 'code_sha256':hashlib.sha256(helper_code).hexdigest(),'instructions_decoded':len(helper_instructions),
 'context_slot_13_call_rva':hex(draw.address),'context_slot_13_call_bytes':draw.bytes.hex(),
 'supplied_named_excerpt_covers_helper_body':False,
 'scope':'Local callee decode confirms bindings and a draw; caller-wide preservation/restoration and shader equations are not established.'}
(review/'local-cs-transform-helper.json').write_text(json.dumps(helper_result,indent=2),encoding='utf-8')
result={'status':'PASS','scope':'Static bytes, selected function-aligned boundaries and exact embedded string offsets; no target execution',
        'excerpt_files_checked':len(files),'gnu_raw_byte_chunks_checked':chunks,'gnu_raw_bytes_checked':byte_count,
        'pd_excerpt_instruction_comparisons':pd_instruction_count,'selected_v5_instruction_records_checked':len(selected),
        'selected_records_with_unwind_boundary_check':sum(r['function_aligned_boundary_checked'] is True for r in selected),
        'embedded_strings_at_supplied_offsets_checked':len(strings['records']),
        'actual_transform_helper':helper_result,'partial_edge_pseudo_records':partial,'excerpt_first_rows':starts,'per_file':files,
        'note':'Literal excerpt bytes can match even when a range starts inside an instruction; partial edge pseudo-records are not complete instructions. No GNU whole-excerpt instruction-boundary claim is made.'}
(review/'local-v5-verification.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
(review/'local-v5-landmark-witnesses.json').write_text(json.dumps(selected,indent=2),encoding='utf-8')
print(json.dumps({k:v for k,v in result.items() if k not in ('partial_edge_pseudo_records','excerpt_first_rows','per_file')},indent=2))
print('partial pseudo records',len(partial));print('unaligned first rows',[r for r in starts if r['first_row_matches_unwind_function_instruction_boundary'] is False])
