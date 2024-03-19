#include "airquality9.h"
#include "uart.h"
#include "as7262.h"
#include "i2c.h"
#include "ST7735.h"
#include "LCD_GFX.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <float.h>
#define BUTTON_PIN PD4

//#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define UV_THRESHOLD 0
#define SAMPLE_INTERVAL 300 // 5 minutes in seconds

int natural_light_score = 0;
int artificial_light_score = 0;
int total_samples = 0;
uint8_t aqi_uba;
uint8_t aqi_uba_prev = 0;
uint8_t aqi_status = 0;
uint8_t Light_update=0;
int scrlen = 0;
int scrlen_adc = 0;
float natural_light_percentage;
float artificial_light_percentage;

uint16_t adc;
uint16_t adc_prev;
uint8_t natural_light=0;
uint8_t artificial_light=0;
int loopcounter=0;

volatile uint8_t buttonPressed = 0;
volatile uint8_t prevbuttonstate = 0;
volatile uint8_t buttonState = 0;

//Timer interrupt setup
void Timer1_Init() {
    TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
    TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt
    OCR1A = 10000; // Set CTC compare value
    TCCR1B |= (1 << CS12) | (1 << CS10); // Start timer at Fcpu/1024
}

void initialize_adc()
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_read()
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

void analyze_light_sample(float uv_index, int v, int b, int g, int y, int o, int r) {
    // Check if the UV index is indicative of natural light
if (uv_index > UV_THRESHOLD) {
        // Assume natural light has a continuous distribution across all wavelengths
        if ((v > 0) && (b > 0) && (g > 0) && (y > 0) && (o > 0) && (r > 0)) {
            natural_light_score++;
            natural_light=1;
            artificial_light=0;

			//UART_putstring("Natural Light incremented");
        }

		else if ((v == 0) || (b == 0) || (g == 0) || (y == 0) || (o == 0) || (r == 0)) {
            artificial_light_score++;
            natural_light=0;
            artificial_light=1;
		//UART_putstring("Artificial Light incremented");
        }

	} else {
        // Artificial light may have peaks at specific wavelengths or missing wavelengths
        if ((v == 0) || (b == 0) || (g == 0) || (y == 0) || (o == 0) || (r == 0)) {
            artificial_light_score++;
            natural_light=0;
            artificial_light=1;
			//UART_putstring("Artificial Light incremented");
        }

        else{
            artificial_light_score++;
            natural_light=0;
            artificial_light=1;
			//UART_putstring("Artificial Light incremented");
        }
    }
    total_samples++;
}

void calculate_exposure_percentage() {
    natural_light_percentage = ((float)natural_light_score / (float)total_samples) * 100.0;
    artificial_light_percentage = ((float)artificial_light_score / (float)total_samples) * 100.0;
    char bufferN[50];
	char bufferA[50];
    // Print or store the percentage results for further analysis or plotting
    sprintf(bufferN,"Natural Light Exposure: %d%%\n", (int)natural_light_percentage);
	//UART_putstring(bufferN);
    sprintf(bufferA,"Artificial Light Exposure: %d%%\n", (int)artificial_light_percentage);
    //UART_putstring(bufferA);

}

//Timer interrupt service routine
ISR(TIMER1_COMPA_vect) {
    // Existing ISR logic for air quality status
    if(aqi_status==1){
        PORTD&=~(1<<PORTD5);
    }
    else if(aqi_uba>=3 && aqi_status==0){
        PORTD|=(1<<PORTD5);
        aqi_status = 1;
    }
    if(aqi_uba<3){
        PORTD&=~(1<<PORTD5);
        aqi_status = 0;
    }
}


