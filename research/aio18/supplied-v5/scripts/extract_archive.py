#!/usr/bin/env python3
"""Read a 7z with system libarchive, rejecting unsafe paths and links."""
import ctypes as C, ctypes.util, hashlib, json, pathlib, sys
src=pathlib.Path(sys.argv[1]); out=pathlib.Path(sys.argv[2]); out.mkdir(parents=True,exist_ok=True)
l=C.CDLL(ctypes.util.find_library('archive'))
for name, rt, at in [
 ('archive_read_new',C.c_void_p,[]),('archive_read_support_filter_all',C.c_int,[C.c_void_p]),
 ('archive_read_support_format_all',C.c_int,[C.c_void_p]),('archive_read_open_filename',C.c_int,[C.c_void_p,C.c_char_p,C.c_size_t]),
 ('archive_read_next_header',C.c_int,[C.c_void_p,C.POINTER(C.c_void_p)]),('archive_entry_pathname',C.c_char_p,[C.c_void_p]),
 ('archive_entry_size',C.c_int64,[C.c_void_p]),('archive_entry_filetype',C.c_uint,[C.c_void_p]),
 ('archive_entry_symlink',C.c_char_p,[C.c_void_p]),('archive_entry_hardlink',C.c_char_p,[C.c_void_p]),
 ('archive_read_data',C.c_ssize_t,[C.c_void_p,C.c_void_p,C.c_size_t]),('archive_error_string',C.c_char_p,[C.c_void_p]),
 ('archive_read_free',C.c_int,[C.c_void_p])]:
 fn=getattr(l,name);fn.restype=rt;fn.argtypes=at
a=l.archive_read_new();l.archive_read_support_filter_all(a);l.archive_read_support_format_all(a)
assert l.archive_read_open_filename(a,str(src).encode(),1<<20)==0
entries=[];buf=C.create_string_buffer(1<<20)
try:
 while True:
  e=C.c_void_p();r=l.archive_read_next_header(a,C.byref(e))
  if r==1: break
  if r<0: raise RuntimeError(l.archive_error_string(a))
  name=l.archive_entry_pathname(e).decode('utf8');p=pathlib.PurePosixPath(name.replace('\\','/'))
  if p.is_absolute() or '..' in p.parts or l.archive_entry_symlink(e) or l.archive_entry_hardlink(e):
   raise ValueError('Unsafe archive path/link: '+name)
  target=out/pathlib.Path(*p.parts)
  if l.archive_entry_filetype(e)==0o040000: target.mkdir(parents=True,exist_ok=True);continue
  target.parent.mkdir(parents=True,exist_ok=True);h=hashlib.sha256();n=0
  with target.open('wb') as f:
   while True:
    nr=l.archive_read_data(a,buf,len(buf))
    if nr==0: break
    if nr<0: raise RuntimeError(l.archive_error_string(a))
    data=buf.raw[:nr];f.write(data);h.update(data);n+=nr
  assert n==l.archive_entry_size(e),(name,n,l.archive_entry_size(e))
  entries.append({'path':str(p),'bytes':n,'sha256':h.hexdigest()})
finally: l.archive_read_free(a)
h=hashlib.sha256()
with src.open('rb') as f:
 for b in iter(lambda:f.read(1<<20),b''): h.update(b)
result={'archive':{'name':src.name,'bytes':src.stat().st_size,'sha256':h.hexdigest()},'files':entries}
print(json.dumps(result,indent=2))
