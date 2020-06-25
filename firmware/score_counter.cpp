#include <stdint.h>
#include <string.h>
#include <avr/wdt.h> 
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "score_counter.h"

// #define F_CPU 4000000UL
#include <util/delay.h>

// ================= Public members ================= 
c_score_counter::c_score_counter(c_io & ref_io, c_encoder & ref_encoder, c_SSD1306_SH1106_i2c & ref_display): 
                                 r_io(ref_io), r_encoder(ref_encoder), r_display(ref_display),
                                 current_position(0u), previous_position(0u), current_player(0u), 
                                 number_of_players(0u), score_to_add(0u), action(NONE), language(0u)
{
    number_of_players = eeprom.read_byte(EEPROM_PLAYERS_NUMBER);
    language = eeprom.read_byte(EEPROM_LANGUAGE);
    eeprom_base = (LANGUAGE_1 == language) ? EEPROM_LANGUAGE_1_BASE : EEPROM_LANGUAGE_2_BASE;

    for (uint8_t index = r_io.INPUT_BUTTON_OK; index <= r_io.NUMBER_OF_IO_PINS; index++)
    {
        r_io.pin_config_input_pullup(index);
    }
}


void c_score_counter::score_counter(void)
{
    static c_score_counter::states_t state = START;

    action = read_controls();

    if (NONE != action)
    {
        r_display.dim(false);        
        dim_timer_sec.start(DIMM_HOLD_TIME);
        sleep_timer_sec.start(SLEEP_HOLD_TIME);
    }
    else
    {
        if (dim_timer_sec.expired())
        {
            r_display.dim(true);
        }

        if (sleep_timer_sec.expired())
        {
            state = LOW_POWER_SLEEP_TIMER;
        }
    }

    switch (state)
    {
        case START:
            reset_players();
            wake_up();
            state = DISPLAY_SCORE;
        break;

        case DISPLAY_LANGUAGE:            
            state = display_language();
        break;

        case DISPLAY_LANGUAGE_EDIT:
            state = display_language_edit();
        break;

        case EDIT_LANGUAGE:
            state = edit_language();
        break;

        case DISPLAY_NUMBER_OF_PLAYERS:            
            state = display_number_of_players();
        break;

        case DISPLAY_NUMBER_OF_PLAYERS_EDIT:
            state = display_number_of_players_edit();
        break;

        case EDIT_NUMBER_OF_PLAYERS:
            state = edit_number_of_players();
        break;
        
        case DISPLAY_PLAYER_NAME:     
            state = display_player_name();            
        break;

        case DISPLAY_PLAYER_NAME_EDIT:
            state = display_player_name_edit();
        break;

        case EDIT_PLAYER_NAME:
            state = edit_player_name();
        break;

        case DISPLAY_SCORE:  
            state = display_score();
        break;

        case DISPLAY_SCORE_NEW:
            state = display_score_new();
        break;

        case DISPLAY_SCORE_TOTAL:
            state = display_score_total();
        break;

        case ADD_SCORE:
            state = add_score();
        break;

        case LOW_POWER_SLEEP_BUTTON:
            low_power_sleep();
            reset_players();
            wake_up();
            state = DISPLAY_SCORE;
        break;

        case LOW_POWER_SLEEP_TIMER:
            low_power_sleep();            
            wake_up();
            state = DISPLAY_SCORE;
        break;

        default:

        break;
    }
}


// ================= Private members ================= 

char c_score_counter::get_next_char(char current_char)
{
    uint8_t ch = (uint8_t)current_char;
    
    if (0xFFu == ch)
    {
        ch = 0x20u;
    }
    else
    {
        if (0x7Au == ch)
        {
            ch = 0xC0u;
        }
        else
        {
            ch++;
        }
    }

    return (char)ch;
}


char c_score_counter::get_previous_char(char current_char)
{
    uint8_t ch = (uint8_t)current_char;

    if (0x20u == ch)
    {
        ch = 0xFFu;
    }
    else
    {
        if (0xC0u == ch)
        {
            ch = 0x7Au;
        }
        else
        {
            ch--;
        }
    }

    return (char)ch;
}


