"""Reject obsolete INI instructions in the renderer's public diagnostics."""
from pathlib import Path
import json
import re
import sys

root = Path(__file__).resolve().parents[1]
files = ['Upscaling/FSRAvailability.h', 'Upscaling/FSRSettings.h',
         'Upscaling/FSRGenerationStatus.h', 'RendererBackendPolicy.h',
         'RendererSettings.h', 'FrameGen/NvidiaHostStartup.cpp',
         'OverlayNeuralPanel.cpp', 'NvidiaBaselinePolicy.h']
obsolete = [r'\[FSR\]\s+ProviderPolicy', r'\[FrameGeneration\]\s+FsrProviderPolicy',
            r'NativeUICompositionMode=0', r'backend [02]', r'use the SourceDLSSG settings',
            r'FSR requires EnableUpscaler=true']
failures = []
schema = json.loads((root / 'tools/ini/schema.json').read_text(encoding='utf-8-sig'))
integer_count = sum(field['type'] == 'Integer' for field in schema['fields'])
for name in ['SKSE/Plugins/RaZkolbaS.ini', 'examples/FSR-SR/RaZkolbaS.ini', 'examples/FSR-FG/RaZkolbaS.ini']:
    text = (root / 'package' / name).read_text(encoding='utf-8-sig')
    if text.splitlines().count('; Whole numbers only.') != integer_count:
        failures.append(f'{name}: integer controls must document their whole-number restriction')
for name in files:
    source = (root / 'src' / name).read_text(encoding='utf-8-sig')
    for line, text in enumerate(source.splitlines(), 1):
        # These patterns concern instructions, rather than internal key reads.
        if any(re.search(pattern, text) for pattern in obsolete):
            failures.append(f'{name}:{line}: {text.strip()}')
if failures:
    print('\n'.join(failures), file=sys.stderr)
    sys.exit(1)
print('PASS: public diagnostics contain no obsolete INI instructions')
