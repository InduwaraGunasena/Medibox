#include <WiFi.h>
#include <mqtt.h>
#include <config.h>
#include <buzzer.h>
#include "freeRTOS\freeRTOS.h"
#include "freeRTOS\task.h"

volatile bool Jmp_to_Menu_Flag = false;

//Interrupt service routine (ISR) to handle MENU button presses.
void IRAM_ATTR Jmp_to_Menu(){
  detachInterrupt(PB_MENU_NEXT);
  Jmp_to_Menu_Flag = true;
  //Serial.println("Jmp_to_Menu_Flag = true");
}

void update_time_with_check_alarm();
void print_display_center(String text, int text_size);
void main_screen();
void check_temp();
void ring_alarm();
int displaymenu();
void displaySetAlarm(int position, int alarm_num);
void displaySetAlarmMenu(int alarm_num);
local_timezone set_time();
void enable_disable_alarms_menu();
void web_task(void *parameter);


void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(warning_LED, OUTPUT);
  pinMode(alarm_LED, OUTPUT);
  pinMode(PB_EXIT_SAVE, INPUT);
  pinMode(PB_MENU_NEXT, INPUT);
  pinMode(PB_UP, INPUT);
  pinMode(PB_DOWN, INPUT);
  pinMode(Right_LDR, INPUT);
  pinMode(Left_LDR, INPUT);

  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servo_pin, 500, 2400);

  attachInterrupt(PB_MENU_NEXT, Jmp_to_Menu, FALLING);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  timeArray[0].hours = EEPROM.read(0)%24;
  timeArray[0].minutes = EEPROM.read(1)%60;
  timeArray[1].hours = EEPROM.read(2)%24;
  timeArray[1].minutes = EEPROM.read(3)%60;
  timeArray[2].hours = EEPROM.read(4)%24;
  timeArray[2].minutes = EEPROM.read(5)%60;
  timeArray[0].hours = 11;
  timeArray[0].minutes = 45;
  timeArray[1].hours = 9;
  timeArray[1].minutes = 00;
  timeArray[2].hours = 6;
  timeArray[2].minutes = 30;

  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(23, 7);
  display.println("Welcome \n    to \n  Medibox!");
  display.display();

  starting_tone();
  delay(2000);

  print_display_center(" ", 1);  // Get the cursor into the middle position.

  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    print_display_center("Connecting to Wifi...", 1);
    delay(250);  
  }

  display.clearDisplay();
  print_display_center("Wifi connected", 1);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

  display.clearDisplay();

  client.setServer("test.mosquitto.org", 1883);
  client.setCallback(callback);
}

void loop() {
  xTaskCreate(web_task, "web_task", 2048, NULL, 2, NULL);
  update_time_with_check_alarm();
  //delay(1000);
    if(Jmp_to_Menu_Flag == true) {
      Jmp_to_Menu_Flag = false;
      while(digitalRead(PB_MENU_NEXT) == LOW) {
        delay(5);
      }
      
      switch(displaymenu()) {
        case SET_TIME:
            local_timezone a;
            a = set_time();
            UTC_OFFSET = (a.hours * 60 + a.minutes)*60;
            if (a.sign == '-'){
              UTC_OFFSET = -UTC_OFFSET;
            }
            configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
            break;
        case ALARM_1:
            displaySetAlarmMenu(1);
            break;
        case ALARM_2:
            displaySetAlarmMenu(2);
            break;
        case ALARM_3:
            displaySetAlarmMenu(3);
            break;
        case DISABLE_ALARM:
            enable_disable_alarms_menu();
            break;
        default:
            break;
      }
      while(digitalRead(PB_MENU_NEXT) == LOW) {
        delay(1000);
      }
      attachInterrupt(PB_MENU_NEXT, Jmp_to_Menu, FALLING);
  }
}

void web_task(void *parameter){
  if (!client.connected()) {
   reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > refresh_rate) { 
    lastMsg = now;
    float left = map(analogRead(Left_LDR), 0, 4095, 0, 1000) / 1000.0;
    float right = map(analogRead(Right_LDR), 0, 4095, 0, 1000) / 1000.0;
    
    String Left = String(left, 2);
    client.publish("/Medibox/leftLDR", Left.c_str()); 
    String Right = String(right, 2);
    client.publish("/Medibox/rightLDR", Right.c_str()); 
  }
  
  autorun_servo();
}


void update_time_with_check_alarm(){
  main_screen();

  for(int i = 0; i < n_alarms; i++){
    if (set_alarms[i] == true){
      if(alarm_triggered[i] == false && timeArray[i].hours == hours && timeArray[i].minutes == minutes){
        ring_alarm();
        alarm_triggered[i] = true;
      }
    }
  }
}


