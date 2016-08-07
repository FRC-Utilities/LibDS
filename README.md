# LibDS C

> **Warning:** This project will be subject to many changes (and complete rewrites) as I learn new things about C.

> If you are interested in learning how the Driver Station works, check the [original LibDS](https://github.com/frc-utilities/libds). It contains everything needed to operate FRC 2014 and 2015/2016 robots.

This is a re-implementation of LibDS in plain C (with SDS for dynamic string handling). You can check the [TODOs](TODO.md) for the current state of the project.

You may find a lot of mistakes here, be it design choices or just stupid mistakes. If you spot something, I would be very grateful if you could tell me about it (or make a pull request).

### Example Project

![Image](example/etc/screenshot.png)

I have made a simple command-line Driver Station with SDL and ncurses/pdcurses to demostrate the uses of LibDS. You can check it [here](example/).

### Quick Introduction

#### Initialization

The LibDS has its own event loop, which runs on a separate thread. To start the DS engine, you must call `DS_Init()`, which initializes all the modules of the LibDS (config, events, joysticks, etc).

You should initialize the DS before initalizing any of your application components that interact with the DS. Check this example:

```c
#include <LibDS.h>

int main() {
   /* Initialize the DS */
   DS_Init();

   /* Now proceed to initializing your applicaiton */
   .
   .
   . 
}
```

#### Communication protocols

After initializing the DS, you must load a protocol, which instructs the LibDS on the following processes:

- How to create client packets:

- DS-to-robot packets
- DS-to-radio packets
- DS-to-FMS packets

- How to read and interpret incoming packets

- How to connect to the different network targets:

- Input and output ports
- IP protocol type (UDP or TCP)
- Which IP addresses to use

- Last but not least, the sender timings, for example:

- Send DS-to-robot packets every 20 ms
- Send DS-to-FMS packets every 500 ms
- Do not send DS-to-radio packets

The LibDS has built-in support for the following protocols:

- FRC 2009-2014
- FRC 2015
- FRC 2016 (same as 2015, but with different robot address)

To load a protocol, use the `DS_ConfigureProtocol()` function. As a final note, you can also implement your own protocols and instruct he LibDS to use it. 


#### Interacting with the DS events

The LibDS registers the different events in a FIFO (First In, First Out) queue, to access the events, use the `DS_PollEvent()` function in a while loop. Each event has a "type" code, which allows you to know what kind of event are you dealing with. 

The easiest way to react to the DS events is the following (pseudo-code):

```c
DS_Event event;
while (DS_PollEvent (event)) {
   switch (event.type) {
      case DS_EVENT_X:
         // react to x event
      case DS_EVENT_Y:
         // react to y event
   }
}
```

The code above must be called periodically. Check the example project for more information.

### Project Architecture

#### 'Private' vs. 'Public' members

- All the functions that a client application would be interested in are located in [`DS_Client.h`](https://github.com/FRC-Utilities/LibDS-C/blob/master/include/DS_Client.h). 

- Functions that are used by the protocols to update the state of the LibDS are made available in [`DS_Config.h`](https://github.com/FRC-Utilities/LibDS-C/blob/master/include/DS_Config.h). Calling any of the 'setter' functions in [`DS_Config`](https://github.com/FRC-Utilities/LibDS-C/blob/master/include/DS_Config.h) will trigger an event (which can later be used by the client application).

#### Protocols

Protocols are encapsulated structures. When a protocol is initialized, it defines its properties and its respective data functions. The different functions of the LibDS will then operate with the data and properties defined by the current protocol.

As with the original LibDS, protocols have access to the `DS_Config` to update the state of the LibDS.

The base protocol is implemented in the [`DS_Protocol`](https://github.com/FRC-Utilities/LibDS-C/blob/master/include/DS_Objects.h#L42) structure.

##### Sockets

Instead of manually initializing a socket for each target, data direction and protocol type (UDP and TCP). The LibDS will use the [`DS_Socket`](https://github.com/FRC-Utilities/LibDS-C/blob/master/include/DS_Objects.h#L34) object to define ports, protocol type and remote targets. 

I will write all the logic code in [`socket.c`](https://github.com/FRC-Utilities/LibDS-C/blob/master/src/socket.c), which will be in charge of managing the system sockets with the information given by a [`DS_Socket`](https://github.com/FRC-Utilities/LibDS-C/blob/master/include/DS_Objects.h#L34) object.
