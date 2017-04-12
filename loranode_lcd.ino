#include <TheThingsNetwork.h>
#include <LiquidCrystal.h>

// Before your start, make sure that in the Tools menu, your Board and your
// Port is set to Arduino Leonardo

const char *appEui = "70B3D57EF0004242";
const char *appKey = "F628C47684990F5A89966D69681322B7";

//Set your frequency plan TTN_FP_EU868; for Europe or TTN_FP_US915; for the United States
const ttn_fp_t freqPlan = TTN_FP_EU868; // Use either TTN_FP_EU868; or TTN_FP_US915; here

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

#define loraSerial Serial1
#define debugSerial Serial

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup() {

  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0,0);
  lcd.print("LoRa init       "); // print a simple message
 
  // Set up the serial interfaces for the debugging serial monitor and LoRa module
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  // Here we activate the device with your address and keys
  ttn.join(appEui, appKey);

  // Show the status on the debugging serial monitor
  lcd.setCursor(0,0);
  lcd.print("LoRa Connected  ");
  ttn.showStatus();
}

int packetCounter = 0;
bool autoSend = false;
volatile long delayTime = 4000;

void loop() {

  // Encode int as bytes
  byte payload[2];
  payload[0] = highByte(10);
  payload[1] = lowByte(10);

 lcd_key = read_LCD_buttons();  // read the buttons

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     if (delayTime < 120000) delayTime = delayTime + 1000;
     delay(100);     
     break;
     }
   case btnLEFT:
     {
     if (delayTime > 2000) delayTime = delayTime - 1000;
     delay(100);
     break;
     }
   case btnUP:
     {
      lcd.setCursor(1,1);
      lcd.print("S");
      ttn.sendBytes(payload, sizeof(payload));
      packetCounter++;
      lcd.setCursor(1,1);
      lcd.print(" ");
      lcd.print(packetCounter);
     break;
     }
   case btnDOWN:
     {
     //lcd.print("DOWN  ");
     break;
     }
   case btnSELECT:
     {
     autoSend = !autoSend;
     delay(100);
     break;
     }
   case btnNONE:
     {
     // lcd.print("NONE  ");
     break;
     }
  }

  if (autoSend) {
    lcd.setCursor(0,1);
    lcd.print("A");
    if (millis()%delayTime == 0) {
      lcd.print("S");
      ttn.sendBytes(payload, sizeof(payload));
      packetCounter++;
      lcd.setCursor(0,1);
      lcd.print("A ");
      lcd.print(packetCounter);
    }
  } else {
    lcd.setCursor(0,1);
    lcd.print("  ");
    lcd.print(packetCounter);
  }
  lcd.setCursor(7,1);
  lcd.print(delayTime/1000);
  lcd.print("   ");
}
