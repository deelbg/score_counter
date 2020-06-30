# score_counter

![Score Counter](https://github.com/deelbg/score_counter/blob/master/sc_side_small.jpg)

  Board games Score Counter is a device meant to be used for keeping the score for up to four players when playing board games like Scrabble etc. The numbers of players is selectable through the settings menu. Names up to ten characters long can be entered for each player.
  For each player an accumulated score is stored, which is the total amount of points up to the current moment in the game, and the points from the current round are entered using a rotary encoder or the Up and Down arrow keys. When OK is pressed the point from the current round are added to the total amount. 
  More detailed explanation and screenshot might be found in the [User manual HERE.](https://github.com/deelbg/score_counter/blob/master/sc_manual.pdf)
  
Main features.
* Small size (10cm / 5.5cm / 1.7cm). 
* Battery powered. Runs on two AAA batteries.
* 1.3” OLED screen.
* Controlled via five buttons and a rotary switch.
* 3D printed enclosure.
* Energy independent memory to store settings and player names.

In the **/firmware** directory is located the firmware. It uses the drivers from my [SSD1306_SH1106_i2c_text_display](https://github.com/deelbg/SSD1306_SH1106_i2c_text_display) and [hal_lib_avr_cpp](https://github.com/deelbg/hal_lib_avr_cpp) repositories. 
The device is based on a Attiny861 microcontroller.

There is a makefile included. Run the folowing:
* make - to build the project
* clean - to delete object files
* size - to see the used RAM and flash.
* flash - to write the .hex file to the microcontroller. Using avrdude and a usbasp programmer.
* eeprom - to write the eeprom.
* fuse - to write the fuses.

In the **/hardware** directory is located the hardware project for EagleCad and a KiCad export.

In the **/enclosure** directory is located a FreeCad project containig the enclosure. The 3 parts are also exported in .obj files that can be directly imported in Cura and sliced for 3d printing.
