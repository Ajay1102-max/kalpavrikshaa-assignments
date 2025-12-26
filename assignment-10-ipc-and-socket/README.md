IPC and Socket Programming Assignment

This assignment demonstrates:
1. IPC mechanisms:
   - File based IPC
   - Pipe IPC
   - Message Queue IPC
   - Shared Memory IPC
2. Socket based ATM transaction system using TCP

How to Compile:
gcc file_ipc.c -o file_ipc
gcc pipe_ipc.c -o pipe_ipc
gcc msg_queue_ipc.c -o msg_queue_ipc
gcc shm_ipc.c -o shm_ipc

Server:
gcc server.c -o server -lpthread
Client:
gcc client.c -o client

Run server first, then client in another terminal.