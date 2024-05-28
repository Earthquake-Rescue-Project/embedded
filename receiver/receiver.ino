#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5);
const uint64_t address = 0xF0F0F0F0F0LL;
int len = 0;
String lastData = "";
void setup()
{
  Serial.begin(921600);
  radio.begin();
  radio.openReadingPipe(0, address); // Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_HIGH);    // You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.setDataRate(RF24_2MBPS);
  radio.startListening(); // This sets the module as receiver
  Serial.println("ok");
}

void loop()
{

  while (radio.available())
  {
    char gotmsg[32] = "";
    len = radio.getDynamicPayloadSize();
    radio.read(&gotmsg, len);
    if (gotmsg[0] == '&')
    {
      Serial.println(lastData);
      lastData = "";
      break;
    }

    lastData += String(gotmsg);
  }
}
