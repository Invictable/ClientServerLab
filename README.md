# ClientServerLab
 A simple c++ client server file transfer program that measure time elapsed.

# Usage
g++ server.cpp -o server
g++ client.cpp -o client
In the first terminal, run ./server
In the second terminal, run ./client

# Results
The file will be transfered using a buffer size designated in bufferSize.txt.
Results will be appended to result.txt which is created if it doesnt exist.
output.txt is created so we can guarentee the file sent matches the original.
