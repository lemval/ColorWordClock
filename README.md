Hi,

This is a personal project with lots of effort to get it up and running.
If you have build a clock, please give a little credit to this project. 
This will be greatly appreciated via [Paypal](https://www.paypal.me/valkyer).

The layout of the clock is Dutch! However, it will be pretty easy to create the clock in any language as long as it fits in text. Design your clockface first before proceeding.

** Hardware

Besides some small stuff, you need the following as mentioned below. Costs will be between 50 and 100 euro on material.

*** Board
Using model esp32-c6 16MB Devkit, which is easy to use and connect (although it is larger in size). You could currently use a 8MB model if you like (4MB will not be enough unless you disable OTA and use a different partition table). 

*** Light sensor
Using model 5516, Power with 3.3V and place a 4K7 resistor between data and ground (3.3V - LDR  - SENSOR - 4K7 - GND).

*** LED strip
You need 173 leds (13 x 13 + 4) on a 60D RGB strip, totaling to 3 meter. Use model HD107HD/APA107 (60 leds per meter) or alike, but make sure you can cut each LED individually. Using another model means also changing led_array.h (at FastLED.addLeds).

FastLED supports the SK6812 WWA version for warm/cold white. The upcoming release uses SK6812WWA (alias to SK6812Controller). See [FastLED/FastLED#2347](https://github.com/FastLED/FastLED/pull/2347). I currently have no knowledge if the software will run nicely using such strip.

*** Power adapter
A 5 volt power adapter (preferrably USB C but other connectors work) able to deliver 45 Watt.

*** Power connector

The design is configured for a 20*14*6.5 mm inlet. See for example [here](https://nl.aliexpress.com/item/1005009400273099.html).

*** Materials

Note: you need a laser cutter or have access to one!
All layers should measure at least 30cm x 30cm square

* Ground layer: 6 mm basewood (or alike)
* LED layer: 2 mm MDF
* Control layer: 6 mm MDF or basewood
* Diffusion layer: A4 size tracing paper
* Text layer: 0.5mm Lasersheet
* Front layer: 3 mm Tinted grey transparent polycarbonate (or alike)
* Frame: Your choice

I created frames from wood (milled) and alu (U-profile). Be creative.
Be careful to not glue the front and text layer. It will show.

** Software

Change settings.h and platform.ini to your own configuration

Compile, run and upload using Visual Studio Code with PlatformIO as plugin.

When started, connect to Wifi network 'Clock' and navigate to 192.168.4.1 to configure the Clock for first use. After that, reconnect to a regular Wifi network and open the browser on the assigned IP address.

Once your clock is closed up you can only update via OTA.

** Pin layout

04 Settings - select (connect other side to GND)
05 Settings - next (connect other side to GND)
06 Light sensor (3.3V - LDR  - SENSOR - 4K7 - GND)
19 LED strip - clock
20 LED strip - data

** Control

The settings buttons will allow you to configure the clock on a basic level. To configure on a finer level, just open the browser to the clock (not very reactive, since the clock is working on showing you the time and transitions) or use the Home Assistant integration (just configure MQTT via the browser, run a MQTT server and let Home Assistant read from that server).

TODO button behavior


** Attibutes

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

