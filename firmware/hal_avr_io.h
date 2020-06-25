#ifndef AVR_IO_H
#define AVR_IO_H

#include <avr/io.h>

#define IO_VERSION_MAJOR 1
#define IO_VERSION_MINOR 0
#define IO_VERSION_PATCH 0


// =================================================================================
// NOTE: The software debaunce is debauncing only the first 32 entries in io_pins_t!
// =================================================================================


// This settings are MCu specific. 
// Uncoment the ports that are available in the used MCU.
#define HAS_PORT_A
#define HAS_PORT_B
// #define HAS_PORT_C
// #define HAS_PORT_D
// #define HAS_PORT_E
// #define HAS_PORT_F

class c_io
{
    public:
        // List the used IO lines. 
        // Do not remove the NUMBER_OF_IO_PINS at the end and keep it at the end.
        // 
        // NOTE: If software inputs debaunce is used, and there are more than 32 IO lines,
        // list the inputs first, bacause the debauncer 
        // is working only with the first 32 entries in this enum.
        enum io_pins_t
        {
            INPUT_BUTTON_OK,
            INPUT_ENCODER_A,
            INPUT_ENCODER_B, 
            INPUT_BUTTON_LEFT,
            INPUT_BUTTON_RIGHT,
            INPUT_BUTTON_UP,
            INPUT_BUTTON_DOWN,
            NC_PA7,
            NC_PB0,
            NC_PB1,
            NC_PB2,
            NC_PB3,
            NC_PB4,
            NC_PB5,
            NC_PB6,            
            RESET_PB7,
            NUMBER_OF_IO_PINS
        };

        enum pin_level_t
        {
            PIN_LOW = 0u,
            PIN_HIGH
        };

        c_io();

        void init(void);
        void pin_config_input(uint8_t pin_number);
        void pin_config_input_pullup(uint8_t pin_number);
        void pin_config_output(uint8_t pin_number);
        bool input_get_raw(uint8_t pin);
        bool input_get(uint8_t pin_number);
        void output_set(uint8_t pin_number);
        void output_reset(uint8_t pin_number);
        void debaunce(void);
        
    private:

    enum port_t
    {
        PORT_A,
        PORT_B,
        PORT_C,
        PORT_D,
        PORT_E,
        PORT_F,
        PORT_MAX
    };

    enum pin_type_t
    {
        PIN_INPUT = 0u,
        PIN_OUTPUT
    };

    enum pin_pullup_t
    {
        PIN_NO_PULLUP = 0u,
        PIN_PULLUP
    };

    struct pin_t
    {
        uint8_t port;
        uint8_t pin_number;        
    };

    // List port and pin for the used IO linets. 
    // Registers for the given IO pin         PORT    PIN 
    const pin_t IO_PIN[NUMBER_OF_IO_PINS] = {{PORT_A, 2u},  // INPUT_BUTTON_OK
                                             {PORT_A, 5u},  // INPUT_ENCODER_A
                                             {PORT_A, 4u},  // INPUT_ENCODER_B
                                             {PORT_A, 0u},  // INPUT_BUTTON_LEFT
                                             {PORT_A, 6u},  // INPUT_BUTTON_RIGHT
                                             {PORT_A, 1u},  // INPUT_BUTTON_UP
                                             {PORT_A, 3u},  // INPUT_BUTTON_DOWN
                                             {PORT_A, 7u},  // NC_PA7
                                             {PORT_B, 0u},  // NC_PB0
                                             {PORT_B, 1u},  // NC_PB1
                                             {PORT_B, 2u},  // NC_PB2
                                             {PORT_B, 3u},  // NC_PB3
                                             {PORT_B, 4u},  // NC_PB4
                                             {PORT_B, 5u},  // NC_PB5
                                             {PORT_B, 6u},  // NC_PB6
                                             {PORT_B, 7u}}; // RESET_PB7 



    static const uint8_t INPUT_DEBAUNCE_SAMPLES  = 5u; 
    volatile uint32_t  inputs_debaunce_buffer[INPUT_DEBAUNCE_SAMPLES] = {};


    void portxn_set(uint8_t port, uint8_t pin);
    void portxn_clear(uint8_t port, uint8_t pin);
    void ddrxn_set(uint8_t port, uint8_t pin);
    void ddrxn_clear(uint8_t port, uint8_t pin);
    pin_type_t ddrxn_get(uint8_t port, uint8_t pin);
    bool pinxn_get(uint8_t port, uint8_t pin);
};

#endif //AVR_IO_H
