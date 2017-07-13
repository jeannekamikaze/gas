/// Various LCD utility functions.

#pragma once

#include <LiquidCrystal.h>

/// Print a number as 2 digits.
/// If the number is less than 2 digits, it is padded with 0s.
void print2 (LiquidCrystal& lcd, int x);

/// Print a number as 3 digits.
/// If the number is less than 3 digits, it is padded with 0s.
void print3 (LiquidCrystal& lcd, int x);
