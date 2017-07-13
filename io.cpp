#include "io.h"
#include <Arduino.h>

int read_3v3 (int pin)
{
  int input = analogRead(pin);

  // analogRead() returns a value in [0, 1023] for 5V inputs.
  // For 3.3V inputs, the range is [0, 700].
  return int((double(input) / 700.0 * 100.0));
}
