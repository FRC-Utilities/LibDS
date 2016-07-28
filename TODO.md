# TODOs

- [x] Define types, events and basic objects
- [x] Define base library functions
- [ ] Implement a way to register events
- [ ] Implement the event loop
- [ ] Implement the protocols (based on the previous LibDS research)
- [ ] Add protocol handler functions that free the generated (and obtained) data after being used 
- [ ] Implement cross-platform socket handling (using `DS_Socket` to define socket data)
- [ ] Implement joystick handling functions
- [ ] Implement the NetConsole (also using `DS_Socket` for the job)

### Notes (mostly to myself, that's sad...)

- We can use [libuv](http://libuv.org/) to handle network programming and events. Its probably more sane than re-inventing the wheel again.
- Using `stdbool.h` may not be the wisest idea...
