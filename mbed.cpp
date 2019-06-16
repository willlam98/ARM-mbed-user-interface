#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include <string>
//Switch input definition
#define SW_PIN p21
#define SW_PIN1 p22
#define SW_PIN2 p23
#define SW_PIN3 p24

//Sampling period for the switch oscillator (us)
#define SW_PERIOD 20000 

//Display interface pin definitions
#define D_MOSI_PIN p5
#define D_CLK_PIN p7
#define D_DC_PIN p8
#define D_RST_PIN p9
#define D_CS_PIN p10

//an SPI sub-class that sets up format and clock speed
class SPIPreInit : public SPI
{
public:
    SPIPreInit(PinName mosi, PinName miso, PinName clk) : SPI(mosi,miso,clk)
    {
        format(8,3);
        frequency(200000000000); // added 5 more zeros
    };
};

//Interrupt Service Routine prototypes (functions defined below)
void sedge();
void sedge1();
void sedge2();
void sedge3();
void tout();
void point(int loc);
//Output for the alive LED
DigitalOut alive(LED1);

//PwmOut define
PwmOut PwmPin (p25);

AnalogOut aout(p18);


//External interrupt input from the switch oscillator
InterruptIn swin(SW_PIN);
InterruptIn swin1(SW_PIN1);
InterruptIn swin2(SW_PIN2);
InterruptIn swin3(SW_PIN3);

//Switch sampling timer
Ticker swtimer;

//Registers for the switch counter, switch counter latch register and update flag
volatile uint16_t scounter=0;
volatile uint16_t scount=0;
volatile uint16_t update=0;

volatile uint16_t scounter1=0;
volatile uint16_t scount1=0;
volatile uint16_t update1=0;

volatile uint16_t scounter2=0;
volatile uint16_t scount2=0;
volatile uint16_t update2=0;

volatile uint16_t scounter3=0;
volatile uint16_t scount3=0;
volatile uint16_t update3=0;

const double pi = 3.141592653589793238462;
const double amplitude = 0.5f;
const double offset = 65535/2;
double rads = 0.0;

//Initialise SPI instance for communication with the display
SPIPreInit gSpi(D_MOSI_PIN,NC,D_CLK_PIN); //MOSI,MISO,CLK

//Initialise display driver instance
Adafruit_SSD1306_Spi gOled1(gSpi,D_DC_PIN,D_RST_PIN,D_CS_PIN,64,128); //SPI,DC,RST,CS,Height,Width

int main() { 
    int freq;
    int d0 = 0, d1 = 0, d2 = 0, d3 = 0;

    double period = 0.000015484; // start from 65kHz
    //Initialisation
    gOled1.setRotation(2); //Set display rotation

    //PwmPin.period(period);
    
    //Attach switch oscillator counter ISR to the switch input instance for a rising edge
    swin.rise(&sedge);
    swin1.rise(&sedge1);
    swin2.rise(&sedge2);
    swin3.rise(&sedge3);
    
    //Attach switch sampling timer ISR to the timer instance with the required period
    swtimer.attach_us(&tout, SW_PERIOD);
    
    //Write some sample text
    //gOled1.printf("%ux%u OLED Display\r\n", gOled1.width(), gOled1.height());
    int dot = 0;
    gOled1.clearDisplay();

    //Main loop
    while(1)
    {
 
        //Has the update flag been set? 
        
              
        if (update) {
            
            //Clear the update flag
            update = 0;
            
            //Set text cursor
            gOled1.setTextCursor(0,0);
            
            if( scount*1000000/SW_PERIOD < 67000){
                //reset button
                
                d0 = 0;
                d1 = 0;
                d2 = 0;
                d3 = 0;
                //gOled1.printf("\nSW0 ON \r");
                freq = d0*1000 + d1*100 + d2*10 + d3;
                PwmPin.period((double) 1/freq);     
                PwmPin.write(0.50f);
                

            }
            else{
                //gOled1.printf("\nSW0 OFF \r");    
            }
            
            
            //Write the latest switch osciallor count
            if( scount1*1000000/SW_PERIOD < 65000){
                //gOled1.printf("\nSW0 ON \r");
                if(dot != 0){
                    dot--;
                }
            }
            else{
                //gOled1.printf("\nSW0 OFF \r");    
            }
            
            
            if( scount2*1000000/SW_PERIOD < 65000){
                //gOled1.printf("\nSW0 ON \r");
                dot++;
            }
            else{
                //gOled1.printf("\nSW0 OFF \r");    
            }
            
            
            if( scount3*1000000/SW_PERIOD < 65000){
                //gOled1.printf("\nSW0 ON \r");
                if(dot%4 == 0){
                    if(d0 >= 9){
                        d0 = 0;
                    }else{
                        d0++;
                    }
                }
                
                if(dot%4 == 1){
                    if(d1 >= 9){
                        d1 = 0;
                    }else{
                        d1 ++;;
                    }
                }
                
                if(dot%4 == 2){
                    if(d2 >= 9){
                        d2 = 0;
                    }else{
                        d2 ++;;
                    }
                }
                
                if(dot%4 == 3){
                    if(d3 >= 9){
                        d3 = 0;
                    }else{
                        d3 ++;;
                    }
                }
            freq = d0*1000 + d1*100 + d2*10 + d3;
            PwmPin.period((double) 1/freq);     
            PwmPin.write(0.50f);
                
            }
            else{
                //gOled1.printf("\nSW0 OFF \r");    
            }
            
            //show frequencies

//            gOled1.printf("\n%05u  ",scount*1000000/SW_PERIOD);
//            gOled1.printf("\n%05u  ",scount1*1000000/SW_PERIOD);
//            gOled1.printf("\n%05u  ",scount2*1000000/SW_PERIOD);
//            gOled1.printf("\n%05u  ",scount3*1000000/SW_PERIOD);
            
            gOled1.printf("\nSW0: Reset Button \r");  
            gOled1.printf("\nSW1: go left \r");  
            gOled1.printf("\nSW2: go right\r");  
            gOled1.printf("\nSW3: Increment digit ");  
        
            
//            freq = d0*1000 + d1*100 + d2*10 + d3;
            gOled1.printf("Frequency: \r");    
            gOled1.printf("\n%d",d0);    
            gOled1.printf("%d", d1);    
            gOled1.printf("%d", d2);    
            gOled1.printf("%d", d3);
            point(dot);
//            PwmPin.period((double) 1/freq);     
//            PwmPin.write(0.50f);
            
            //Copy the display buffer to the display
            gOled1.display();
            
            //Toggle the alive LED
            alive = !alive;
            wait(0.1);
        }
        
        
    }
}


//Interrupt Service Routine for rising edge on the switch oscillator input
void sedge() {
    //Increment the edge counter
    scounter++;    
}
void sedge1() {
    //Increment the edge counter
    scounter1++;    
}
void sedge2() {
    //Increment the edge counter
    scounter2++;    
}
void sedge3() {
    //Increment the edge counter
    scounter3++;    
}

//Interrupt Service Routine for the switch sampling timer
void tout() {
    //Read the edge counter into the output register
    scount = scounter;
    scount1 = scounter1;
    scount2 = scounter2;
    scount3 = scounter3;
    //Reset the edge counter
    scounter = 0;
    scounter1 = 0;
    scounter2 = 0;
    scounter3 = 0;
    //Trigger a display update in the main loop
    update = 1;
}

void point(int loc){
  
    gOled1.printf("\n"); 
    for(int i = 0; i < 4; i++){
        if(i == loc%4){
            gOled1.printf(".");  
        }
        else{
            gOled1.printf(" ");  
        }
    }
    
}