void c_score_counter::wake_up(void)
{    
    dim_timer_sec.start(DIMM_HOLD_TIME);
    sleep_timer_sec.start(SLEEP_HOLD_TIME);
    r_display.init(c_SSD1306_SH1106_i2c::VCC_SWITCHCAP);
    r_display.set_offset_x(2u);
    r_display.set_contrast(0xEF, 0x00);
    r_display.clear();
    r_display.set_font_size(1);
}


c_score_counter::states_t c_score_counter::display_language(void)
{
    r_display.clear();

    eeprom.read_block((uint8_t *)str_buff, (eeprom_base + EEPROM_LANGUAGE_OFFSET), STR_BUFF_LENGTH - 1u);
    str_buff[STR_BUFF_LENGTH - 1u] = '\0';
    r_display.print(str_buff);

    return DISPLAY_LANGUAGE_EDIT;
}


c_score_counter::states_t c_score_counter::display_language_edit(void)
{ 
    eeprom.read_block((uint8_t *)str_buff, (eeprom_base + EEPROM_LANGUAGE_NAME_OFFSET), STR_BUFF_LENGTH - 1u);
    str_buff[STR_BUFF_LENGTH - 1u] = '\0';

    r_display.set_possition(0u, 1u);
    r_display.print(str_buff);

    memset(str_buff, ' ', STR_BUFF_LENGTH - 1);
    str_buff[STR_BUFF_LENGTH - 1u] = '\0';    
    r_display.print(str_buff);

    return EDIT_LANGUAGE;  
}


c_score_counter::states_t c_score_counter::edit_language(void)
{
    c_score_counter::states_t return_state = EDIT_LANGUAGE;

    switch(action)
    {
        case ENCODER_LEFT:
            language = ( 0 < language) ? language - 1: NUMBER_OF_LANGUAGES - 1;  
            eeprom_base = (LANGUAGE_1 == language) ? EEPROM_LANGUAGE_1_BASE : EEPROM_LANGUAGE_2_BASE;
            return_state = DISPLAY_LANGUAGE_EDIT;                  
        break;

        case ENCODER_RIGHT:
            language = ((NUMBER_OF_LANGUAGES - 1u) > language) ? language + 1: 0u;            
            eeprom_base = (LANGUAGE_1 == language) ? EEPROM_LANGUAGE_1_BASE : EEPROM_LANGUAGE_2_BASE;
            return_state = DISPLAY_LANGUAGE_EDIT;
        break;

        case BUTTON_OK:
            return_state = DISPLAY_NUMBER_OF_PLAYERS;
        break;

        default:
            //ignore other buttons
        break;
    }

    return return_state;
}


c_score_counter::states_t c_score_counter::display_number_of_players(void)
{
    r_display.clear();

    eeprom.read_block((uint8_t *)str_buff, (eeprom_base + EEPROM_STR_NUMBER_OF_OFFSET), STR_BUFF_LENGTH - 1u);
    str_buff[STR_BUFF_LENGTH - 1u] = '\0';
    r_display.println(str_buff);

    eeprom.read_block((uint8_t *)str_buff, (eeprom_base + EEPROM_STR_PLAYERS_OFFSET), STR_BUFF_LENGTH - 1u);
    str_buff[STR_BUFF_LENGTH - 1u] = '\0';
    r_display.print(str_buff);

    return DISPLAY_NUMBER_OF_PLAYERS_EDIT;
}


c_score_counter::states_t c_score_counter::display_number_of_players_edit()
{
    r_display.set_possition(9u, 1u);
    r_display.print_char('1' + number_of_players);

    return EDIT_NUMBER_OF_PLAYERS;    
}


c_score_counter::states_t c_score_counter::edit_number_of_players(void)
{
    c_score_counter::states_t return_state = EDIT_NUMBER_OF_PLAYERS;

    switch(action)
    {
        case ENCODER_LEFT:
            number_of_players = ( 0 < number_of_players) ? number_of_players - 1: 
                                                           MAX_NUMBER_OF_PLAYERS - 1;  
            return_state = DISPLAY_NUMBER_OF_PLAYERS_EDIT;                  
        break;

        case ENCODER_RIGHT:
            number_of_players = ((MAX_NUMBER_OF_PLAYERS - 1) > number_of_players) ? 
                                                            number_of_players + 1: 0u;
            return_state = DISPLAY_NUMBER_OF_PLAYERS_EDIT;
        break;

        case BUTTON_OK:
            current_player    = 0u;
            previous_position = 0u;
            current_position  = 0u;
            return_state = DISPLAY_PLAYER_NAME;
        break;

        default:
            //ignore other buttons
        break;
    }

    return return_state;
}


