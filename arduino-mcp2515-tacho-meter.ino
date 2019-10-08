/**
 * Create by Tonci14
 * Any modifications or redistributions allowed. Happy Coding ;-)
 */
 
#include <MCP2515.h> // the OBD2 library depends on the CAN library
#include <OBD2.h>
#include <Arduino.h>
#include <U8g2lib.h>

#define DEMO false

#define CS_PIN 31
#define INT_PIN 29    

#define PID_ENGIN_PRM       0x0C
#define PID_VEHICLE_SPEED   0x0D
#define PID_COOLANT_TEMP    0x05

byte COLOR_WHITE[3]        = {0xFF, 0xFF, 0xFF};
byte COLOR_BLACK[3]        = {0x00, 0x00, 0x00};
byte COLOR_RED[3]          = {0xFF, 0x00, 0x00};
byte COLOR_GREEN[3]        = {0x00, 0xFF, 0x00};
byte COLOR_BLUE[3]         = {0x00, 0x00, 0xFF};
byte COLOR_VIOLET[3]       = {0x9A, 0x32, 0xCD};
byte COLOR_ORANGE[3]       = {0xFF, 0xA5, 0x00};
byte COLOR_CYAN[3]         = {0x98, 0xF5, 0xFF};
byte COLOR_GOLD[3]         = {0xFF, 0xB9, 0x0F};

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

int rpm = 0; 
int temp = 0;
int spd = 0;

int demoRPM[4] = { 5, 55, 555, 5555};
int demoTMP[4] = {-55, -5,  5, 55};
int demoSPD[4] = {5,55,555,555};

const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

void setup() {
  Serial.begin(500000);
  u8g2.begin();
  draw();
  
  while (!DEMO) {
    Serial.print(F("Connect to CAN bus ... "));
    OBD2.setPins(CS_PIN, INT_PIN);
//    OBD2.setClockFrequency(8E6);
    if (!OBD2.begin(500E3)) {
      Serial.println(F("failed!"));
      rpm = 0; 
      temp = 0;
      spd = 0;
      delay(1000);
    } else {
      Serial.println(F("success"));
      break;
    }
  }
}

void loop() {
  if(!DEMO){
    processPid(PID_ENGIN_PRM);
    processPid(PID_VEHICLE_SPEED);
    processPid(PID_COOLANT_TEMP);
    draw();
  } else {
    for(int i=0; i <150; i++) {
      rpm = i * 20;
      temp = i - 20;
      spd = i;
      draw();
      delay(100);
    }
  }
}

void processPid(int pid) {
  if (!OBD2.pidSupported(pid)) {
    // PID not supported, continue to next one ...
    return;
  }

  if (!OBD2.pidValueRaw(pid)) {
    // read the PID value
    float pidValue = OBD2.pidRead(pid);

    switch (pid) {
      case PID_ENGIN_PRM: 
        rpm = pidValue;
        break;
      case PID_COOLANT_TEMP: 
        temp = pidValue;
        break;
      case PID_VEHICLE_SPEED: 
        spd = pidValue;
        break;    
    }

    if (!isnan(pidValue)) {
      Serial.print(pidValue);
      Serial.print(F(" "));
      Serial.print(OBD2.pidUnits(pid));
      Serial.println();
    }
  }
}

void draw()
{
  u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7Segments_26x42_mn);
    if(spd >= 100) {
      u8g2.setCursor(21, 42);       
    } else if(spd >= 10) {
      u8g2.setCursor(53, 42);
    } else {
      u8g2.setCursor(85, 42);
    }
    u8g2.print(spd);
    
    u8g2.setFont(u8g2_font_fur14_tn);
    if(temp >= 100) {
      u8g2.setCursor(80, 64); // 555    
    } else if(temp >= 10) {
      u8g2.setCursor(90, 64); //  55
    } else if(temp >= 0) {
      u8g2.setCursor(101, 64); //   5
    } else if(temp <= -10) {
      u8g2.setCursor(83, 64); // -55
    } else {
      u8g2.setCursor(94, 64); //  -5
    }
    u8g2.print(temp);    
    
    u8g2.setFont(u8g2_font_fur14_tn);
    if(rpm >= 1000) {
      u8g2.setCursor(3, 64);      
    } else if(rpm >= 100) {
      u8g2.setCursor(14, 64);
    } else if(rpm >= 10) {
      u8g2.setCursor(25, 64);
    } else {
      u8g2.setCursor(36, 64);
    }
    u8g2.print(rpm);    

    
    u8g2.setFont(u8g2_font_courR10_tf);
    u8g2.setCursor(48, 64); 
    u8g2.print("RPM"); 
    
    u8g2.setCursor(113, 64);
    u8g2.print(DEGREE_SYMBOL);   
    u8g2.setCursor(118, 64); 
    u8g2.print("C"); 
  u8g2.sendBuffer();
}
