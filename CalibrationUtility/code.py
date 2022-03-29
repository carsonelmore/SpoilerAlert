# Servo Calibration Utility for SPOILER ALERT
# Formula SAE Interdisciplinary Design project
# Oklahoma State University College of Engineering Architecture and Technology
# Initial Commit: 2/28/2022 Carson Elmore
# Document State: PRE-ALPHA
# Language: CircuitPython 4
# Board: ItsyBitsy M0
#
# Current stage: Adaptation of Itsy Bitsy M0 Express IO demo
# Welcome to CircuitPython 4 :)

import time
import board
import pwmio
from adafruit_motor import servo

# setup
i2c = board.I2C()
GY521 = 0x68 # I2C DEVICE ADDRESS

pwm1 = pwmio.PWMOut(board.D11, duty_cycle = 2**15, frequency = 50)
pwm2 = pwmio.PWMOut(board.D10, duty_cycle = 2**15, frequency = 50)
pwm3 = pwmio.PWMOut(board.A1, duty_cycle = 2**15, frequency = 50)
pwm4 = pwmio.PWMOut(board.A2, duty_cycle = 2**15, frequency = 50)
pwm5 = pwmio.PWMOut(board.D9, duty_cycle = 2**15, frequency = 50)

servo1 = servo.Servo(pwm1)
servo2 = servo.Servo(pwm2)
servo3 = servo.Servo(pwm3)
servo4 = servo.Servo(pwm4)
servo5 = servo.Servo(pwm5)


# main loop
ready = True;
while True:
    if ready:
        print("Enter servo number:desired angle, eg 2:045")
    command = read(5)
    if command is None:
        continue
    if(command = "STOP":
        break
    servo = int(command[0:1])
    match servo:
        case 1: 
            servo1.angle = int(command[2:5])
        case 2: 
            servo2.angle = int(command[2:5])
        case 3: 
            servo3.angle = int(command[2:5])
        case 4: 
            servo4.angle = int(command[2:5])
        case 5: 
            servo5.angle = int(command[2:5])
        case _:
            print("Command not parseable. To stop, enter STOP.")
    
    

# while not i2c.try_lock():
    # pass

# try:
    # while True:
        # print(
            # "I2C addresses found:",
            # [hex(device_address) for device_address in i2c.scan()],
        # )
        # sleep(2)

# finally:  # unlock the i2c bus when ctrl-c'ing out of the loop
    # i2c.unlock()