c_score_counter::states_t c_score_counter::display_player_name(void)
{
    r_display.clear();
    
    eeprom.read_block((uint8_t *)str_buff, (eeprom_base + EEPROM_STR_PLAYER_OFFSET), STR_BUFF_LENGTH - 1u);
    str_buff[STR_BUFF_LENGTH - 1u] = '\0';
    r_display.print(str_buff);
    r_display.print_char('1' + current_player);

    r_display.set_possition(0u, 1u);
    players[current_player].get_name(str_buff, STR_BUFF_LENGTH);
    r_display.print(str_buff);

    return DISPLAY_PLAYER_NAME_EDIT;
}


c_score_counter::states_t c_score_counter::display_player_name_edit()
{   
    if (previous_position != current_position)
    {
        r_display.hide_cursor();
        r_display.set_possition(previous_position, 1u);
        r_display.print_char(str_buff[previous_position]);
    }

    r_display.show_cursor(current_position, 1u);
    r_display.set_possition(current_position, 1u);
    r_display.print_char(str_buff[current_position]);

    return EDIT_PLAYER_NAME;
}


c_score_counter::states_t c_score_counter::edit_player_name(void)
{
    c_score_counter::states_t return_state = EDIT_PLAYER_NAME;
    previous_position = current_position;
 
    switch(action)
    {
        case BUTTON_LEFT:
            current_position = (0u < current_position) ? 
                                current_position - 1u : current_position;
            return_state = DISPLAY_PLAYER_NAME_EDIT;
        break;

        case BUTTON_RIGHT:                
            current_position = ((STR_BUFF_LENGTH - 2u) > current_position) ? 
                                current_position + 1u : current_position;
            return_state = DISPLAY_PLAYER_NAME_EDIT;
        break;

        case BUTTON_UP:
            current_player = (current_player < number_of_players) ? current_player + 1 : 0u;
            current_position = 0u;
            return_state = DISPLAY_PLAYER_NAME;
        break;

        case BUTTON_DOWN:
            current_player = (0u < current_player) ? current_player - 1 : number_of_players;
            current_position = 0u;
            return_state = DISPLAY_PLAYER_NAME;
        break;

        case ENCODER_LEFT:
            players[current_player].get_name(str_buff, STR_BUFF_LENGTH);
            str_buff[current_position] = get_previous_char(str_buff[current_position]);
            players[current_player].set_name(str_buff);

            return_state = DISPLAY_PLAYER_NAME_EDIT;
        break;

        case ENCODER_RIGHT:
            players[current_player].get_name(str_buff, STR_BUFF_LENGTH);
            str_buff[current_position] = get_next_char(str_buff[current_position]);
            players[current_player].set_name(str_buff);
            return_state = DISPLAY_PLAYER_NAME_EDIT;
        break;

        case BUTTON_OK:
            if (current_player < number_of_players)
            {
                current_player = (current_player < number_of_players) ? current_player + 1 : 0u;
                current_position = 0u;
                return_state = DISPLAY_PLAYER_NAME;
            }
            else
            {
                eeprom.write_byte(EEPROM_PLAYERS_NUMBER, number_of_players);
                eeprom.write_byte(EEPROM_LANGUAGE, language);

                for (uint8_t index = 0; index < MAX_NUMBER_OF_PLAYERS; index++)
                {
                    uint16_t temp_address = EEPROM_PLAYERS_NAMES + (index * STR_BUFF_LENGTH);

                    players[index].get_name(str_buff, STR_BUFF_LENGTH);
                    eeprom.write_block((uint8_t *)str_buff, temp_address, STR_BUFF_LENGTH);
                }

                current_player   = 0u;
                current_position = 0u;
                return_state = DISPLAY_SCORE;
            }
        break;

        default:
            //ignore other buttons
        break;
    }

    return return_state;
}


