#include "globals.h"
#include "boot_animation.h"
#include "init_screen.h"

int animation_state = 0;

void draw_boot_logo(){
    if (setupInitScreen == true){
    initScreen();
    setupInitScreen = false;
    global_screen_value +=1;
    }

    for (int x = 0; x <9; x++){
        delay(100);
        if (x == 1) delay(400);
        animation_state = x;
        drawAnimation();
    }
    delay(500);
}

void drawAnimation() {
 u8g2.firstPage();
 do {
   switch (animation_state) {
     case 0:
     u8g2.setColorIndex(0);
       u8g2.drawXBMP(0, 0, logo0_width, logo0_height, logo0_bits);
       break;
     case 1:
       u8g2.drawXBMP(0, 0, logo1_width, logo1_height, logo1_bits);
       break;
     case 2:
       u8g2.drawXBMP(0, 0, logo2_width, logo2_height, logo2_bits);
       break;
     case 3:
       u8g2.drawXBMP(0, 0, logo3_width, logo3_height, logo3_bits);
       break;
     case 4:
       u8g2.setColorIndex(1);
       u8g2.drawXBMP(0, 0, logo4_width, logo4_height, logo4_bits);
       break;
     case 5:
       u8g2.drawXBMP(0, 0, logo5_width, logo5_height, logo5_bits);
       break;
    case 6:
       u8g2.drawXBMP(0, 0, logo6_width, logo6_height, logo6_bits);
       break;
    case 7:
       u8g2.drawXBMP(0, 0, logo7_width, logo7_height, logo7_bits);
       break;
    case 8:
       u8g2.drawXBMP(0, 0, logo8_width, logo8_height, logo8_bits);
       break;
   }
 } while ( u8g2.nextPage() );
}