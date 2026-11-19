"""Synthetic transport tests; no serial hardware or real photographs are used."""
import csv
import contextlib
import importlib.util
import io
from pathlib import Path
import tempfile
import unittest

MODULE = Path(__file__).resolve().parents[1] / "python" / "recibir_foto.py"
# The author's receiver creates its output directory on import. Keep that side
# effect inside a temporary directory during these tests.
import os
with tempfile.TemporaryDirectory() as initial:
    previous = Path.cwd()
    try:
        os.chdir(initial)
        spec = importlib.util.spec_from_file_location("receiver", MODULE)
        receiver = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(receiver)
    finally:
        os.chdir(previous)

# Marker-only fixtures exercise framing/carving, not JPEG decoding.
JPEG1 = b"\xff\xd8camera-one\xff\xd9"
JPEG2 = b"\xff\xd8camera-two\xff\xd9"


class FakeSerial:
    def __init__(self, wire, chunk=3):
        self.stream = io.BytesIO(wire)
        self.chunk = chunk
        self.written = b""

    def read(self, size):
        return self.stream.read(min(size, self.chunk))

    def readline(self):
        return self.stream.readline()

    def write(self, data):
        self.written += data
        return len(data)

    def flush(self):
        pass


def frame(camera, payload):
    return f"JPEG {camera} {len(payload)}\n".encode() + payload + f"END {camera}\n".encode()


class ReceiverTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.old_folder = receiver.CARPETA
        receiver.CARPETA = Path(self.temp.name)
        self.addCleanup(setattr, receiver, "CARPETA", self.old_folder)
        old_csv = receiver.ARCHIVO_CSV
        receiver.ARCHIVO_CSV = receiver.CARPETA / "mediciones_mpu6050.csv"
        self.addCleanup(setattr, receiver, "ARCHIVO_CSV", old_csv)
        self.stdout = contextlib.redirect_stdout(io.StringIO())
        self.stdout.__enter__()
        self.addCleanup(self.stdout.__exit__, None, None, None)

    def test_fragmented_pair_drains_bytes_after_eoi(self):
        wire = frame(1, b"prefix" + JPEG1 + b"trailer\n") + frame(2, JPEG2 + b"extra") + b"DONE\n"
        serial = FakeSerial(wire, chunk=2)
        receiver.capturar_dos(serial)
        self.assertEqual(serial.written, b"F\n")
        self.assertEqual((receiver.CARPETA / "camara1_0001.jpg").read_bytes(), JPEG1)
        self.assertEqual((receiver.CARPETA / "camara2_0001.jpg").read_bytes(), JPEG2)
        self.assertEqual(serial.read(1), b"")

    def test_imu_values_append_csv_once_with_pair_names(self):
        wire = frame(1, JPEG1) + frame(2, JPEG2) + b"IMU 66.52 1.31\nDONE\n"
        receiver.capturar_dos(FakeSerial(wire))
        receiver.capturar_dos(FakeSerial(wire))
        with receiver.ARCHIVO_CSV.open(newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))
        self.assertEqual(len(rows), 2)
        self.assertEqual(rows[0]["roll_grados"], "66.52")
        self.assertEqual(rows[0]["pitch_grados"], "1.31")
        self.assertEqual(rows[0]["camara1"], "camara1_0001.jpg")
        self.assertEqual(rows[1]["camara2"], "camara2_0002.jpg")

    def test_imu_error_still_saves_pair_and_blank_csv_angles(self):
        wire = frame(1, JPEG1) + frame(2, JPEG2) + b"IMU ERROR\nDONE\n"
        receiver.capturar_dos(FakeSerial(wire))
        with receiver.ARCHIVO_CSV.open(newline="", encoding="utf-8") as f:
            row = next(csv.DictReader(f))
        self.assertEqual(row["roll_grados"], "")
        self.assertEqual(row["pitch_grados"], "")
        self.assertEqual(len(list(receiver.CARPETA.glob("*.jpg"))), 2)

    def test_invalid_imu_or_missing_done_does_not_log_csv(self):
        for tail in (b"IMU invalid 1\nDONE\n", b"IMU 1 2\n"):
            with self.subTest(tail=tail):
                with self.assertRaises((RuntimeError, TimeoutError)):
                    receiver.capturar_dos(FakeSerial(frame(1, JPEG1) + frame(2, JPEG2) + tail))
                self.assertFalse(receiver.ARCHIVO_CSV.exists())

    def test_truncated_binary_payload(self):
        with self.assertRaises(TimeoutError):
            receiver.capturar_dos(FakeSerial(b"JPEG 1 100\nshort"))
        self.assertEqual(list(receiver.CARPETA.iterdir()), [])

    def test_out_of_order_camera(self):
        with self.assertRaisesRegex(RuntimeError, "Se esperaba camara 1"):
            receiver.capturar_dos(FakeSerial(frame(2, JPEG2)))

    def test_oversized_frame_rejected_before_reading(self):
        with self.assertRaises(ValueError):
            receiver.capturar_dos(FakeSerial(f"JPEG 1 {receiver.MAX_JPEG + 1}\n".encode()))

    def test_zero_length_rejected(self):
        with self.assertRaises(ValueError):
            receiver.capturar_dos(FakeSerial(b"JPEG 1 0\n"))

    def test_wrong_end(self):
        wire = f"JPEG 1 {len(JPEG1)}\n".encode() + JPEG1 + b"END 2\n"
        with self.assertRaisesRegex(RuntimeError, "Final inesperado"):
            receiver.capturar_dos(FakeSerial(wire))
        self.assertEqual(list(receiver.CARPETA.iterdir()), [])

    def test_blank_line_before_end_is_rejected(self):
        wire = f"JPEG 1 {len(JPEG1)}\n".encode() + JPEG1 + b"\nEND 1\n"
        with self.assertRaisesRegex(RuntimeError, "Final inesperado"):
            receiver.capturar_dos(FakeSerial(wire))

    def test_missing_done_keeps_files_but_rejects_pair(self):
        with self.assertRaises(TimeoutError):
            receiver.capturar_dos(FakeSerial(frame(1, JPEG1) + frame(2, JPEG2)))
        self.assertEqual(len(list(receiver.CARPETA.glob("*.jpg"))), 2)

    def test_missing_markers_retains_raw_diagnostic(self):
        with self.assertRaises(ValueError):
            receiver.guardar_jpeg(b"no image markers", 1, 7)
        self.assertEqual((receiver.CARPETA / "camara1_0007_invalida.bin").read_bytes(), b"no image markers")

    def test_valid_image_cannot_be_overwritten(self):
        receiver.guardar_jpeg(JPEG1, 1, 1)
        with self.assertRaises(FileExistsError):
            receiver.guardar_jpeg(JPEG2, 1, 1)
        self.assertEqual((receiver.CARPETA / "camara1_0001.jpg").read_bytes(), JPEG1)

    def test_numbering_uses_largest_valid_camera_filename(self):
        for name in ("camara1_0002.jpg", "camara2_0010.jpg", "camara3_9999.jpg", "camara1_1000_invalida.bin", "camara1_9000Xjpg"):
            (receiver.CARPETA / name).touch()
        self.assertEqual(receiver.siguiente_numero(), 11)

    def test_firmware_error_and_ready(self):
        with self.assertRaisesRegex(RuntimeError, "ERROR INICIALIZACION"):
            receiver.esperar_ready(FakeSerial(b"ERROR INICIALIZACION\n"))
        receiver.esperar_ready(FakeSerial(b"boot message\nREADY\n"))
        with self.assertRaisesRegex(RuntimeError, "ERROR capture"):
            receiver.capturar_dos(FakeSerial(b"ERROR capture\n"))

    def test_second_camera_failure_leaves_first_image(self):
        with self.assertRaises(RuntimeError):
            receiver.capturar_dos(FakeSerial(frame(1, JPEG1) + b"ERROR camera2\n"))
        self.assertEqual(len(list(receiver.CARPETA.glob("*.jpg"))), 1)


if __name__ == "__main__":
    unittest.main()
