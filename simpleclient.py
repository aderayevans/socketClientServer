import socket            

network_socket = socket.socket()

port = 49153               

network_socket.connect(('127.0.0.1', port))

print (network_socket.recv(1024).decode())
network_socket.close()