import socket
# import time

def receive_data(ip_address, port):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        client_socket.connect((ip_address, port))

        data = client_socket.recv(1024)  # Ajusta tamanho do buffer de chegada dos dados
        received_data = data.decode('utf-8')
        
        # Printa dados e salva no arquivo
        if received_data != '':
            print("Received data:", received_data)
            with open("dados_gps.txt", "a") as file:
                    file.write(received_data + "\n")
    
    except ConnectionRefusedError:
        print("Connection was refused. Make sure the server is running.")
    except Exception as e:
        print("An error occurred:", e)
    finally:
        client_socket.close()

server_ip_address = '192.168.4.1'
server_port = 2503

while 1:
    receive_data(server_ip_address, server_port)
    # time.sleep(200)
