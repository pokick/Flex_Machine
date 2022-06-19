import tkinter as Tk
import tkinter.ttk as Ttk
from tkinter import  Label ,Spinbox ,Toplevel,Entry, messagebox, Radiobutton, Frame,Text
from tkinter.ttk import Combobox
from pynput.keyboard import Key,Controller
import tkinter.font as font
import time
import serial
from threading import Thread
import string

sampleSize = "0"

cycles = "0"






def main():
    
    def enable():
        if messagebox.askokcancel("Enable"," Czy chcesz odblokowac silniki ? "):
            
            ser.write(str('arm\n').encode("utf-8"))

    def homeSteppers():
        if messagebox.askokcancel("HOME"," CZY JESTES PEWIEN?"):
            ser.write(str('home_step\n').encode("utf-8"))
            
    def disable():
        if messagebox.askokcancel("Disable"," Czy chcesz zablokowac silniki ? "):
            ser.write(str('disarm\n').encode("utf-8"))

    def sample():
        if messagebox.askokcancel("Probka"," Czy chcesz umiescic probke o rozmiarze "+ sampleSizeEntry.get() + "?"):
            sampleStr = 'sample ' + sampleSizeEntry.get() + '\n'
            ser.write(sampleStr.encode("utf-8"))
    def start():
        if messagebox.askokcancel("Rozpocznij test",
                                  " Rozpoczynam test z probka o dlugiosci "+ sampleSizeEntry.get() + " liczba powtorzen "+ cyclesEntry.get() + " promien giecia " +radiusEntry.get() ):
            startStr = 'test_sample ' + sampleSizeEntry.get() + ' ' + radiusEntry.get() + ' ' + delayEntry.get() + ' ' + cyclesEntry.get()+' ' + flexType.get() + '\n'
            ser.write(startStr.encode("utf-8"))

    ser = serial.Serial(
        port='/dev/ttyS0',
        baudrate = 115200,
        parity = serial.PARITY_NONE,
        stopbits = serial.STOPBITS_ONE,
        bytesize = serial.EIGHTBITS,
        timeout = 1
        )
    
    root = Tk.Tk()
    root.geometry("800x480")
    root.title("FLEX")
        
    fontButton = font.Font(family='Helvetica',size=36,weight='bold')
    fontLabel = font.Font(family='Helvetica',size=20,weight='bold')
    fontNumpad = font.Font(family='Helvetica',size=35,weight='bold')
    

    #guzik arm home disarm
    enable = Tk.Button(text='Enable', command = enable, padx = 23,pady = 30)
    enable.grid(row=0,column=0)
    
    home = Tk.Button(text='HOME', command = homeSteppers,padx = 26,pady = 30)
    home.grid(row=0,column=1)
 
    disarm = Tk.Button(text='Disable', command = disable,padx = 22,pady = 30)
    disarm.grid(row=0,column=2)
    
    sample = Tk.Button(text='Probka', command = sample,padx = 22,pady = 30)
    sample.grid(row=0,column=3)
    
    start = Tk.Button(text='START',  command = start,padx = 24,pady = 33)
    start.grid(row=1,column=3)
    
    
    # labelka flex
    flexType = Tk.StringVar()
    R1 = Radiobutton(root,text="w dol",variable = flexType,value = "1", height =5,width =10,indicatoron=0)
    R1.grid(row=1,column=0)
    R2 = Radiobutton(root,text="w gore",variable = flexType,value = "2", height =5,width =10,indicatoron=0)
    R2.grid(row=1,column=1)
    R2 = Radiobutton(root,text="obustronnie",variable = flexType,value = "3", height =5,width =10,indicatoron=0)
    R2.grid(row=1,column=2)
    
    
    
    sampleSizeLabel = Label(text="Rozmiar probki [mm]")
    sampleSizeLabel.grid(row=3,column=0,columnspan=2)
    
    sampleSizeEntry = Entry(font = fontButton,width =7 )
    sampleSizeEntry.grid(row=4,column=0,columnspan=2)
    
    

    cyclesLabel = Label(text="Liczba powtorzen")
    cyclesLabel.grid(row=3,column=2,columnspan=2)
    
    cyclesEntry = Entry(font = fontButton,width =7 )
    cyclesEntry.grid(row=4,column=2,columnspan=2)
    
    delayLabel = Label(text="Opoznienie [ms]")
    delayLabel.grid(row=7,column=0,columnspan=2)
    
    delayEntry = Entry(font = fontButton,width =7 )
    delayEntry.grid(row=8,column=0,columnspan=2)
    
    radiusLabel = Label(text="Promien Giecia [mm]")
    radiusLabel.grid(row=7,column=2,columnspan=2)
    
    radiusEntry = Entry(font = fontButton,width =7 )
    radiusEntry.grid(row=8,column=2,columnspan=2)
    
    rightFrame = Frame(root, width = 700, height =500)
    rightFrame.grid(row = 0,column =5,rowspan=15,columnspan = 15,padx=40,pady=5)
    
    def press(num):
        keyboard.press(num)
        keyboard.release(num)
        print(num)
    def pressX():
        keyboard.press(Key.backspace)
        keyboard.release(Key.backspace)
     
    keyboard = Controller()
    _padx=40
    _pady=3
    
  
       
            
    button7 = Tk.Button(rightFrame,text='7',font = fontNumpad ,padx=_padx,pady=_pady,command = lambda : press('7'))
    button7.grid(row =0, column =5)
            
    button8 = Tk.Button(rightFrame,text='8',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('8') )
    button8.grid(row =0, column =6)
        
    button9 = Tk.Button(rightFrame,text='9',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('9'))
    button9.grid(row =0, column =7)
            
    button4 = Tk.Button(rightFrame,text='4',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('4') )
    button4.grid(row =1, column =5)
            
    button5 = Tk.Button(rightFrame,text='5',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('5') )
    button5.grid(row =1, column =6)
            
    button6 = Tk.Button(rightFrame,text='6',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('6'))
    button6.grid(row =1, column =7)
            
    button1 = Tk.Button(rightFrame,text='1',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('1'))
    button1.grid(row =2, column =5)
            
    button2 = Tk.Button(rightFrame,text='2',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('2'))
    button2.grid(row =2, column =6)
            
    button3= Tk.Button(rightFrame,text='3',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('3'))
    button3.grid(row =2, column =7)
            
    button0 = Tk.Button(rightFrame,text='0',font = fontNumpad,padx=_padx,pady=_pady,command = lambda : press('0'))
    button0.grid(row =3, column=5)
              
    buttonX = Tk.Button(rightFrame,text='X',font = fontNumpad,padx=_padx-3,pady=_pady,command =pressX)
    buttonX.grid(row =3, column =7 )
        
    serialInOut = Text(rightFrame,width = 43, height = 10)
    serialInOut.grid(row=4,column=0,rowspan=15,columnspan=15)
    
    

    def readSerial():
        
        while True:
            inp = ser.readline()
            if inp != b'':
                
                print(inp.decode())
                inp = inp[:-2]
                serialInOut.insert('end',inp.decode()+'\n')
                serialInOut.see("end") 
 
               
        
    th= Thread(target=readSerial)
    th.daemon = True
    th.start()    
  
   
    
    root.mainloop()
    
    
    

    
    
    
    
if __name__ == '__main__':
    main()


    