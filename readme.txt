1. First compile the client.cpp file in client folder with command "g++ client.cpp -pthread" and compile with "./a.out".
2. Then compile the server.cpp file in server folder with command "g++ server.cpp -pthread" and compile with "./a.out".
3. Enter the filename in client.cpp program(as stdin not parameter) that you want to download from server.
4. If the file exists(in the server folder) then the file is transmitted and both the programs close.
5. If the file doesn't exist on server,then both the programs close with file doesn't exist message.

CHANGES MADE
We had to include acks for filname as well, to work in case of packet loss. We also had to include a few NAKs when the file was tranferred to stop the thread accepting acks to avoid segmentation fault.
