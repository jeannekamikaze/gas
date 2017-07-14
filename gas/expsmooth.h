#pragma once

/// Simple exponential smoothing of values.
class Smoothed
{
public:

  /// Initialise the exponential smoothing.
  Smoothed (double smoothing_factor, double initial_value = 0)
    : a(smoothing_factor), s(initial_value) {}
  
  /// Update the running average with a new observation.
  /// Return the current running average.
  double operator() (double x) {
    s = x*a + s*(1-a);
    return s;
  }
    
private:
  
  const double a; // smoothing factor
  double s; // current average
};
