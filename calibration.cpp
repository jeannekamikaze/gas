#include "calibration.h"

namespace
{
  int min (int a, int b) { return a < b ? a : b; }
  int max (int a, int b) { return a > b ? a : b; }
  int clamp (int x, int a, int b) { return min(b, max(a, x)); }
}

// Calibrated input

CalibratedInput::CalibratedInput ()
  : min_value(100), max_value(0) {} // min > max means the input is uncalibrated

CalibratedInput::CalibratedInput (int min_value, int max_value)
  : min_value(min_value), max_value(max_value) {}

int CalibratedInput::operator() (int value) const
{
  if (min_value < max_value) // calibrated
  {
    // If the calibration did not cover the full range of the input, the calibrated output
    // value may come out negative or above 100.
    // To avoid this, clamp the value to [min_value, max_value].
    value = clamp(value, min_value, max_value);
    
    return int( double(value - min_value) / double(max_value - min_value) * 100.0 );
  }
  else // uncalibrated
  {
    return value;
  }
}

// Calibration

void Calibration::start (unsigned long t, unsigned long calibration_duration, int input)
{
  _state = Running;
  
  t_start = t;
  this->calibration_duration = calibration_duration;
  
  min_value = input;
  max_value = input;
}

Calibration::State Calibration::update (unsigned long t, int input, CalibratedInput& output)
{
  if (t - t_start > calibration_duration*1000)
  {
    _state = Idle;
  }
  else
  {
    min_value = min(min_value, input);
    max_value = max(max_value, input);
    output = CalibratedInput(min_value, max_value);
  }
  return _state;
}

unsigned long Calibration::time_remaining (unsigned long t) const
{
  unsigned long elapsed = (t - t_start) / 1000;
  return calibration_duration - elapsed;
}

