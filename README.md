# Tricorder
> Like the one on StarTrek but not as cool

## What is a tricorder?
The tricorder is a device that is used to inspect unfamiliar surroundings used by the Starfleet in the Star Trek universe. The name tricorder is an abbreviation for "tri-function recorder" as the device performs three operations, sensing, computing and recording according to the Voyager Technical Manual. There are more special-purpose tricorders such as the medical tricorder which performs the same tasks as a diagnostic scanner, which takes readings from sensors attached to the patient, that are found in sick bays.

## Our tricorder
We can read altitude, pressure and temperature through a BMP280 sensor which communicates using I2C. Also on the I2C bus, we also have the 16x2 LCD with the Adafruit LCD backpack which gives it the I2C capabilities. We initially wanted to go with the Nokia 3310 screen using SPI but pins were scarce on the Metro Mini. We also have the EM-506 GPS which communicates using serial, this provides both location in the form of longitude and latitude and the time.

The user and interact with the device using the up, down and select button. There is also a large green LED button the side that switches the tricorder on or off, when on the LED lights up. The device is powered from the 9V battery placed in the lower compartment. 