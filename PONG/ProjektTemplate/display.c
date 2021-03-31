#include <stdint.h>
#include <pic32mx.h>
#include "header.h"
uint8_t pixels[128*4] = {0};
int advancedCount = 0;
int temp1 = 0;
int temp2 = 0;
int temp3 = 0;
int temp4 = 0;
int highScore = 0;

//Simple delay
void quicksleep(int cyc) {
    int i;
    for (i = cyc; i > 0; i--);
}

//SPI send/recieve. A vital function for the exchange of data between master/slave
uint8_t spi_send_recv(uint8_t data) {
    while(!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while(!(SPI2STAT & 1));
    return SPI2BUF;
}

//Setup for the SPI-protocol. Will only be called once in the beginning of the program
void spi_init(){

    SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
    SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
    while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
    while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
    SYSKEY = 0x0;  /* Lock OSCCON */

    /* Set up output pins */
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;

    /* Output pins for display signals */
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;

    /* Set up input pins */
    TRISDSET = (1 << 8);
    TRISFSET = (1 << 1);

    /* Set up SPI as master */
    SPI2CON = 0;
    SPI2BRG = 4;
    /* SPI2STAT bit SPIROV = 0; */
    SPI2STATCLR = 0x40;
    /* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
    /* SPI2CON bit MSTEN = 1; */
    SPI2CONSET = 0x20;
    /* SPI2CON bit ON = 1; */
    SPI2CONSET = 0x8000;
}

//Setup for the screen. Will only be called once in the beginning of the program
void display_init(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);
    DISPLAY_ACTIVATE_VDD;
    quicksleep(1000000);

    spi_send_recv(0xAE);
    DISPLAY_ACTIVATE_RESET;
    quicksleep(10);
    DISPLAY_DO_NOT_RESET;
    quicksleep(10);

    spi_send_recv(0x8D);
    spi_send_recv(0x14);

    spi_send_recv(0xD9);
    spi_send_recv(0xF1);

    DISPLAY_ACTIVATE_VBAT;
    quicksleep(10000000);

    spi_send_recv(0xA1);
    spi_send_recv(0xC8);

    spi_send_recv(0xDA);
    spi_send_recv(0x20);

    spi_send_recv(0xAF);
}

//A function to reset the pixels array to {0} (blackscreen)
void resetScreen(uint8_t param[]) {
    int i = 0;
    for(; i  < sizeof(pixels); i++) {
        pixels[i] = param[i];
    }
}

//Updating the display
void renderScreen(uint8_t data[]) {
    int i, j;

    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0 & 0xF);
        spi_send_recv(0x10 | ((0 >> 4) & 0xF));

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 128; j++)
            spi_send_recv(data[i*128 + j]);
    }
}

//------------------------------------------#DRAWING_ON_DISPLAY_FUNCTIONS#----------------------------------------------

//Writing a single pixel to the display
void light_pixel(int x, int y) {
    int offset = 0;
    if (y > 0) {
        offset = y / 8;
    }
    pixels[offset * 128 + x] |= 1 << (y - offset * 8);
}

void turnoff_pixel(int x, int y) {
    int offset = 0;
    if (y > 0) {
        offset = y / 8;
    }
    hs_tarena[offset * 128 + x] = (0 << (y - offset * 8));
}

//A function to reset the pixels array to {0} (blackscreen)
void highscore_update(uint8_t param[], int x, int y) {
    int i = 0;
    for(; i  < sizeof(pixels); i++) {
        hs_tarena[i] = param[i];
    }
    int j;
    int k;
    for(j = 0; j < 22; j++) {
        turnoff_pixel(x + j, y);
        for(k = 1; k < 6; k++) {
            turnoff_pixel(x + j, y + k);
        }
    }
}

//Writing the paddle to the display by using light_pixel();
void draw_paddle(Player p1, Player p2) {
    int i = 0, j = 0;
    for(; i < PADDLE_HEIGHT; i++) {
        light_pixel(p1.x, p1.y + i);
    }
    for(; j < PADDLE_HEIGHT; j++) {
        light_pixel(p2.x, p2.y + j);
    }
}

