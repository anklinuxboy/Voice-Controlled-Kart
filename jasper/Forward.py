# Prepared by Team BlueGum
# Juan Jhong Chung, Laura Kamfonik & Ankit Sharma
# EC535 Fall 2014 Final Project

import random
import re
import bluetooth
from semantic.numbers import NumberService
import time
import BlueGlobals

# List of commands accepted by Jasper
WORDS = ["STOP", "FORWARD", "REVERSE", "RIGHT", "LEFT", "FAST", "SLOW", "DANCE", "FIVE", "TEN"]

# This is the feedback we will hear from Jasper
MESSAGE = [     "STOPPING",
                "MOVING FORWARD",
                "MOVING BACKWARD",
                "TURNING RIGHT",
                "TURNING LEFT",
                "CHANGING SPEED TO FAST",
                "CHANGING SPEED TO SLOW",
                "YIPPEEEEEEE!",
                "CHANGING MOVE TIME TO 5 SECONDS",
                "CHANGING MOVE TIME TO 10 SECONDS" ]

def handle(text, mic, profile):
    """
        Responds to user-input, typically speech text, by relaying the
        meaning of life.

        Arguments:
        text -- user-input, typically transcribed speech
        mic -- used to interact with the user (for both input and output)
        profile -- contains information related to the user (e.g., phone
                   number)
    """

    words_heard = text.split()
    message = "CAN YOU REPEAT THAT?"
    btnum = "0"

    for i in range(len(words_heard)):
        for j in range (len(WORDS)):
                if (words_heard[i] == WORDS[j]):
                        message = MESSAGE[j]
 			btnum = str(j)
                        break
        if (message != "CAN YOU REPEAT THAT?"):
                break

    mic.say (message)
    print message
    print "tx_init: %d" % (BlueGlobals.tx_init)
    print "rx_init: %d" % (BlueGlobals.rx_init)

#If a Bluetooth connection has not been established. Connect now.
    if(BlueGlobals.tx_init == 0):
        BlueGlobals.inittx()

# Send the number associated with the command to the Gumstix
    BlueGlobals.BlueSend(btnum)
# Attempt at receiving message back from Gumstix
    #time.sleep(3)
    #data = BlueGlobals.BlueReceive()
    #mic.say(data)


def isValid(text):
    """
        Returns True if the input is related to the meaning of life.

        Arguments:
        text -- user-input, typically transcribed speech
    """
    return bool(re.search(r'\b(forward|reverse|left|right|stop|fast|slow|dance|five|ten)\b', text, re.IGNORECASE))
