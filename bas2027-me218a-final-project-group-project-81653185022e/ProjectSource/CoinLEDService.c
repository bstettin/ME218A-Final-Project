/****************************************************************************
 Module
   TemplateService.c

 Revision
   1.0.1

 Description
   This is a template file for implementing a simple service under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "CoinLEDService.h"
#include "dbprintf.h"
#include "WelcomeService.h"


/*----------------------------- Module Defines ----------------------------*/

#define COIN 0
#define NO_COIN 1

#define ONE_SEC 1000
#define HALF_SEC (ONE_SEC / 2)
#define TWO_SEC (ONE_SEC * 2)
#define THREE_SEC (ONE_SEC * 3)
#define FIVE_SEC (ONE_SEC * 5)
#define TEN_SEC (ONE_SEC * 10)
#define TWENTY_SEC (ONE_SEC * 20)


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
static uint8_t coin_counter;
/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateService

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 01/16/12, 10:00
****************************************************************************/
bool InitCoinLEDService(uint8_t Priority)
{
    
    
  puts("\rStarting Coin LED Service \r");
  DB_printf( "compiled at %s on %s\n", __TIME__, __DATE__);
  DB_printf( "\n\r\n");
  ES_Event_t ThisEvent;
  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  
  
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  
  //Configuring pin for first coin LED
  ODCBbits.ODCB10 = 1; //configure pinB10 to open drain configuration 
  TRISBbits.TRISB10 = 0; 
  LATBbits.LATB10 = 0;
  
  //Configuring pin for second LED
  ODCBbits.ODCB9 = 1; //configure pinB10 to open drain configuration 
  TRISBbits.TRISB9 = 0; 
  LATBbits.LATB9 = 0;
  //Configuring pin on the coin counter
  TRISBbits.TRISB4 = 1;
  
  coin_counter = 0; //initializing the coin counter
  
  
  
  if (ES_PostToService(MyPriority, ThisEvent) == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/****************************************************************************
 Function
     PostTemplateService

 Parameters
     EF_Event_t ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostCoinLEDService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event_t RunCoinLEDService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
  {
    case ES_LED1_ON:
    {
        LATBbits.LATB10 = 1;
        puts("LED1 ON\r");
        ES_Timer_InitTimer(AFK_TIMER, TWENTY_SEC);
    }
    break;
    
    case ES_LED2_ON:
    {
        LATBbits.LATB9 = 1;
        puts("LED2 ON\r");
        DB_printf("Taking you to countdown mode\n");
        ES_Timer_SetTimer(AFK_TIMER, TWENTY_SEC);
        
        //Stopping the welcome display
        ThisEvent.EventType = ES_STOP_WELCOME;
        PostWelcomeService(ThisEvent);

        //Clearing the display
        ThisEvent.EventType = ES_CLEAR_DISPLAY;
        PostGameCountDownService(ThisEvent);

        //displaying the initial time
        ThisEvent.EventType = ES_SHOW_INITIAL_TIME;
        PostGameCountDownService(ThisEvent);
    }
    break;
    
    
    //may need to implement debouncing for the coin detection 
    //alternative could be to add a state with the number of LEDS on
    //0 LEDS ON --- 1 LED ON -- 2 LED ON --> beging game 
    case ES_NEW_COIN:
    {
        if (coin_counter == 0)
        {
            ThisEvent.EventType=ES_LED1_ON;
            coin_counter++;
            PostCoinLEDService(ThisEvent);
        }
        
        else if (coin_counter == 1)
        {
            ThisEvent.EventType=ES_LED2_ON;
            PostCoinLEDService(ThisEvent);
        }
        //DB_printf("ES_NEW_COIN DETECTED -- counter=%d\r", (char)coin_counter);
        
    }
    break;
    
    case ES_LEDS_OFF:
    {
        LATBbits.LATB10 = 0; //turn LED1 off
        LATBbits.LATB9 = 0; //turn LED2 OFF
        puts("ES LEDS OFF\r");
        coin_counter = 0;
    }   
    break;
    
    default: {}
    break;
  }
  
  
  return ReturnEvent;
}



bool Check4Coin(void)
{
  static uint8_t  LastPinState = 0; //The IR normally reads 0
  uint8_t         CurrentPinState; //current state of the pin
  bool            ReturnVal = false;

  CurrentPinState = PORTBbits.RB4; //read pin RB4 from the PIC32
  bool gameMode = WelcomeorGame();
  WelcomeState_t gameState = getState();
  
  // check for pin high AND different from last time
  // do the check for difference first so that you don't bother with a test
  // of a port/variable that is not going to matter, since it hasn't changed
  if ((CurrentPinState != LastPinState) &&
      (CurrentPinState == COIN) && (gameState == Welcome)) // event detected, so post detected event
  {
    ES_Event_t ThisEvent;
    ThisEvent.EventType   = ES_NEW_COIN;
    ThisEvent.EventParam  = 1;
    // this could be any of the service post functions, ES_PostListx or
    // ES_PostAll functions
    ES_PostAll(ThisEvent);
    ReturnVal = true;
  }
  LastPinState = CurrentPinState; // update the state for next time

  return ReturnVal;
}




/***************************************************************************
 private functions
 ***************************************************************************/




/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

