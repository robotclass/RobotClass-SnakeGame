#include <U8g2lib.h>
#include <SPI.h>
#include "RobotClass_SnakeGame.h"

#define WIDTH 96
#define HEIGHT 68

U8G2_HX1230_96X68_1_3W_SW_SPI disp(U8G2_R0, 13, 11, 10, 8); // SPI SCK,MISO,CS,RST

const uint16_t melody_intro[] = {440,90,0,90,440,90,0,90,220,90,220,90,220,90,0,900,440,90,0,90,440,90,0,90,220,90,220,90,220,90,0,900,440,90,0,90,440,90,0,90,220,90,220,90,220,90,0,900,293,90,0,90,293,90,0,90,146,90,146,90,146,90,0,900,4096};
const uint16_t melody_gamestart[] = {659,98,783,98,987,98,783,98,622,98,739,98,987,98,739,98,523,98,659,98,783,98,659,98,493,98,622,98,739,98,987,98,987,293,4096};
const uint16_t melody_gameover[] = {369, 75, 349, 75, 311, 75, 277, 75, 261, 75, 233, 75, 220, 450, 220, 225, 4096};

class Snake: public RobotClass_SnakeGame {
    public:
      Snake(int pinUp, int pinDown, int pinLeft, int pinRight, int pinSpeaker);
      void setPixel(int16_t x, int16_t y, uint16_t color);
      void drawFrame();
      void drawIntro();
      void drawGameover();
      void drawInterface();
      uint16_t *getMelody(uint8_t m);
};

Snake::Snake(int pinUp, int pinDown, int pinLeft, int pinRight, int pinSpeaker) : RobotClass_SnakeGame(pinUp, pinDown, pinLeft, pinRight, pinSpeaker, WIDTH, HEIGHT){
    // set game frame
    setGameFrame(1,16,WIDTH-2,HEIGHT-2);

    // set game pallete
    setPallete(0,1,1,1);
}

void Snake::setPixel(int16_t x, int16_t y, uint16_t color){
    disp.setDrawColor(color);
    disp.drawPixel(x, y);
}

void Snake::drawFrame(){
    disp.firstPage();  
    do {
        draw();
    } while( disp.nextPage() );  
}

void Snake::drawIntro(){
    disp.setDrawColor(COLOR_BLUE);
    disp.setFont(u8g2_font_ncenB10_tr);
    disp.drawStr(6, 24, "RobotClass");
    disp.drawStr(20, 40, "Snake");

    disp.setFont(u8g2_font_ncenB08_tr);
    int fw = disp.getStrWidth("Press any key");
    int fh = disp.getMaxCharHeight();
    if( intro_state ){
        disp.setDrawColor(COLOR_BLUE);
        disp.drawStr(10, 55, "Press any key");
    } else {
        disp.setDrawColor(COLOR_BLACK);
        disp.drawBox(10,55-fh,fw,fh); // remove text
    }
}

void Snake::drawGameover(){
    disp.setDrawColor(COLOR_BLUE);
    disp.setFont(u8g2_font_ncenB10_tr);
    disp.drawStr(8, 24, "Gameover");
    disp.setFont(u8g2_font_ncenB08_tr);
    disp.drawStr(20, 40, "Points: ");
    disp.drawStr(64, 40, st_itoa(player_points));
}

void Snake::drawInterface(){
    // draw border
    disp.setDrawColor(COLOR_BLUE);
    disp.drawVLine( 0, 15, height-10 );
    disp.drawVLine( width-1, 15, height-10 );
    disp.drawHLine( 1, 15, width-2 );
    disp.drawHLine( 1, height-1, width-2 );

    // draw player points
    disp.setFont(u8g2_font_ncenB08_tr);
    int fh = disp.getMaxCharHeight();
    disp.drawStr(2, 0+fh, "points: ");
    disp.drawStr(45, 0+fh, st_itoa(player_points));
}

uint16_t *Snake::getMelody(uint8_t m){
  switch( m ){
  case MELODY_INTRO:
      return melody_intro;
      break;
  case MELODY_GAMESTART:
      return melody_gamestart;
      break;
  case MELODY_GAMEOVER:
      return melody_gameover;
      break;
  }
}

Snake game(A0, A1, A2, A3, 3);

void setup() {
    disp.begin();
    disp.setContrast(128);

    game.setup();
    game.resetGame();
}

void loop() {
    game.handler();
    game.drawFrame();
}
