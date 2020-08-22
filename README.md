# grinder_timer

## Rev2

More information can be found at my [project page][link_clews_grinder_timer_rev2].

Improvements since rev1:

- Hardware:
	- [X] Pull-ups on I2C SDA and CLK lines.
	- [X] Drive relay with a transistor instead of direct from IO.
	- [X] Add flyback diode protection to relay coil.
	- [X] Smaller transformer.
	- [X] Smaller relay.
	- [X] Switch to SMD where possible.
	- [X] Two-sided PCB (have it fabricated instead of etching).
	- [X] Switch buttons from mushy panel-mount to tactile SMD.
	- [X] Balanced caps for the 32.768kHz crystal.
	- [X] Put grind button LED on a PWM output.
- Firmware:
	- [X] Switch from C to C++.
	- [X] New OLED driver (change from SPI SSD1306 to IIC SH1106 controller).
	- [X] Pulse/fade effect for grind button LED.
	- [X] Ability to cancel a grind once started.
	- [X] Remember selected preset after a power-cycle.

Repo contents:
- [data][link_repo_data]: Datasheets and specs for the various components used.
- [firmware][link_repo_firmware]: The code witten in C++ and developed in VSCode with the PlatformIO IDE.
- [hardware][link_repo_hardware]: KiCad files including the [schematic][link_repo_schematic] and PCB layout.
- [rev1_archive][link_repo_rev1_archive]: Firmware developed for the original grinder_timer (written in C).

![The completed grinder_timer_rev2.][image_grinder_timer_rev2]
---

## Rev1

This is a programmable timer tethered to a [Rancilio Rocky][link_web_rancilio_rocky] coffee grinder. The Rocky is a "dumb" grinder and electrically very simple. Basically it's an ac motor with an on/off switch and a momentary push-button to grind. This project adds an electronic timer so that repeatable grind quantities can be produced without having to stand over the grinder. Features:
- Four programmable presets (A, B, C or D) stored in EEPROM to survive powercycle.
- Left/Right buttons to select preset.
- Up/Down buttons to adjust grind duration for selected preset (0.25s increments up to 60s).
- Grind button to initiate grind and begin countdown timer.
- Oled display (128x64) to show preset menu and countdown timer.
- Aluminium enclosure.
- Single cable (3-core) tether to grinder for power and control.

The code, schematic and PCB layout are all included here on gitlab.  More information can be found at my [project page][link_clews_grinder_timer].

Proudly mentioned at [hackaday.com][link_web_hackaday_grinder_timer].

![The completed grinder_timer.][image_grinder_timer]

[link_clews_grinder_timer]:https://clews.pro/projects/grinder_timer.php
[link_clews_grinder_timer_rev2]:https://clews.pro/projects/grinder_timer_rev2.php

[link_repo_data]:data/
[link_repo_firmware]:firmware/
[link_repo_hardware]:hardware/
[link_repo_rev1_archive]:rev1_archive/
[link_repo_schematic]:hardware/schematic.pdf

[link_web_hackaday_grinder_timer]:https://hackaday.com/2017/12/14/dumb-coffee-grinder-gets-smarter-with-time/
[link_web_rancilio_rocky]:https://www.ranciliogroup.com/rancilio/rocky/rocky/

[image_grinder_timer]:images/grinder_timer_photo.jpg
[image_grinder_timer_rev2]:images/grinder_timer_rev2_photo.jpg
