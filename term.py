import serial
import sys
class Terminal:
    
    def __init__(self, port_name: str, baudrate=115200):
        self.serial = serial.Serial(port=port_name, baudrate=baudrate, timeout=0.1 )
        #self.serial.open()

    def run(self):
        while self.serial.is_open:
            b = self.serial.read(8)
            if b and b:
                print(b.decode('ascii', 'ignore'), end='')

if __name__=="__main__":
    port = sys.argv[1]
    print(f"~~opening {port}...~~")
    Terminal(port).run()
    print("~~done~~")
