# -*- coding: utf-8 -*-

import smbus
import time
import sys
import json
from datetime import datetime
import os
import requests

<<<<<<< HEAD
# # for RPI version 1, use bus = smbus.SMBus(0)
bus = smbus.SMBus(1)
=======
# # for RPI version 1, use �bus = smbus.SMBus(0)�
# bus = smbus.SMBus(1)
>>>>>>> 17c753a50977b5fadc2bb8d207255a264d193c35

# # This is the address we setup in the Arduino Program
arduino = 0x40
number1 = 0;

# testsrv = "https://hookb.in/ZdNM1x1P"
#domain = "http://ptyxiakiweb/"
domain = "http://192.168.2.7/ptyxiaki/"
testsrv = "dbWorks.php"
connectivity = "chkConnection.php"
interval = 5


def writeNumber(value):
    bus.write_byte(arduino, value)
    bus.write_byte_data(arduino, 0, value)
    return -1

def readNumber():
    data_received_from_Arduino = bus.read_i2c_block_data(arduino,1)
    c = data_received_from_Arduino[0]
    # number = bus.read_byte_data(arduino, 1)
    return c
    
def readMessageFromArduino():
    # retval = 0
    try:
        data_received_from_Arduino = bus.read_i2c_block_data(arduino,1)
        smsMessage=""
        for x in range(len(data_received_from_Arduino)):
            if data_received_from_Arduino[x] == 255:
                break
                
            c = chr(data_received_from_Arduino[x])
            smsMessage += c
           # intMess += data_received_from_Arduino[i]
            # retval = smsMessage
        print(smsMessage)
            # print(str(data_received_from_Arduino[x]).encode('utf-8').strip())
        # print(data_received_from_Arduino)
        data_received_from_Arduino = ""
        # smsMessage = ""
    
    except:
         print("bla")
         print("ERROR_!: " + str(sys.exc_info()[0]))
         smsMessage=""
         retval = 1
    return smsMessage
    
def writeMessageToArduino(value,isInt):
    retval=0
    strVal = str(value)
    if len(strVal)>30:
        print("Error: Overflow error. Trying to send " + str(len(strVal)) + " bytes. Only 30 allowed. \n    Value: "+strVal)
        exit()

    var=[];
    for c in strVal:
        var.append(ord(c))
        
    print(str(var))
        
    try:
        bus.write_block_data(arduino,isInt,var)
    except:
        print("ERROR: " + str(sys.exc_info()[0]))
        retval = 1
    return retval
    
def arrayToJson(arrayX,ts):
    sensors=[]
    for x in arrayX:
        name,value=x.split("=")
        datoX = {}
        datoX["name"]=name
        datoX["data"]=value.split(",")
        sensors.append(datoX)
    print(sensors)
    jsonX = json.dumps({"ARID":"dasdasa","time":ts,"sensors":sensors})
    print(jsonX)
    res = post_data(testsrv, jsonX)
    
def JsonToData(dato):
    # json_str = json.dumps(dato)
    print ("\njson_str: \n" + dato)
    data = json.loads(dato)
    # print (data)
    return data
    
def post_data(url, mydata):
    print("\n\n"+"-"*50)
    print("\nPosting data to: " + domain)
    print("\nEstablishing connection..")
    try:
        r = requests.get(domain+connectivity)
        print("Success! ["+str(r.status_code) + "] " + r.reason)
    except:
        print("Cannot establish connection!")
        print("\nERROR: " + str(sys.exc_info()[0]))
        print("INFO : " + str(sys.exc_info()[1])+"\n")
        print("Exiting...\n")
        exit()
        
    print("Uploading data to: " + url + " \nPlease wait...")
    filename = fname_path(url)
    
    headers = {'user-agent': 'application/json'}
    try:
        # response = requests.post(url, headers=headers, data = {'key':'value'})
        response = requests.post(url, headers=headers, data={'json' : mydata})
        print("Response Status: HTML_" + str(response.status_code))
    except:
        print("\nERROR: "+str(sys.exc_info()[0]) + " " + str(sys.exc_info()[1]))

    if response.status_code / 100 != 2:
        print("\nError [" + str(response.status_code) + "] " + response.reason + " [ " + url + " ]\nExiting...\n")
        exit()
    else:
        print("\n\nData uploaded: \n" + str(mydata))
        
    return response;
    
def fname_path(path):
    fname = os.path.basename(path)
    return fname;
    
def take_actions(actions):
    ardID = actions[0]
    actionsDict = actions[1]
    
    # 0:no_action, 1:low, 2:medium, 3:high, 4:critical
    for sensor, priority in actionsDict.items():
        # print ("Priority: " + priority + " - " + sensor)
        # priority=int(priority)
        
        if priority=="0":
            prioritySTR = "No action required."
        elif priority=="1":
            prioritySTR = "LOW"
        elif priority=="2":
            prioritySTR = "MEDIUM"
        elif priority=="3":
            prioritySTR = "HIGH"
        elif priority=="4":
            prioritySTR = "CRITICAL"
        else:
            prioritySTR = "!!! ERROR: Wrong priority number"
            # print ("\n!!! ERROR: Wrong priority number [" + sensor + "(" + priority + ")] on Arduino [" + ardID + "]")
            
        print ("ArdID:[" + ardID + "], Sensor:[" + sensor + "], Priority:[" + prioritySTR + "]")

while True:
    # dtt='{"ARDID":"0001","sensors":[{"name":"gps","time":123123,"data":[1.123456,2.123456]},{"name":"temp","time":123123,"data":[23.2]},{"name":"pressure","time":123123,"data":[900000]},{"name":"humidity","time":123123,"data":[30]},{"name":"sensor05","time":0,"data":[0]},{"name":"sensor06","time":0,"data":[0]},{"name":"sensor07","time":0,"data":[0]},{"name":"sensor08","time":0,"data":[0]},{"name":"sensor09","time":0,"data":[0]},{"name":"sensor10","time":0,"data":[0]}]}'
    # dtt="paparitsa"
    
    #r=post_data(domain+testsrv,dtt)
    #print("---")
    #print("\nResponse Headers:\n" + str(r.headers))
    #print("\nResponse Text:\n" + r.text)
    #if ("!!! Error") in r.text:
    #    print("Exiting...")
    #    print("-"*50+"\n\n")
    #    exit();

    #actions = JsonToData(r.text)
    # print("\nActions: ")
    # print(actions)
    
    take_actions(actions);
    
    time.sleep(interval)
#    val = "What's the temp of ARDie?"
     # writeMessageToArduino("RST",1)
     # number = readNumber()
     # print(number)
    # timestamp='{:%y%m%d_%H:%M:%S}'.format(datetime.now())
#    time.sleep(1)
    # print(5*"-")
    dato = [0] * number
    for x in range(0,number):
        #print(x)
        dato[x]=readMessageFromArduino();
    
    #print(dato)
    arrayToJson(dato,timestamp)
    

