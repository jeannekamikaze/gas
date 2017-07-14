#pragma once

/// A calibrated, linear, analog input.
class CalibratedInput
{
public:

  /// Construct an uncalibrated input.
  CalibratedInput ();
  
  /// Construct a calibrated input.
  CalibratedInput (int min_value, int max_value);
  
  /// Map a raw input value to a calibrated value in the range [0,100].
  /// If the input has not been calibrated, return the raw value.
  int operator() (int value) const;

private:

  int min_value;
  int max_value;
};

/// Calibrates a linear, analog input.
class Calibration
{
public:

  enum State
  {
    Idle,
    Running
  };
  
  /// Initialise the calibration.
  /// 'calibration_duration' is the duration of the calibration in seconds.
  void start (unsigned long t, unsigned long calibration_duration, int input);
  
  /// Update the calibration.
  /// Return the state of the calibration.
  State update (unsigned long t, int input, CalibratedInput& output);
  
  /// Return the calibration's state.
  State state () const { return _state; }
  
  /// Return the calibration time remaining in seconds.
  unsigned long time_remaining (unsigned long t) const;
  
private:

  State _state = Idle;
  unsigned long t_start; // point in time when the calibration started
  unsigned long calibration_duration; // seconds
  
  int min_value;
  int max_value;
};
