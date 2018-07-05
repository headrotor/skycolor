# Simple demo of reading color data with the TCS34725 sensor.
# Will read the color from the sensor and print it out along with lux and
# color temperature.
# Author: Tony DiCola
# License: Public Domain
import time
import datetime

# Import the TCS34725 module.
import Adafruit_TCS34725

#if i2c is None:
import Adafruit_GPIO.I2C as i2c
#i2c = I2C

#import smbus



chan = 0xFF
mplex = i2c.get_i2c_device(0x70,1)
mplex.writeRaw8(0x02)
#chan = mplex.readRawU8(0x00)
chan = mplex.readRaw8()
print chan


#col = i2c.get_i2c_device(0x29, 1)
#chip_id = col.readU8(Adafruit_TCS34725.TCS34725_ID)
#print chip_id

#while True:
#    r = col.readU16LE(Adafruit_TCS34725.TCS34725_RDATAL)
#    print r
#    time.sleep(1)
    
#bus = smbus.SMBus(1)
#bus.write_byte(0x70,0x00)
#chan = bus.read_byte(0x70)


#long read_byte_data(int addr,char cmd)
#ID = bus.read_byte_data(0x29, 0x12)
#bus.write_byte(0x29,0x12)
#ID = bus.read_byte(0x29)

#print ID

print "OK!"





# class multiplex:

#     def __init__(self, bus=1):
#         self.bus = smbus.SMBus(bus)
#         self.chan = None
        
#     def set_channel(self, address=0x70,channel=0):  
#         self.bus.write_byte(address,channel)  
#         self.chan = self.bus.read_byte(0x70)
#         print self.chan
        
# create a logfile with current date and time

dest = "./gain_4x/"
fname = dest + "c" + datetime.datetime.now().strftime("%y-%m-%d-%I_%M") + ".csv"

logfile = open(fname,"w")
logfile.write('started ' + datetime.datetime.now().strftime("%y-%m-%d %I:%M:%S\n"))

logfile.close()
print("log file: " + fname)

# Create a TCS34725 instance with default integration time (2.4ms) and gain (4x).

#tcs = Adafruit_TCS34725.TCS34725()

# You can also override the I2C device address and/or bus with parameters:
#tcs = Adafruit_TCS34725.TCS34725(address=0x30, busnum=2)

#tcs = Adafruit_TCS34725.TCS34725(
#    integration_time=Adafruit_TCS34725.TCS34725_INTEGRATIONTIME_154MS,
#    gain=Adafruit_TCS34725.TCS34725_GAIN_1X)



#mplex = multiplex()

#mplex.set_channel(0)


# Or you can change the integration time and/or gain:
tcs = Adafruit_TCS34725.TCS34725(
    integration_time=Adafruit_TCS34725.TCS34725_INTEGRATIONTIME_154MS,
    gain=Adafruit_TCS34725.TCS34725_GAIN_4X)


# Possible integration time values:
#  - TCS34725_INTEGRATIONTIME_2_4MS  (2.4ms, default)
#  - TCS34725_INTEGRATIONTIME_24MS
#  - TCS34725_INTEGRATIONTIME_50MS
#  - TCS34725_INTEGRATIONTIME_101MS
#  - TCS34725_INTEGRATIONTIME_154MS
#  - TCS34725_INTEGRATIONTIME_700MS
# Possible gain values:
#  - TCS34725_GAIN_1X
#  - TCS34725_GAIN_4X
#  - TCS34725_GAIN_16X
#  - TCS34725_GAIN_60X

# Disable interrupts (can enable them by passing true, see the set_interrupt_limits function too).
tcs.set_interrupt(False)


while True:
    # Read the R, G, B, C color data.

    
    #mplex.set_channel(0x04)

    r, g, b, c = tcs.get_raw_data()

    # Calculate color temperature using utility functions.  You might also want to
    # check out the colormath library for much more complete/accurate color functions.
    #color_temp = Adafruit_TCS34725.calculate_color_temperature(r, g, b)

    # Calculate lux with another utility function.
    #lux = Adafruit_TCS34725.calculate_lux(r, g, b)

    # Print out the values.
    #print('Color: red={0} green={1} blue={2} clear={3}'.format(r, g, b, c))
    print('{4},{0},{1},{2},{3}'.format(r, g, b, c, time.time()))

    # try:
    #     logfile = open(fname,"a")
    #     logfile.write('{4},{0},{1},{2},{3}\n'.format(r, g, b, c, time.time()))
    #     logfile.close()
    # except:
    #     raise
        
    time.sleep(1.)


# Print out color temperature.
if color_temp is None:
    print('Too dark to determine color temperature!')
else:
    print('Color Temperature: {0} K'.format(color_temp))

# Print out the lux.
print('Luminosity: {0} lux'.format(lux))

# Enable interrupts and put the chip back to low power sleep/disabled.
tcs.set_interrupt(True)
tcs.disable()
