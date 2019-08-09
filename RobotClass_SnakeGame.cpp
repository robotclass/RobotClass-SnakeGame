/*
 * Snake game engine
 * Written by Oleg Evsegneev for RobotClass.  
 */
 #include "RobotClass_SnakeGame.h"

RobotClass_SnakeGame::RobotClass_SnakeGame(int pinUp, int pinDown, int pinLeft, int pinRight, int pinSpeaker, int width, int height){
    pinCTRL[0] = pinUp;
    pinCTRL[1] = pinDown;
    pinCTRL[2] = pinLeft;
    pinCTRL[3] = pinRight;

    this->pinSpeaker = pinSpeaker;
    this->width = width;
    this->height = height;

    half_width = width/2;
    half_height = height/2;
}

void RobotClass_SnakeGame::setup(){
    pinMode(pinSpeaker,OUTPUT);

    // set button pins to input
    for(int i=0; i<4; i++){
        pinMode(pinCTRL[i],INPUT_PULLUP);
    }

    pinMode(A4,INPUT);
    randomSeed(analogRead(A4));
    pinMode(A4,INPUT_PULLUP);
}

void RobotClass_SnakeGame::draw(){
  if( mode == MODE_READY ){
    drawIntro();
  }
  else if( mode == MODE_RESET ){
    drawGameover();
  }
  else if( mode == MODE_GAME ){
    drawInterface();
    drawFood();
    drawSnake();
  }
}

void RobotClass_SnakeGame::handler(){
    byte x, y, z;

    time = millis();
    play();

    if( mode == MODE_END ){
        reset_next = time + RESET_TIMEOUT;
        mode = MODE_RESET;
    }

    if( mode == MODE_RESET && time > reset_next ){
        resetGame();
    }

    if( mode == MODE_READY && time > intro_next ){
        intro_next = time + INTRO_TIMEOUT;
        intro_state = !intro_state;
    }
    if(( mode == MODE_READY || mode == MODE_GAME ) && time > ctrl_next ){
        ctrl_next = time + CTRL_TIMEOUT;
        uint8_t pressed = 0;
        if( !digitalRead(pinCTRL[0])){
            xinc = 1;
            yinc = 0;
            pressed = 1;
        } else if( !digitalRead(pinCTRL[1])){
            xinc = -1;
            yinc = 0;
            pressed = 1;
        } else if( !digitalRead(pinCTRL[2])){
            xinc = 0;
            yinc = 1;
            pressed = 1;
        } else if( !digitalRead(pinCTRL[3])){
            xinc = 0;
            yinc = -1;
            pressed = 1;
        }
        if( pressed && mode == MODE_READY ){
            mode = MODE_GAME;
        }
    }

    if( mode == MODE_GAME && time > update_next ){
        update_next = time + update_timeout;

        x = snake[0][0] + xinc;
        y = snake[0][1] + yinc;

        // move snake tail
        for( z=snake_size; z!=0; z-- ){
            snake[z][0] = snake[z-1][0];
            snake[z][1] = snake[z-1][1];
            // check snake self intersection
            if( x==snake[z-1][0] && y==snake[z-1][1] )
                mode = MODE_END;
        }

        snake[0][0] = x;
        snake[0][1] = y;
        
        // check walls
        if(snake[0][0] == game_frame[X1]-1 || snake[0][0] == game_frame[X2]+1 || snake[0][1] == game_frame[Y1]-1 || snake[0][1] == game_frame[Y2]+1){
            mode = MODE_END;

         // draw snake
        } else {
            // rise snake on feeding
            int8_t food = checkEat();
            if( food>=0 ){
                player_points++;

                replaceFood(food); // set new food

                rise_snake = 1;
                snake_size++;
                // speed up snake
                //update_timeout = 300 - 20*(snake_size>>2);
                if( update_timeout >= SNAKE_SPEEDUP )
                    update_timeout -= SNAKE_SPEEDUP;
                else
                    update_timeout = 0;
                beep();
            }
        }
    }
}

void RobotClass_SnakeGame::resetGame(){
    byte x,y;

    player_points = 0;
    update_timeout = SNAKE_START_SPEED;

    // set start length and position
    snake_size = 1;
    snake[0][0] = half_width;
    snake[0][1] = half_height;

    // set food
    for( uint8_t i=0; i<FOOD_MAX_Q; i++ ){
        replaceFood(i);
    }

    mode = MODE_READY;
}

void RobotClass_SnakeGame::setGameFrame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ){
    game_frame[X1] = x1;
    game_frame[Y1] = y1;
    game_frame[X2] = x2;
    game_frame[Y2] = y2; 
}

