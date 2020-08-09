# grinder_timer

## NOTE: Rev_2 of grinder_timer is currently under development.
Planned improvements
- Hardware:
	- [ ] Pull-ups on I2C SDA and CLK lines.
	- [ ] Drive relay with a transistor instead of direct from IO.
	- [ ] Add flyback diode protection to relay coil.
	- [ ] Smaller transformer.
	- [ ] Smaller relay.
	- [ ] Switch to SMD where possible.
	- [ ] Two-sided PCB (have it fabricated instead of etching).
	- [ ] Switch buttons from mushy panel-mount to tactile SMD.
	- [ ] Balanced caps for the 32.768kHz crystal.
	- [ ] Put grind button LED on a PWM output.
- Firmware:
	- [ ] Switch from C to C++.
	- [ ] New OLED driver (change from SPI SSD1306 to IIC SH1106 controller).
	- [ ] Pulse/fade effect for grind button LED.
	- [ ] Ability to cancel a grind once started.
	- [ ] Remeber selected preset after a power-cycle.

---

This is a programmable timer tethered to a [Rancilio Rocky][link_web_rancilio_rocky] coffee grinder. The Rocky is a "dumb" grinder and electrically very simple. Basically it's an ac motor with an on/off switch and a momentary push-button to grind. This project adds an electronic timer so that repeatable grind quantities can be produced without having to stand over the grinder. Features:
- Four programmable presets (A, B, C or D) stored in EEPROM to survive powercycle.
- Left/Right buttons to select preset.
- Up/Down buttons to adjust grind duration for selected preset (0.25s increments up to 60s).
- Grind button to initiate grind and begin countdown timer.
- Oled display (128x64) to show preset menu and countdown timer.
- Aluminium enclosure.
- Single cable (3-core) tether to grinder for power and control.

The code, [schematic][link_repo_schematic] and [PCB layout][link_repo_pcb] are all included here on gitlab.

Proudly mentioned at [hackaday.com][link_web_hackaday_grinder_timer].

![The completed grinder_timer.][image_grinder_timer]

[link_web_rancilio_rocky]:https://www.ranciliogroup.com/rancilio/rocky/rocky/
[link_repo_schematic]:schematic/grinder_timer/
[link_repo_pcb]:schematic/grinder_timer/
[link_web_hackaday_grinder_timer]:https://hackaday.com/2017/12/14/dumb-coffee-grinder-gets-smarter-with-time/

[image_grinder_timer]:images/grinder_timer_photo.jpg
