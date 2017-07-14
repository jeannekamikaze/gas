#include "lcd.h"

void print2 (LiquidCrystal& lcd, int x)
{
  if (x < 10)  lcd.print(0);
  lcd.print(x);
}

void print3 (LiquidCrystal& lcd, int x)
{
  if (x < 100) lcd.print(0);
  if (x < 10)  lcd.print(0);
  lcd.print(x);
}