void draw_paddle_training(Player p1) {
    int i = 0, j = 0;
    for (; i < PADDLE_HEIGHT; i++) {
        light_pixel(p1.x, p1.y + i);
    }
}

//Writing the ball to the display by using light_pixel();
void draw_ball(Ball b) {
    int i;
    int j;
    for(i = 0; i < BALL_LENGTH; i++) {
        light_pixel(b.x + i, b.y);
        for(j = 1; j < BALL_HEIGHT; j++) {
            light_pixel(b.x + i, b.y + j);
        }
    }
}

void draw_selector(int x, int y) {
    int i;
    int j;
    for(i = 0; i < 2; i++) {
        light_pixel(x + i, y);
        for(j = 1; j < 2; j++) {
            light_pixel(x + i, y + j);
        }
    }
}

void draw_colon() {
    light_pixel(60, 5);
    light_pixel(60, 8);
}

//The function which is called from main.c when we want to update the screen
void draw(Player p1, Player p2, ScoreP1 sp1, ScoreP2 sp2, Ball b) {
    draw_paddle(p1, p2);
    draw_ball(b);
    draw_colon();
    draw_score(p1.score,sp1.x ,sp1.y);
    draw_score(p2.score, sp2.x, sp2.y);
    advancedCount++;
}

void draw_training(Player p1, ScoreP1 sp1, Ball b) {
    if(highScore <= p1.score){
        resetScreen(tarena);
    }else{
        resetScreen(hs_tarena);
    }

    temp1 = p1.score;

    if(p1.score >= 10 || p1.score < 100) {
        temp1 = p1.score % 10;
        temp2 = p1.score / 10;
    }
    if(p1.score >= 100 || p1.score < 1000){
        temp3 = p1.score / 100;
        temp2 = (p1.score / 10) % 10;
        temp1 = p1.score % 10;
    }
    if(p1.score >= 1000){
        temp4 = p1.score / 1000;
        temp3 = (p1.score / 100) % 10;
        temp2 = (p1.score / 10) % 10;
        temp1 = p1.score % 10;
    }

    draw_score(temp1,  124, 25);
    draw_score(temp2, 118, 25);
    draw_score(temp3, 112, 25);
    draw_score(temp4, 106, 25);

    if(highScore <= p1.score){
        draw_score(temp1,  124, 8);
        draw_score(temp2, 118, 8);
        draw_score(temp3, 112, 8);
        draw_score(temp4, 106, 8);
    }

    if(highScore <= p1.score){
        highscore_update(pixels, 104, 25);
        highScore = p1.score;
    }
    draw_paddle_training(p1);
    draw_ball(b);
}

