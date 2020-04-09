# grinder_timer

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

![The completed grinder_timer.][image_grinder_timer_photo]

[link_web_rancilio_rocky]:https://www.ranciliogroup.com/rancilio/rocky/rocky/
[link_repo_schematic]:grinder_timer_kicad/grinder_timer_schematic.pdf
[link_repo_pcb]:grinder_timer_kicad/
[link_web_hackaday_grinder_timer]:https://hackaday.com/2017/12/14/dumb-coffee-grinder-gets-smarter-with-time/

[image_grinder_timer]:images/grinder_timer_photo.jpg
