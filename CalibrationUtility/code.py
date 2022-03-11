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

# setup
i2c = board.I2C()


# main loop

while not i2c.try_lock():
    pass

try:
    while True:
        print(
            "I2C addresses found:",
            [hex(device_address) for device_address in i2c.scan()],
        )
        time.sleep(2)

finally:  # unlock the i2c bus when ctrl-c'ing out of the loop
    i2c.unlock()