// =============================================================================
// Fuse settings to use while programming
// SELFPRGEN = [ ]  - flash self program enable
// RSTDISBL = [ ]   - reset pin disable
// DWEN = [ ]       - debug wire enable
// SPIEN = [X]      - SPI programing enable
// WDTON = [ ]      - watch dog on in all modes
// EESAVE = [X]     - save eeprom on chip erase
// BODLEVEL = 1V8   - brown out detector voltage
// CKDIV8 = [X]     - clock devider 
// CKOUT = [ ]      - clock out pin enable
// SUT_CKSEL = INTRCOSC_8MHZ_6CK_14CK_0MS - clock source

// EXTENDED = 0xFF (valid)
// HIGH = 0xD6 (modified)
// LOW = 0x42 (valid)
// =============================================================================
#define SC_VERSION_MAJOR 1
#define SC_VERSION_MINOR 0
#define SC_VERSION_PATCH 0

#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h> 
#include <avr/interrupt.h>
#include "timer.h"
#include "hal_avr_io.h"
#include "hal_avr_i2c.h"
#include "hal_encoder.h"
#include "SSD1306_SH1106_i2c_text_display.h"
#include "score_counter.h"

static const uint8_t  OLED_ADDRESS  = 0x3Cu;

c_io                        g_io          = c_io();
c_i2c                       g_i2c         = c_i2c();
c_timer                     g_timer_1_s   = c_timer();
c_SSD1306_SH1106_i2c        g_display     = c_SSD1306_SH1106_i2c(g_i2c, OLED_ADDRESS);
c_encoder                   g_encoder     = c_encoder(g_io, true,  g_io.INPUT_ENCODER_A, g_io.INPUT_ENCODER_B);
c_score_counter             g_counter     = c_score_counter(g_io, g_encoder, g_display);

void mcu_init(void);
void main_loop(void);
void timer_0_init(void);


int main(void)
{    
    mcu_init();
    main_loop();
}


void mcu_init()
{
    // Power Reduction Register
    // PRR register
    // +----------------------------+-----+
    // |Timer/counter 0 ------------| ON  |
    // |USI - used for I2C ---------| ON  |
    // |Brown-out Detector ---------| ON  |
    // |Internal Voltage Reference -| ON  |
    // |Watchdog Timer -------------| ON  |
    // |Port Pins ------------------| ON  |
    // +----------------------------+-----+
    // |Timer Counter 1 ------------| OFF |
    // |Analog Comparator ----------| OFF |
    // |Analog to Digital Converter | OFF |    
    // +----------------------------+-----+
    PRR = (1 << PRADC) | (1 << PRTIM1);

    // Set prescaler to 1:2 to get 4Mhz system clock.
    CLKPR = (1u << CLKPCE);
    CLKPR = (0u << CLKPS3) | (0u << CLKPS2) | (0u << CLKPS1) | (1u << CLKPS0);

    // Enable watch dog timer at 4 seconds.
    wdt_enable(WDTO_4S);

    timer_0_init();
    g_i2c.init();
}


void main_loop()
{
    while (1) 
    {
        wdt_reset();

        g_counter.score_counter();
        
        if (g_timer_1_s.expired())
        {
            g_counter.timer_tick();
            g_timer_1_s.start(1000u);
        }
    }
}


void timer_0_init(void)
{
    cli();              //stop interrupts

    // In Clear Timer on Compare or CTC mode
    // 4 000 000 hz / (64 * 63) = 1000Hz (1mS)
    OCR0A   = 63u;      // Timer 0 Output Compare Register A, low: 50 

    TCCR0A  = 0x01u;    // Timer/Counter 0 Control Register A: 
                        // STS0 - Clear Timer on Compare Match (CTC) mode
    
    TCCR0B  = 0x03u;    // Timer/Counter 0 Control Register B: 
                        // CS01 - 1:8 prescaler
    
    TIFR    |= 0x1Bu;   // Timer/Counter 0 Interrupt Flag Register: 
                        // OCF0A | OCF0B | TOV0 | ICF0 Clear all interrupt flags 
    
    TIMSK   = 0x10u;    // Timer/Counter0 Interrupt Mask Register:
                        // OCIE0A - Timer/Counter0 Output Compare Match A 
                        // Interrupt Enable
    sei();              //allow interrupts
}


//timer0 interrupt every 1 ms (1kHz)
ISR(TIMER0_COMPA_vect)
{
    static uint8_t counter_10ms = 0u;
    
    if (0u == counter_10ms)
    {
        g_io.debaunce();  

        counter_10ms = 10u;
    }

    counter_10ms--;

    g_encoder.scan(); 
    g_timer_1_s.decrement();
}

ISR(INT1_vect) 
{
    // No need for any action here. 
    // The interrupt only wakes up MCU from sleep.
}
