/* pong.h (originally mipslab.h)
   Header file for project.
   This file was originally written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson
   Updated by Leo Hansson Åkerberg and Erik Vinblad

   Latest update 2020-02-25 by Leo Hansson Åkerberg

   For copyright and licensing, see file COPYING */

//----------------------------------------------------------------------------------------------------------------------
int gameSpeed;
int advancedCount;
int MULTIPLAYER;
int SINGLE_SELECT;
int DIFF;

//Control for period value of clock (value may not override 2^16)
#define TMR2PERIOD (80000000 / (2560*4))
#if TMR2PERIOD > 0xffff
#error "Timer period is too big."
#endif


//----------------------------------------------------------------------------------------------------------------------

//GLOBAL VARIABLES PIC-DISPLAY
#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)
#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)
#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)
#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

//GLOBAL VARIABLES GAME
#define BUTTON_COMMON (getbtns() & 0x2)
#define BUTTON_P2 (getbtns() & 0x1)
#define BUTTON_P1 (getbtns() & 0x4)
#define SWITCH4 (getsw() & 0x8)
//#define SWITCHFAST (getsw() & 0x4)
#define SWITCH1 (getsw() & 0x1)
#define PADDLE_HEIGHT 8
#define SCORE_HEIGHT 6
#define BALL_SPEED 1
#define BALL_LENGTH 3
#define BALL_HEIGHT 3
#define GAME_SPEED_SLOW (80000000 / (2560*4))
#define GAME_SPEED_FAST (80000000 / (2560*8))

//DISPLAY.c
uint8_t spi_send_recv(uint8_t data);
void quicksleep();
void spi_init();
void display_init();
void resetScreen();
void renderScreen();
void draw_selector();
void light_pixel();
void draw_paddle();
void draw_paddle_training();
void draw_ball();
void draw_score();
void draw();
void draw_training();

//LABWORK.s
void delay();

//INPUT
int getbtns();
int getsw();

//GAME.c
void init();
void advanceBall();
void game_init();
void win();
int rand(void);

//PICTUREFRAMEWORK.c
extern uint8_t start[128*4];
extern uint8_t arena[128*4];
extern uint8_t tarena[128*4];
extern uint8_t pixels[128*4];
extern uint8_t menu[128*4];
extern uint8_t p1wins[128*4];
extern uint8_t p2wins[128*4];
extern uint8_t diff[128*4];
extern uint8_t sMode[128*4];
extern uint8_t gameOver[128*4];
extern uint8_t hs_tarena[128*4];


//STRUCTS
typedef struct Player {
    short x , y, speed, score;
}Player;

typedef struct Ball {
    short x, y, speedX, speedY;
}Ball;

typedef struct ScoreP1 {
    short x, y;
}ScoreP1;

typedef struct ScoreP2 {
    short x, y;
}ScoreP2;

Player p1;
Player p2;
Player cpu;
Ball b;
ScoreP1 sp1;
ScoreP2 sp2;


