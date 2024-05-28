#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_ADS1X15.h>
#include "soc/rtc_wdt.h"
TaskHandle_t Task1;
Adafruit_ADS1115 ads;

RF24 radio(4, 5); // CE, CSN
const uint64_t address = 0xF0F0F0F0F0LL;
bool flag = true;
/* Be sure to update this value based on the IC and the gain settings! */
// float   multiplier = 3.0F;    /* ADS1015 @ +/- 6.144V gain (12-bit results) */
float multiplier = 0.0625F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */
String Message = "";
String Message2 = "";

const int interval = 500000;

int send_start = 0, send_finish = 0;
unsigned int old_time;
volatile int16_t results;
String sub_S = "";
String end_char = "&";
void setup()
{
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.setGain(GAIN_TWO); // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  ads.setDataRate(224); // 0x00E0
  /*
  #define RATE_ADS1115_8SPS (0x0000)   ///< 8 samples per second
  #define RATE_ADS1115_16SPS (0x0020)  ///< 16 samples per second
  #define RATE_ADS1115_32SPS (0x0040)  ///< 32 samples per second
  #define RATE_ADS1115_64SPS (0x0060)  ///< 64 samples per second
  #define RATE_ADS1115_128SPS (0x0080) ///< 128 samples per second (default)
  #define RATE_ADS1115_250SPS (0x00A0) ///< 250 samples per second
  #define RATE_ADS1115_475SPS (0x00C0) ///< 475 samples per second
  #define RATE_ADS1115_860SPS (0x00E0) ///< 860 samples per second
  */
  if (!ads.begin())
  {
    while (1)
      ;
  }
  rtc_wdt_protect_off();                   // Turns off the automatic wdt service
  rtc_wdt_enable();                        // Turn it on manually
  rtc_wdt_set_time(RTC_WDT_STAGE0, 10000); // Define how long you desire to let dog wait.

  delay(200);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_2MBPS);
  radio.stopListening();

  xTaskCreatePinnedToCore(
      Task1code, /* Task function. */
      "Task1",   /* name of task. */
      100000,    /* Stack size of task */
      NULL,      /* parameter of the task */
      0,         /* priority of the task */
      &Task1,    /* Task handle to keep track of created task */
      0);        /* pin task to core 0 */
  delay(50);
  old_time = micros();
}

void Task1code(void *pvParameters)
{
  while (1)
  {
    if (micros() - old_time > interval)
    {
      rtc_wdt_feed();
      flag = false;
      int i = 0, lng = Message.length();
      Message2 = Message;
      flag = true;

      send_start = micros();
      for (i; i < lng; i += 28)
      {
        sub_S = Message2.substring(i, i + 28);
        radio.write(sub_S.c_str(), 28);
      }
      radio.write(end_char.c_str(), 1);
      send_finish = micros();

      flag = false;
      Message = Message.substring(lng);
      flag = true;

      old_time = micros();
    }
  }
}

void loop()
{
  if (flag == true)
  {
    results = ads.readADC_Differential_0_1();
    Message = Message + String(results * multiplier, 1) + "/";
    delay(7);
  }
}
