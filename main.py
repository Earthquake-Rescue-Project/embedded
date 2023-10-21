from machine import ADC
import utime, os
# import gc

import json
from pico_lte.utils.status import Status
from pico_lte.core import PicoLTE
from pico_lte.common import debug
from pico_lte.utils.helpers import get_parameter
"""
picoLTE = PicoLTE()

picoLTE.network.register_network()
picoLTE.http.set_context_id()
picoLTE.network.get_pdp_ready()
picoLTE.http.set_server_url()

url = get_parameter(["https", "server"])

if url:
    url = url.replace("https://", "").replace("http://", "")
    index = url.find("/") if url.find("/") != -1 else len(url)
    host = url[:index]
    query = url[index:]
else:
    debug.error("Missing argument: server")
"""

sending_period = 4000 # ms
sending_time_handler = 0
positive_signal = ADC(26)
negative_signal = ADC(27)
time_data = []
sensor_data = []
initial_time = utime.ticks_ms()
last_sending = 0
   
try:
    os.remove('data.txt')
except:
    pass
file = open("data.txt", "w")
"""
while True:
    if sending_time_handler > sending_period:
        file.close()
        debug.info("Sending POST request.")
        # payload_dict = {"row": time_data,"column": sensor_data}
        #payload_dict = {"column": sensor_data}
        file = open("data.txt", "r")
        all_data = file.read()
        #print("Data: ",all_data)
        os.remove('data.txt')
        
        payload_json = json.dumps({"column": all_data})
        
        HEADER = "\n".join(
            [
                f"POST {query} HTTP/1.1",
                f"Host: {host}",
                "Content-Type: application/json",
                f"Content-Length: {len(payload_json)+1}",
                "\n\n",
            ]
        )
        
        print(HEADER)
        result = picoLTE.http.post(data=HEADER + payload_json, header_mode=1)
        debug.info(result)
        del payload_json
        utime.sleep(20)
        result = picoLTE.http.read_response()
        if result["status"] == Status.SUCCESS:
            debug.info("Post request succeeded.")
        debug.info(result)
        time_data = []
        sensor_data = []
        payload_dict = {}
        last_sending = utime.ticks_diff(utime.ticks_ms(), initial_time)
        #gc.collect()
        file = open('data.txt', 'w')

    value = positive_signal.read_u16() - negative_signal.read_u16()
    value *= 0.05035400390625
    file.write(str((value))+'\n')
    
    time = utime.ticks_diff(utime.ticks_ms(), initial_time)
    # print("Data: ",value, "\n")
    utime.sleep_ms(10)
    # time_data.append(time)
    #sensor_data.append(value)
    sending_time_handler = time - last_sending


"""
while True:
    a = positive_signal.read_u16() - negative_signal.read_u16()
    #a *= 0.05035400390625
    print(a)
    utime.sleep_ms(20)

