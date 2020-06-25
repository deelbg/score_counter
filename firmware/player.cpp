#include <stdint.h>
#include <string.h>
#include "player.h"

c_player::c_player() :
                  player_score(0u)
{
}


uint16_t c_player::get_score(void)
{

    return player_score;
}


void c_player::get_name(char * str, uint8_t max_length)
{
    if (str != NULL)
    {
        if (NAME_LENGTH > max_length)
        {
            strcpy(str, name);
        }
        else
        {
            memcpy(str, name, (max_length - 1));
            str[max_length - 1] = '\0';
        }
    }
}


uint8_t c_player::get_length(void)
{
    return NAME_LENGTH;
}


void c_player::set_score(uint16_t score)
{
    player_score = score;
}


void c_player::set_name(char * str)
{
    if (str != NULL)
    {
        if (NAME_LENGTH > strlen(str))
        {
            strcpy(name, str);
        }
        else
        {
            memcpy(name, str, (NAME_LENGTH - 1));
            name[NAME_LENGTH - 1] = '\0';
        }
    }
}
