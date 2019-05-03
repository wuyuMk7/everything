#!/usr/bin/env python

from gpiozero import MCP3008, DigitalOutputDevice, DigitalInputDevice
from time import sleep, time
from datetime import datetime
from tzlocal import get_localzone
from send import send
import database as db
import send_dict_to_firebase as sender

LOAD_CELL_DOUT = 19
LOAD_CELL_SCK  = 26
LOAD_CELL_GAIN = 128

def convert_twos_complement(value):
    return (value & 0x7fffff) - (value & 0x800000)

def data_processor(list, threshold):
    smaller = [e for e in list if e < threshold]
    return len(smaller) < len(list) - len(smaller)

def collector(sample_length=45, 
        dout=LOAD_CELL_DOUT, sck=LOAD_CELL_SCK, gain=LOAD_CELL_GAIN,
        loadcell_threshold=-200000, fsr_threshold=0.5):
    start_time = time()

    d_out = DigitalInputDevice(pin=dout, pull_up=True)
    sck = DigitalOutputDevice(pin=sck)
    adc = MCP3008(channel=0)

    load_cell_results = []
    fsr_results = []
    sck.off()

    timeout = 2
    while time() - start_time < sample_length:
        # Retrieve data from MCP3008
        fsr_results.append(adc.value)

        # Retrieve data from HX711
        loop_start_time = time()
        while not d_out.is_active:
            if time() - loop_start_time > timeout:
                break

        data = 0
        for j in range(24):
            data = data << 1
            sck.on()
            sck.off()
            if d_out.value == 0:
                data = data + 1
        #print("data", data, hex(data), convert_twos_complement(data))
        # load_cell_results.append(data)
        load_cell_results.append((hex(data), convert_twos_complement(data)))
        
        # 25th pulse
        sck.on()
        sck.off()

        # 128 pulses
        for i in range(gain):
            sck.on();
            sck.off();

    # Process data
    loadcell_data = ','.join(str(e) for e in load_cell_results)
    fsr_data = ','.join(str(e) for e in fsr_results)

    loadcell_results = [ e[1] for e in load_cell_results if e[0] != '0xffffff' ]
    loadcell_result = 1 if data_processor(loadcell_results, loadcell_threshold) else 0
    fsr_result = 1 if data_processor(fsr_results, fsr_threshold) else 0

    final_result = 1 if fsr_result == 1 or loadcell_result == 1 else 0

    # Send data to remote server
    # firebase_values = { 
    #         'isSitting': final_result, 
    #         'datetime': datetime.fromtimestamp(start_time, get_localzone()),
    #         'timestamp': int(start_time)
    # }
    # sender.sendToFirebase(firebase_values=firebase_values)
    resp = send(final_result, datetime.fromtimestamp(start_time, get_localzone()))
    print(resp)

    # Save data into local database
    db.insert((int(start_time), fsr_data, fsr_result, loadcell_data, loadcell_result, final_result))

    adc.close()
    d_out.close()
    sck.close()

    return (load_cell_results, fsr_results)

if __name__ == "__main__":
    try:
        results = collector(sample_length=30, dout=LOAD_CELL_DOUT, sck=LOAD_CELL_SCK, gain=LOAD_CELL_GAIN)
        # print("Result:")
        # print(results)
        print("Collection finsihed.")
    except Exception as e:
        print("Exception caught - ", e)