c_score_counter::states_t c_score_counter::display_score(void)
{
    r_display.clear();
    r_display.hide_cursor();    

    r_display.set_possition(0u, 2u);
    eeprom.read_block((uint8_t *)str_buff, (eeprom_base + EEPROM_STR_SCORE_OFFSET), STR_BUFF_LENGTH - 1u);
    str_buff[STR_BUFF_LENGTH - 1u] = '\0';    
    r_display.println(str_buff);

    return DISPLAY_SCORE_TOTAL;
}


c_score_counter::states_t c_score_counter::display_score_new()
{
    r_display.set_possition(7u, 3u);
    r_display.print_number(score_to_add, 2u);

    return ADD_SCORE;    
}


c_score_counter::states_t c_score_counter::display_score_total()
{
    r_display.set_possition(0u, 0u);
    players[current_player].get_name(str_buff, STR_BUFF_LENGTH);
    r_display.print(str_buff);
 
    memset(str_buff, ' ', STR_BUFF_LENGTH - 1);
    str_buff[STR_BUFF_LENGTH - 1u] = '\0';    
    r_display.print(str_buff);

    r_display.set_possition(5u, 3u);
    r_display.print_char(':');

    r_display.set_possition(0u, 3u);
    r_display.print_number(players[current_player].get_score(), 4u);

    return DISPLAY_SCORE_NEW;
}


c_score_counter::states_t c_score_counter::add_score(void)
{
    c_score_counter::states_t return_state = ADD_SCORE;

    switch(action)
    {
        case BUTTON_DOWN:
        case ENCODER_LEFT:
            score_to_add = (0u < score_to_add) ? score_to_add - 1 : 99u;
            return_state = DISPLAY_SCORE_NEW;
        break;

        case BUTTON_UP:
        case ENCODER_RIGHT:
            score_to_add = (99u > score_to_add) ? score_to_add + 1: 0u;                    
            return_state = DISPLAY_SCORE_NEW;
        break;

        case BUTTON_LEFT:
            current_player = (0u < current_player) ? current_player - 1 : number_of_players;
            return_state = DISPLAY_SCORE_TOTAL;
        break;

        case BUTTON_RIGHT:
            current_player = (current_player < number_of_players) ? current_player + 1 : 0u;
            return_state = DISPLAY_SCORE_TOTAL;
        break;

        case BUTTON_OK:
            // save score
            players[current_player].set_score(players[current_player].get_score() + score_to_add);
            score_to_add = 0u;
            current_player = (current_player < number_of_players) ? current_player + 1 : 0u;
            return_state = DISPLAY_SCORE_TOTAL;
        break;

        case BUTTON_SETTINGS:
            return_state = DISPLAY_LANGUAGE;
        break;

        case BUTTON_ON_OFF:
            return_state = LOW_POWER_SLEEP_BUTTON;
        break;

        case BUTTON_CLEAR_SCORE:
            return_state = START;
        break;

        default:
        break;
    }

    return return_state;
}


void c_score_counter::low_power_sleep(void)
{
    r_display.power_on(false); 
    
    // Wait for the button to be released.
    while (!r_io.input_get(r_io.INPUT_BUTTON_OK))
    {
        _delay_ms(50u);
        wdt_reset();
    }
    
    wdt_reset();
    wdt_disable();
    MCUCR |= (1u << PUD);                       // Disable IO pullups.
    MCUCR &= ~((1u << ISC01) | (1u << ISC00));  // Low level interrupt on INT1 or INT0.
    GIFR  |= (1u << INT1);                      // Clear INT1 interrupt flag.
    GIMSK |= (1u << INT1);                      // Enable INT1 interrupt.
    cli();                                      // Disable global interrupts     
    _delay_ms(100u);

    // ==================================================
    // Sleep the MCU. 
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_bod_disable();
    sei();
    sleep_cpu();
    // Continue after wakeup
    sleep_disable();
    cli();    
    // ==================================================
    
    GIMSK &= ~(1u << INT1); // Disable INT1 interrupt.
    GIFR  |= (1u << INT1);  // Clear INT1 interrupt flag.
    MCUCR &= ~(1u << PUD);  // Enable IO pullups.
    sei();                  // Enable global interrupts 
    wdt_reset();
    wdt_enable(WDTO_4S);  
    r_display.power_on(true);
}


