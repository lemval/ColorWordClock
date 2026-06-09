# Dutch Color clock

Hi,

This is a personal project with lots of effort to get it up and running.
If you have build a clock, please give a little credit to this project. 
This will be greatly appreciated via [Paypal](https://www.paypal.me/valkyer).

The layout of the clock is Dutch! However, it will be pretty easy to create the clock in any language as long as it fits in text. Design your clockface first before proceeding.

## The clock

This is the first version I've build. It uses an aluminium U-frame to hold all layers together. Wood (milled) is easier to work with. It also still uses a DC power connector. Later models are equiped with a USB C connector.

![First version](instructions/images/First%20version.JPG "The first clock")

All configurations can be done via the website on the clock.

### Faces

1. Warm (text is warm white)
1. Custom (text is a custom color)
1. Rainbow (text uses a rainbow spectrum as seen in the picture above)
1. Animate (same, but slowly shifting)
1. ColorWord (each word another color)
1. ColorChar (each character another color)

The seconds can blink in a color of your own choice or can be switched off.

### Transitions

1. None (no transition)
1. Random (each minute a random pick of transition)
1. Animate (each minute a random FastLED pattern)
1. Flow down (characters fall down...)
1. Fade (text fades out and in)
1. Swipe (remove and reappear by swipe from bottom to top and back)
1. Scanner (same, but starting from right)
1. Spiral (FastLED spiral)
1. Fire (FastLED fire)
1. Rings (FastLED rings)
1. Wave (FastLED waves)
1. Mill (FastLED rotating like a mill)

You have the option to set the duration and animation speed.

### Status bar

The status bar is by default the date in binary. You have the option for some fixed patterns or use the 13 LEDs as status fields from Home Assistant. I tend to like having the date displayed.

## Hardware

Besides some small stuff, you need the following as mentioned below. Costs will be between 50 and 100 euro on material.

### Board
The clock can fit a model esp32-c6 devkit, which is easy to use and to connect, although the devkit is larger in size.
 
You need at least a 4MB card, however it has only be tested on a 8MB and 16MB card. If you skip OTA capability (you then have to disassemble the clock to update!) you can use a 2MB card.

### Light sensor
Using model 5516, power with 3.3V and place a 4K7 resistor between data and ground (3.3V - LDR  - SENSOR - 4K7 - GND).

### LED strip
You need 173 leds (13 x 13 + 4) on a 60D RGB strip, totaling to 3 meter. Use model HD107HD/APA107 (60 leds per meter) or alike, but make sure you can cut each LED individually. Using another model means also changing led_array.h (at FastLED.addLeds).

FastLED supports the SK6812 WWA version for warm/cold white. The upcoming release uses SK6812WWA (alias to SK6812Controller). See [FastLED/FastLED#2347](https://github.com/FastLED/FastLED/pull/2347). I currently have no knowledge if the software will run nicely using such strip.

### Power adapter
A 5 volt power adapter (preferrably USB C but other connectors work) able to deliver at least 30W (6A) but preferrably 45W (9A).

### Power connector

The design is configured for a 20 x 14 x 6.5 mm inlet. See for example [here](https://nl.aliexpress.com/item/1005009400273099.html).

### Materials

Note: you need a laser cutter or have access to one!
All layers should measure at least 30cm x 30cm square

* Ground layer: 6 mm basewood (or alike)
* LED layer: 2 mm MDF
* Control layer: 6 mm MDF (or basewood)
* Diffusion layer: A4 size tracing paper
* Text layer: 0.5mm Lasersheet
* Front layer: 3 mm Tinted grey transparent polycarbonate (or alike)
* Frame: Your choice

I created frames from wood (milled) and alu (U-profile). Be creative.
Be careful to not glue the front and text layer. It will show.

## Software

Create a `settings.h` based on `settings.h.template`. You can show a 'happy birthday' on given dates as transition.

Create a `platformio-local.ini` in the same manner. This allows for using your own password for OTA updates.

Change to your board size in `platformio.ini` by using one of the preconfigured board size config files (set to 16MB standard).

Compile, run and upload using Visual Studio Code with PlatformIO as plugin.

When started, connect to Wifi network 'Clock' and navigate to 192.168.4.1 to configure the Clock for first use. After that, reconnect to a regular Wifi network and open the browser on the assigned IP address.

Once your clock is closed up (assembled) you can only update via OTA.

## Pin layout

* 04 Settings - select (connect other side to GND)
* 05 Settings - next (connect other side to GND)
* 06 Light sensor (3.3V - LDR - SENSOR - 4K7 - GND)
* 19 LED strip - clock
* 20 LED strip - data

## Control

The settings buttons will allow you to configure the clock on a basic level. To configure on a finer level, just open the browser to the clock (not very reactive, since the clock is working on showing you the time and transitions) or use the Home Assistant integration (just configure MQTT via the browser, run a MQTT server and let Home Assistant read from that server).

### Configuration

Press SELECT once to enter settings mode. Press NEXT to cycle through the options of the setting or press SELECT again for the next setting or for closing the settings mode.

Settings are:
1. Change clockface
1. Change transition
1. Activate or disable light sensor
1. Activate or disable seconds
1. Set transition time 

Active = white, disabled = blue.

### Reset

Press NEXT, followed by SELECT. Lower left LED will turn green. Keep pressed and the LED will turn orange. When it turns red after 5 seconds, the clock and network will reset. You have to reconnect to the AP 'Clock' after that.

## Attributes

The following components or libraries are used:

* [WiFiManager by tzapu](https://github.com/tzapu/WiFiManager) (MIT license)
* [home-assistant-integration by dawidchyrzynski](https://github.com/dawidchyrzynski/arduino-home-assistant) (AGPL-3.0 license)
* [NTPClient by arduino-libraries](https://github.com/arduino-libraries/NTPClient) (MIT license)
* [TimeZone by jchristensen](https://github.com/JChristensen/Timezone) (GPL-3.0 license)
* [GoogleTest by Google](https://github.com/google/googletest) (BSD-3-Clause license)
* [FastLED by fastled](https://github.com/FastLED/FastLED) (MIT license)
* [Visual Studio Code](https://code.visualstudio.com/) for coding
* [PlatformIO](https://platformio.org/) for ESP support
* [OpenSCAD](https://openscad.org/) for designing the clock layout
* [LaserGRBL](https://lasergrbl.com/) for burning the layout
* [Cliche](https://www.1001fonts.com/cliche-font.html) for the clock text