int main(void) {
    lcd_init();
    UART_init(BAUD_PRESCALER);
    //UART_putstring("UART Initialized\r\n");
    initialize_adc();
    airquality9_t air_quality_sensor;
    airquality9_init(&air_quality_sensor, AIRQUALITY9_DEVICE_ADDRESS);
    if(as7262_init() != 0) {
        // UART_putstring("Device Initialization Failed.\n");
        return 1;
    }
        initiateOneShotMode(); // Initiate measurement in one-shot mode

    // UART_putstring("Device Initialized.\n");    
    DDRD |= (1 << DDD5); // Configure DDD5 as output
    DDRD &= ~(1 << BUTTON_PIN); // Set BUTTON_PIN as input
    PORTD |= (1 << BUTTON_PIN); // Enable pull-up on BUTTON_PIN

    // Air quality sensor initialization
    if (airquality9_set_operating_mode(&air_quality_sensor, AIRQUALITY9_OPMODE_STANDARD) != 0) {
        //UART_putstring("Failed to set operating mode\r\n");
        return -1;
    }

    char buffer[10];
	char adcbuffer[10];
    char databuffer[256];
    LCD_setScreen(BLACK);
    LCD_drawString(15, 2, "AQI:", WHITE, BLACK);
    LCD_drawLine(0, 10, LCD_WIDTH, 10, YELLOW);
    LCD_drawLine(10, 0, 10, LCD_HEIGHT, YELLOW);

    // Initialize and start the timer
    Timer1_Init();

    sei(); // Enable global interrupts


    
    while (1) {

		
		adc = ADC_read();
		sprintf(adcbuffer,"%u\r\n", adc);
		//UART_putstring(adcbuffer);
		//_delay_ms(100);

        // Read the calibrated color values
        int cv = as7262_readCalibratedViolet();
        int cb = as7262_readCalibratedBlue();
        int cg = as7262_readCalibratedGreen();
        int cy = as7262_readCalibratedYellow();
        int co = as7262_readCalibratedOrange();
        int cr = as7262_readCalibratedRed();

        char output[200];
        sprintf(output, "V: %d, B: %d, G: %d, Y: %d, O: %d, R: %d\n", cv, cb, cg, cy, co, cr);
        //UART_putstring(output);

		analyze_light_sample(adc, cv, cb, cg, cy, co, cr);
		

		if(total_samples>=10){

          calculate_exposure_percentage();
		  total_samples=0;
		  natural_light_score=0;
		  artificial_light_score=0;
		  Light_update=1;
		}

        uint8_t currentButtonState = PIND & (1 << BUTTON_PIN);

        if (!currentButtonState && !buttonState) {
            // Button is pressed and was not pressed before
            buttonPressed = (buttonPressed + 1) % 3;
            buttonState = 1; // Mark the button as pressed
        } else if (currentButtonState) {
            buttonState = 0; // Mark the button as not pressed
        }

        if (buttonPressed != prevbuttonstate) {
            switch(buttonPressed) {
                case 0: // AQI Screen
                    LCD_setScreen(BLACK);
                    LCD_drawString(15, 2, "AQI:", WHITE, BLACK);
                    LCD_drawLine(0, 10, LCD_WIDTH, 10, YELLOW);
                    LCD_drawLine(10, 0, 10, LCD_HEIGHT, YELLOW);
                    break;
                case 1: // LIGHT Screen
                    LCD_setScreen(BLACK);
                    LCD_drawString(15, 2, "Nat:", WHITE, BLACK);
					LCD_drawString(80, 2, "Art:", WHITE, BLACK);
                    LCD_drawLine(0, 10, LCD_WIDTH, 10, YELLOW);
                    LCD_drawLine(10, 0, 10, LCD_HEIGHT, YELLOW);
                    break;
                case 2: // UV Screen
                    LCD_setScreen(BLACK);
                    LCD_drawString(15, 2, "UV:", WHITE, BLACK);
                    LCD_drawLine(0, 10, LCD_WIDTH, 10, YELLOW);
                    LCD_drawLine(10, 0, 10, LCD_HEIGHT, YELLOW);
                    break;
            }
            prevbuttonstate = buttonPressed;
        }
		 if (airquality9_check_status(&air_quality_sensor) == AIRQUALITY_STATUS_VALID_NORMAL_OP) {
                if (airquality9_read_aqi_uba(&air_quality_sensor, &aqi_uba) == 0) {
                    snprintf(buffer, sizeof(buffer), "AQI: %d\r\n", aqi_uba);
                    //UART_putstring(buffer);

                    _delay_ms(100); // Delay for data refresh
                } else {
                    //UART_putstring("Error reading AQI UBA\r\n");
                }
            } else {
                //UART_putstring("Sensor not ready\r\n");
            }
        // AQI Screen logic
        if (buttonPressed == 0) {
           
                    // Displaying AQI on LCD
            scrlen += 10;
            char int_str[20];
            sprintf(int_str, "%d", aqi_uba);
            if (scrlen >= 110) {
                LCD_setScreen(BLACK);
                LCD_drawString(15, 2, "AQI:", WHITE, BLACK);
                LCD_drawLine(0, 10, LCD_WIDTH, 10, YELLOW);
                LCD_drawLine(10, 0, 10, LCD_HEIGHT, YELLOW);
                        scrlen = 15;
               }

            LCD_drawLine(10 + (aqi_uba_prev) * 10, scrlen, 10 + (aqi_uba) * 10, scrlen + 10, GREEN);
            LCD_drawString(40, 2, int_str, WHITE, BLACK);
            aqi_uba_prev = aqi_uba;

        }

		else if(buttonPressed==1){
            char int_str_L1[20];
			char int_str_L2[20];
            sprintf(int_str_L1, "%u", (int)natural_light_percentage);
            sprintf(int_str_L2, "%u", (int)artificial_light_percentage);
			if(Light_update){
             LCD_setScreen(BLACK);
             LCD_drawString(15, 2, "Nat:", WHITE, BLACK);
			 LCD_drawString(80, 2, "Art:", WHITE, BLACK);
             LCD_drawLine(0, 10, LCD_WIDTH, 10, YELLOW);
             LCD_drawLine(10, 0, 10, LCD_HEIGHT, YELLOW);

			 LCD_drawBlock(10,40,(int)natural_light_percentage,50,GREEN);
			 LCD_drawBlock(10,80,(int)artificial_light_percentage,90,RED);
			 Light_update=0;
			}
			LCD_drawString(40, 2, int_str_L1, WHITE, BLACK);
			LCD_drawString(110, 2, int_str_L2, WHITE, BLACK);
		}

		else if(buttonPressed==2){
            scrlen_adc += 10;
            char int_str_adc[20];
            sprintf(int_str_adc, "%u", adc);

            if (scrlen_adc >= 110) {
                LCD_setScreen(BLACK);
                LCD_drawString(15, 2, "UV:", WHITE, BLACK);
                LCD_drawLine(0, 10, LCD_WIDTH, 10, YELLOW);
                LCD_drawLine(10, 0, 10, LCD_HEIGHT, YELLOW);
                scrlen_adc = 15;
               }
             LCD_drawLine(20 + (adc_prev) * 20, scrlen_adc, 20 + (adc) * 20, scrlen_adc + 10, GREEN);
             LCD_drawString(40, 2, int_str_adc, WHITE, BLACK);
             adc_prev = adc;

		}
        loopcounter++;
        _delay_ms(10); // General delay for loop iteration

        if(loopcounter>=20){
        _delay_ms(100);
        sprintf(databuffer, "AQI: %d, UV Index: %d, Artificial Light: %d, Natural Light: %d\r\n", (int)aqi_uba, (int)adc, (int)artificial_light, (int)natural_light);
        UART_putstring(databuffer); // Send the string over UART
        _delay_ms(100);
        loopcounter=0;
        }
    }

    return 0;
}
