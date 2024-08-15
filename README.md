# Medibox
Managing medications is crucial for recovery from illness, but in today's busy world, it’s easy to miss doses. Medibox is a device designed to remind users to take their medicine on time, ensuring they never miss a dose. Using an ESP32 microcontroller, Medibox alerts users when it’s time to take their medication, displaying the relevant details for each dose.

## Considered Requirement
Medibox is designed to help patients adhere to their medication schedules. To achieve this, several requirements were identified:

- ## functional requirements

  - **Timely Reminders:** The device alerts users to take their medicine using a buzzer and LED.
  - **Secure Storage:** Medibox stores medicines in a secure container, with a shaded sliding window to protect from excess light and a DHT sensor to monitor temperature and humidity.

- ## non-functional requirements
  - **Accurate Timekeeping:** The device synchronizes time using an NTP server and allows users to set their timezone.
  - **Long Battery Life:** Power-saving features include a display that sleeps after 1 minute of inactivity and deep sleep mode for the microcontroller when not in use.
  - **User-Friendly Design:** Medibox features both physical and web-based user interfaces, making it easy to interact with the device and manage settings.

## Components which I use

1. 0.96 inch 128X64 OLED Display Module Blue I2C IIC
2. ESP32 Dev kit V1
3. Push buttons
4. Passive buzzer
5. Green and Red color LEDs
6. DHT22 Temperature Humidity Sensor
7. Resistors
8. SG90 servo motor

## Wiring diagram
Use the **diagram.json** file to build the circuit on the Wokwi platform.

![Wiring diagram](/images/wiring_diagram.png)

> [!NOTE]
> This project is fully done in Wokwi platform. Therefore, there are some issues can arrise when it implement in real world.

Figure 2 shows the basic architecture of this project. I uses, test.mosquitto.org/ as the broker.
<p align="center">
<img src="/images/architecture" alt="Dashboard" width="600"/>
</p>

> **Reference** :
> R. A. Light, "Mosquitto: server and client implementation of the MQTT protocol," The Journal of Open Source Software, vol. 2, no. 13, May 2017, DOI: 10.21105/joss.00265

## How this work

### Setting Up Wokwi for VS Code
This project is built using Wokwi. Due to its size, you’ll need to use the Wokwi extension for VS Code. For installation instructions, refer to [this page](https://docs.wokwi.com/vscode/getting-started). You can also watch [this video](https://www.youtube.com/watch?v=fUlAPdekVO0) for guidance.

You’ll also need the PlatformIO extension. After installing it, create a new project in the PIO Home page and name it **Medibox**. Choose **DOIT ESP32 DEVKIT V1** as the board and **Arduino** as the framework.

<p align="center">
<img src="/images/create_new_project.png" alt="Create a new project on PlatformIO" width="600"/>
</p>

### Code files
The project directory is organized as follows:

- The **lib** folder contains all necessary library files.
- The src folder contains the main code file, **main.cpp**.
- The include folder contains header files:
  - **buzzer.h**: Contains tone and buzzer-related functions and constants.
  - **config.h**: Contains pin configuration variables and constants.
  - **mqtt.h**: Contains web-based activities and functions.
   
### Display Interfaces
Medibox features several display interfaces on the OLED screen. Users can navigate through options using the UP and DOWN buttons, select options with the MENU/NEXT button, and save or exit with the EXIT/SAVE button.

1. Welcome page  
   <img src="/images/welcome.png" alt="Welcome Page" width="300"/>

2. Main page  
   <img src="/images/main_screen.png" alt="Main Screen Page" width="300"/>

3. Menu page  
   <img src="/images/main_menu.png" alt="Menu Page" width="300"/>

4. Set Timezone page  
   <img src="/images/set_timmezone.png" alt="Set Timezone Page" width="300"/>

5. Set alarm page (There are 3 independent interfaces for each 3 alarms)  
   <img src="/images/set_alarm_display.png" alt="Set Alarm Page" width="300"/>

6. Enable/Disable alarms page  
   <img src="/images/enable_disable_alarm_menu.png" alt="Enable/Disable Alarms Page" width="300"/>

To control the shaded sliding window I use this equation to calculate the servo motor angle.

$$
\theta = \min\left( \theta_{\text{offset}} \times D + (180 − \theta_{\text{offset}}) \times I \times \gamma , 180 \right)
$$


where,
- $\theta$ is the motor angle
- $\theta$<sub>offset</sub> is the minimum angle (default value of 30 degrees)
- $I$ is the max intensity of light, ranging from 0 to 1
- $\gamma$ is the controlling factor (default value of 0.75)
- D = 0.5 if right LDR gives max intensity, D = 1.5 if left LDR gives max intensity

### Node-RED Dashboard
For the web-based dashboard, I used Node-RED. It’s a simple platform for building attractive dashboards. The necessary files are in the **Node-RED dashboard** folder. Import the **flows.json** file into your Node-RED application and modify it as needed.

The Node-RED dashboard has three interfaces:
1. Dashboard
  <p align="center">
  <img src="/images/dashboard.png" alt="Dashboard" width="600"/>
  </p>
2. Alarms
  <p align="center">
  <img src="/images/set_alarm.png" alt="Set Alarms" width="600"/>
  </p>
3. Settings
  <p align="center">
  <img src="/images/settings.png" alt="Settings" width="600"/>
  </p>

## Message from the developer
I welcome contributions to this project. Feel free to fork the repository, add improvements, and fix bugs. However, **please ensure proper credit is given to the author**:grin:.

Thank you :heart:
