/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <stdlib.h>

#include "ssd1306.h"

#define DISPLAY_ADDRESS 0x3C // 011110+SA0+RW - 0x3C or 0x3D

int main(){

    I2COLED_Start();

    CyGlobalIntEnable;

    display_init(DISPLAY_ADDRESS);

    
    for(;;){

        // clearing display
        display_clear();    
        display_update();    


        // drawing a pixel
        gfx_drawPixel(40, 40, WHITE);    
        display_update(); 

        CyDelay(1000);
        display_clear();    
        display_update();    

        
        // line drawing
        int16_t i = 0;
        for( i = 0 ; i < gfx_width(); i+=4) {
            gfx_drawLine(0, 0, i, gfx_height()-1, WHITE);
            display_update();
        }
        for( i = 0 ; i < gfx_height() ; i += 4 ){
            gfx_drawLine(0, 0, gfx_width()-1, i, WHITE);
            display_update();
        }    
     
        CyDelay(3000);
        display_clear();    
        display_update();    

        // text settings
        gfx_setTextSize(1);
        gfx_setTextColor(WHITE);
        gfx_setCursor(0,0);

        // text display
        for( i = 0 ; i < 168 ; i++){
            if( i == '\n' ) 
                continue;
            gfx_write(i);
          }    
        display_update(); 
        
        CyDelay(3000);
        display_clear();    
        display_update();    
        
        // cursor
        gfx_setCursor( 0, 0 );
        
        // text sizes
        gfx_setTextSize(2);
        gfx_write( '8' );
        gfx_setTextSize(3);
        gfx_write( '8' );
        gfx_setTextSize(4);
        gfx_write( '8' );
        display_update(); 
        
        CyDelay(3000);
        display_clear();    
        display_update();    

        // line printing
        gfx_setTextSize(1);
        gfx_setTextColor(WHITE);
        gfx_setCursor(0,0);
        gfx_println("test line 1 ...");
        gfx_println("2nd line ...");
        display_update();    

        CyDelay(3000);
        display_clear();    
        display_update();    

        
        // scrolling
        gfx_setTextSize(1);
        gfx_setTextColor(WHITE);
        gfx_setCursor(0,0);

        for( i = 1 ; i < 16 ; i++ ){
            gfx_println("scroll");
        
        }
        display_update();    
        
        
        
        display_scroll(SCROLL_PAGE_0,SCROLL_PAGE_7,SCROLL_RIGHT,SCROLL_SPEED_7);
        CyDelay(2000);
        display_stopscroll();
        display_scroll(SCROLL_PAGE_2,SCROLL_PAGE_4,SCROLL_LEFT,SCROLL_SPEED_3);
        CyDelay(2000);
        display_stopscroll();
        
        

        CyDelay(3000);
        display_clear();    
        display_update();    
        
        
        // rectangles
        for( i = 0 ; i < gfx_height()/2 ; i += 2 ){
            gfx_drawRect(i, i, gfx_width()-2*i, gfx_height()-2*i, WHITE);
            display_update();   
        }
        
        CyDelay(3000);
        display_clear();    
        display_update();    

        // filled rectangles
        uint8_t color = 1;
        for( i = 0 ; i < gfx_height()/2 ; i += 3 ){
            // alternate colors
            gfx_fillRect(i, i, gfx_width()-i*2, gfx_height()-i*2, color%2);
            display_update();    
            color++;
        }    

        CyDelay(3000);
        display_clear();    
        display_update();    
        
        // circles
        for( i = 0 ; i < gfx_height() ; i += 2 ){
            gfx_drawCircle( gfx_width()/2, gfx_height()/2, i, WHITE );
            display_update();    
        }    

        CyDelay(3000);
        display_clear();    
        display_update();    
        
        
    #define MIN(x, y) (((x) < (y)) ? (x) : (y))
        
        // triangles
        for( i = 0 ; i < MIN(gfx_width(),gfx_height())/2; i+=5) {
            gfx_drawTriangle(gfx_width()/2, gfx_height()/2-i,
                             gfx_width()/2-i, gfx_height()/2+i,
                             gfx_width()/2+i, gfx_height()/2+i, WHITE);
            display_update();    
        }    

        
        CyDelay(3000);
        display_clear();    
        display_update();    
       
        // rotation
        int16_t j = 0;
        for( j = 0 ; j < 4 ; j++ ){
            gfx_setRotation(j);
            for( i = 0 ; i < MIN(gfx_width(),gfx_height())/2; i+=5) {
                gfx_drawTriangle(gfx_width()/2, gfx_height()/2-i,
                                 gfx_width()/2-i, gfx_height()/2+i,
                                 gfx_width()/2+i, gfx_height()/2+i, WHITE);
                display_update(); 
                CyDelay(200);
            }    
        }
        
        CyDelay(3000);
       
        // invert and dim
        display_invert(1);
        CyDelay(1000);
        
        for( i = 256 ; i >= 0 ; i-- ){
            display_contrast(i); 
            CyDelay(10);
        }
        
        
        
        display_invert(0);
        CyDelay(1000);
        for( i = 0 ; i < 256 ; i++ ){
            display_contrast(i); 
            CyDelay(10);
        }
        
    
        CyDelay(3000);

    
    
    
    }
}

/* [] END OF FILE */
