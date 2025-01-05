import time
from Arm_Lib import Arm_Device

Arm = Arm_Device()
time.sleep(.1)
global isRunning

def action(mode,action=4,angles=[]):
    isRunning = True
    if mode==1:
        if action == 0:
            clap()
        elif action == 1:
            nod()
        elif action == 2:
            hope()
        elif action == 3:
            dance()
        elif action == 4:
            origin()
        elif action == 5:
            xia()
        elif action == 6:
            rock()
        elif action == 7:
            wait()
        elif action == 8:
            run()
        elif action == 9:
            walk()
    if mode==0:
        time.sleep(.2)
        Arm.Arm_serial_servo_write6(*angles,500)
        
    isRunning = False


def clap():
    Arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)
    time.sleep(1)
    Arm.Arm_serial_servo_write6(90, 90, 120, 120, 90, 90, 500)
    time.sleep(1)
    for i in range(3):
        Arm.Arm_serial_servo_write(6, 0, 500)
        time.sleep(1)        
        Arm.Arm_serial_servo_write(6,180, 500)
        time.sleep(1)

def nod():
    for i in range(3):
        Arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)
        time.sleep(1)
        Arm.Arm_serial_servo_write6(90, 90, 150, 90, 90, 90, 500)
        time.sleep(1)
        Arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)

def origin():
    Arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)

def rock():
    for i in range(3):
        Arm.Arm_serial_servo_write6(90, 160, 90, 90, 90, 90, 500)
        time.sleep(2)
#         Arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)
#         time.sleep(1)

def wait():
    Arm.Arm_serial_servo_write6(90, 90, 0, 90, 90, 90, 500)
    time.sleep(1)
    Arm.Arm_serial_servo_write6(90, 90, 0, 0, 90, 90, 500)
    time.sleep(1)

def run():
    for i in range(5):
        Arm.Arm_serial_servo_write6(90, 90, 20, 0, 90, 90, 500)
        time.sleep(1)
        Arm.Arm_serial_servo_write6(90, 90, 20, 60, 90, 90, 500)
        time.sleep(1)

def walk():
    for i in range(5):
        Arm.Arm_serial_servo_write6(90, 90, 20, 0, 90, 90, 700)
        time.sleep(1.5)
        Arm.Arm_serial_servo_write6(90, 90, 20, 60, 90, 90, 700)
        time.sleep(1.5)

def hope():
    Arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)
    time.sleep(1)
    Arm.Arm_serial_servo_write6(90, 90, 90, 120, 90, 90, 500)
    time.sleep(1)
    Arm.Arm_serial_servo_write6(90, 90, 90, 160, 90, 180, 500)
    time.sleep(1)

def xia():
    Arm.Arm_serial_servo_write6(90, 120, 120, 70, 90, 90, 500)

def dance():
    Arm.Arm_serial_servo_write6(90, 90, 0, 90, 90, 90, 500)
    time.sleep(1)
    for i in range(3):
        Arm.Arm_serial_servo_write6(90, 60, 60, 0, 90, 90, 500)
        time.sleep(1)
        Arm.Arm_serial_servo_write6(90, 120, 120, 60, 90, 90, 1000)
        time.sleep(1)
        Arm.Arm_serial_servo_write6(90, 70, 20, 0, 120, 90, 500)
        time.sleep(1)
        Arm.Arm_serial_servo_write6(90, 120, 20, 60, 60, 90, 500)
        time.sleep(1)