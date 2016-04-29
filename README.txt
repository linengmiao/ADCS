This code runs on the cortex-M4 microcontroller (TM4C1294NCPDT). It reads the value of different sensors
via i2c and uses the TLE (in combination with the 4th order Runge Kutta algorithm) to calculate the attitude (aka orientation)
matrices and quaterions that define the orientation of the spacecraft. The implemented algorithms for
the calculation of the attitude are QUEST and TRIAD. The Q-davenport algorithm is implemented 
in matlab. 

Next to this the b-dot detumbling algorithm was implemented. It uses data received from the GUI via UART to
determine the needed PWM dutycycle for the magnetorquers in order to make itself detumble.