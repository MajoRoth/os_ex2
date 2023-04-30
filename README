amit.roth, erel
Amit Roth (213212798), Erel Nahum (325177715)
EX: 2

FILES:
Thread.h -- headers for Thread class
Thread.cpp -- implementation of Thread class
uthread.cpp -- implementation of uthread library

REMARKS:

ANSWERS:

Q1:<br>
**sigsetjmp** - saves the current location, CPU state, and signal mask<br>
**siglongjmp** - goes to the same location, restoring the states and signal mask<br>
`sigsetjump` saved enviroment includes the signal mask, and `siglongjump` can restore it.

Q2:<br>
HTTP server - as a server developers, we don't want that while our server is processing 1 request,
it will ignore all other requests. hence, we use threads to process different requests.
user level threads allow us to keep listening into incoming requests while processing previous requests.
Main advantage of user-level threads in this context, is that they are managed entirely by the program rather than the 
operation system. This means that you have more control over thread scheduling, resource allocation, and synchronization
, which cn be important for achieving good performance and scalability in a web server.<br>
Q3:<br>
Few advantages of creating a new process for each tab in chrome:
- More secured - processes are more isolated compared to threads which prevents malicious code from one tab from affecting other tabs.
- More stable - even if one tab crashes or freezes, the rest of the tabs remain functioning.

Disadvantages:
- Higher memory usage
- Higher CPU usage

Q4:<br>
While typing the command, We used the keys. Every time a key has been pressed, an **interrupt** is sent from the keyboard (HW) to the OS.
After we had finished typing and pressed enter, the OS has to fulfill our request to send a `SIGTERM` signal to `shotwell` process.
<br>

Q5:<bar>
Real time is the actual elapsed time that it takes for a process or task to complete, measured by a physical clock. Virtual time is an abstraction of real time, used to measure the progress of a process relative to some predefined notion of time.
One example of virtual time being slick, is for physical simulations. If we want to simulate a long process in a little amount of time, we can use a virtual timer.
One example of real time being useful, is when we want our program to react to real world time.


