/*
 * Snake game engine
 * Written by Oleg Evsegneev for RobotClass.  
 */
#ifndef _RobotClass_SnakeGame_h_
#define _RobotClass_SnakeGame_h_

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#define X1 0
#define Y1 1
#define X2 2
#define Y2 3

#define COLOR_BLACK 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_BLUE 3

#define MODE_GAME 0
#define MODE_END 1
#define MODE_RESET 2
#define MODE_READY 3

#define MELODY_NONE 0
#define MELODY_INTRO 1
#define MELODY_GAMESTART 2
#define MELODY_GAMEOVER 3

#define SNAKE_START_SPEED 200
#define SNAKE_SPEEDUP 3
#define SNAKE_MAX_LENGTH 128
#define SNAKE_SIZE 3
#define HALF_SNAKE_SIZE SNAKE_SIZE/2

#define FOOD_MAX_Q 5

#define RESET_TIMEOUT 5000
#define CTRL_TIMEOUT 50
#define BEEP_TIMEOUT 100
#define INTRO_TIMEOUT 500

class RobotClass_SnakeGame {
    public:
        RobotClass_SnakeGame(int pinUp, int pinDown, int pinLeft, int pinRight, int pinSpeaker, int width, int height);

        uint16_t player_points;

        uint16_t width, height;
        uint16_t half_width, half_height;
        uint16_t game_frame[4];
        uint8_t pallete[4] = {0,1,1,1};
        uint8_t intro_state;

        virtual void setPixel(int16_t x, int16_t y, uint16_t color);
        virtual void drawFrame();
        virtual void drawIntro();
        virtual void drawGameover();
        virtual void drawInterface();
        virtual uint16_t *getMelody(uint8_t m);

        void setup();
        void draw();
        void handler();
        void resetGame();
        void setGameFrame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 );
        void setPallete(uint8_t black, uint8_t red, uint8_t green, uint8_t blue );

        char *st_itoa(unsigned long v);
    
    private:
        int pinCTRL[4];
        int pinSpeaker;
        char st_itoa_buf[12];

        unsigned long time, ctrl_next, update_next, reset_next, play_next, intro_next;
        uint16_t update_timeout = SNAKE_START_SPEED;
        
        uint8_t rise_snake = 0;
        uint8_t sound_playing = 0;
        uint8_t melody_playing = 0;
        uint8_t melody_note = 0;

        uint16_t xinc = -1;
        uint16_t yinc = 0;
        uint8_t mode;

        // snake coords
        uint16_t snake[SNAKE_MAX_LENGTH][2];
        uint8_t snake_size = 1;
        
        // food coords
        uint16_t food[FOOD_MAX_Q][2];

        //void sound( uint8_t play, unsigned long time );
        void beep();
        int8_t checkEat();
        void drawSnake();
        void drawSnakeSegment(uint16_t x, uint16_t y);
        void drawFood();
        void replaceFood(uint8_t i);
        void play();
        void playMelody(uint8_t m, uint8_t cycle);
};
#endif
