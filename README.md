# Task Scheduler Demo for NUCLEO-L432KC
A simple demo project to show basic capabilities of the [arm task scheduler](https://github.com/eardali/arm-task-scheduler).  

# Notes
- Project based on STM32 CUBE IDE and SDK, tested on NUCLEO-L432KC board.  
- It demonstrates some basic features of task scheduler such as task creation, scheduling a task to start at defined time, updating task period by changing return value of the task itself, stopping a task by another task.  
## Demo Details
- Clone repository under ST SDK (`/path/to/sdk/STM32Cube_FW_L4_V1.18.0/Projects/NUCLEO-L432KC/`), this because given project template in SDK implies relative path for SDK source and headers.  
- Also clone `arm task scheduler` submodule.  
`git submodule update --init --recursive`
- Within CUBE IDE `File` -> `Open Projects from File System` ->  `Import Source` at `../task-scheduler-demo/STM32CubeIDE/`.  
- Main demonstration code under `../task-scheduler-demo/tasks` folder.  
- Basically, there are two tasks. One is the main led blink task (`taskLed()`) which also schedules another task to stop itself (`taskLedStop()`).  
    - `taskLed()` starts after 5sec, and blinks led every 1sec period, after 10 blinks period changes 5sec.  
    - `taskLedStop()` is scheduled to run at 55th sec (relative to `taskLed()` starting time), it runs once and stops led blinking task.  
    - `taskInfo()` prints some basic information about OS state for demontration purpose.  
- `printf()` output redirected to serial port, details can be found under `../task-scheduler-demo/Src/nucleo_l432_uart_debug.c`.  
- Example print out of demo code (`taskInfo()`) given below.  
```
-----Starting OS Task Execution-----
OS time: 2000
Task state: 1
Task period: 1000 msec
Task Ptr: 0x8003f7d, next execution time: 5000 msec
Task Ptr: 0x8003f7d (which creates Task Ptr: 0x8003fd5) is not executed yet
Second call scheduled to check task status later
OS time: 8000
Task state: 2
Task period: 1000 msec
Task Ptr: 0x8003f7d, next execution time: 9000 msec
Task Ptr: 0x8003fd5, next execution time: 60000 msec
Remaning time for the task (0x8003fd5): 52000 msec
``` 

# References 
- [STM32 Cube IDE](https://www.st.com/en/development-tools/stm32cubeide.html) V1.18.  
- [STM32L4 SDK](https://www.st.com/en/embedded-software/stm32cubel4.html) V1.18.0.  
- [NUCLEO-L432KC](https://www.st.com/en/evaluation-tools/nucleo-l432kc.html) development board which uses [STM32L432KC MCU](https://www.st.com/en/microcontrollers-microprocessors/stm32l432kc.html).  
- Origin repo at github.com/eardali.  

# License
Distributed under the terms of the BSD 3-Clause license.  
