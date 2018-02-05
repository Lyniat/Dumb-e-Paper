import socket

sock = socket.socket()

host = "192.168.1.78"  # ESP32 IP in local network
port = 80  # ESP32 Server Port

sock.connect((host, port))

message = "Hello World"
sock.send(message)

data = ""

while len(data) < len(message):
    data += sock.recv(1)

print(data)

sock.close()