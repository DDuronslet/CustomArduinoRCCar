# CustomArduinoRCCar
Custom RC Car built with an arduino, controlled by a PS4 controller.

Transmitter built using an arduino uno, nrf24l01+ with a 3.3 voltage regulator, 9V battery with switch, and 5K potentiometer.
Reciever built with a 16hz arduino pro micro(uploaded as an arduino leonardo eth),  nrf24l01+ with a 3.3 voltage regulator, 9V battery powering the arduino, and a on board Li-ion battery to power the motor driver, dc motor, servo(buck boost set to 5V).

Motor Driver: TB9051FTG Single Brushed DC Motor Driver Carrier https://www.pololu.com/product/2997
The servo and dc motor were already on board the original RC car which the original reciever was removed.