void RobotClass_SnakeGame::setPallete(uint8_t black, uint8_t red, uint8_t green, uint8_t blue ){
    pallete[COLOR_BLACK] = black;
    pallete[COLOR_RED] = red;
    pallete[COLOR_GREEN] = green;
    pallete[COLOR_BLUE] = blue;
}

void RobotClass_SnakeGame::beep(){
    tone(pinSpeaker, 147, BEEP_TIMEOUT);
}

int8_t RobotClass_SnakeGame::checkEat(){
    for( uint8_t i=0; i<FOOD_MAX_Q; i++ ){
        if( SNAKE_SIZE == 1 ){
            if( snake[0][X1]==food[i][X1] && snake[0][Y1]==food[i][Y1] )
                return i;
        } else {
            if( snake[0][X1]-HALF_SNAKE_SIZE < food[i][X1]+HALF_SNAKE_SIZE && 
            snake[0][X1]+HALF_SNAKE_SIZE > food[i][X1]-HALF_SNAKE_SIZE && 
            snake[0][Y1]-HALF_SNAKE_SIZE < food[i][Y1]+HALF_SNAKE_SIZE && 
            snake[0][Y1]+HALF_SNAKE_SIZE > food[i][Y1]-HALF_SNAKE_SIZE )
                return i;
        }
    }
    return -1;
}

void RobotClass_SnakeGame::drawSnake(){
    for( uint8_t z=0; z<snake_size; z++ ){
        drawSnakeSegment( snake[z][X1], snake[z][Y1]);
    }
}

void RobotClass_SnakeGame::drawSnakeSegment(uint16_t x, uint16_t y){
    if( SNAKE_SIZE == 1 ){
        setPixel(x, y, pallete[COLOR_GREEN]);
    } else {
        for( uint8_t i=0; i<SNAKE_SIZE; i++){
            setPixel(x-HALF_SNAKE_SIZE+i, y, pallete[COLOR_GREEN]);
            setPixel(x, y-HALF_SNAKE_SIZE+i, pallete[COLOR_GREEN]);
        }
    }
}

void RobotClass_SnakeGame::drawFood(){
    for( uint8_t f=0; f<FOOD_MAX_Q; f++ ){
        if( SNAKE_SIZE == 1 ){
            setPixel(food[f][0], food[f][1], pallete[COLOR_RED]);
        } else {
            for( uint8_t i=0; i<SNAKE_SIZE; i++){
                setPixel(food[f][X1]-HALF_SNAKE_SIZE+i, food[f][Y1], pallete[COLOR_RED]);
                setPixel(food[f][X1], food[f][Y1]-HALF_SNAKE_SIZE+i, pallete[COLOR_RED]);
            }
        }
    }
}

void RobotClass_SnakeGame::replaceFood(uint8_t i){
    food[i][0] = random(game_frame[X1]+1, game_frame[X2]-1);
    food[i][1] = random(game_frame[Y1]+1, game_frame[Y2]-1);
}

void  RobotClass_SnakeGame::play(){
  if( mode == MODE_READY ){
    playMelody(MELODY_INTRO, 1);
  }
  else if( mode == MODE_RESET ){
    playMelody(MELODY_GAMEOVER, 0);
  }
  else if( mode == MODE_GAME ){
    playMelody(MELODY_GAMESTART, 0);
  }
}

void RobotClass_SnakeGame::playMelody(uint8_t m, uint8_t cycle){
    if( m == MELODY_NONE ){
        return;
    }
    unsigned long t = millis();
    if( melody_playing != m ){
        melody_playing = m;
        melody_note = 0;
        play_next = 0;
    }
    if( melody_playing && t > play_next ){
        uint16_t *melody = getMelody(m);
        if( melody[melody_note] != 4096 ){
            if( !melody[melody_note] ){
                noTone(pinSpeaker);
            } else {
                tone(pinSpeaker, melody[melody_note], melody[melody_note+1]);
            }
            play_next = t + melody[melody_note+1]*1.11;
            melody_note += 2;
        } else if( cycle ){
            melody_note = 0;
            play_next = 0;
        }
    }
}

char *RobotClass_SnakeGame::st_itoa(unsigned long v){
  volatile unsigned char i = 11;
  st_itoa_buf[11] = '\0';
  while( i > 0)
  {
      i--;
      st_itoa_buf[i] = (v % 10)+'0';
      v /= 10;
      if ( v == 0 )
  break;
  }
  return st_itoa_buf+i;
}
