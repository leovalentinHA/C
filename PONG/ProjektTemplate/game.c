#include <stdint.h>
#include <stdbool.h>
#include <pic32mx.h>
#include "header.h"
//HEAD SWITCH-STATEMENT REGARDING OVERALL GAME-STATE
#define GAME_STATE_MENU 0
#define GAME_STATE_PONG 1
#define GAME_STATE_END 2
//SUB SWITCH-STATEMENT REGARDING MENU
#define MENU_STATE_START_MENU 0
#define MENU_STATE_SINGLE_SELECT 1
#define MENU_STATE_DIFF 2

int gameState = GAME_STATE_MENU;
int menuState = MENU_STATE_START_MENU;
int ballcount = 0;
int targetPos = 0;
int lastBounceX;
int distToPaddle;
int hitY = 16;
int ftime1 = 0;
int ftime2 = 0;
int tmr2counter = 0;

//--------------------------------------------------#CLOCK#------------------------------------------------------------

void init(void) {
    TRISD &= (0x7f << 5);       //SETUP BUTTONS AND SWITCHES AS INPUTS IN TRI-STATE REGISTER

    //Timer_setup
    T2CON = 0x70;               //Stop the clock and set the prescaling to 256
    gameSpeed = (80000000 / (2560*4));
    PR2 = gameSpeed;           //Set the period to 1/10th of a second: (80000000/256)/10, which is within 2^16
    TMR2 = 0;                   //Reset the clock
    T2CONSET = 0x8000;          //Start the timer!

    //Interrupt_setup
    IEC(0) = (1 << 8);
    IPC(2) = 4;
    IECSET(0) = 0x00008000;
    IPC(3) = 0x1f000000;
    return;
}

void reinit(speed) {
    TRISD &= (0x7f << 5);       //SETUP BUTTONS AND SWITCHES AS INPUTS IN TRI-STATE REGISTER

    //Timer_setup
    T2CON = 0x70;               //Stop the clock and set the prescaling to 256
    PR2 = speed;           //Set the period to 1/10th of a second: (80000000/256)/10, which is within 2^16
    TMR2 = 0;                   //Reset the clock
    T2CONSET = 0x8000;          //Start the timer!

    //Interrupt_setup
    IEC(0) = (1 << 8);
    IPC(2) = 4;
    IECSET(0) = 0x00008000;
    IPC(3) = 0x1f000000;
    return;
}

//-----------------------------------------------#INPUT_FUNCTIONS#------------------------------------------------------

int getbtns(void) {
    int tmp = ((PORTD >> 5) & 0x7);
    return tmp;
}

int getsw(void) {
    int tmp = ((PORTD >> 8) & 0xf);
    return tmp;
}

//------------------------------------------------#GAME_FUNCTIONS#------------------------------------------------------

void advanceBall() {    //BALL-MOVEMENT IN X- & Y-AXIS
    b.x += b.speedX;
    b.y += b.speedY;
}

void game_init() {          //INITIALIZE GAME
    renderScreen(start);    //START-SCREEN
    delay(1000);

    //INITIALIZE DISPLAY_POS AND SPEED FOR ALL OBJECTS
    p1.x = 1;                //SET PADDLE'S AND BALL'S POSITION
    p1.y = 12;
    p1.score = 0;
    p2.x = 126;
    p2.y = 12;
    p2.score = 0;
    b.x = tmr2counter % 40 + 40;
    b.y = tmr2counter % 14 + 8;
    if(tmr2counter % 2 == 0){
        b.speedX = 1;
    }else{
        b.speedX = -1;
    }if(tmr2counter % 2 == 0){
        b.speedY = -1;
    }else {
        b.speedY = 1;
    }
    sp1.x = 53;
    sp1.y = 4;
    sp2.x = 68;
    sp2.y = 4;
}

void posReset(){
    p1.x = 1;                //SET PADDLE'S AND BALL'S POSITION
    p1.y = 12;
    p1.score = 0;
    p2.x = 126;
    p2.y = 12;
    p2.score = 0;
    b.x = tmr2counter % 40 + 40;
    b.y = tmr2counter % 14 + 8;
    if(tmr2counter % 2 == 0){
        b.speedX = 1;
    }else{
        b.speedX = -1;
    }if(tmr2counter % 2 == 0){
        b.speedY = -1;
    }else {
        b.speedY = 1;
    }
    sp1.x = 53;
    sp1.y = 4;
    sp2.x = 68;
    sp2.y = 4;
}

