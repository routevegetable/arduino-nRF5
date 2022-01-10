

#include "nrf_sdm.h"
#include "ble.h"
#include "verify.h"
#include "nrf_ble_utils.h"

#define MANUFACTURER_ID   0x0059
#define MAJOR 420
#define MINOR 69
#define RSSI_AT_1M -54


// "nRF Connect" app can be used to detect beacon
uint8_t beaconUuid[16] =
{
  0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x04,
  0x20, 0x69, 0x69, 0x69, 0x69, 0x69, 0x69, 0x69
};



bool transmit_temperature(int temperature) {

  return beacon_init(MANUFACTURER_ID,
  beaconUuid,
  MAJOR,
  temperature,
  RSSI_AT_1M);
}


void setup() { 
  NRF_POWER->TASKS_LOWPWR = 1;
  Serial.begin(115200);

  // Green LED
  pinMode(18,OUTPUT);
  digitalWrite(18, LOW);


  NRF_TEMP->TASKS_START = 0x1;
  
  while(!NRF_TEMP->EVENTS_DATARDY)
    Serial.print("Waiting...\n");

  Serial.print("Temperature: ");
  float temp = (float)NRF_TEMP->TEMP * 0.25f ;
  Serial.print(temp);
  Serial.print("\n");

  if(transmit_temperature((int)temp)) {
    Serial.println("Broadcasting temperature");  
  } else {
    Serial.println("Failed to init BLE!");
  }
}

void loop() {
    static int i = 0;

    // Blinky light
    for(float brightness = 0.0; brightness < 1; brightness+=0.05) {
      digitalWrite(18, LOW);
      delay(brightness*20.0);
      digitalWrite(18,HIGH);
      delay((1.0-brightness)*20.0);
    }
    

    bool done = handle_sd_events();

    if(done) {
      Serial.end();
      // Disable softdevice
      sd_softdevice_disable();

      // Kill RTC interrupt for better quality sleep
      NVIC_DisableIRQ(RTC1_IRQn);

      // Kill timers?
      NRF_TIMER0->TASKS_STOP = 1;
      NRF_TIMER1->TASKS_STOP = 1;
      NRF_TIMER2->TASKS_STOP = 1;

      // Kill RTCs?
      NRF_RTC0->TASKS_STOP = 1;
      NRF_RTC1->TASKS_STOP = 1;
      
      // Kill GPIOs

      // Wake after some time
#define WAKE_SECS 30
      NRF_RTC0->PRESCALER = 4095; // 8Hz
      NRF_RTC0->CC[0] = WAKE_SECS * 8;
      
      NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Msk;
      NRF_RTC0->EVTENSET = 0;
      
      NVIC_EnableIRQ(RTC0_IRQn);
      NRF_RTC0->TASKS_START = 1;
      
      __WFI();
      NVIC_SystemReset();
    }
}


extern "C" void RTC0_IRQHandler(void) {
      NRF_RTC0->TASKS_STOP = 1;
      NVIC_SystemReset();
}
