import socket           

network_socket = socket.socket()        
print ("Socket successfully created")

port = 49153          

network_socket.bind(('', port))        
print ("socket binded to %s" %(port))

network_socket.listen(5)    
print ("socket is listening")           

while True:
    c, addr = network_socket.accept()    
    print ('Got connection from', addr )
    
    c.send('Thank you for connecting'.encode())
    
    c.close()
    
    break