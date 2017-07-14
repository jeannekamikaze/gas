#include "calibration.h"
#include "expsmooth.h"
#include "lcd.h"
#include "io.h"

#include <LiquidCrystal.h>
#include <EEPROM.h>

// Pin analogico de entrada donde va conectado el gas.
#define GAS_INPUT A0

// Pin digital de entrada donde va conectado el boton de calibracion.
#define CALIBRATE_BUTTON 2

// Duracion de la calibracion en segundos.
#define CALIBRATION_DURATION 10 // seconds

// Valor entre 0 y 1 que suaviza la senyal de gas de entrada.
// 0 -> suavizado maximo.
// 1 -> sin suavizado.
// 0 < x < 1 -> suavizado intermedio.
#define SMOOTHING_FACTOR 0.3

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

void render_input (int input, LiquidCrystal& lcd)
{
  lcd.setCursor(6,0);
  print3(lcd, input);
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

  // Smoothing the signal to prevent flickering.
  int raw_gas = int(smoothed_input(read_3v3(GAS_INPUT)));

  // Calibration.
  if (calibrate_button == HIGH && calibration.state() == Calibration::Idle)
  {
    calibration.start(t, CALIBRATION_DURATION, raw_gas);
  }
  
  if (calibration.state() == Calibration::Running)
  {
    calibration.update(t, raw_gas, calibrated_input);
    render_calibration(t, lcd, calibration);

    if (calibration.state() == Calibration::Idle) // calibration is done
    {
      lcd.clear(); // erase the calibration information from the lcd
      write_state(calibrated_input);
    }
  }

  // Output.
  int calibrated_gas = calibrated_input(raw_gas);
  render_input(calibrated_gas, lcd);
}
