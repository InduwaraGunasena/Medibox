#ifndef MQTT_H_
#define MQTT_H_

#include <PubSubClient.h>
#include <config.h>

WiFiClient espClient;
PubSubClient client(espClient);

float r = 0.75; // controlling factor
int offset = 30;
int refresh_rate = 500; // Sending data refresh rate

unsigned long lastMsg = 0;

void autorun_servo()
{
  float left = map(analogRead(Left_LDR), 0, 1023, 0, 1000) / 1000.0;
  float right = map(analogRead(Right_LDR), 0, 1023, 0, 1000) / 1000.0;

  float D = 0.0;
  float I = 0.0;

  if (left > right)
  {
    D = 1.5;
    I = left;
  }
  else
  {
    D = 0.5;
    I = right;
  }
  int expression = (offset * D) + (180 - offset) * I * r;
  int angle = min(180, expression);

  myservo.write(angle);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // Serial.print("Message arrived [");
  // Serial.print(topic);
  // Serial.print("] ");

  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  // Serial.println(message);

  if (strcmp(topic, "/Medibox/offset") == 0)
  {
    Serial.print("offset: ");
    offset = message.toInt();
    Serial.println(offset);
  }

  if (strcmp(topic, "/Medibox/control_factor") == 0)
  {
    Serial.print("control factor: ");
    r = message.toFloat();
    Serial.println(r);
  }

  if (strcmp(topic, "/Medibox/timezone") == 0)
  {
    local_timezone zone_fromWeb;
    Serial.print("timezone: ");
    zone_fromWeb = message.toFloat();
    Serial.print(zone_fromWeb.sign);

    long UTC_OFFSET1 = (zone_fromWeb.hours * 60 + zone_fromWeb.minutes) * 60;
    if (zone_fromWeb.sign == '-')
    {
      UTC_OFFSET1 = -UTC_OFFSET;
    }
    configTime(UTC_OFFSET1, UTC_OFFSET_DST, NTP_SERVER);
  }

  if (strcmp(topic, "/Medibox/alarm1") == 0)
  {
    Serial.print("alarm 1: ");
    timeArray[0] = message.toFloat();
    Serial.println(r);
    EEPROM.write(0, timeArray[0].hours);
    EEPROM.write(1, timeArray[0].minutes);
    set_alarms[0] = true;
  }

  if (strcmp(topic, "/Medibox/alarm2") == 0)
  {
    Serial.print("alarm 2: ");
    timeArray[0] = message.toFloat();
    Serial.println(r);
    EEPROM.write(1, timeArray[1].hours);
    EEPROM.write(2, timeArray[1].minutes);
    set_alarms[1] = true;
  }

  if (strcmp(topic, "/Medibox/alarm3") == 0)
  {
    Serial.print("alarm 3: ");
    timeArray[0] = message.toFloat();
    Serial.println(r);
    EEPROM.write(2, timeArray[2].hours);
    EEPROM.write(3, timeArray[2].minutes);
    set_alarms[2] = true;
  }

  if (strcmp(topic, "/Medibox/disable_enable_all_alarm") == 0)
  {
    Serial.print("all alarm: ");
    r = message.toInt();
    if (r == 1)
    {
      Serial.println(" Enable");
      for (int j = 0; j < n_alarms; j++)
      {
        set_alarms[j] = true;
      }
    }
    else
    {
      Serial.println(" Disable");
      for (int j = 0; j < n_alarms; j++)
      {
        set_alarms[j] = false;
      }
    }
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESPClient123"))
    {
      Serial.println("Connected");
      client.subscribe("/Medibox/control_factor");
      client.subscribe("/Medibox/offset");

      client.subscribe("/Medibox/timezone");
      client.subscribe("/Medibox/alarm1");
      client.subscribe("/Medibox/alarm2");
      client.subscribe("/Medibox/alarm3");
      client.subscribe("/Medibox/disable_enable_all_alarm");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

#endif /* CONFIG_H_ */
