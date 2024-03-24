### Console-Based Chat Application
This chat application enables multiple clients to connect to a server and exchange messages in real-time, functioning as a basic group chat. The application consists of two parts: a server that handles incoming connections and broadcasts messages to all connected clients, and a client that sends and receives messages.

### Features
- Real-time Messaging: Clients can send and receive messages instantly.
- Multi-client Support: The server can handle multiple client connections simultaneously.
- Dynamic Usernames: Clients can set their usernames upon connection, which are prepended to their messages for easy identification.

### Requirements
- Linux or UNIX-like operating system
- GCC compiler
- POSIX Threads (Pthreads) support

### Compilation
First, ensure you have GCC installed. Then compile the server and client using the following commands:

gcc -o server server.c -pthread
gcc -o client client.c -pthread

### Running the Application
Server
Start the server in a terminal window:

./server
The server will listen for incoming connections on port 8080.

Client
Open a new terminal window for each client and run:

./client
Upon connection, the client will prompt for a username. After setting the username, you can start sending messages to the group chat.

### Implementation Details
Server
The server uses socket programming to accept connections on port 8080. Each client connection is handled in a separate thread, allowing the server to manage multiple clients concurrently. When a message is received from one client, it's broadcast to all other connected clients.

Client
The client establishes a connection to the server and uses two threads: one for sending messages and another for receiving. The send thread reads messages from the standard input and sends them to the server. The receive thread listens for messages from the server and prints them to the standard output.

### Usage Instructions

- Run the server in one terminal window.
- Open multiple terminal windows for each client you wish to connect.
- In each client window, enter a unique username when prompted.
- Start chatting! Messages typed in one client's window will appear in all other clients' windows.

### Limitations
The chat application is console-based and does not feature a graphical user interface.
It lacks advanced features like private messaging, message encryption, or persistent chat history.
The server and clients are designed to run on the same local network. Modifications are required for operation over the internet.

### Future Improvements
- Implement a graphical user interface using a library like GTK or Qt.
- Add support for private messages between clients.
- Introduce message encryption for secure communication.
- Store chat history in a database or file for persistence.
