import serial as s
import time

ser = s.Serial(
    "COM8",
    9600,
)


def write(bytes: bytearray):
    time.sleep(0.9)
    for b in bytes:
        ser.write(b.to_bytes())
    ser.flush()


def close():
    ser.close()
