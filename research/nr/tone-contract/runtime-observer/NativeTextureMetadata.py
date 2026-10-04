"""Passive metadata reads for one independently qualified Windows texture getter.

Never calls a target COM method. Unknown implementations remain unsupported.
The caller must qualify the physically mapped d3d11 image before passing its base.
"""
import hashlib
import struct

D3D11_SHA = '722871e4ac32972617483197709fe0d924ced5ed894b18fd13e0813d0b25950f'
GETTER_RVA = 0x36da0
GETTER_CODE_SHA = '01e2427e17cfee4361b92e20e6590ffb0b5dcfe3363adcc4bf200f119abef61f'


def describe_texture(read, texture, qualified_d3d11_base):
    if not texture:
        return {'status': 'absent'}
    if not qualified_d3d11_base:
        return {'status': 'unsupported', 'reason': 'Unqualified mapped d3d11 image'}
    try:
        vtable = struct.unpack('<Q', read(texture, 8))[0]
        getter = struct.unpack('<Q', read(vtable + 80, 8))[0]
        if getter != qualified_d3d11_base + GETTER_RVA:
            return {'status': 'unsupported', 'reason': 'Unqualified texture GetDesc implementation'}
        if hashlib.sha256(read(getter, 128)).hexdigest() != GETTER_CODE_SHA:
            return {'status': 'unsupported', 'reason': 'Mapped texture getter code differs'}

        def fields():
            width, height = struct.unpack('<II', read(texture + 0x148, 8))
            mip = read(texture + 0x118, 1)[0]
            array = struct.unpack('<H', read(texture + 0x152, 2))[0]
            fmt = struct.unpack('<h', read(texture + 0x104, 2))[0]
            count, quality = struct.unpack('<II', read(texture + 0x108, 8))
            return dict(width=width, height=height, mipLevels=mip, arraySize=array,
                        format=fmt, sampleCount=count, sampleQuality=quality)

        first = fields(); second = fields()
        if (first != second or struct.unpack('<Q', read(texture, 8))[0] != vtable
                or struct.unpack('<Q', read(vtable + 80, 8))[0] != getter):
            return {'status': 'inconsistent', 'reason': 'Texture metadata or interface changed during reads'}
        if not (1 <= first['width'] <= 16384 and 1 <= first['height'] <= 16384
                and 1 <= first['mipLevels'] <= 15 and 1 <= first['arraySize'] <= 2048
                and first['format'] > 0 and first['sampleCount'] in (1, 2, 4, 8, 16, 32)):
            return {'status': 'unsupported', 'reason': 'Texture metadata outside qualified bounds'}
        return dict(status='captured', getterRva=hex(GETTER_RVA), **first)
    except (OSError, ValueError, struct.error) as error:
        return {'status': 'read-failed', 'reason': repr(error)}