void print_display_center(String text, int text_size) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

  // display on horizontal and vertical center
  display.clearDisplay(); // clear display
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor((SCREEN_WIDTH - width*text_size) / 2, (SCREEN_HEIGHT - height*text_size) / 2);
  display.println(text); // text to display
  display.display();
}

void main_screen(){
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  hours = timeinfo.tm_hour;
  minutes = timeinfo.tm_min;
  seconds = timeinfo.tm_sec;

  display.clearDisplay(); // Clear the display
  display.setTextColor(SSD1306_WHITE); // Set text color
  
  display.setCursor(0, 0); // Set new position
  display.setTextSize(1);
  display.print("Time:");
  
  display.setTextSize(2);
  // Format and display hours
  if (hours < 10)
    display.print('0'); // print the front zero for every hours having one digit.
  display.print(hours);
  display.print(':');

  // Format and display minutes
  if (minutes < 10)
    display.print('0'); // print the front zero for every minutes having one digit.
  display.print(minutes);
  display.print(':');

  // Format and display seconds
  if (seconds < 10)
    display.print('0'); // print the front zero for every seconds having one digit.
  display.println(seconds);

  display.setCursor(0, 20);
  display.setTextSize(1);
  display.print("Upcoming Alarm:");

  
  // ################# THIS ALARM FILTERING PROCESS ALWAYS RUN. IT WILL AFFECT THE CODE EFFICIENCY. ######################
  // Find the nearest upcoming alarm
  int nearestAlarmIndex = -1; // Initialize index of nearest alarm
  int nearestAlarmDiff = INT_MAX; // Initialize difference to maximum value

  for (int i = 0; i < n_alarms; i++) {
    int alarmHours = timeArray[i].hours;
    int alarmMinutes = timeArray[i].minutes;

    int diff = (alarmHours - hours) * 60 + (alarmMinutes - minutes);

    // Calculate difference between current time and alarm time
    if (diff <= 0){
      diff = 24*60 - diff;
    }

    // Check if this alarm is nearer than the previous nearest one
    if (diff < nearestAlarmDiff && set_alarms[i] == true) {
      nearestAlarmDiff = diff;
      nearestAlarmIndex = i;
    }
  }

  display.setCursor(35, 30); // Set new position
  display.setTextSize(2);
  
  if(nearestAlarmIndex != -1){
    if (timeArray[nearestAlarmIndex].hours < 10)
      display.print('0');
    display.print(timeArray[nearestAlarmIndex].hours);
    display.print(':');
    if (timeArray[nearestAlarmIndex].minutes < 10)
      display.print('0');
    display.print(timeArray[nearestAlarmIndex].minutes);
  }else{
    display.print("--:--");  // if there is no alarms set or disabled all alarms.
  }

  check_temp();

  display.display();
}

void check_temp(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  
  unsigned long currentMillis = millis();
  if ((data.temperature > high_temp || data.temperature < low_temp || data.humidity > high_humidity || data.humidity < low_humidity) &&
      (currentMillis - previousMillisBlink >= warning_led_delay)) {
    previousMillisBlink = currentMillis; 
    warning_led_state = !warning_led_state;
    digitalWrite(warning_LED, warning_led_state);
    if(warning_led_state){
      tone(BUZZER, A);
    }else{
      noTone(BUZZER);
    }

  } else {
    digitalWrite(warning_LED, LOW);
    noTone(BUZZER);
  }

  static bool highTempDisplayed = false;
  static bool lowTempDisplayed = false;
  static bool highHumidityDisplayed = false;
  static bool lowHumidityDisplayed = false;

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  if (data.temperature > high_temp && !highTempDisplayed) {
    display.setCursor(0, 46); 
    display.setTextSize(1);
    display.print("HIGH Temperature!");
  }

  if (data.temperature < high_temp && highTempDisplayed) {
    highTempDisplayed = true;
  }

  if (data.temperature < low_temp && !lowTempDisplayed) {
    display.setCursor(0, 46); 
    display.setTextSize(1);
    display.print("LOW Temperature!");
  }

  if (data.temperature > low_temp && lowTempDisplayed) {
    lowTempDisplayed = true;
  }

  if (data.humidity > high_humidity && !highHumidityDisplayed) {
    display.setCursor(0, 56); 
    display.setTextSize(1);
    display.print("HIGH Humidity!");
  }

  if (data.humidity < high_humidity && highHumidityDisplayed) {
    highTempDisplayed = true;
  }

  if (data.humidity < low_humidity && !lowHumidityDisplayed) {
    display.setCursor(0, 56); 
    display.setTextSize(1);
    display.print("LOW Humidity!");
  }

  if (data.humidity > low_humidity && lowHumidityDisplayed) {
    lowTempDisplayed = true;
  }
}

