import serial
import threading


def sender(ser):
    while True:
        buf = map(int, input().split())
        for b in buf:
            ser.write(b.to_bytes(1, 'big'))

def receiver(ser):
    while True:
        buffer = ser.read(1)
        print(buffer.decode("utf-8"), end="", flush=True)
        

with serial.Serial("/dev/ttyACM0", baudrate=115200) as ser:
    receiver_th = threading.Thread(target=receiver, args=(ser,))
    receiver_th.start()
    sender(ser)
