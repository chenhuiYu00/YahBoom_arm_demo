import time
from Arm_Lib import Arm_Device

Arm = Arm_Device()
time.sleep(.1)
is_running = True
def action(mode,action=4,angles=[]):
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
    time.sleep(1)
    for i in range(3):
        Arm.Arm_serial_servo_write6(90, 120, 120, 70, 180, 180, 500)
        time.sleep(1)
        Arm.Arm_serial_servo_write6(90, 120, 120, 70, 0, 180, 500)
        time.sleep(.5)
        Arm.Arm_serial_servo_write6(90, 120, 120, 70, 90, 180, 500)
        time.sleep(.5)


def dance():
    Arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 180, 500)
    time.sleep(1)
    for i in range(3):
        # Control the up and down operation of No. 3 and No. 4 steering gear
        # 控制3号和4号舵机上下运行
        Arm.Arm_serial_servo_write(1, 180, 500)
        time.sleep(.5)
        Arm.Arm_serial_servo_write(3, 0, 1000)
        time.sleep(.5)
        Arm.Arm_serial_servo_write(5, 0, 1000)
        time.sleep(.5)
        Arm.Arm_serial_servo_write(6, 180, 1000)
        time.sleep(.5)
        Arm.Arm_serial_servo_write(1, 0, 500)
        time.sleep(.001)
        Arm.Arm_serial_servo_write(6, 0, 1000)
        time.sleep(.001)
        Arm.Arm_serial_servo_write(4, 180, 1000)
        time.sleep(1)
        Arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 180, 1000)
        time.sleep(1.5)
