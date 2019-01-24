import sys
import socket
import time
import numpy
from PIL import Image
import json


'''
    ESP_socket class handles the socket connection with ESP8266
'''
class ESP_Socket:

    ANSWER = "OK"

    def __init__(self, sock=None):
        #                                 x           y           w           h           s
        self.commandHeader = numpy.array([0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x00, 0x02, 0x80, 0x19],dtype=numpy.dtype('b'))

        if sock is None:
            self.sock = socket.socket()
        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect((host, port))

    def disconnect(self):
        self.sock.close()

    def send(self, bytes, isData = 0):
        print bytes
        print len(bytes)
        try:
            self.sock.send(bytes)
        except:
            print "send failed"
        print isData
        if isData == 1:
            answer = ""
            while(True):
                answer += self.sock.recv(1)
                if answer == self.ANSWER:
                    print "received: "+answer
                    print "send next chunk!"
                    break

'''
    handles communication with ESP8266 and creates sockets
'''
class Communicator:

    def __init__(self,filename):

        print "STARTED"

        self.file = filename

        self.CONF_FILE = './conf.json'

        self.PORT = 0

        self.BYTE_ZERO = 0x00

        self.ip = ''

        self.commandHeader = numpy.array([0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x00, 0x02, 0x80, 0x10, 0x60, 0x00],dtype=numpy.dtype('b'))

        self.epdTemplate = numpy.array([0x3d, 0x04, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00],dtype=numpy.dtype('b'))

        self.loadConf()

        self.upload(filename)

    def loadConf(self):
        try:
            conf = json.load(open(self.CONF_FILE))

            self.ip = conf["client"]["ip"]
            self.PORT = conf["client"]["port"]
        except:
            print "FAILED LOADING CONFIGURATION FILE"
            sys.exit()

    def chunks(self,data, size):
        chunkAmount = (len(data) / size ) + 1
        chunk = []
        for i in range(chunkAmount):
            chunk.append(numpy.arange(size, dtype=numpy.dtype('b')))

        for i in range (len(data)):
            chunkNum = i / size
            pos = i % size
            c = chunk[chunkNum]
            c[pos] = data[i]
        return chunk




    def upload(self,filename):
        print "UPLOADING"
        data = []

        headerSize = len(self.epdTemplate)
        arraySize = 1280 * 1024 / 8 + headerSize
        print arraySize
        dataArray = numpy.arange(arraySize,dtype=numpy.dtype('b'))

        starttime = time.time()

        if filename[-4:].lower() == ".png" or filename[-4:].lower() == ".jpg": # might also work with other image formats
            image = Image.open(filename)

            # save pixel data into array. attention: it's still one byte per pixel!
            image_data = [image.getpixel((x, y)) for y in range(image.height) for x in range(image.width)]

            for i in range(headerSize):
                dataArray[i] = self.epdTemplate[i]

            # image data has to be one bit per pixel, so each byte of the source data
            # has to be merged into one bit (which is enough for a monochrome image)
            j = 8   # bit index of current byte
            buf = 0 # buffer to store bits
            for i in range(len(image_data)):
                j -= 1

                # shift new bit to left by index, then or it over the current buffer
                buf |= (image_data[i] == 0) << j

                # if our byte is completed, push it into data array
                if j == 0:
                    j = 8
                    dataArray[i/8 + headerSize] = buf
                    buf = 0

        else:
            print "Error: Unsupported file format!"
            return

        print("time for decoding image: "+str(time.time() - starttime))

        print format(arraySize, "#08x")

        dataChunks = self.chunks(dataArray, 24576)#24576) #8192

        print "number of chunks: "
        print len(dataChunks)

        print "send command header"
        sock = ESP_Socket()
        sock.connect(self.ip, self.PORT)
        sock.send(self.commandHeader)
        sock.disconnect()

        j = 0

        print "start sending..."

        print "send data"
        for chunk in dataChunks:
            print j
            j += 1
            sock = ESP_Socket()
            sock.connect(self.ip, self.PORT)
            sock.send(chunk, 1)
            sock.disconnect()

        print "finished"


'''
    entry point for this file
    creates Communicator
'''
def upload(file):
    Communicator(file)

