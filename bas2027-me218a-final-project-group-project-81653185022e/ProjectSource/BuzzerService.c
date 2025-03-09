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
#include "BuzzerService.h"
#include "WelcomeService.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
#define ON 1
#define OFF 0


// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

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
bool InitBuzzerService(uint8_t Priority)
{
    
    
  puts("\rStarting BuzzerService \r");
  DB_printf( "compiled at %s on %s\n", __TIME__, __DATE__);
  DB_printf( "\n\r\n");
    
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  
  
   //setting RB12 to be a digital output to power the buzzer
  ANSELBbits.ANSB12 = 0;
  TRISBbits.TRISB12 = 0; 
  
  ODCBbits.ODCB5 = 1; //configure pinB10 to open drain configuration 
  TRISBbits.TRISB5 = 0; 
  LATBbits.LATB5 = 0;
 
  //initializing PWM hardware @20ms rising edge period (i.e. 50,000 ticks)
  
  
  
  
  
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
bool PostBuzzerService(ES_Event_t ThisEvent)
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
ES_Event_t RunBuzzerService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
  {
    case ES_BUZZER:
    {
        //code for Turret LED ON
        LATBbits.LATB5 = ON;
        
        LATBbits.LATB12 = ON;
        puts("ES BUZZER\r");
    }
    break;
    case ES_STOP_BUZZER:
    {
        //code for Turret LED OFF
        LATBbits.LATB5 = OFF;
        LATBbits.LATB12 = OFF;
        puts("ES STOP BUZZER\r");
     }
    
    break;
    
    case ES_TIMEOUT: {
        DB_printf("AFK Clock has timed out! Reseting game ... \n");
        ES_Event_t tempEvent;
        bool gameState = WelcomeorGame();
        if (gameState == 0) { //game is in welcome state still
            tempEvent.EventType = ES_LEDS_OFF;
            PostCoinLEDService(tempEvent);
            
            tempEvent.EventType = ES_NOINPUT;
            PostWelcomeService(tempEvent);
        } else if (gameState == 1) { //game is in game state
            
            ES_Timer_StopTimer(FULL_GAME_TIMER);
            tempEvent.EventType = ES_CLEAR_DISPLAY;
            PostGameCountDownService(tempEvent);
            
            tempEvent.EventType = ES_LEDS_OFF;
            PostCoinLEDService(tempEvent);
            
            tempEvent.EventType = ES_POINTRESET;
            PostButtonService(tempEvent);
            
            tempEvent.EventType = ES_CLEAR_DISPLAY;
            PostGameCountDownService(tempEvent);
            
            
            
            tempEvent.EventType = ES_COUNTDOWNRESET;
            PostGameCountDownService(tempEvent);
            
            tempEvent.EventType = ES_CLEAR_DISPLAY;
            PostGameCountDownService(tempEvent);
            
            
            tempEvent.EventType = ES_NOINPUT;
            PostWelcomeService(tempEvent);
        }
        
    }
    default: {}
    break;
  }
  
  
  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

