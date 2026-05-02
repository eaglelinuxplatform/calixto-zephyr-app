import serial
import canopen
import can
import threading
import queue
import os
import time


class WaveshareSerial:
    FRAME_LEN = 20

    def __init__(self, port, baudrate=2000000):
        self.ser = serial.Serial(port=port, baudrate=baudrate, timeout=0.05)
        self.buf = bytearray()
        self._rx_queue = queue.Queue()
        self._write_lock = threading.Lock()
        self._stop = threading.Event()
        self._rx_thread = threading.Thread(target=self._reader, daemon=True)
        self._rx_thread.start()

    def _parse_frame(self, frame):
        if frame[0:2] != b'\xaa\x55':
            return None
        is_ext = bool(frame[2] & 0x02)
        can_id = int.from_bytes(frame[5:8], 'little')   # ID at bytes [5:8]
        dlc    = frame[9] & 0x0F                         # DLC at byte 9
        data   = bytes(frame[10:10 + dlc])               # data starts at byte 10
        print(f"  [RX] ID={hex(can_id)} DLC={dlc} data={data.hex()}")
        return can.Message(
            arbitration_id=can_id,
            data=data,
            is_extended_id=is_ext,
            timestamp=time.time()
        )

    def _reader(self):
        while not self._stop.is_set():
            try:
                chunk = self.ser.read(64)
            except Exception:
                break
            if not chunk:
                continue
            self.buf.extend(chunk)
            while len(self.buf) >= self.FRAME_LEN:
                idx = self.buf.find(b'\xaa\x55')
                if idx == -1:
                    self.buf.clear()
                    break
                if idx > 0:
                    del self.buf[:idx]
                if len(self.buf) < self.FRAME_LEN:
                    break
                frame = bytes(self.buf[:self.FRAME_LEN])
                del self.buf[:self.FRAME_LEN]
                msg = self._parse_frame(frame)
                if msg:
                    self._rx_queue.put(msg)

    def recv(self, timeout=1.0):
        try:
            return self._rx_queue.get(timeout=timeout)
        except queue.Empty:
            return None

    def send(self, msg: can.Message):
        frame = bytearray(20)
        frame[0]   = 0xAA
        frame[1]   = 0x55
        frame[2]   = 0x02 if msg.is_extended_id else 0x01
        frame[3]   = 0x01                                       # fixed padding
        frame[4]   = 0x00                                       # fixed padding
        frame[5:8] = msg.arbitration_id.to_bytes(3, 'little')  # ID, 3 bytes LE
        frame[8]   = 0x00                                       # fixed padding
        frame[9]   = len(msg.data)                              # DLC
        frame[10:10 + len(msg.data)] = msg.data                 # data
        frame[19]  = sum(frame[2:19]) & 0xFF                    # checksum
        print(f"  [TX] ID={hex(msg.arbitration_id)} raw={frame.hex()}")
        with self._write_lock:
            self.ser.write(bytes(frame))

    def close(self):
        self._stop.set()
        self._rx_thread.join(timeout=2)
        self.ser.close()


class WaveshareBus(can.BusABC):
    def __init__(self, port, baudrate=2000000, **kwargs):
        super().__init__(channel=port, **kwargs)
        self.ws = WaveshareSerial(port, baudrate)

    def recv(self, timeout=None):
        return self.ws.recv(timeout=timeout if timeout is not None else 1.0)

    def send(self, msg, timeout=None):
        self.ws.send(msg)

    def shutdown(self):
        self.ws.close()

    @staticmethod
    def _detect_available_configs():
        return []



ZEPHYR_BASE = os.environ['ZEPHYR_BASE']
EDS = os.path.join(ZEPHYR_BASE, 'samples', 'modules',
                   'canopennode', 'objdict', 'objdict.eds')
NODEID = 10

# Build network and node first so listeners are populated
network = canopen.Network()
node = network.add_node(NODEID, EDS)

# Wire up custom bus
bus = WaveshareBus(port='COM18', baudrate=2000000)
network.bus = bus
network.notifier = can.Notifier(bus, network.listeners)

node.sdo.RESPONSE_TIMEOUT = 2.0

# Reset node and wait for boot-up
print("Resetting node...")
node.nmt.send_command(0x81)
try:
    node.nmt.wait_for_bootup(timeout=10)
    print("Node booted!")
except canopen.nmt.NmtError:
    print("No boot-up message, continuing...")

time.sleep(0.5)

# Read Manufacturer Device Name via SDO
print("Reading Manufacturer Device Name...")

try:
    name = node.sdo['Manufacturer device name']
    device_name = name.raw   # already a string
    print(f"Device name: '{device_name}'")
except Exception as e:
    print(f"Failed to read device name: {e}")

button = node.sdo['Button press counter']

print("Reading TPDO mapping...")
node.tpdo.read()

# Enter pre-operational state to map TPDO
print("Entering PRE-OPERATIONAL...")
node.nmt.state = 'PRE-OPERATIONAL'

# Map TPDO 1 to transmit the button press counter on changes
node.tpdo[1].clear()
node.tpdo[1].add_variable('Button press counter')
node.tpdo[1].trans_type = 254
node.tpdo[1].enabled = True

# Save TPDO mapping
print("Saving TPDO mapping...")
node.tpdo.save()
node.nmt.state = 'OPERATIONAL'

# Reset button press counter
print("Resetting button counter...")
button.raw = 0
print("Press the button 10 times")

while True:
    node.tpdo[1].wait_for_reception()
    count = node.tpdo['Button press counter'].phys
    print(f"Button press counter: {count}")
    if count >= 10:
        break

print("Done!")
network.notifier.stop()
bus.shutdown()