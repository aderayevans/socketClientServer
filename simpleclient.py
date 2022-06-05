from ipaddress import ip_address
import socket 
import sys           

network_socket = socket.socket()

SERVER = sys.argv[1]
PORT = int(sys.argv[2])
# PORT = 8080
# SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER, PORT)

network_socket.connect(ADDR)
# network_socket.connect(('127.0.0.1', PORT))

print (network_socket.recv(1024).decode())
network_socket.close()