import sys
import socket
import time
from PIL import Image
import json


'''
    ESP_socket class handles the socket connection with ESP8266
'''
class ESP_Socket:

    ANSWER = "OK"

    def __init__(self, sock=None):
        #                                 x           y           w           h           s
        self.commandHeader = [0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x00, 0x02, 0x80, 0x19]

        if sock is None:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect((host, port))

    def disconnect(self):
        self.sock.close()

    def send(self, bytes, isData = 0):
        for b in bytes:
            enc = bytearray()
            enc.append(b)
            try:
                self.sock.send(enc)
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

        self.commandHeader = [0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x00, 0x02, 0x80, 0x10, 0x60, 0x00]

        self.epdTemplate = ['\x3d', '\x04', '\x00', '\x05', '\x00', '\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
                       '\x00', '\x00', '\x00', '\x00']

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

    def chunks(self,l, n):
        for i in xrange(0, len(l), n):
            yield l[i:i + n]


    def upload(self,filename):
        print "UPLOADING"
        data = []

        starttime = time.time()

        if filename[-4:].lower() == ".png" or filename[-4:].lower() == ".jpg": # might also work with other image formats
            image = Image.open(filename)
            image = image.convert('1') # convert to monochrome

            # save pixel data into array. attention: it's still one byte per pixel!
            image_data = [image.getpixel((x, y)) for y in range(image.height) for x in range(image.width)]

            data.extend(self.epdTemplate) # workaround: epd header is not used but expected

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
                    data.extend([chr(buf)])
                    buf = 0

        else:
            print "Error: Unsupported file format!"
            return

        print("time for decoding image: "+str(time.time() - starttime))

        print format(len(data), "#08x")

        dataChunks = list(self.chunks(data, 24576)) #8192

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

