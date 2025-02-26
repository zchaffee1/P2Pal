<<<<<<< HEAD
# BEFORE SUBMITTING
* zip from git repo
* comprehensive documentation
* build instructions
* basic test cases
* scripts to automate deployment and testing

# P2Pal
P2Pal is a small peer-to-peer messaging app 

# Running
Running this application is simple just run `./run.sh` and it will execute a bash
script to cmake and execute the P2Pal application.

`./run.sh <Port Number>` allows you to specify a Port Number with Port Number 
defaulting to 55000.


# Features

## GUI
Utilizes Qt6

- Chat log area displaying messages
- text input for entering new messages
- Multiple lines w/ word wrap support
- Auto-focus on text input area when launched

## Backend

UDP-based messaging system viaa QUdpSocket

### Network Communication

This allows for:
* Message serialization/deserialization using QVariantMap
* Support for local port discovery
* Basic message format containing:
    * ChatText: actual message content
    * Origin: Unique id for each P2Pal instance
    * Sequence number: For message ordering

### Gossip Protocol
1. Rumor Mongering
    * Messages are uniquely id using:
        * Origin: unique id for each P2Pal instance
        * Sequence number: From 1, incrementing each message from that origin
    * Messages are propagated in sequence number order
        * ex. send message 3 before message 4
    * Implementation has
        * Setting timers (1-2 sec) using QTimer
        * Resend messages if no response is recieved
        * Tracking which peers have which messages

2. Auto-Entropy:
    * Peers periodically compare message histories
    * User vector clock concept where each node tracks the following:
        * Which messages seen from each origin
        * up to what sequence number for each origin

### Peer Discovery
* Neighbor discover on local ports
* Dynamic peer addition via IP/hostname
* Maintain connections with immediate neighboring ports
>>>>>>> 410895d (Added documentation throught code, implemented bash run script, and timer implementation)