void game_mechanics(int mp, int ss, int diff) {
    //PADDLE MECHANICS
    //MOVE UP AND DOWN PLAYER1 PADDLE UNTIL BARRIER
    if (BUTTON_P1) {
        if (!SWITCH4 && p1.y != 31 - PADDLE_HEIGHT) {
                p1.y++;
        } else if (SWITCH4 && p1.y != 1) {
                p1.y--;
        }
    }
    //MOVE UP AND DOWN PLAYER2 PADDLE UNTIL BARRIER (ONLY IF MULTIPLAYER MODE IS ACTIVATED)
    if (mp == 1) {
        if (BUTTON_P2) {
            if (!SWITCH1 && p2.y != 31 - PADDLE_HEIGHT) {
                p2.y++;
            } else if (SWITCH1 && p2.y != 1) {
                p2.y--;
            }
        }
    }
    //IF SINGLEPLAYER - INIT CPU
    if (!ss && !DIFF && b.x > 115 &&  !mp && b.speedX > 0) {           //BASIC CPU PLAYER
        if (p2.y < b.y && p2.y + PADDLE_HEIGHT < 31) {
            p2.y++;
        } else {
            p2.y--;
        }
    }
    if (!ss && DIFF && b.x > 80 && !mp && b.speedX > 0) { //ADVANCED CPU                                      //ADVANCED CPU PLAYER
        if (p2.y < b.y && p2.y + PADDLE_HEIGHT < 31) {
            p2.y++;
        } else {
            p2.y--;
        }
        /*if (b.x <= 127 - 30 && b.x > 127 - 60) {                            //IF BOUNCE HERE, ONE MORE BOUNCE X+30
            if (b.y >= 30 - BALL_HEIGHT + 1) {                              //SECOND LAST BOUNCE (DOWN)
                lastBounceX = b.x + 30;
                distToPaddle = p2.x - lastBounceX;
                hitY = 30 - distToPaddle;


            } else if (b.y <= 2) {
                lastBounceX = b.x + 30;
                distToPaddle = p2.x - lastBounceX;
                hitY = 1 + distToPaddle;
            }
        }
        if((p2.y + PADDLE_HEIGHT / 2) != hitY){
            if((p2.y + PADDLE_HEIGHT/2) > hitY){
                p2.y++;
            }else if ((p2.y) < hitY){
                p2.y--;
            }
        }*/
    }

    //BALL MECHANICS
    if (b.y <= 1 || b.y >= 31 - BALL_HEIGHT) {
        b.speedY *= -1;
    }
    if (b.x > 126 - BALL_LENGTH + 1 && (b.speedX > 0)) {
        p1.score++;
        b.speedX *= -1;
    }
    if (b.x < 1 && (b.speedX < 0)) {
        p2.score++;
        b.speedX *= -1;
    }
    if (b.x <= p1.x && b.y + BALL_HEIGHT - 1 > p1.y && b.y < p1.y + PADDLE_HEIGHT && b.speedX < 0) {
        b.speedX *= -1;
    } else if (b.x + BALL_LENGTH - 1 >= p2.x && b.y + BALL_HEIGHT - 1 > p2.y && b.y < p2.y + PADDLE_HEIGHT && b.speedX > 0) {
        b.speedX *= -1;
    }

    //BALL MOVEMENT
    ballcount++;
    if(ballcount = BALL_SPEED) {
        ballcount = 0;
        advanceBall();
    }
}

void game_mechanics_training(){
    if (BUTTON_P1) {
        if (!SWITCH4 && p1.y != 31 - PADDLE_HEIGHT) {
            p1.y++;
        } else if (SWITCH4 && p1.y != 1) {
            p1.y--;
        }
    }
    //BALL MECHANICS
    if (b.y <= 1 || b.y >= 30 - BALL_HEIGHT) {
        b.speedY *= -1;
    }
    if (b.x >= 81 - BALL_LENGTH && b.speedX > 0) {
        p1.score++;
        b.speedX *= -1;
    }

    if (((b.x <= 1) && (b.y > p1.y && b.y < p1.y + PADDLE_HEIGHT))) {
        b.speedX *= -1;
    }

    //BALL MOVEMENT
    ballcount++;
    if(ballcount = BALL_SPEED) {
        ballcount = 0;
        advanceBall();
    }
}

void win(Player p1, Player p2) {
    if (p1.score >= 5 || p2.score >= 5) {
        if (p1.score >= 5) {
            delay(500);
            resetScreen(p1wins);
            renderScreen(pixels);
            gameState = GAME_STATE_END;
        } else {
            delay(500);
            resetScreen(p2wins);
            renderScreen(pixels);
            gameState = GAME_STATE_END;
        }
    }
}

