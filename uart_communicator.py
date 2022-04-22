import serial
import threading


PACKET_LENGTH = 9
TIME_STR_LENGTH = 8

def sender(ser):
    while True:
        data_to_send = input().split()
        try:
            if data_to_send[0] == "0" or data_to_send[0] == "1":
                ser.write(int(data_to_send[0]).to_bytes(1, 'big'))
                ser.write(int(data_to_send[1]).to_bytes(1, 'big'))
                for _ in range(PACKET_LENGTH - 2):
                    ser.write(int(0).to_bytes(1, 'big'))
            elif data_to_send[0] == "2":
                if (len(data_to_send[1]) == TIME_STR_LENGTH):
                    ser.write(int(data_to_send[0]).to_bytes(1, 'big'))
                    ser.write(bytes(data_to_send[1], "utf-8"))
                else:
                    print("Wrong length of time string")
        except:
            print("Wrong input")

def receiver(ser):
    while True:
        buffer = ser.read(1)
        print(buffer.decode("utf-8"), end="", flush=True)
        

print("0 - fist n logs")
print("1 - last n logs")
print("2 - logs with given time")

with serial.Serial("/dev/ttyACM0", baudrate=115200) as ser:
    receiver_th = threading.Thread(target=receiver, args=(ser,))
    receiver_th.start()
    sender(ser)
