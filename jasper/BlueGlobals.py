# Prepared by Team BlueGum
# Juan Jhong Chung, Laura Kamfonik & Ankit Sharma
# EC535 Fall 2014 Final Project

import bluetooth

# Create two sockets, one to send and one to receive
global tx_sock
global rx_sock
global tx_init
global rx_initddddd

tx_init = 0
rx_init = 0
tx_sock = bluetooth.BluetoothSocket(bluetooth.L2CAP)
rx_sock = bluetooth.BluetoothSocket(bluetooth.L2CAP)

# Init code to set up the BT link
def inittx():
    global tx_init
    bd_addr = "00:03:19:50:29:15"
    txport = 0x1001
    tx_sock.connect((bd_addr, txport))
    tx_init = 1

def initrx():
    global rx_init
    global rx_sock
    rx_sock = bluetooth.BluetoothSocket(bluetooth.L2CAP)
    rxport = 0x1003
    rx_sock.bind(("", rxport))
    rx_sock.listen(1)
    rx_init = 1
def BlueSend(num):
    tx_sock.send(num)

def BlueReceive():
    global rx_sock
    client, addr = rx_sock.accept()
    data = rx_sock.recv(1024)
    return data
