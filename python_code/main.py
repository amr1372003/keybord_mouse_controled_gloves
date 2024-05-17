import serial
import pyautogui

pyautogui.FAILSAFE = False

ser = serial.Serial('COM6', baudrate=9600, timeout=1)  # open serial port

while True:
    line = ser.readline().decode('utf-8').strip()
    print(line)
    if line.startswith('AccX:') and 'AccY:' in line:
        values = line.split(", ")
        accX = float(values[0].split(':')[1])
        accY = float(values[1].split(':')[1])
        sensitivity = 15 # movment sensitivity
        pyautogui.move(accX * sensitivity, accY * sensitivity) # Move the mouse
        if line.startswith('AccX:') and 'AccY:' in line and 'clk:' in line:
            values = line.split(", ")
            click = values[2].split(':')[1]
            if click == 'R':
                pyautogui.click(button = 'right')
            elif click == 'L':
                pyautogui.click(button = 'left')
            elif click == 'D':
                pyautogui.click(clicks = 2)
    else:
        if line and (line[0] == "^"):
            pyautogui.press('enter')
        elif line and line[0] == "<":
            pyautogui.press('backspace')
        elif line and (len(line) == 6):
            pyautogui.write(line[0])
        '''
        
        '''