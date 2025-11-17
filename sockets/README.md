# Sockets Lab

## Overview
Implements a TCP client–server system that sends and receives raw byte streams over the network using POSIX sockets.

## Features
- Server accepts incoming TCP connections and prints all bytes received  
- Client connects and transmits arbitrary data exactly as entered  
- Uses low-level socket APIs for connection setup and data transfer  
- Handles network addresses, connection lifecycle, and error checking  

## Tech
- C  
- POSIX networking: `socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`  
- TCP communication and byte-level I/O  
