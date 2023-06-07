#include <Wire.h>

#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define HL1 5 //Head Light Supply 1

#define HL2 7 //Head Light Supply 2

#define B1 4 //Beam toggle for Head light 1

#define B2 6 //Beam toggle for Head light 2

#define Engine A3 //Engine pin Always LOW

#define LB_SW A0 // Low Beam Control Switch

#define HB_SW A1 // High Beam Control Switch

#define HL_SW A2 //Head light Switch

#define Battery_Drain_SW 11 //Battery drain switch

#define on_key 9 //key pin for ON state

#define ign_key 10 //Key pin for Ignition state

#define MOTOR 2 //Key pin for MOTOR

#define BUZZER 13 //key pin for buzzer

#include <SoftwareSerial.h>

SoftwareSerial BTserial(0, 1); // RX, TX

String BT_input;

bool vehicleOnSent = false; //Bool set as false for later verification

bool VIP = false;

bool BATTERYDRAINOFF = false; 
  
bool MON = false;

float shuntvoltage = 0, busvoltage = 0, current = 0, loadvoltage = 0, power = 0;

int Head_Light, on, menu, start;

int HL_state, count, HL_disp;

void setup() {

 // put your setup code here, to run once:

 Serial.begin(9600);

 BTserial.begin(9600);

 pinMode(MOTOR, OUTPUT);

 pinMode(HL1, OUTPUT);

 pinMode(HL2, OUTPUT);

 pinMode(B1, OUTPUT);

 pinMode(B2, OUTPUT);

 digitalWrite(Engine, 0);

 digitalWrite(HL1, 1);

 digitalWrite(HL2, 1);

 digitalWrite(B1, 1);

 digitalWrite(B2, 1);

 pinMode(LB_SW, INPUT_PULLUP);

 pinMode(HB_SW, INPUT_PULLUP);

 pinMode(HL_SW, INPUT_PULLUP);

 pinMode(Battery_Drain_SW, INPUT_PULLUP);

 pinMode(Engine, INPUT_PULLUP);

 pinMode(on_key, INPUT_PULLUP);
  
 pinMode(ign_key, INPUT_PULLUP);

 lcd.begin();

 lcd.clear();

 lcd.setCursor(0, 0);

 lcd.print("SAFETY ASSURANCE");

 lcd.setCursor(0, 1);

 lcd.print(" SYSTEM ");

 Serial.println("SAFETY ASSURANCE SYSTEM");

 delay(2000);

 lcd.clear();

 if (!ina219.begin()) {

 Serial.println("Failed to find INA219 chip");

 while (1) { delay(10); }

 }

 delay(200);

}

