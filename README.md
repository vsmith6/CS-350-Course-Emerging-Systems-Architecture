# CS-350-Course-Emerging-Systems-Architecture
#   
```
This project introduced embedded programming skills and included the use of a microprocessor board, the CS3220S from Texas Instruments.  
In this course we utilized the C++ programming language with Code Composer Studio in order to further develop programs that would run
on the board.  This repository contains examples of the work that was completed in order to work with timers and interrupts based upon
a button press on the board. 

 The first project is a prelude to the final project and in this one we worked on setting up a timer and the interrupts necessary
 to get the board to flash a SOS signal.  The SOS blinking functionality is programmed with a state machine.  The second project 
 introduces further concepts, such as a schedule, that will evoke certain events based upon what state the timer is in.  The timer
 will invoke a task at 100, 500, and 1000 ms.  Based upon the concept of a smart thermometer, the program will allow a setpoint temperature.
 The temperature sensor on the board will monitor the ambient air temperature and if the temperature falls below the setpoint, an 
 indicator light will signal on to indicate that the system should be “heating”. 
 
 As far as what I did well, I felt that I did well reaching outside of my comfort zone in order to provide a solution that I have 
 never had experience with before.  I feel that the solutions I provided are working and that it was straightforward to get into to 
 debug. I felt that I did a pretty good job understanding the state machine in the first example.  I could have done a better job with
 the state machine in the final example in order to provide a more elegant solution.
 
 I found that there are plentiful resources for this type of programming and know that if I would ever choose to take this path of 
 computer science that they are there. The documentation for the architecture of the various tools, such as the Timer and the GPIO, 
 were thorough. There is a lot of existing documentation for this particular microprocessor board as well. The tools and resources 
 that I will be adding skill set are further development with the C++ programming language, more hands on work with object oriented 
 programming, and an extensive use and understanding of the debugger.  
 
In order to make this project maintainable and readable, I ensured that I followed recommended C++ programming guidelines and used 
code comments.  A lot of the work in the first example could be reused in the final example. Both projects could easily adapt to further use cases. 
```