void ring_alarm(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); // Set text color
  
  display.setCursor(17, 15); // Set new position
  display.setTextSize(2);
  display.print("MEDICINE");
  display.setCursor(40, 33); 
  display.setTextSize(2);
  display.print("TIME!");
  display.display();

  digitalWrite(alarm_LED, HIGH);

  bool break_happened = false;

  while(break_happened == false && digitalRead(PB_EXIT_SAVE)== HIGH){
    for(int i = 0; i < n_notes; i++){
      if(digitalRead(PB_EXIT_SAVE) == LOW){
        delay(50);
        break_happened = true;
        break;
      }
      tone(BUZZER, notes[i]);
      delay(500);
      noTone(BUZZER);
      delay(2);
    }
  }
  digitalWrite(alarm_LED, LOW);
  display.clearDisplay();
}

const int buttonDelay = 50;

Button_Direction readButton(void){
  //Serial.println("readButton");
  while(true)
  {
    if(digitalRead(PB_EXIT_SAVE) == LOW)
    {
      delay(buttonDelay);
      while(digitalRead(PB_EXIT_SAVE) == LOW);
      //Serial.println("readButton CANCEL");
      return EXIT_SAVE;
    }
    else if(digitalRead(PB_MENU_NEXT) == LOW)
    {
      delay(buttonDelay);
      while(digitalRead(PB_MENU_NEXT) == LOW);
      //Serial.println("readButton OK");
      return MENU_NEXT;
    }
    else if(digitalRead(PB_UP) == LOW)
    {
      delay(buttonDelay);
      while(digitalRead(PB_UP) == LOW);
      //Serial.println("readButton UP");
      return UP;
    }
    else if(digitalRead(PB_DOWN) == LOW)
    {
      delay(buttonDelay);
      while(digitalRead(PB_DOWN) == LOW);
      //Serial.println("readButton DOWN");
      return DOWN;
    }
  }
}


int displaymenu(){
    int position = 0;
    Button_Direction B_dir = UP;
    while(true)
    {
        display.clearDisplay();
        display.display();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE); 
        display.setCursor(0, 0);
        display.println(F("Main Menu"));
        display.println("");
        for (int i = 0; i < NumberOfOptions; i++) {
          if (i == position) {
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.println(options[i]);
          } else{
            display.setTextColor(SSD1306_WHITE);
            display.println(options[i]);
          }
        }
        display.display();
        B_dir = readButton();
        if(B_dir == UP)
        {
          if(position > 0)
          {
            position--;
          }
        }
        else if(B_dir == DOWN)
        {
          if(position < NumberOfOptions-1)
          {
            position++;
          }
        }   
        else if(B_dir == MENU_NEXT)
        {
          display.clearDisplay();
          display.display();
          return position;
        }  
        else if(B_dir == EXIT_SAVE)
        {
          display.clearDisplay();
          display.display();
          return -1;
        }  
  }
}

  
void displaySetAlarm(int position, int alarm_num){
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);

  display.println("Set Alarm - " + String(alarm_num));
  display.setCursor(0, 20); // Set new position
  display.setTextSize(2);

  if (position == 1)
  {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  }
  if (timeArray[alarm_num - 1].hours < 10)
    display.print('0');
  display.print(timeArray[alarm_num - 1].hours);
  if (position == 1)
  {
    display.setTextColor(SSD1306_WHITE);
  }  

  display.print(':');
    if (position == 2)
  {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  }
  if (timeArray[alarm_num - 1].minutes < 10)
    display.print('0');
  display.print(timeArray[alarm_num - 1].minutes);
    if (position == 2)
  {
    display.setTextColor(SSD1306_WHITE);
  }  
  display.display();
  
}


void incAlarm(int position, int alarm_num){
  if(position == 1)
  {
    if(timeArray[alarm_num - 1].hours < 23)
    {
      timeArray[alarm_num - 1].hours ++;
    }else{
      timeArray[alarm_num - 1].hours = 0;
    }
  }
  else if(position == 2)
  {
    if(timeArray[alarm_num - 1].minutes < 59)
    {
      timeArray[alarm_num - 1].minutes ++;
    }else{
      timeArray[alarm_num - 1].minutes = 0;
    }
  }
}


void decAlarm(int position, int alarm_num){
  if(position == 1)
  {
    if(timeArray[alarm_num - 1].hours > 0)
    {
      timeArray[alarm_num - 1].hours --;
    }else{
      timeArray[alarm_num - 1].hours = 23;
    }
  }
  else if(position == 2)
  {
    if(timeArray[alarm_num - 1].minutes > 0)
    {
      timeArray[alarm_num - 1].minutes --;
    }else{
      timeArray[alarm_num - 1].minutes = 59;
    }
  }
}

