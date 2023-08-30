# detector-building
Arduino code written for the Detector Building event (sub-contest) in the Science Olympiad competition.

Each file in the repository is a standalone C++ Arduino program run on an Arduino Uno board to create a detector for measuring a physical quantity - temperature or mass. The code works in conjunction having a circuit with a resistor capable of varying its resistance when the physical quantity in question also varies. For temperature, a thermistor was used as the variable resistor. For mass, a force-sensitive resistor was used first, then changed to a load cell with strain guages as the variabel resistors. 

The temperature and force-sensitive resistor detector projects utilized a voltage divider circuit to measure the value of the variable resistor. The load cell project utilized a NAU7802 ADC and low-noise gain amplifier to take readings of the variable resistor.
