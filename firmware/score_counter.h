#ifndef SCORE_COUNTER_H
#define SCORE_COUNTER_H

#include "hal_avr_io.h"
#include "hal_encoder.h"
#include "hal_avr_eeprom.h"
#include "SSD1306_SH1106_i2c_text_display.h"
#include "timer.h"
#include "player.h"

#define SCORE_COUNTER_VERSION_MAJOR 1
#define SCORE_COUNTER_VERSION_MINOR 0
#define SCORE_COUNTER_VERSION_PATCH 0


class c_score_counter
{        
    public:
        c_score_counter(c_io & ref_io, c_encoder & ref_encoder, c_SSD1306_SH1106_i2c & ref_display);

        void score_counter(void);
        void timer_tick(void);

    private: 
        enum actions_t
        {  
            NONE,
            ENCODER_LEFT,
            ENCODER_RIGHT,
            BUTTON_LEFT,
            BUTTON_RIGHT,
            BUTTON_UP,
            BUTTON_DOWN,
            BUTTON_OK,
            BUTTON_SETTINGS,
            BUTTON_ON_OFF,            
            BUTTON_CLEAR_SCORE,
            NUMBER_OF_ACTIONS
        };

        enum states_t
        {
            START,
            DISPLAY_LANGUAGE,
            DISPLAY_LANGUAGE_EDIT,
            EDIT_LANGUAGE,
            DISPLAY_NUMBER_OF_PLAYERS,
            DISPLAY_NUMBER_OF_PLAYERS_EDIT,
            EDIT_NUMBER_OF_PLAYERS,
            DISPLAY_PLAYER_NAME,
            DISPLAY_PLAYER_NAME_EDIT,
            EDIT_PLAYER_NAME,
            DISPLAY_SCORE,
            DISPLAY_SCORE_NEW,
            DISPLAY_SCORE_TOTAL,
            ADD_SCORE,
            LOW_POWER_SLEEP_BUTTON,
            LOW_POWER_SLEEP_TIMER,
            NUMBER_OF_STATES            
        };

        enum languages_t
        {
            LANGUAGE_1 = 0u,
            LANGUAGE_2,
            NUMBER_OF_LANGUAGES            
        };

        static const uint8_t MAX_NUMBER_OF_PLAYERS        = 4u;
        static const uint8_t STR_BUFF_LENGTH              = 11u;

        static const uint8_t EEPROM_PLAYERS_NUMBER        = 0u;
        static const uint8_t EEPROM_LANGUAGE              = 1u;

        static const uint8_t EEPROM_LANGUAGE_1_BASE       = 10u;
        static const uint8_t EEPROM_LANGUAGE_2_BASE       = 70u;

        static const uint8_t EEPROM_LANGUAGE_OFFSET       = 0u;
        static const uint8_t EEPROM_LANGUAGE_NAME_OFFSET  = 10u;
        static const uint8_t EEPROM_STR_NUMBER_OF_OFFSET  = 20u;
        static const uint8_t EEPROM_STR_PLAYERS_OFFSET    = 30u;
        static const uint8_t EEPROM_STR_PLAYER_OFFSET     = 40u;
        static const uint8_t EEPROM_STR_SCORE_OFFSET      = 50u;

        static const uint8_t EEPROM_PLAYERS_NAMES         = 150u;

        static const uint8_t ON_OFF_HOLD_TIME             = 3u;
        static const uint8_t SETTINGS_HOLD_TIME           = 3u;
        static const uint8_t CLEAR_SCORE_HOLD_TIME        = 3u;
        static const uint8_t DIMM_HOLD_TIME               = 10u;
        static const uint16_t SLEEP_HOLD_TIME             = 600u;

        static const uint8_t BUTTON_PRESSED               = false;
        static const uint8_t BUTTON_RELEASED              = true;

        c_io & r_io;
        c_encoder & r_encoder;
        c_SSD1306_SH1106_i2c & r_display;
        uint8_t current_position;
        uint8_t previous_position;
        uint8_t current_player;
        uint8_t number_of_players;        
        uint8_t score_to_add;
        actions_t action;        
        uint8_t language;
        uint8_t eeprom_base;
        char str_buff[STR_BUFF_LENGTH] = {};

        
        c_eeprom eeprom = c_eeprom();
        c_player players[MAX_NUMBER_OF_PLAYERS];
        c_timer on_off_timer_sec = c_timer();
        c_timer sleep_timer_sec = c_timer();
        c_timer settings_timer_sec = c_timer();
        c_timer clear_score_timer_sec = c_timer();
        c_timer dim_timer_sec  = c_timer();


        char get_next_char(char current_char);
        char get_previous_char(char current_char);
        void wake_up(void);
        states_t display_language(void);
        states_t display_language_edit(void);
        states_t edit_language(void);
        states_t display_number_of_players(void);
        states_t display_number_of_players_edit(void);
        states_t edit_number_of_players(void);
        states_t display_player_name(void);
        states_t display_player_name_edit(void);
        states_t edit_player_name(void);
        states_t display_score(void);
        states_t display_score_new(void);
        states_t display_score_total(void);
        states_t add_score(void);
        void low_power_sleep(void);        
        void reset_players(void);
        actions_t read_controls(void);
};

#endif //SCORE_COUNTER_H
