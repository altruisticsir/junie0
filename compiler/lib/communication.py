import serial as s
import time

ser = s.Serial(
    "COM5",
    9600,
)


def write(bytes: bytearray):
    time.sleep(1)
    for b in bytes:
        ser.write(b.to_bytes())
    ser.flush()


def close():
    ser.close()