c_score_counter::actions_t c_score_counter::read_controls(void)
{
    uint8_t tmp_pulses = 0u;
    c_score_counter::actions_t action = NONE;

    static bool button_ok_old       = false;
    static bool button_left_old     = false;
    static bool button_right_old    = false;
    static bool button_up_old       = false;
    static bool button_down_old     = false;

    bool button_ok_current          = r_io.input_get(r_io.INPUT_BUTTON_OK);
    bool button_left_current        = r_io.input_get(r_io.INPUT_BUTTON_LEFT);
    bool button_right_current       = r_io.input_get(r_io.INPUT_BUTTON_RIGHT);
    bool button_up_current          = r_io.input_get(r_io.INPUT_BUTTON_UP);
    bool button_down_current        = r_io.input_get(r_io.INPUT_BUTTON_DOWN);

    tmp_pulses = r_encoder.get_pulses_left(false);
    
    if (0u != tmp_pulses)
    {
        r_encoder.set_pulses_left(tmp_pulses - 1);

        action = ENCODER_LEFT; 
    }

    tmp_pulses = r_encoder.get_pulses_right(false);
    
    if (0u != tmp_pulses)
    {
        r_encoder.set_pulses_right(tmp_pulses - 1);

        action = ENCODER_RIGHT; 
    }

    if (BUTTON_PRESSED == button_ok_current)
    {
        if (BUTTON_RELEASED == button_ok_old)
        {
            action = BUTTON_OK;         
            on_off_timer_sec.start(ON_OFF_HOLD_TIME);
        }
        else
        {
            if (on_off_timer_sec.expired())
            {
                action = BUTTON_ON_OFF;
            }
        }
    }
    
    if ((BUTTON_PRESSED == button_left_current) && (BUTTON_RELEASED == button_left_old))
    {
        action = BUTTON_LEFT;        
    }

    if ((BUTTON_PRESSED == button_right_current) && (BUTTON_RELEASED == button_right_old))
    {
        action = BUTTON_RIGHT;
    }
    
    if (BUTTON_PRESSED == button_up_current)
    {
        if (BUTTON_RELEASED == button_up_old)
        {
            action = BUTTON_UP;         
            settings_timer_sec.start(SETTINGS_HOLD_TIME);
        }
        else
        {
            if (settings_timer_sec.expired())
            {
                action = BUTTON_SETTINGS;
            }
        }
    }

    if (BUTTON_PRESSED == button_down_current)
    {
        if (BUTTON_RELEASED == button_down_old)
        {
            action = BUTTON_DOWN;         
            clear_score_timer_sec.start(CLEAR_SCORE_HOLD_TIME);
        }
        else
        {
            if (clear_score_timer_sec.expired())
            {
                action = BUTTON_CLEAR_SCORE;
            }
        }
    }

    button_ok_old       = button_ok_current;
    button_left_old     = button_left_current;
    button_right_old    = button_right_current;
    button_up_old       = button_up_current;
    button_down_old     = button_down_current;

    return action;
}


void c_score_counter::reset_players(void)
{            
    current_player = 0u;
    score_to_add = 0u;

    for (uint8_t index = 0; index < MAX_NUMBER_OF_PLAYERS; index++)
    {
        uint16_t temp_address = EEPROM_PLAYERS_NAMES + (index * STR_BUFF_LENGTH);

        eeprom.read_block((uint8_t *)str_buff, temp_address, STR_BUFF_LENGTH - 1u);
        str_buff[STR_BUFF_LENGTH - 1u] = '\0';

        players[index].set_name(str_buff);
        players[index].set_score(0u);
    }
}


void c_score_counter::timer_tick(void)
{
    on_off_timer_sec.decrement();
    sleep_timer_sec.decrement();
    settings_timer_sec.decrement();
    clear_score_timer_sec.decrement();
    dim_timer_sec.decrement();
}
