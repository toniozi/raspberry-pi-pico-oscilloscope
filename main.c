#include "GUI_Paint.h"
#include "LCD_2in.h"

#include <string.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

#define arrLength 320+1

#define ADC_NUM 0
#define ADC0_PIN (26 + ADC_NUM)
#define ADC1_PIN (27 + ADC_NUM)
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))    

static volatile bool timer_completed;

int limit = 0;

UWORD *BlackImage;

int coordY[arrLength];

float firstinarr, secondinarr;

float step, previous_step;

// Core 1 Main Code
void core1_entry() {
    int coordYfifo[arrLength];
    
    while (1){
        //time scaling : read pot value
        adc_select_input(1);
        uint adc_pot2;        
        adc_pot2 = adc_read();
        float scale_voltage = adc_pot2 * ADC_CONVERT;
        float toScale = (
                    (scale_voltage)
                    *320)
                    /(3.3);
        int scaleSize = (int)toScale;
        float arrLenthinfloat= (float)arrLength;
        step = arrLenthinfloat/toScale;
        
        for (size_t 𓂀 = 0; 𓂀 < arrLength; 𓂀++)
        {
            Paint_DrawLine((𓂀)*previous_step, coordYfifo[𓂀], (𓂀+1)*previous_step, coordYfifo[𓂀+1], BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
             
            coordYfifo[𓂀]=(int) multicore_fifo_pop_blocking();
            if (𓂀>0)
            {
                Paint_DrawLine((𓂀-1)*step, coordYfifo[𓂀-1], 𓂀*step, coordYfifo[𓂀], BLUE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
            }       
        }
        previous_step=step;
        LCD_2IN_Display((UBYTE *) BlackImage);
    }    
}

static void capture_adc_values(void) {
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);
    
    //capture value between 0 and 3.3v
    adc_select_input(0);
    uint adc_raw;    
    adc_raw = adc_read();   
    float new_voltage = adc_raw * ADC_CONVERT;  
     
    //convert to screen value
    float float_screen_value = (
                (new_voltage)
                *240)
                /(3.3);
    int screen_value = (int)float_screen_value;
      
    if (limit==0){ 
        //locking mechanism     
        adc_select_input(2);
        uint adc_pot;
        adc_pot=adc_read();
        float new_pot_voltage;
        new_pot_voltage = adc_pot * ADC_CONVERT;
       
        float range = 0.02;
        if ((new_pot_voltage-range)>new_voltage || new_voltage>(new_pot_voltage+range)){
            timer_completed=true;
            return;
        }            
        firstinarr=new_voltage;           
    }
    
    if (limit==1){
        if (new_voltage<firstinarr){
            limit=0;
            timer_completed=true;
            return;
        }        
    }
    if (limit==2){
        if (secondinarr>firstinarr)
        {
            limit=0;
            timer_completed=true;
            return;
        }        
    }
    
    memmove(coordY, coordY+1, sizeof(coordY)-sizeof(coordY[0]));
    coordY[arrLength-1]=screen_value;
    
    limit++;
    if (limit==arrLength)
    {
        for (size_t i = 0; i < arrLength; i++)
        {
            multicore_fifo_push_blocking((uint32_t) coordY[i]);
        }
        limit=0;
    }

    timer_completed = true;
}

static void interrupt_function(uint32_t delay_us) {
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);       
    
    irq_set_exclusive_handler(ALARM_IRQ, capture_adc_values);
    irq_set_enabled(ALARM_IRQ, true);

    uint64_t target = timer_hw->timerawl + delay_us;
    timer_hw->alarm[ALARM_NUM] = (uint32_t) target;
}

void initPins(){    
    stdio_init_all();

    int pins[] = {15, 17, 2, 3};
    for (size_t i = 0; i < 4; i++)
    {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], GPIO_IN);
        gpio_pull_up(pins[i]);
    }

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
}

int main(void)
{   
    stdio_init_all();    

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    adc_init();
    adc_gpio_init(ADC0_PIN);

    if(DEV_Module_Init()!=0){return -1;}
    multicore_launch_core1(core1_entry);
    
    DEV_SET_PWM(50);
    LCD_2IN_Init(HORIZONTAL);
    LCD_2IN_Clear(BLACK);
    
    UDOUBLE Imagesize = LCD_2IN_HEIGHT*LCD_2IN_WIDTH*2;
    
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    Paint_NewImage((UBYTE *)BlackImage,LCD_2IN.WIDTH,LCD_2IN.HEIGHT, 270, BLACK);
    Paint_SetScale(65);
    Paint_Clear(BLACK);
    DEV_Delay_ms(1000);
    Paint_Clear(BLACK);
    Paint_SetRotate(90);

    int led_check = 1;

    bool state;
    while (true){  
        timer_completed = false;

        interrupt_function(1);
        while (!timer_completed); 
    }       
    return 0;
}