# Servo Calibration Utility for SPOILER ALERT
# Formula SAE Interdisciplinary Design project
# Oklahoma State University College of Engineering Architecture and Technology
# Initial Commit: 2/28/2022 Carson Elmore
# Document State: 1.1
# Language: CircuitPython 7
# Board: ItsyBitsy M0
#

import time
import board
import pwmio
from adafruit_motor import servo
import usb_cdc
import digitalio

# setup
i2c = board.I2C()
GY521 = 0x68 # I2C DEVICE ADDRESS

pwm1 = pwmio.PWMOut(board.D11, duty_cycle = 2**15, frequency = 50)
servo1 = servo.Servo(pwm1)
pwm2 = pwmio.PWMOut(board.A3, duty_cycle = 2**15, frequency = 50)
servo2 = servo.Servo(pwm2)
pwm3 = pwmio.PWMOut(board.A4, duty_cycle = 2**15, frequency = 50)
servo3 = servo.Servo(pwm3)
pwm4 = pwmio.PWMOut(board.A2, duty_cycle = 2**15, frequency = 50)
servo4 = servo.Servo(pwm4)
pwm5 = pwmio.PWMOut(board.D9, duty_cycle = 2**15, frequency = 50)
servo5 = servo.Servo(pwm5)

modeA = digitalio.DigitalInOut(board.A1)
modeA.direction = digitalio.Direction.INPUT
modeA.pull = digitalio.Pull.UP
modeB = digitalio.DigitalInOut(board.D10)
modeB.direction = digitalio.Direction.INPUT
modeB.pull = digitalio.Pull.UP
lowDrag = digitalio.DigitalInOut(board.D7)
lowDrag.direction = digitalio.Direction.INPUT
lowDrag.pull = digitalio.Pull.UP
brakeSensor = digitalio.DigitalInOut(board.D2)
brakeSensor.direction = digitalio.Direction.INPUT
modeA.pull = digitalio.Pull.DOWN

# main loop
ready = True;
while True:
    if ready:
        print("Enter servo number:desired angle, eg 2:045, or STOP!")
    command = usb_cdc.console.read(5)
    if command is None:
        continue
    if "STOP" in command:
        break
    whichServo = int(command[0:1])
    print("Parse: servo ", whichServo, "\n       Angle ", int(command[2:5]))
    try:
        if whichServo is 1:
            servo1.angle = int(command[2:5])
            #pwm1.deinit()
        elif whichServo is 2:
            servo2.angle = int(command[2:5])
            #pwm2.deinit()
        elif whichServo is 3:
            servo3.angle = int(command[2:5])
            #pwm3.deinit()
        elif whichServo is 4:
            servo4.angle = int(command[2:5])
            #pwm4.deinit()
        elif whichServo is 5:
            servo5.angle = int(command[2:5])
            #pwm5.deinit()
        else:
            print("Command not parseable. To stop, enter STOP.")
    except ValueError as err:
        print(command, " is a number out of range")
        print(err)
    
    

# # while not i2c.try_lock():
    # # pass

# # try:
    # # while True:
        # # print(
            # # "I2C addresses found:",
            # # [hex(device_address) for device_address in i2c.scan()],
        # # )
        # # sleep(2)

# # finally:  # unlock the i2c bus when ctrl-c'ing out of the loop
    # # i2c.unlock()
while True:
    if ready:
        print("Poll Sensors?")
    command = usb_cdc.console.read(1)
    print("Mode A: ", modeA.value)
    print("Mode B: ", modeB.value)
    print("LowDrag:", lowDrag.value)
    print("Brake : ", brakeSensor.value)
    if "!" in command:
        break
    
i2c.unlock()