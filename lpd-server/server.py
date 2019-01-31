#!/usr/bin/env python

import logging

import socket
import time
import os
import sys
from PIL import Image, ImageFile
from thread import start_new_thread
import json
import commands
import get_ip

import com_2_7

class Server:

    CONF_FILE = './conf.json'

    TEMP_FILE = ''
    CONVERTED_FILE = ''

    WIDTH = 0
    HEIGHT = 0

    QUALITY = 1 #starting at 1

    PRINTER_IP = ''
    PRINTER_PORT = 0

    waiting = False

    startTime = -1

    '''
        load configuration file which can also be configured for your needs
    '''
    def loadConf(self):
        try:
            conf = json.load(open(self.CONF_FILE))

            self.TEMP_FILE = conf["files"]["temp"]
            self.CONVERTED_FILE = conf["files"]["converted"]

            self.WIDTH = conf["image"]["width"]
            self.HEIGHT = conf["image"]["height"]
            self.QUALITY = conf["image"]["quality"]

            self.PRINTER_IP = conf["printer"]["ip"]
            self.PRINTER_PORT = conf["printer"]["port"]
        except:
            print "FAILED LOADING CONFIGURATION FILE"
            sys.exit()

    '''
        the postscript driver writes "%%BoundingBox:" with values to the end of the file
        but PIL can not convert the ps document if "%%BoundingBox:" has no values at the beginning
        this fix writes the values from the last "%%BoundingBox:" to the first one
    '''
    def fixBoundingBox(self):
        #read file
        with open(self.TEMP_FILE, 'r') as file:
            content = file.read()

        #change bounding box
        lines = content.split("\n")

        beginning = 0
        ending = len(lines) -1

        while True:
            line = lines[beginning]
            if "%%BoundingBox:" in line:
                break
            else:
                beginning += 1

        while True:
            line = lines[ending]
            if "%%BoundingBox:" in line:
                break
            else:
                ending -= 1

        lines[beginning] = lines[ending]

        changed = "\n".join(lines)

        #delete old file
        try:
            os.remove(self.TEMP_FILE)
        except OSError:
            pass

        #write to new file
        f = open(self.TEMP_FILE, 'wb')
        f.write(changed)
        f.close()

    '''
        converts ps file to png and sends image to upload script
    '''
    def convert(self):
        timeBeforeConversion = time.time()
        print("fixing image")
        try:
           self.fixBoundingBox() #PIL has problem with eps bounding box if not fixed
        except:
           pass
        print("starting converting image")
        image = Image.open(self.TEMP_FILE)
        image.load(scale=self.QUALITY)
        image = image.resize((self.HEIGHT,self.WIDTH), Image.ANTIALIAS)
        #image = image.convert('1') #monochrome
        image.save(self.CONVERTED_FILE)

        try:
            os.remove(self.TEMP_FILE)
        except OSError:
            pass

        print("finished converting image")

        deltaConversion = time.time() - timeBeforeConversion
        print ("took " + str(deltaConversion) + " seconds for converting image")

        timeBeforeUpload = time.time()
        com_2_7.upload(self.CONVERTED_FILE)
        deltaUpload = time.time() - timeBeforeUpload
        print ("took " + str(deltaUpload) + " seconds for uploading and decoding image")

        print ("UPLOADED SUCCESSFULLY!")
        delta = time.time() - self.startTime
        print ("took "+str(delta)+" seconds for whole process")
        self.startTime = -1

    '''
        hosts the server which simulates an lpd printer
    '''

    def host(self):
        sendingTime = -1
        global resetTime
        print("started server")
        try:
            os.remove(self.TEMP_FILE)
        except OSError:
            pass

        ZERO = bytearray([0x00])

        PACKET_SIZE = 1024

        count = 0

        mySocket = socket.socket()
        mySocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        mySocket.bind((self.PRINTER_IP, self.PRINTER_PORT))

        mySocket.listen(1)
        conn, addr = mySocket.accept()
        print ("Connection from: " + str(addr))
        while True:
            try:
                data = conn.recv(PACKET_SIZE)

                #print ("count: "+str(count))

                if not data:
                    time.sleep(0.001)
                else:
                    resetTime = time.time()
                    if sendingTime < 0:
                        sendingTime = time.time()
                    #start measuring time
                    if self.startTime < 0:
                        self.startTime = time.time()
                    #print ("from connected  user with length "+str(len(data)))
                    if count >= 4:
                        f = open("temporary", 'ab')
                        f.write(data.encode())
                        f.close()

                        if len(data) != PACKET_SIZE: #chance 1/PACKET_SIZE to fail
                            conn.sendall(ZERO)
                            conn.close()
                    else:
                        print (data)
                        time.sleep(0.5)
                        conn.sendall(ZERO)

                    count += 1
            except:
                print ("no connection")
                if count > 0:
                    break


        try:
            deltaSending = time.time() - sendingTime
            print("receiving data from CUPS took "+str(deltaSending))
            self.convert()
        except:
            print("\n\nUPLOAD FAILED!\n")

       # try:
        #    os.remove(self.CONVERTED_FILE)
        #except OSError:
           # pass


resetTime = -1
'''
    program entry point
    starts lpd socket
'''
if __name__ == '__main__':
    print("started application")

    print("fetching esp ip")

    '''
        get ip from raspberry and save to config
    '''
    values = commands.getstatusoutput('sh get-ip.sh')
    ip = values[1]
    print (ip)
    get_ip.change_ip(ip)

    server = Server()

    server.loadConf()

    global resetTime

    while True:
        try:
            if resetTime > 0:
                if time.time() - resetTime > 30: #max waiting time
                    print("printing takes too long. probably something did not work as intended. script restarts now!")
                    raise Exception('took too long!')
            if server.waiting is False:
                #try:
                    server.host()
               # except:
                   # print("LPD Server failed. Starting again")
        except Exception as err:
            print("Exception: "+str(err))
            print("starting server again")
            resetTime = -1
