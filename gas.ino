#include "calibration.h"
#include "expsmooth.h"
#include "lcd.h"
#include "io.h"

#include <LiquidCrystal.h>
#include <EEPROM.h>

#define GAS_INPUT A0
#define CALIBRATE_BUTTON 2
#define CALIBRATION_DURATION 10 // seconds
#define SMOOTHING_FACTOR 0.9
#define STATE_HEADER 0xcafebabe

void read_state (CalibratedInput& calibrated_input)
{
  int addr = 0;
  
  unsigned long header;
  EEPROM.get(addr, header);
  addr += sizeof(header);
  
  if (header == STATE_HEADER) // state was written previously
  {
    EEPROM.get(addr, calibrated_input);
  }
  // otherwise no state was written previously
}

void write_state (const CalibratedInput& calibrated_input)
{
  int addr = 0;
  
  unsigned long header = STATE_HEADER;
  EEPROM.put(addr, header);
  addr += sizeof(header);
  
  EEPROM.put(addr, calibrated_input);
}

void render_calibration (unsigned long t, LiquidCrystal& lcd, const Calibration& calibration)
{
  lcd.setCursor(0,1);
  lcd.print("Calibrando...");
  
  lcd.setCursor(14,1);
  print2(lcd, calibration.time_remaining(t));
}

void process_input (int input, CalibratedInput& calibrated_input, Smoothed& smoothed_input, LiquidCrystal& lcd)
{  
  // Map the input to a calibrated range of values.
  input = calibrated_input(input);
  
  // Smoothing the input to avoid flickering in the lcd.
  int smooth_input = int(smoothed_input(input));
  
  lcd.setCursor(6,0);
  print3(lcd, smooth_input);
}

LiquidCrystal lcd(7, 6, 8, 9, 10, 11);

Calibration calibration;
CalibratedInput calibrated_input;

Smoothed smoothed_input(SMOOTHING_FACTOR);

void setup ()
{
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  
  read_state(calibrated_input);
  
  pinMode(CALIBRATE_BUTTON, INPUT);
}

void loop ()
{
  unsigned long t = millis();
  int calibrate_button = digitalRead(CALIBRATE_BUTTON);
  int input = read_3v3(GAS_INPUT);
  
  if (calibrate_button == HIGH && calibration.state() == Calibration::Idle)
  {
    calibration.start(t, CALIBRATION_DURATION, input);
  }
  
  if (calibration.state() == Calibration::Running)
  {
    calibration.update(t, input, calibrated_input);
    render_calibration(t, lcd, calibration);

    if (calibration.state() == Calibration::Idle) // calibration is done
    {
      lcd.clear(); // erase the calibration information from the lcd
      write_state(calibrated_input);
    }
  }
  
  process_input(input, calibrated_input, smoothed_input, lcd);
}
