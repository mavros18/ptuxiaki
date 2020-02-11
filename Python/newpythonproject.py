# -*- coding: utf-8 -*-


# Imports
import datetime
from datetime import datetime
import io
import json
import os
import requests
import sys
import time


# Globals
dashes = 100
testsrv = "https://hookb.in/vDkRzpdb"
#srvurl = "http://www.google.gr/robots.txt"
srvurl = "http://norvig.com/big.txt"
#srvurl = "http://www.gutenberg.org/files/1184/1184-0.txt"
extLOG = False
extLOGFile = "extLOG.log"
file1 = "functions.json"
file2 = "sample.json"

data = {"ARDID":"0001",
        "sensors":[
            {"name":"gps","time":123123,"data":[1.123456,2.123456]},
            {"name":"temp","time":123123,"data":[23.2]},
            {"name":"pressure","time":123123,"data":[900000]},
            {"name":"humidity","time":123123,"data":[30]},
            {"name":"sensor05","time":0,"data":[0]},
            {"name":"sensor06","time":0,"data":[0]},
            {"name":"sensor07","time":0,"data":[0]},
            {"name":"sensor08","time":0,"data":[0]},
            {"name":"sensor09","time":0,"data":[0]},
            {"name":"sensor10","time":0,"data":[0]}
        ]
    }
#data2=str(data)

def header():
    # Header
    print(dashes * "*")
    start = time.time()
    print("Started at:\t {:%H:%M:%S}".format(datetime.now()))
    print("Version:\t " + sys.version)
    print("Encoding:\t " + sys.getdefaultencoding())
    print(dashes * "*" + "\n")
    return(start);

def footer(start):
    # Footer
    print("\n" + dashes * "*")
    end = time.time()
    elapsed = end - start
    print("Completed at:\t {:%H:%M:%S}".format(datetime.now()))
    print("Elapsed time:\t " + str(round(elapsed, 5)) + " sec.")
    print(dashes * "*")
    return;

start = header();
    
try:
    to_unicode = unicode
except:
    to_unicode = str

def fname_path(path):
    fname = os.path.basename(path)
    return fname;
    
def printme(string):
    print (string)
    write_log_file(string)
    return;

def write_log_file(message):
    timestamp = '{:%y%m%d_%H:%M:%S}'.format(datetime.now())
    with io.open(extLOGFile, "a", encoding='utf8') as extlogfile:
        try:
            extlogfile.write(timestamp + " " + message+"\n")
        except:
            print(sys.exc_info()[0])
    return;

def write_file(file, mode, string):
    printme("Writing file: " + file)
    str_ = string
    try:
        with io.open(file, mode, encoding='utf8') as outfile:
            outfile.write(to_unicode(str_))
    except:
        printme("ERROR: " + sys.exc_info()[0])
    if (extLOG):
        printme("File writen: " + file)
    printme("File writen: " + file)
    return;

def read_file(file):
    printme("Reading file: " + file)
    try:
        with open(file) as f:
            content = f.read() 
    except:
        printme(sys.exc_info()[0])
    if (extLOG):
        printme("File read: " + file) 
    printme("File read: " + file)
    return(content);

def write_json_file (file, mode, string):
    printme("Writing JSON file: " + file)
    try:
        with io.open(file, mode, encoding='utf8') as outfile:
            str_ = json.dumps(string,
                              indent=4, sort_keys=True,
                              separators=(',', ':'),
                              ensure_ascii=False
                              )
            outfile.write(to_unicode(str_)) 
    except:
        printme(sys.exc_info()[0])
    if (extLOG):
        printme("JSON file writen: " + file)
    printme("JSON file writen: " + file)
    return;

def read_json_file (file):
    printme("Loading JSON file: " + file)
    try:
        with open(file) as data_file:
            data_loaded = json.load(data_file) 
    except:
        printme(sys.exc_info()[0])
    if (extLOG):
        printme("JSON file loaded: " + file)
    printme("JSON file loaded: " + file)
    return data_loaded;

def get_url(url):
    printme("Downloading file: " + url + " \nPlease wait...")
    filename = fname_path(url)
    try:
        response = requests.get(url)
    except:
        printme((sys.exc_info()[0]))
        exit()
    if response.status_code / 100 != 2: 
        printme("HTTPError " + str(response.status_code) + ": [" + url + "] " + response.reason)
        exit()
    else:
        printme("File downloaded " + response.reason + ": " + filename)
    return response.text;

def post_data(url, data):
    printme("Uploading data to: " + url + " \nPlease wait...")
    filename = fname_path(url)
    headers = {'user-agent': 'my-app/0.0.1'}
    try:
        response = requests.post(url, headers=headers, json=data)
    except:
        printme(sys.exc_info()[0])
    if response.status_code / 100 != 2:
        printme("HTTPError " + str(response.status_code) + ": [" + url + "] " + response.reason + " Exiting...")
        exit()
    else:
        printme("Data uploaded " + response.reason + ": " + filename)
    return response;

#print(read_file(file1)) #functions.json



#print(srvurl)
#print(fname_path(srvurl))
#pantelis = get_url(srvurl)
#write_file(fname_path(srvurl), "w",get_url(srvurl) )

jsonDATA = json.loads(read_file(file2))


#jsonDATA = read_json_file(file2)
#print(type(jsonDATA))
#~ print(jsonDATA)
print(20*"=")
for x in range(0,(len(jsonDATA['sensors']))):
    if jsonDATA['sensors'][x]['name'] == 'gps':
        print(jsonDATA['sensors'][x]['name'] + ": " + str(jsonDATA['sensors'][x]['data'][0]) + ", " + str(jsonDATA['sensors'][x]['data'][1]))
print(20*"=")
#print(jsonDATA['another dict'])
#print(jsonDATA)

#res = post_data(testsrv,read_file("bigh.txt"))
#res = post_data(testsrv, jsonDATA)
#print("POST big.txt: " + str(res.status_code) + ": " + res.reason)

footer(start);



#####################################
#~ import RPi.GPIO as GPIO

#~ GPIO.setmode(GPIO.BCM)
#~ print("Mode:\t" + GPIO.getmode())
#~ print("RPi info:\t\n\t\t" + GPIO.RPI_INFO)
#~ print("GPIO Ver:\t" + GPIO.VERSION)

#~ led=GPIO.setup(18, GPIO.OUT)

#~ led.output
