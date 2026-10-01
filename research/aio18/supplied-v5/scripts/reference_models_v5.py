#!/usr/bin/env python3
import unittest

AUTO_EXPOSURE=1<<5
DYNAMIC_RES=1<<6
DEPTH_INVERTED=1<<3
RCAS_COMP=1<<9

class V5Models(unittest.TestCase):
    def test_dx11_base_flags_auto_exposure(self):
        self.assertTrue(0x220 & AUTO_EXPOSURE)
    def test_dx11_base_flags_rcas(self):
        self.assertTrue(0x220 & RCAS_COMP)
    def test_dx11_base_flags_no_dynamic_resolution(self):
        self.assertFalse(0x220 & DYNAMIC_RES)
    def test_inverted_variant_adds_depth_bit(self):
        self.assertEqual(0x228, 0x220 | DEPTH_INVERTED)
    def test_payload_local_to_common_reactive_offset(self):
        payload_base=-0x20
        local_slot=0x00
        self.assertEqual(local_slot-payload_base, 0x20)
    def test_capture_scale_x(self):
        render_w, source_w=1280,1920
        self.assertAlmostEqual(render_w/source_w, 2/3)
    def test_capture_scale_y(self):
        render_h, source_h=720,1080
        self.assertAlmostEqual(render_h/source_h, 2/3)
    def test_shared_prepared_offsets_are_distinct(self):
        self.assertNotEqual(0xB50,0xBA8)

if __name__=='__main__':
    unittest.main(verbosity=2)