void draw_score(int select, int x, int y) {
    int i = 0, j = 0;
    switch(select) {
        case 0:
            light_pixel(x, y);
            light_pixel(x - 1, y);
            light_pixel(x - 2, y + 1);
            light_pixel(x - 2, y + 2);
            light_pixel(x - 2, y + 3);
            light_pixel(x - 2, y + 4);
            light_pixel(x + 1, y + 1);
            light_pixel(x + 1, y + 2);
            light_pixel(x + 1, y + 3);
            light_pixel(x + 1, y + 4);
            light_pixel(x, y + 5);
            light_pixel(x - 1, y + 5);
            break;
        case 1:
            light_pixel(x, y);
            light_pixel(x - 1, y + 1);
            light_pixel(x, y + 1);
            light_pixel(x, y + 2);
            light_pixel(x, y + 3);
            light_pixel(x, y + 4);
            light_pixel(x, y + 5);
            light_pixel(x - 1, y + 5);
            light_pixel(x + 1, y + 5);
            break;
        case 2:
            for (; i < 4; i++) {
                light_pixel(x - 2 + i, y + 5);
            }
            for(; j < 3; j++) {
                light_pixel(x - 1 + j, y + 4 - j);
            }
            light_pixel(x - 2, y + 1);
            light_pixel(x - 1, y);
            light_pixel(x, y);
            light_pixel(x + 1, y + 1);
            break;
        case 3:
            light_pixel(x, y);
            light_pixel(x - 1, y);
            light_pixel(x + 1, y + 1);
            light_pixel(x, y + 2);
            light_pixel(x - 1, y + 2);
            light_pixel(x + 1, y + 3);
            light_pixel(x + 1, y + 4);
            light_pixel(x - 1, y + 5);
            light_pixel(x, y + 5);
            break;
        case 4:
            light_pixel(x + 1, y);
            light_pixel(x, y + 1);
            light_pixel(x - 1, y + 2);
            light_pixel(x - 1, y + 3);
            light_pixel(x, y + 3);
            light_pixel(x + 1, y + 3);
            light_pixel(x + 1, y + 4);
            light_pixel(x + 1, y + 5);
            break;
        case 5:
            light_pixel(x, y);
            light_pixel(x + 1, y);
            light_pixel(x - 1, y);
            light_pixel(x - 1, y + 1);
            light_pixel(x - 1, y + 2);
            light_pixel(x, y + 2);
            light_pixel(x + 1, y + 2);
            light_pixel(x + 1, y + 3);
            light_pixel(x + 1, y + 4);
            light_pixel(x, y + 5);
            light_pixel(x - 1, y + 5);
            break;
        case 6:
            light_pixel(x, y);
            light_pixel(x - 1, y);
            light_pixel(x - 2, y + 1);
            light_pixel(x - 2, y + 2);
            light_pixel(x - 2, y + 3);
            light_pixel(x - 2, y + 4);
            light_pixel(x - 1, y + 2);
            light_pixel(x, y + 2);
            light_pixel(x + 1, y + 3);
            light_pixel(x + 1, y + 4);
            light_pixel(x, y + 5);
            light_pixel(x - 1, y + 5);
            break;
        case 7:
            light_pixel(x + 1, y);
            light_pixel(x, y);
            light_pixel(x - 1, y);
            light_pixel(x - 2, y);
            light_pixel(x + 1, y + 1);
            light_pixel(x, y + 2);
            light_pixel(x - 1, y + 3);
            light_pixel(x - 1, y + 4);
            light_pixel(x - 1, y + 5);
            break;
        case 8:
            light_pixel(x, y);
            light_pixel(x - 1, y);
            light_pixel(x - 2, y + 1);
            light_pixel(x + 1, y + 1);
            light_pixel(x, y + 2);
            light_pixel(x - 1, y + 2);
            light_pixel(x - 2, y + 3);
            light_pixel(x - 2, y + 4);
            light_pixel(x + 1, y + 3);
            light_pixel(x + 1, y + 4);
            light_pixel(x, y + 5);
            light_pixel(x - 1, y + 5);
            break;
        case 9:
            light_pixel(x, y);
            light_pixel(x - 1, y);
            light_pixel(x + 1, y + 1);
            light_pixel(x + 1, y + 2);
            light_pixel(x + 1, y + 3);
            light_pixel(x + 1, y + 4);
            light_pixel(x - 2, y + 1);
            light_pixel(x - 2, y + 2);
            light_pixel(x - 1, y + 3);
            light_pixel(x, y + 3);
            light_pixel(x, y + 5);
            light_pixel(x - 1, y + 5);
            break;
        default:
            light_pixel(x, y);
            light_pixel(x - 1, y);
            light_pixel(x + 1, y + 1);
            light_pixel(x + 1, y + 2);
            light_pixel(x + 1, y + 3);
            light_pixel(x + 1, y + 4);
            light_pixel(x - 2, y + 1);
            light_pixel(x - 2, y + 2);
            light_pixel(x - 1, y + 3);
            light_pixel(x, y + 3);
            light_pixel(x, y + 5);
            light_pixel(x - 1, y + 5);
            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------