# Medibox

When you have any disease you need medicine to recover. Taking medicines on time is essential for recover fast. Today almost all the people are very busy. So they can mistake their doses. So this medibox will help you to do this on time.

Medibox is a device used to remind users to take their medicine on time using an ESP32. This device reminds you every doses with relevent medicine on that dose. Therefore, you will never miss any medicine or dose again.

## Requirements what I consider

Now you already know this is a device that reminds paticients to take their medicies on time. To fullfill this I identified several requirements as listed below.

- ## functional requirements

  - **Device must remind the relevent doses on time:** The device will remind doses by using its buzzer and LED.
  - **Device should store the medicines under secure container:** The shaded sliding window is used to prevent the excessive light from entering the medibox. It has a DHT sensor to measure the temperature and humidity of the environment.

- ## non-functional requirements
  - **The time should be accurately represent the current time:** To do this, the device update the time by usin NTP server. Also the user can also setup their timezone.
  - **The device should last long time:** The device uses several power saving mechanisms to save the power. The display will go to sleep after 1 minute from the last user button press. The microcontroller always go to deep sleep mode if there is no user interaction.
  - **User friendly device:** It has a simple design and a user interface in both physical and web based interface to interact with the device. Users can change and visualize everything using both interfaces.

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
You can use **diagram.json** file to build the circuit in the Wokwi platform. 

![Wiring diagram](/images/wiring_diagram.png)

> [!NOTE]
> This project is fully done in Wokwi platform. Therefore, there are some issues can arrise when it implement in real world.

## How this work

### Installing Wokvi for VS code and build your workspace

I use Wokwi to build my project. Since this is a big project, you have to use VS code extension for the Wokwi. To install, please refer [this page](https://docs.wokwi.com/vscode/getting-started) for more information.

You can also watch [this video](https://www.youtube.com/watch?v=fUlAPdekVO0) to install the extension correctly.

Also you need to install PlatformIO extension. After install it, you can create a new project using PIO Home page. I named it as '**Medibox**'. Since I use ESP32 Dev kit V1 as my microcontroller, you can choose '**DOIT ESP32 DEVKIT V1**' as your board. Choose framework as '**Arduino**'.
![Create a new project on PlatformIO](/images/create_new_project.png)
### Code files

In my code directory, There are several folders in it.

- All the neccesory library files in the **lib** folder.
- The main code file which is **main.cpp** in the **src** folder.
- All the header files in the **include** folder. In **include** file, there are 3 header files called, **buzzer.h**, **config.h** and **mqtt.h**.
  - All the tone and buzzer related function, constants are in the **buzzer.h** file.
  - All the pin configurations variables and constants are in the **config.h** file.
  - All the web based activities and functions are in the **mqtt.h**

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

### NodeRED Dashboard
For web based dashboard I used NodeRED dashboard. It is a very simple platform to build an attractive dashboards. All the necessory files to build NodeRED dashboard are in the **Node-RED dashboard** folder. You can import the **flows.json** file into your NodeRED application and change it as you want.


## Message from the developer
I really welcome everyone who are like to develop, improve and correct bugs. You can freely fork this project and add anything you want. But **please secure the credits of author**:grin:.

Thank you :heart:
