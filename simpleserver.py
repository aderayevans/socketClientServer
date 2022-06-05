import socket           

PORT = 8080
SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER, PORT)


network_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_IP)        
print ("Socket successfully created")

network_socket.bind(ADDR)        
print (f"socket binded to {SERVER}:{PORT}")

network_socket.listen(5)    
print ("socket is listening")           

while True:
    conn, addr = network_socket.accept()    
    print ('Got connection from', addr )
    
    conn.send('Thank you for connecting'.encode())
    
    conn.close()
    
    break