void lose(){
    if (b.x <= 0 && b.speedX < 0) {
        resetScreen(gameOver);
        renderScreen(pixels);
        delay(500);
        gameState = GAME_STATE_END;
    }
}

//------------------------------------------------#INTERRUPT_FUNCTIONS#-------------------------------------------------

void user_isr(void) {
    IFSCLR(0) = 0x100;                                  //CLEAR INTERRUPT FLAG
    tmr2counter++;

    switch(gameState) {
        case GAME_STATE_MENU:
        switch(menuState) {
            case MENU_STATE_START_MENU:
                resetScreen(menu);
                posReset();
                if (SWITCH4) {
                    draw_selector(20, 16);
                    renderScreen(pixels);
                    if (BUTTON_COMMON) {
                        MULTIPLAYER = 0;
                        menuState = MENU_STATE_SINGLE_SELECT;
                        resetScreen(sMode);
                        renderScreen(pixels);
                        if (SWITCH4) {
                            draw_selector(20, 16);
                            renderScreen(pixels);
                        } else {
                            draw_selector(20, 26);
                            renderScreen(pixels);
                        }
                        delay(500);
                    }
                } else {
                    draw_selector(20, 26);
                    renderScreen(pixels);
                    if (BUTTON_COMMON) {
                        MULTIPLAYER = 1;
                        SINGLE_SELECT = 0;
                        menuState = MENU_STATE_DIFF;
                        resetScreen(diff);
                        renderScreen(pixels);
                        if (SWITCH4) {
                            draw_selector(25, 16);
                            renderScreen(pixels);
                        } else {
                            draw_selector(25, 26);
                            renderScreen(pixels);
                        }
                        delay(500);

                    }
                }
                break;
            case MENU_STATE_SINGLE_SELECT:
                resetScreen(sMode);
                if (SWITCH4) {
                    draw_selector(20, 16);
                    renderScreen(pixels);
                    if (BUTTON_COMMON) {
                        SINGLE_SELECT = 1;
                        menuState = MENU_STATE_DIFF;
                        resetScreen(diff);
                        renderScreen(pixels);
                        if (SWITCH4) {
                            draw_selector(25, 16);
                            renderScreen(pixels);
                        } else {
                            draw_selector(25, 26);
                            renderScreen(pixels);
                        }
                        delay(500);
                    }
                } else {
                    draw_selector(20, 26);
                    renderScreen(pixels);
                    if (BUTTON_COMMON) {
                        SINGLE_SELECT = 0;
                        menuState = MENU_STATE_DIFF;
                        resetScreen(diff);
                        renderScreen(pixels);
                        if (SWITCH4) {
                            draw_selector(25, 16);
                            renderScreen(pixels);
                        } else {
                            draw_selector(25, 26);
                            renderScreen(pixels);
                        }
                        delay(500);
                    }
                }
                break;
            case MENU_STATE_DIFF:
                resetScreen(diff);
                if (SWITCH4) {
                    draw_selector(25, 16);
                    renderScreen(pixels);
                    if (BUTTON_COMMON) {
                        DIFF = 0;
                        gameState = GAME_STATE_PONG;
                        delay(500);
                    }
                } else {
                    draw_selector(25, 26);
                    renderScreen(pixels);
                    if (BUTTON_COMMON) {
                        DIFF = 1;
                        gameState = GAME_STATE_PONG;
                        delay(500);
                    }
                }
                break;
        }
            break;

        case GAME_STATE_PONG:
            if(ftime1 == 0) {
                if (DIFF == 1) {
                    reinit(GAME_SPEED_FAST);
                }
                ftime1++;
            }
            if(SINGLE_SELECT == 0) {
                resetScreen(arena);
                game_mechanics(MULTIPLAYER, SINGLE_SELECT, DIFF);
                draw(p1, p2, sp1, sp2, b);
                renderScreen(pixels);
                win(p1, p2);
            }else{
                game_mechanics_training();
                draw_training(p1, sp1, b);
                renderScreen(pixels);
                lose();
            }

            break;

        case GAME_STATE_END:
            if(ftime2 == 0) {
                if (DIFF == 1) {
                    reinit(GAME_SPEED_SLOW);
                }
                ftime2++;
            }
            ftime1 = 0;
            ftime2 = 0;
            if(BUTTON_COMMON) {
                resetScreen(menu);
                gameState = GAME_STATE_MENU;
                menuState = MENU_STATE_START_MENU;
                if (SWITCH4) {
                    draw_selector(20, 16);
                    renderScreen(pixels);
                } else {
                    draw_selector(20, 26);
                    renderScreen(pixels);
                }
                delay(500);
                break;
            }
    }
}