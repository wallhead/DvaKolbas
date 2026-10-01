#!/usr/bin/env python3
"""Pure Python reference models for the recovered contracts. NEVER executes a DLL.
These tests check arithmetic/branch interpretations, not renderer correctness.
"""
from __future__ import annotations
import math, struct, unittest

def f32(x: float) -> float:
    return struct.unpack('<f', struct.pack('<f', x))[0]

def halton(index: int, base: int) -> float:
    if index < 0 or base < 2: raise ValueError('nonnegative index and base >=2 required')
    result, fraction = 0.0, 1.0
    while index:
        fraction = f32(fraction / base)
        result = f32(result + f32(fraction * (index % base)))
        index //= base
    return result

def jitter(phase: int, count: int) -> tuple[float,float]:
    if phase < 0: raise ValueError('this model covers nonnegative host phase counters')
    index = phase % max(count,8) + 1
    return f32(halton(index,2)-0.5), f32(halton(index,3)-0.5)

def ui_viewport(vp: tuple, render: tuple, display: tuple, ui: bool, count: int) -> tuple:
    """Only normal UI branch; the earlier special predicate is intentionally excluded."""
    x,y,w,h,zmin,zmax = vp
    return (x,y,*display,zmin,zmax) if ui and count == 1 and (w,h)==render else vp

def fg_reset(payload_reset: bool, latch: bool) -> bool:
    return payload_reset or latch

def retire_reset(latch: bool, prepare_ok: bool, configure_ok: bool, enabled: bool) -> bool:
    return False if latch and prepare_ok and configure_ok and enabled else latch

def sr_resource_slots(payload: dict) -> dict:
    return dict(color=payload.get(0x08), depth=payload.get(0x18), motionVectors=payload.get(0x10),
                exposure=None, reactive=payload.get(0x20), transparencyAndComposition=None,
                output=payload.get(0x30) or payload.get(0x28))

class RecoveredContractTests(unittest.TestCase):
    def test_halton_known_first(self):
        self.assertEqual(halton(1,2),0.5)
        self.assertAlmostEqual(halton(1,3),1/3,places=6)
    def test_jitter_min_period(self): self.assertEqual(jitter(0,1),jitter(8,1))
    def test_jitter_period(self): self.assertEqual(jitter(3,18),jitter(21,18))
    def test_engine_signs(self):
        x,y=jitter(1,18); self.assertEqual(-2*x/1280, 2*(-x)/1280)
        self.assertEqual(2*y/720,-2*(-y)/720)
    def test_ui_preserves_origin_depth(self):
        self.assertEqual(ui_viewport((3,5,1280,720,.1,.9),(1280,720),(1920,1080),True,1),
                         (3,5,1920,1080,.1,.9))
    def test_ui_count_guard(self):
        vp=(0,0,1280,720,0,1); self.assertEqual(ui_viewport(vp,(1280,720),(1920,1080),True,2),vp)
    def test_ui_size_guard(self):
        vp=(0,0,640,360,0,1); self.assertEqual(ui_viewport(vp,(1280,720),(1920,1080),True,1),vp)
    def test_ui_phase_guard(self):
        vp=(0,0,1280,720,0,1); self.assertEqual(ui_viewport(vp,(1280,720),(1920,1080),False,1),vp)
    def test_reset_or_latch(self): self.assertTrue(fg_reset(False,True))
    def test_reset_prepare_failure_retains(self): self.assertTrue(retire_reset(True,False,True,True))
    def test_reset_configure_failure_retains(self): self.assertTrue(retire_reset(True,True,False,True))
    def test_reset_disabled_retains(self): self.assertTrue(retire_reset(True,True,True,False))
    def test_reset_success_clears(self): self.assertFalse(retire_reset(True,True,True,True))
    def test_output_fallback(self): self.assertEqual(sr_resource_slots({0x28:'out'})['output'],'out')
    def test_output_preference(self): self.assertEqual(sr_resource_slots({0x28:'a',0x30:'b'})['output'],'b')
    def test_reactive_not_output(self):
        result=sr_resource_slots({0x20:'mask',0x28:'out'})
        self.assertEqual(result['reactive'],'mask'); self.assertNotEqual(result['reactive'],result['output'])

if __name__=='__main__': unittest.main()
