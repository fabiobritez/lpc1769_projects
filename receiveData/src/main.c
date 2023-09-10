#include <LPC17xx.h>

#define STATE_IDLE      0      
#define STATE_PRE_SYNC  1
#define STATE_SYNC      2
#define STATE_POST_SYNC 3
#define STATE_ERR_SYNC	4

volatile uint32_t syncTime = 0;
volatile uint32_t bitCount = 0;
volatile uint32_t milliseconds = 0;
volatile uint32_t syncStartTime = 0;
volatile uint32_t receivedData = 0;
volatile uint8_t cycleCount = 0;
volatile uint8_t pinValue = 0;
volatile uint8_t currentState = STATE_IDLE;

void delay_ms(uint32_t ms);
void config_GPIO();
void generatePWM(int data);
void reset();

int main() {
    SysTick_Config(SystemCoreClock / 1000);

    while (1) {
    	syncTime = milliseconds - syncStartTime;

    	switch (currentState) {
            case STATE_IDLE:
                if (syncStartTime != 0) currentState = STATE_PRE_SYNC;
                break;

            case STATE_PRE_SYNC:
                if (syncTime >= 30) {
                	currentState = STATE_SYNC;
                	cycleCount = -2; // read in the 3rd millisecond
                	}
                break;

            case STATE_SYNC:
            	if(syncTime <= 80){
            		if (cycleCount%5==0 && bitCount <= 10) { 
            			receivedData = (receivedData << 1) | pinValue;
            			bitCount++;
                	}
            	}else {
                    bitCount = 0;
            		currentState = STATE_POST_SYNC;
            	}
            	break;
            case STATE_POST_SYNC:
                if (syncTime >= 110) {
                    generatePWM(receivedData);
                    reset();
                }
                break;
            case STATE_ERR_SYNC:
                reset();
            	break;
        }
    }
}
 
/*Every 1ms*/
void SysTick_Handler(void) {
    milliseconds++;
    pinValue = (LPC_GPIO0->FIOPIN0 >> 0) & 1;  //get value from P0.0

    switch (currentState) {
        case STATE_IDLE:
            if (pinValue == 0) syncStartTime = milliseconds;
            break;
        case STATE_PRE_SYNC:
            if (pinValue == 1) currentState = STATE_ERR_SYNC;
            break;
        case STATE_SYNC:
        	cycleCount++;	//
            break;
        case STATE_POST_SYNC:
        	if(pinValue == 1) currentState = STATE_ERR_SYNC;
        default:
            break;
    }
}

void config_GPIO(){
	LPC_PINCON -> PINSEL0 &= ~(0b11<<0);   // GPIO P0.0
	LPC_PINCON -> PINSEL0 &= ~(3<<2);      // GPIO P0.2

	LPC_GPIO0 -> FIODIR0 &= ~(0b11<<0);    // input P0.0
	LPC_GPIO0 -> FIODIR0 |= (3<<2);        // output P0.2

	LPC_PINCON -> PINMODE0 |= (0b11<<0);   // pull_up P0.0
}

void generatePWM(int data) {
    if (data > 1023) {
        data = 1023;
    }

    uint32_t pulseWidth = data * 50 / 1023;

    uint32_t offTime = 50 - pulseWidth;

    LPC_GPIO0->FIOSET |= (1 << 2);
    delay_ms(pulseWidth); 

    LPC_GPIO0->FIOCLR |= (1 << 2);
    delay_ms(offTime);
}

void delay_ms(uint32_t ms) {
    uint32_t startTime = milliseconds;
    while ((milliseconds - startTime) < ms) {}
}

void reset() {
    syncStartTime = 0;
    receivedData = 0;
    currentState = STATE_IDLE;
    syncTime = 0;
}