void displaySetAlarmMenu(int alarm_num){
  int position = 1;
  Button_Direction B_dir = UP;

  while(position != 0)
  {
    displaySetAlarm(position, alarm_num);
    B_dir = readButton();
    if(B_dir == UP)
    {
      incAlarm(position, alarm_num);
    }
    else if(B_dir == DOWN)
    {
      decAlarm(position, alarm_num);
    }
    else if(B_dir == MENU_NEXT)
    {
      if(position >  1)
      {
        position--;
      }else{
        position++;
      }
    }
    else if(B_dir == EXIT_SAVE)
    {
      EEPROM.write(alarm_num - 1, timeArray[alarm_num - 1].hours);
      EEPROM.write(alarm_num, timeArray[alarm_num - 1].minutes);
      set_alarms[alarm_num-1] = true;
      return;
    }        
  }
}


void set_timezone(int position, local_timezone zone){
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);

  display.println("Set timezone");
  display.setCursor(0, 20); // Set new position
  display.setTextSize(2);
  display.print("UTC");

  if (position == 0)
  {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  }
  display.print(zone.sign);
  if (position == 0)
  {
    display.setTextColor(SSD1306_WHITE);
  }


  if (position == 1)
  {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  }
  if (zone.hours < 10)
    display.print('0');
  display.print(zone.hours);
  if (position == 1)
  {
    display.setTextColor(SSD1306_WHITE);
  }  

  display.print(':');
    if (position == 2)
  {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  }
  if (zone.minutes < 10)
    display.print('0');
  display.print(zone.minutes);
    if (position == 2)
  {
    display.setTextColor(SSD1306_WHITE);
  }  
  display.display();
}


local_timezone set_time() {
  int position = 0;
  
  local_timezone zone; 
  zone.hours = 0;
  zone.minutes = 0;
  zone.sign = '+';
  
  Button_Direction B_dir = UP;

  while(position != -1)
  {
    set_timezone(position, zone);
    B_dir = readButton();
    if(B_dir == UP)
    {
          if(position == 0) {
            if(zone.sign == '+')
            {
              zone.sign = '-';
            }else{
              zone.sign = '+';
            }
          }
          else if(position == 1) {
            // Alarm 1 Hour
            if(zone.hours < 14)
            {
              zone.hours ++;
            }else{
              zone.hours = 0;
            }
          }
          else if(position == 2)
          {
            if(zone.minutes == 45)
            {
              zone.minutes = 0;
            }else if(zone.minutes == 30){
              zone.minutes = 45;
            }else{
              zone.minutes = 30;
            }
          }
    }
    else if(B_dir == DOWN)
    {
         if(position == 0) {
            if(zone.sign == '-')
            {
              zone.sign = '+';
            }else{
              zone.sign = '-';
            }
          }
         else if(position == 1)
          {
            // Alarm 1 Hour
            if(zone.hours > 0)
            {
              zone.hours --;
            }else{
              zone.hours = 14;
            }
          }
          else if(position == 2)
          {
             if(zone.minutes == 45)
            {
              zone.minutes = 30;
            }else if(zone.minutes == 30){
              zone.minutes = 0;
            }else{
              zone.minutes = 45;
            }
          }
    }
    else if(B_dir == MENU_NEXT)
    {
      if(position == 2)
      {
        position = 0;
      }else{
        position++;
      }
    }
    else if(B_dir == EXIT_SAVE)
    {
      return zone;
    }        
  }
}


void enable_disable_alarms_menu() {
    int position = 0;
    Button_Direction B_dir = UP;
    while(true) {
        display.clearDisplay();
        display.display();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE); 
        display.setCursor(0, 0);
        display.println(F("Enable/Disable Alarms"));
        display.println("");
        for (int i = 0; i < Options; i++) {
            if (i == position) {
                display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
                display.print(alarm_options[i]);
            } else {
                display.setTextColor(SSD1306_WHITE);
                display.print(alarm_options[i]);
            }

            // Print the current state of the alarm, unless it's the "Disable all alarms" option
            if (i != Options - 1) {
                if (set_alarms[i]) {
                    display.println(F("On"));
                } else {
                    display.println(F("Off"));
                }
            }
        }
        display.display();

        B_dir = readButton();
        if (B_dir == UP) {
            if (position > 0) {
                position--;
            }
        } else if (B_dir == DOWN) {
            if (position < NumberOfOptions - 1) {
                position++;
            }
        } else if (B_dir == MENU_NEXT) {
            if (position == Options - 1) { // "Disable all alarms" option selected
                // Set all elements of set_alarms to false
                for (int j = 0; j < n_alarms; j++) {
                    set_alarms[j] = false;
                }
            } else {
                // Toggle the state of the selected alarm
                set_alarms[position] = !set_alarms[position];
            }
        } else if (B_dir == EXIT_SAVE) {
            display.clearDisplay();
            display.display();
            return;
        }
    }
}


