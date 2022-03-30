# SPDX-FileCopyrightText: 2018 Kattni Rembor for Adafruit Industries
# Mods by Carson Elmore 2022
# SPDX-License-Identifier: MIT

"""CircuitPython Essentials PWM pin identifying script"""
import board
import pwmio

for pin_name1 in dir(board):
    pin1 = getattr(board, pin_name1)
    for pin_name2 in dir(board):
        pin2 = getattr(board, pin_name2)
        try:
            p1 = pwmio.PWMOut(pin1)
            p2 = pwmio.PWMOut(pin2)
            p2.deinit()
            print("PWM on:", pin_name2, " with ", pin_name1)  # Prints the valid, PWM-capable pins!
        except ValueError:  # This is the error returned when the pin is invalid.
            pass
            #print("No PWM on:", pin_name2, " with ", pin_name1)  # Prints the invalid pins.
        except RuntimeError:  # Timer conflict error.
            #pass
            print("Timers in use:", pin_name2, " with ", pin_name1)  # Prints the timer conflict pins.
        except TypeError:  # Error returned when checking a non-pin object in dir(board).
            pass  # Passes over non-pin objects in dir(board).