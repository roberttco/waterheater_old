 
#define VALVE 4
#define PUMP 3
#define GREEN_LED 13
#define FLOW 2  // must be 2 because this is one of the two pins that support interrupts
#define IN1 A5

#define SECONDSTORESTART 5

#define PUMP_OFF LOW
#define PUMP_ON	HIGH
#define VALVE_CLOSED HIGH
#define VALVE_OPEN LOW

#define STATE_IDLE 0
#define STATE_HEATSTART 1
#define STATE_PUMPSTART 2
#define STATE_HEATEND 3
#define STATE_DEMANDSTART 4
#define STATE_DEMANDEND 5

bool healthLED;
int input;
int state;
bool heat;
volatile int NbTopsFan; //measuring the rising edges of the signal
int flowrate;  
int demandrestart;

void rpm ()     //This is the function that the interupt calls 
{ 
    NbTopsFan++;  //This function measures the rising and falling edge of the hall effect sensors signal
} 

void setup()
{
    // put your setup code here, to run once:
    pinMode(GREEN_LED,OUTPUT);
    pinMode(VALVE,OUTPUT);
    pinMode(PUMP,OUTPUT);
    pinMode(FLOW,INPUT_PULLUP); //initializes digital pin 2 as an input
  
    pinMode(IN1,INPUT);
  
    digitalWrite(VALVE,VALVE_CLOSED);     // relay - active low
    digitalWrite(PUMP,PUMP_OFF);          // SSR - active high
  
    healthLED = false;
    state = STATE_IDLE;
    heat = false;
    flowrate = 0;
  
    attachInterrupt(0, rpm, RISING); //and the interrupt is attached

    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    } 

    Serial.println("setup complete...");
}

String debugstr;

void loop() 
{
    debugstr = "";
  
    // put your main code here, to run repeatedly:
  
    NbTopsFan = 0;      //Set NbTops to 0 ready for calculations

    sei();            //Enables interrupts
  
    delay(1000);
    
    cli();            //Disable interrupts

    flowrate = (NbTopsFan / 5.5); //(Pulse frequency) / 5.5Q, = flow rate in L/min
    
    input = analogRead(IN1);
    if (input > 10)
    {
        heat = true;
    }
    else
    {
        heat = false;
    }

    switch (state)
    {
        case STATE_IDLE:
            digitalWrite(VALVE,VALVE_CLOSED);
            digitalWrite(PUMP,PUMP_OFF);

            if (flowrate > 0)
            {
                state = STATE_DEMANDSTART;
            }
            else if (heat == true)
            {
                state = STATE_HEATSTART;
            }
            break;
        case STATE_HEATSTART:  // open VALVE valve
            if (flowrate > 0)
            {
                state = STATE_DEMANDSTART;
            }
            else if (heat == true)
            {
                digitalWrite(VALVE,VALVE_OPEN);  // open valve then start the pump
                state = STATE_PUMPSTART;
            }
            else
            {
                state = STATE_IDLE;    // pump is not started yet so just go back to state 0
            }
            break;
        case STATE_PUMPSTART:  // start pump
            if (flowrate > 0)
            {
                state = STATE_DEMANDSTART;
            }
            else if (heat == true)
            {
                digitalWrite(PUMP,PUMP_ON);    // start pump and stay here until there is no more heat demand
            }
            else
            {
                state = STATE_HEATEND;  // pump might have been started so go to state 3 and turn off pump before closing valve to prevent pipe bang.          
            }
            break;
        case STATE_HEATEND:  // stop pump - then go to idle state (where the valve will be closed)
            digitalWrite(PUMP,PUMP_OFF);
            state = STATE_IDLE;
            break;
        case STATE_DEMANDSTART:     // if demand starts, stop the pump and reset the timeout, then wait unti the timeout elapses
            digitalWrite(PUMP,PUMP_OFF);
            demandrestart = SECONDSTORESTART;
            state = STATE_DEMANDEND;
            break;
        case STATE_DEMANDEND:  // wait until there is no more demand.  It eh timeout has elapsed, go to heating again or idle
            digitalWrite(VALVE,VALVE_CLOSED);
            
            demandrestart = demandrestart - 1;  // decrement the timeout
            
            if (flowrate > 0)
            {
                state = STATE_DEMANDSTART;  // the water is still flowing so go back to demand start top reset things.
            }
            else
            {
                if (demandrestart < 1)
                {
                    if (heat == true)
                    {
                        state = STATE_HEATSTART;
                    }
                    else
                    {
                        state = STATE_IDLE;
                    }
                }
            }
            break;
        default:
            state = STATE_IDLE;
            break;
    }
 
    if (healthLED == false)
    {
        digitalWrite(GREEN_LED,HIGH);
        healthLED = true;
    }
    else
    {
        digitalWrite(GREEN_LED,LOW);
        healthLED = false;
    }

}

