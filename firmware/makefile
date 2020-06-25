#Compiler
COMPILER = avr-gcc

#Copy objects 
OBJECTCOPY = avr-objcopy

#Print Sizes
SIZES = avr-size

#Compiler options
CFLAGS = -std=c++11 -Wall -Os -mmcu=$(MCUTYPE)

#MCU type
MCUTYPE = attiny861a

SOURSEDIR 	= .
SOURCES 	= $(wildcard *.cpp)
HEADERDIR 	= .
HEADERS 	= $(addprefix $(HEADERDIR)/,$(wildcard *.h))


TARGETDIR 	= ./dist
TARGET 		= sc

OBJECTDIR 	= ./build
OBJECTS 	=   $(addprefix $(OBJECTDIR)/,$(patsubst %.cpp, %.o, $(SOURCES)))

 
#
# Recompile i: .cpp or any .h files had changed. 
# There might be more than one .h per .cpp file.
#
$(OBJECTDIR)/%.o: $(SOURSEDIR)/%.cpp $(HEADERS)
	$(COMPILER) -c $(CFLAGS) $< -o $@ 


#
# Check if any object files had chanegd and if so, link again in an .elf file
#
$(TARGETDIR)/$(TARGET).elf: $(OBJECTS)
	@echo " "
	@echo "============================================================="
	@echo "Link and make hex:"
	@echo "============================================================="
	$(COMPILER) $(CFLAGS)  $^ -o $@
	$(OBJECTCOPY) -O ihex -j .text -j .data $(TARGETDIR)/$(TARGET).elf $(TARGETDIR)/$(TARGET).hex
	@echo " "
	@echo "============================================================="
	@echo "Sizes:"
	@echo "-------------------------------------------------------------"
	@$(SIZES) $(TARGETDIR)/$(TARGET).elf
	@echo "-------------------------------------------------------------"
	@echo "DONE"
	@echo "============================================================="


#
# Print out sizes.
#
.PHONY: size
size: 
	@echo " "
	@echo "============================================================="
	@echo "Sizes:"
	@echo "-------------------------------------------------------------"
	@$(SIZES) $(TARGETDIR)/$(TARGET).elf 	
	@echo "-------------------------------------------------------------"
	@echo "DONE"
	@echo "============================================================="


#
# Remove object files
#
.PHONY: clean
clean: 
	@echo " "
	@echo "============================================================="
	@echo "Clear object files:"
	@echo "-------------------------------------------------------------"
	rm -f $(OBJECTDIR)/*.*
	@echo "-------------------------------------------------------------"
	@echo "DONE"
	@echo "============================================================="


#
# Flash .hex to device using avrdude and usbasp programmer
#
.PHONY: flash
flash: 
	@echo " "
	@echo "============================================================="
	@echo "Flash target:"
	@echo "-------------------------------------------------------------"	
	avrdude -c usbasp -P U -p t861 -e -B 10 -U flash:w:$(TARGETDIR)/$(TARGET).hex:i 
	@echo "-------------------------------------------------------------"
	@echo "DONE"
	@echo "============================================================="

#
# Write eeprom from .eep file to device using avrdude and usbasp programmer
#
.PHONY: eeprom
eeprom: 
	@echo " "
	@echo "============================================================="
	@echo "Write eeprom to target:"
	@echo "-------------------------------------------------------------"	
	avrdude -c usbasp -P U -p t861 -e -B 10 -U eeprom:w:$(SOURSEDIR)/$(TARGET).eep:i 
	@echo "-------------------------------------------------------------"
	@echo "DONE"
	@echo "============================================================="
	

#
# Write Fuses to device using avrdude and usbasp programmer
#
# Fuse settings to use while programming
# SELFPRGEN = [ ]  - flash self program enable
# RSTDISBL = [ ]   - reset pin disable
# DWEN = [ ]       - debug wire enable
# SPIEN = [X]      - SPI programing enable
# WDTON = [ ]      - watch dog on in all modes
# EESAVE = [X]     - save eeprom on chip erase
# BODLEVEL = 1V8   - brown out detector voltage
# CKDIV8 = [X]     - clock devider 
# CKOUT = [ ]      - clock out pin enable
# SUT_CKSEL = INTRCOSC_8MHZ_6CK_14CK_64MS - clock source
# EXTENDED = 0xFF (valid)
# HIGH = 0xD6 (modified)
# LOW = 0x62 (valid)
.PHONY: fuse
fuse: 
	@echo " "
	@echo "============================================================="
	@echo "Flash target:"
	@echo "-------------------------------------------------------------"	
	avrdude -c usbasp -P U -p t861 -e -B 10 -U lfuse:w:0x62:m -U hfuse:w:0xd6:m -U efuse:w:0xff:m 
	@echo "-------------------------------------------------------------"
	@echo "DONE"
	@echo "============================================================="