void loop() {

 // put your main code here, to run repeatedly:

 busvoltage = ina219.getBusVoltage_V();

 current = ina219.getCurrent_mA() * 0.001;

 power = ina219.getPower_mW() * 0.001;

 // send from serial monitor to the Bluetooth device connected the HC05.

 if (Serial.available()) {

 BT_input = Serial.readString(); // read input string from bluetooth

 BTserial.write(Serial.read());

 }

 // print incoming data from the Bluetooth device connected the HC05 to the serial monitor
  
if (BTserial.available()) {

 Serial.print(char(BTserial.read()));

}

 if (digitalRead(on_key) == LOW) {

 on = 1;

 } else {

 on = 0;

 menu = 0;

 start = 0;

 lcd.clear();

 lcd.setCursor(0, 0);

 lcd.print("Put Key to ");

 lcd.setCursor(0, 1);

 lcd.print("START ... ");

 delay(50);

 }

 if (on == 1 and start == 0) {

 menu = 1;

 }

 if (digitalRead(ign_key) == LOW and digitalRead(Engine) == LOW) {

 start = 1;

 count = 0;

 menu = 2;

 HL_state = 0;

 delay(50);

 }

 if (digitalRead(ign_key) == LOW and digitalRead(Engine) == LOW && !MON) {

 digitalWrite(MOTOR, HIGH); // Turn on the motor

 delay(8000);

 digitalWrite(MOTOR, LOW);
  
 MON = true;

 }

 if (digitalRead(ign_key) == LOW and digitalRead(Engine) == HIGH) {

 lcd.clear();

 lcd.setCursor(0, 0);

 lcd.print("Theft detected");

 Serial.println(BT_input);

 Serial.println("THEFT DETECTED");

 lcd.setCursor(0, 1);

 lcd.print("Engine OFF");

 digitalWrite(BUZZER, HIGH); // Turn on the buzzer

 delay(5000);

 digitalWrite(BUZZER, LOW); // Turn off the buzzer

 lcd.clear();

 }

 if (menu == 1) {

 lcd.setCursor(0, 0);

 lcd.print(" VEHICLE ON ");

 lcd.setCursor(0, 1);

 lcd.print(" ");

 delay(100);

 }

 if (menu == 1 && !vehicleOnSent) {

 Serial.println(BT_input);

 Serial.println("VEHICLE ON");

 vehicleOnSent = true;

 }

 if (menu == 2) {

 lcd.setCursor(0, 0);

 lcd.print("V: I: ");

 lcd.setCursor(0, 1);
  
 lcd.print("Power: ");

 lcd.setCursor(2, 0);

 lcd.print(busvoltage, 1);

 lcd.print(" V");

 lcd.setCursor(11, 0);

 lcd.print(current, 1);

 lcd.print(" A");

 lcd.setCursor(6, 1);

 lcd.print(power, 1);

 lcd.print(" W");

 delay(150);

 }

 if (menu == 2 && !VIP) {

 Serial.println(BT_input);

 Serial.print("V: I: ");

 Serial.print("Power: ");

 Serial.print(busvoltage, 1);

 Serial.print(" V");

 Serial.print(current, 1);

 Serial.print(" A");

 Serial.print(power, 1);

 Serial.print(" W");

 VIP = true;

 }

 //-----------------For Battery Drain

 if (digitalRead(Battery_Drain_SW) == LOW and on == 1) {
 
 lcd.clear();

 lcd.setCursor(0, 0);

 lcd.print("Battery Drain");

 lcd.setCursor(0, 1);

 lcd.print("Detected");

 Serial.println(BT_input);

 Serial.println("BATTERY DRAINAGE DETECTED");

 delay(2000);

 lcd.clear();

 }

 //-------------------For HeadLight Supply On/off switch

 if (digitalRead(HL_SW) == LOW) {

 Head_Light = 1;

 HL_disp = 1;

 if (HL_state == 0) {

 HL_state = 1;

 digitalWrite(HL1, LOW);

 lcd.clear();

 lcd.setCursor(0, 0);

 lcd.print("Head Light - 1");

 lcd.setCursor(0, 1);

 lcd.print("ON");

 Serial.println(BT_input);

 Serial.println("PRIMARY HEADLIGHT - ON");

 delay(2000);

 power = ina219.getPower_mW() * 0.001;

 delay(100);

 lcd.clear();

 }

 if (HL_state == 1 and power <= 2.5) {

 HL_state = 2;

 lcd.clear();
             
 lcd.setCursor(0, 0);

 lcd.print("Head Light - 2");

 lcd.setCursor(0, 1);

 lcd.print("ON");

 Serial.println(BT_input);

 Serial.println("EMERGENCY HEADLIGHT - ON"); 

 delay(2000);

 lcd.clear();

 digitalWrite(HL2, LOW);

 digitalWrite(HL1, HIGH);

 }

 }

 if (digitalRead(HL_SW) == HIGH and Head_Light == 1) {

 Head_Light = 0;

 HL_state = 0;

 count = 0;

 digitalWrite(HL1, HIGH);

 digitalWrite(HL2, HIGH);

 if (HL_disp == 1) {

 HL_disp = 0;

 lcd.clear();

 lcd.setCursor(0, 0);

 lcd.print("Head Light");

 lcd.setCursor(0, 1);

 lcd.print("OFF");

 Serial.println(BT_input);

 Serial.println("HEADLIGHT - OFF");

 delay(2000);

 lcd.clear();

 }

 }
                                   
//-------------------For Low Beam , High Beam Control

 if (digitalRead(LB_SW) == LOW and Head_Light == 1) {

 digitalWrite(B1, HIGH);

 digitalWrite(B2, HIGH);

 } else if (digitalRead(HB_SW) == LOW and Head_Light == 1) {

 digitalWrite(B1, LOW);

 digitalWrite(B2, LOW);

 } else {

 digitalWrite(B1, HIGH);

 digitalWrite(B2, HIGH);

 }

 //-------------------For Head Light Power Efficient Mode

 if (Head_Light == 1 and on == 0 and start == 0) {

 count++;

 if (count >= 30) {

 digitalWrite(HL1, HIGH);

 digitalWrite(HL2, HIGH);

 if (HL_disp == 1 && !BATTERYDRAINOFF) {

 HL_disp = 0;

 lcd.clear();

 lcd.setCursor(0, 0);

 lcd.print("Head Light");

 lcd.setCursor(0, 1);

 lcd.print("OFF");

 Serial.println(BT_input);

 Serial.println("HEADLIGHT - OFF");

 BATTERYDRAINOFF = true;

 delay(2000);

 lcd.clear();                            
 
  }

 }

 }

 delay(350);

}