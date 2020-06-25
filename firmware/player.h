#ifndef PLAYER_H
#define PLAYER_H


class c_player
{        
    public:
        c_player();

        uint16_t get_score(void);
        void get_name(char * str, uint8_t max_length);
        uint8_t get_length(void);

        void set_score(uint16_t score);
        void set_name(char * str);

    private:        
        static const uint8_t NAME_LENGTH = 11u;

        char name[NAME_LENGTH] = {};
        uint16_t player_score;
};


#endif //PLAYER_H
