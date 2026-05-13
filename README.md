# S29GL064S NOR Flash Family Arduino Interface Library

### Library structure

#### async_parallel_bus_HAL base class
This class defines a abstracted simple software interface for communicating with
devices along the standard asynchronous parallel interface that many NOR flash 
and SRAM devices utilize.

#### S29GL064S interface class
This is the derived class that users will actually instantiate in Arduino
sketches. It defines the specific commands that the S29GL064S IC family utilizes
for flash operations (program, erase, status register access).
<mark>Not all S29GL064S commands have been implemented in this class!</mark>

### Microcontroller pin assignments
This library utilizes C++ vectors for storing the address and data pins of the 
Arduino DUE microcontroller. These assignments are passed into the class 
instatiation using the parameterized constructor inherited from the 
async_parallel_bus_HAL base class.

### Arduino support targets
- Arduino DUE (AT91SAM3X8E)

More Arduino devices could be implemented with updating the digitalWriteFast() 
macro implementation for additional devices.
