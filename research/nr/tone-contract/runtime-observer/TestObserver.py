"""Integration contract for the external observer, using an owned CPU fixture.

Catch lost detach/restore, wrong frame register/offsets, missed new threads,
silent sampling failures and admission of an unknown image. No GPU or Skyrim.
"""
import argparse
import json
import pathlib
import shutil
import subprocess
import tempfile
import time
import unittest


class ObserverTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.bin = pathlib.Path(ARGS.bin)

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self.tmp.name)
        self.fixture = subprocess.Popen(
            [str(self.bin / 'NrObserverFixture.exe')], stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        self.ready = json.loads(self.fixture.stdout.readline())

    def tearDown(self):
        if self.fixture.poll() is None:
            self.fixture.stdin.write('quit\n')
            self.fixture.stdin.flush()
            self.fixture.communicate(timeout=10)
        self.assertEqual(self.fixture.returncode, 0, 'fixture crashed or retained observer registers')
        self.tmp.cleanup()

    def observer(self, *extra):
        tool = subprocess.Popen([
            str(self.bin / 'NrBoundaryObserver.exe'), '--pid', str(self.fixture.pid),
            '--fixture', '--output', str(self.root / 'capture.json'),
            '--seconds', '3', '--samples', '24', '--stride', '1', *extra],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        def cleanup():
            # Only our owned tool is stopped, never the observation target.
            if tool.poll() is None:
                (self.root / 'cancel').touch()
                try:
                    tool.communicate(timeout=10)
                except subprocess.TimeoutExpired:
                    tool.kill()
            tool.communicate(timeout=10)
        self.addCleanup(cleanup)
        return tool

    def run_sources(self, mode='run'):
        self.fixture.stdin.write(mode + '\n')
        self.fixture.stdin.flush()

    def check_alive_restored(self):
        self.fixture.stdin.write('check\n')
        self.fixture.stdin.flush()
        result = json.loads(self.fixture.stdout.readline())
        self.assertTrue(result['registersRestored'], result)
        self.assertTrue(result['alive'])
        return result

    def test_limit_detaches_and_captures_effective_values(self):
        tool = self.observer()
        self.assertIn('ARMED', tool.stdout.readline())
        self.run_sources()
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 0, stdout + stderr)
        data = json.loads((self.root / 'capture.json').read_text())
        self.assertEqual(data['finishReason'], 'sample-limit')
        self.assertEqual(len(data['samples']), 24)
        self.assertTrue(data['detached'])
        self.assertTrue(data['registersRestored'])
        self.assertEqual({s['style'] for s in data['samples']}, {0, 1})
        for row in data['samples']:
            self.assertEqual(row['tone'], 1)
            self.assertEqual(row['structure'], 0.75)
            self.assertEqual(row['intensity'], 0.5)
            self.assertEqual(row['color']['width'], 2560)
            self.assertEqual(row['color']['height'], 1440)
            self.assertEqual(row['mvec']['width'], 1280)
            self.assertEqual(row['depth']['x'], 4)
            self.assertEqual(row['coefficients'][2], -0.125)
            self.assertEqual(row['controlMask']['pointer'], '0x0')
        before = self.check_alive_restored()['calls']
        time.sleep(0.1)
        self.assertGreater(self.check_alive_restored()['calls'], before)

    def test_timeout_detaches_when_nr_never_runs(self):
        tool = self.observer('--seconds', '1')
        self.assertIn('ARMED', tool.stdout.readline())
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 3, stdout + stderr)
        data = json.loads((self.root / 'capture.json').read_text())
        self.assertEqual(data['samples'], [])
        self.assertEqual(data['finishReason'], 'timeout')
        self.assertTrue(data['registersRestored'])
        self.check_alive_restored()

    def test_new_threads_are_observed_and_restored(self):
        tool = self.observer()
        self.assertIn('ARMED', tool.stdout.readline())
        self.run_sources('newthreads')
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 0, stdout + stderr)
        rows = json.loads((self.root / 'capture.json').read_text())['samples']
        self.assertGreaterEqual(len({r['threadId'] for r in rows}), 2)
        self.check_alive_restored()

    def test_read_failure_restores_and_keeps_target_running(self):
        tool = self.observer()
        self.assertIn('ARMED', tool.stdout.readline())
        self.run_sources('badframe')
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 1, stdout + stderr)
        data = json.loads((self.root / 'capture.json').read_text())
        self.assertEqual(data['finishReason'], 'error')
        self.assertIn('ReadProcessMemory', data['error'])
        self.assertTrue(data['registersRestored'])
        self.assertTrue(data['detached'])
        self.check_alive_restored()

    def test_cancellation_cleans_up(self):
        cancel = self.root / 'cancel'
        tool = self.observer('--cancel-file', str(cancel))
        self.assertIn('ARMED', tool.stdout.readline())
        cancel.touch()
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 3, stdout + stderr)
        data = json.loads((self.root / 'capture.json').read_text())
        self.assertEqual(data['finishReason'], 'cancelled')
        self.assertTrue(data['registersRestored'])
        self.assertTrue(data['detached'])
        self.check_alive_restored()

    def test_cancel_with_active_workers_drains_pending_faults(self):
        cancel = self.root / 'cancel'
        tool = self.observer('--seconds', '30', '--samples', '64', '--stride', '120',
                             '--cancel-file', str(cancel))
        self.assertIn('ARMED', tool.stdout.readline())
        self.run_sources('newthreads')
        time.sleep(0.05)
        cancel.touch()
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 0, stdout + stderr)
        data = json.loads((self.root / 'capture.json').read_text())
        self.assertEqual(data['finishReason'], 'cancelled')
        self.assertTrue(data['registersRestored'])
        self.assertTrue(data['detached'])
        self.check_alive_restored()

    def test_process_exit_is_not_reported_as_sample_limit(self):
        tool = self.observer()
        self.assertIn('ARMED', tool.stdout.readline())
        self.fixture.stdin.write('quit\n')
        self.fixture.stdin.flush()
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 3, stdout + stderr)
        data = json.loads((self.root / 'capture.json').read_text())
        self.assertEqual(data['finishReason'], 'target-exited')
        self.fixture.communicate(timeout=10)

    def test_unknown_image_hash_is_rejected_before_attach(self):
        changed = self.root / 'NrObserverFixture.exe'
        shutil.copyfile(self.bin / 'NrObserverFixture.exe', changed)
        with changed.open('ab') as out:
            out.write(b'unknown build overlay')
        with subprocess.Popen([str(changed)], stdin=subprocess.PIPE,
                              stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True) as target:
            json.loads(target.stdout.readline())
            result = subprocess.run([
                str(self.bin / 'NrBoundaryObserver.exe'), '--pid', str(target.pid),
                '--fixture', '--output', str(self.root / 'unknown.json')],
                capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 1)
            self.assertIn('Unknown fixture build', result.stderr)
            target.stdin.write('check\n')
            target.stdin.flush()
            self.assertTrue(json.loads(target.stdout.readline())['registersRestored'])
            target.communicate('quit\n', timeout=10)
            self.assertEqual(target.returncode, 0)

    def test_existing_breakpoint_is_preserved_on_partial_arm_failure(self):
        self.fixture.stdin.write('occupied\n')
        self.fixture.stdin.flush()
        self.assertTrue(json.loads(self.fixture.stdout.readline())['occupied'])
        tool = self.observer()
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 1, stdout + stderr)
        data = json.loads((self.root / 'capture.json').read_text())
        self.assertIn('already has a hardware breakpoint', data['error'])
        self.assertTrue(data['registersRestored'])
        self.assertTrue(data['detached'])
        self.check_alive_restored()

    def test_target_breakpoint_during_attach_reaches_its_handler(self):
        self.fixture.stdin.write('foreign-on-attach\n')
        self.fixture.stdin.flush()
        self.assertTrue(json.loads(self.fixture.stdout.readline())['foreignConfigured'])
        tool = self.observer('--seconds', '1')
        self.assertIn('ARMED', tool.stdout.readline())
        stdout, stderr = tool.communicate(timeout=10)
        self.assertEqual(tool.returncode, 3, stdout + stderr)
        self.assertEqual(self.check_alive_restored()['foreignHandled'], 1)

    def test_unpinned_fixture_is_rejected_in_game_mode(self):
        tool = subprocess.run([
            str(self.bin / 'NrBoundaryObserver.exe'), '--pid', str(self.fixture.pid),
            '--output', str(self.root / 'rejected.json')],
            capture_output=True, text=True, timeout=10)
        self.assertEqual(tool.returncode, 1)
        self.assertIn('target executable', tool.stderr)
        self.check_alive_restored()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--bin', required=True)
    ARGS, remaining = parser.parse_known_args()
    unittest.main(argv=[__file__, *remaining])
