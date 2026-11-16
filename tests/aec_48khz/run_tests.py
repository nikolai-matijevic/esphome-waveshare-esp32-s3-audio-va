#!/usr/bin/env python3
"""Run host DSP checks without ESPHome or an ESP32 toolchain."""
from pathlib import Path
import re
import subprocess
import tempfile

here = Path(__file__).resolve().parent
component = here.parents[1] / 'components/aec_speexdsp'
header = (component / 'capture_decimator.h').read_text()
coefficients = list(map(int, re.search(r'COEFFICIENTS\[TAPS\] = \{([^}]+)', header)[1].split(',')))
assert coefficients == coefficients[::-1]
assert sum(coefficients) == 32768
assert sum(map(abs, coefficients[:63])) * 65536 <= 2**31 - 1
with tempfile.TemporaryDirectory(prefix='lounge-aec-tests-') as folder:
    work = Path(folder)
    defines = work / 'esphome/core/defines.h'
    defines.parent.mkdir(parents=True)
    defines.touch()
    # Independent full int64 convolution checks the symmetric int32 fast path.
    a = header.index('          // Symmetry')
    b = header.index('          const int32_t value', a)
    reference = header[:a] + '''          int64_t sum = 0;
          for (size_t k = 0; k < TAPS; ++k)
            sum += static_cast<int64_t>(latest[-static_cast<int>(k)]) * COEFFICIENTS[k];
''' + header[b:]
    (work / 'reference_decimator.h').write_text(reference.replace('CaptureDecimator', 'ReferenceDecimator'))
    cases = [('test_capture_decimator.cpp', []), ('test_equivalence.cpp', [])]
    cases += [('test_wake_beep.cpp', [f'-DAEC_SPEEXDSP_PHYSICAL_SAMPLE_RATE={rate}']) for rate in (16000, 48000)]
    for source, flags in cases:
        executable = work / 'test'
        subprocess.run(['g++', '-std=c++17', '-O2', '-ftrapv', '-Wall', '-Wextra',
                        '-I', str(component), '-I', str(work), *flags,
                        str(here / source), '-o', str(executable)], check=True)
        subprocess.run([str(executable)], check=True)
