 #ifndef CONFIG_H_
 #define CONFIG_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <DHTesp.h>
#include <EEPROM.h>
#include <ESP32Servo.h>


// EEPROM settings
#define EEPROM_SIZE 6  //EEPROM memory in bytes.
//################################################################################


// All pinout configurations are listed here.
#define Left_LDR         34
#define Right_LDR        26
#define servo_pin         6 
#define BUZZER            5
#define alarm_LED        15
#define warning_LED       2
#define PB_EXIT_SAVE     35
#define PB_MENU_NEXT     32
#define PB_UP            33
#define PB_DOWN          25
#define DHT_PIN          12
//################################################################################


// Button settings. 
enum Button_Direction { //This defines an enumeration representing button directions
  UP,
  DOWN,
  MENU_NEXT,
  EXIT_SAVE
}; 
//################################################################################


// All OLED display configurations are here.
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT    64
#define OLED_RESET       -1
#define SCREEN_ADDRESS 0X3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Alarm options 
const int Options = 4;
const char *alarm_options[Options] = {
    " Alarm (1): ",
    " Alarm (2): ",
    " Alarm (3): ",
    " Disable all alarms"
};

// Menu options
const int NumberOfOptions = 5;
const char *options[NumberOfOptions] = {
    " Set Time",
    " Set alarm - (1)",
    " Set alarm - (2)",
    " Set alarm - (3)",
    " Disable alarms"
};

//This defines an enumeration representing different options in the main menu.
enum displaymenu_options {
  SET_TIME = 0,
  ALARM_1,
  ALARM_2,
  ALARM_3,
  DISABLE_ALARM
}; 
//################################################################################



// All DHT sensor configurations are here.
DHTesp dhtSensor;

const int high_temp = 32;      //Temperature high value
const int low_temp   = 26;     //Temperature low value
const int high_humidity = 80;  //Humidity high value
const int low_humidity = 60;   //Humidity low value
//################################################################################


// All buzzer configurations and neccery tones are here.
int n_notes = 8;
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[] = {C, D, E, F, G, A, B, C_H};

//welcome melody tones
int melody[] = {
  262, 196, 196, 220, 196, 0, 247, 262
};
//welcome melody tone duration
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
//################################################################################


//Relevent time settings.
#define NTP_SERVER     "pool.ntp.org" //constants for NTP server and UTC offset.
#define UTC_OFFSET_DST  0
long UTC_OFFSET =  0;

//This defines a structure to hold hours and minutes for alarm times.
struct alarmTime {
  int hours;
  int minutes;
};

// timezone
struct local_timezone {
  int hours;
  int minutes;
  char sign;
};

const int n_alarms = 3;
alarmTime timeArray[n_alarms];
bool alarm_triggered[] = {false, false, false};
bool set_alarms[] = {false, false, false};

int seconds = 0;
int minutes = 0;
int hours = 0;
//################################################################################


// Servo motor configurations
Servo myservo;
//################################################################################


// LED settings
bool warning_led_state            = false;
unsigned long warning_led_delay   = 200;
unsigned long previousMillisBlink = 0; 
//################################################################################


#endif /* CONFIG_H_ */
