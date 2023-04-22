#include "mbed.h"
#include "USBSerial.h"

using namespace mbed;
using namespace rtos;
using namespace std::chrono;

const PinName STATE_PIN_1 = p14;
const PinName STATE_PIN_2 = p16;
const PinName LED_PIN_1 = p18;
const PinName LED_PIN_2 = p19;
const PinName RELAY_PIN_1 = p26;
const PinName RELAY_PIN_2 = p27;
const PinName LED_BIN_PIN = p25;
const PinName TARGET_TX_PIN = p0, TARGET_RX_PIN = p1;

DigitalIn state_1(STATE_PIN_1);
DigitalIn state_2(STATE_PIN_2);

DigitalOut led_1(LED_PIN_1);
DigitalOut led_2(LED_PIN_2);

DigitalOut relay_1(RELAY_PIN_1);
DigitalOut relay_2(RELAY_PIN_2);

DigitalOut led_bin(LED_BIN_PIN);

USBSerial serial;
UnbufferedSerial hw_serial(TARGET_TX_PIN, TARGET_RX_PIN, 115200);
FileHandle *mbed_override_console(int fd) {
  return &hw_serial;
}
int main(void) {
  led_bin = 1;

  for(;;) {

    led_1 = !state_1;
    led_2 = !state_2;

    relay_1 = !state_1;
    relay_2 = !state_2;

    String s = String("State1: ") + state_1 + String("    State2: ") + state_2;   // Get the current LED state
    serial.printf("%s\n", s.c_str());          // Send a message to USB serial
    printf("%s\n", s.c_str());                 // Send the same message to Serial0

    ThisThread::sleep_for(milliseconds(100));  // Delay for 100 msec
  }
}