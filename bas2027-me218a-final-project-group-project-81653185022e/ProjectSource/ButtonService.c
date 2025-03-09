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
#include "ES_DeferRecall.h"
#include "ES_Port.h"
#include "terminal.h"
#include "dbprintf.h"
#include "ButtonService.h"
#include "BuzzerService.h"
#include "WelcomeService.h"
#include "PIC32_AD_Lib.h"
#include "PotService.h"
#include "DM_Display.h"
#include "PIC32_SPI_HAL.h"

#include <xc.h>

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
#define UP 0
#define DOWN 1

#define ONE_SEC 1000
#define HALF_SEC (ONE_SEC / 2)
#define TWO_SEC (ONE_SEC * 2)
#define THREE_SEC (ONE_SEC * 3)
#define FIVE_SEC (ONE_SEC * 5)
#define TEN_SEC (ONE_SEC * 10)
#define TWENTY_SEC (ONE_SEC * 20)


// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
static bool timer_on = 0;
static uint8_t points = 0;
static uint32_t leftmostLEDs = 0b11111111000000000000000000000000;

static uint16_t tolerance = 100;
static uint16_t asteroid1Loc = 1600;
static uint16_t asteroid2Loc = 1950;
static uint16_t asteroid3Loc = 2380;
static uint16_t asteroid4Loc = 2700;
static uint16_t asteroid5Loc = 3150;
static uint16_t asteroid6Loc = 3600;
static uint16_t asteroid7Loc = 4350;
static uint16_t asteroid8Loc = 4950;



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
bool InitButtonService(uint8_t Priority)
{
    
    
  puts("\rStarting Button Service \r");
  DB_printf( "compiled at %s on %s\n", __TIME__, __DATE__);
  DB_printf( "\n\r\n");
  
  ANSELBbits.ANSB3 = 0;
  TRISBbits.TRISB3 = 1; //1 is an input
  
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  
  
   //setting RB12 to be a digital output to power the buzzer
  
 
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
bool PostButtonService(ES_Event_t ThisEvent)
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
ES_Event_t RunButtonService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
  {
    
    case ES_DBBUTTONDOWN:
    {
        DB_printf("ES BUTTON PRESS\n");
        ES_Event_t tempEvent;
        tempEvent.EventType = ES_BUZZER;
        PostBuzzerService(tempEvent);
        ES_Timer_InitTimer(RELOAD_TIMER, 3750);
        timer_on = 1;
        ES_Timer_SetTimer(AFK_TIMER, TWENTY_SEC);
        
        
        //add code to determine if asteroid has been hit
        uint16_t servoIndex = ThisEvent.EventParam;
        
        bool gameMode = WelcomeorGame();
        
        if (((asteroid1Loc - tolerance) < servoIndex) && (servoIndex < (asteroid1Loc + tolerance)) && gameMode == true) {
            points++;
            asteroid1Loc = 0;
            leftmostLEDs &= 0b11111110000000000000000000000000;
        }
        
        if (((asteroid2Loc - tolerance) < servoIndex) && (servoIndex < (asteroid2Loc + tolerance)) && gameMode == true) {
            points++;
            asteroid2Loc = 0;
            leftmostLEDs &= 0b11111101000000000000000000000000;
        }
        
        if (((asteroid3Loc - tolerance) < servoIndex) && (servoIndex < (asteroid3Loc + tolerance)) && gameMode == true) {
            points++;
            points++;
            asteroid3Loc = 0;
            leftmostLEDs &= 0b11111011000000000000000000000000;
        }
        if (((asteroid4Loc - tolerance) < servoIndex) && (servoIndex < (asteroid4Loc + tolerance)) && gameMode == true) {
            points++;
            asteroid4Loc = 0;
            leftmostLEDs &= 0b11110111000000000000000000000000;
        }
        if (((asteroid5Loc - tolerance) < servoIndex) && (servoIndex < (asteroid5Loc + tolerance)) && gameMode == true) {
            points++;
            points++;
            asteroid5Loc = 0;
            leftmostLEDs &= 0b11101111000000000000000000000000;
        }
        if (((asteroid6Loc - tolerance) < servoIndex) && (servoIndex < (asteroid6Loc + tolerance)) && gameMode == true) {
            points++;
            points++;
            asteroid6Loc = 0;
            leftmostLEDs &= 0b11011111000000000000000000000000;
        }
        if (((asteroid7Loc - tolerance) < servoIndex) && (servoIndex < (asteroid7Loc + tolerance)) && gameMode == true) {
            points++;
            points++;
            asteroid7Loc = 0;
            leftmostLEDs &= 0b10111111000000000000000000000000;
        }
        if (((asteroid8Loc - tolerance) < servoIndex) && (servoIndex < (asteroid8Loc + tolerance)) && gameMode == true) {
            points++;
            asteroid8Loc = 0;
            leftmostLEDs &= 0b01111111000000000000000000000000;
        }
//        if (((asteroid9Loc - tolerance) < servoIndex) && (servoIndex < (asteroid9Loc + tolerance)) && gameMode == true) {
//            points++;
//            asteroid9Loc = 0;
//            leftmostLEDs &= 0b10111111110000000000000000000000;
//        }
//        if (((asteroid10Loc - tolerance) < servoIndex) && (servoIndex < (asteroid10Loc + tolerance)) && gameMode == true) {
//            points++;
//            asteroid10Loc = 0;
//            leftmostLEDs &= 0b01111111110000000000000000000000;
//        }
        
        DB_printf("Your Current Points: %d \n", points);
        
        
     } break;
    
    case ES_POINTRESET: {
        points = 0;
        asteroid1Loc = 1600;
        asteroid2Loc = 1950;
        asteroid3Loc = 2380;
        asteroid4Loc = 2700;
        asteroid5Loc = 3150;
        asteroid6Loc = 3600;
        asteroid7Loc = 4350;
        asteroid8Loc = 4950;
        leftmostLEDs = 0b11111111000000000000000000000000;
    } break;
    

    case ES_TIMEOUT:   // re-start timer & announce
    {
      DB_printf("Button timer timed out! \n");
      ES_Event_t tempEvent;
      tempEvent.EventType = ES_STOP_BUZZER;
      PostBuzzerService(tempEvent);
      timer_on = 0;
    } break;
    default: {}
    break;
  }
  
  
  return ReturnEvent;
}


bool CheckButton(void)
{
  static uint8_t  LastPinState = 0; //defaulting to not reading it
  //ES_Event_t ThisEvent;
  uint8_t         CurrentPinState = PORTBbits.RB3;
  bool            ReturnVal = false;
  bool gameState;
  gameState = WelcomeorGame();

  // check for pin high AND different from last time
  // do the check for difference first so that you don't bother with a test
  // of a port/variable that is not going to matter, since it hasn't changed
  if ((CurrentPinState != LastPinState) &&
      (CurrentPinState == DOWN) && timer_on == 0 && gameState == 1) // event detected, so post detected event
  {
    ES_Event_t ThisEvent;
    ThisEvent.EventType   = ES_DBBUTTONDOWN;
    
    uint32_t CurrentAD_Value[1];
    ADC_MultiRead(CurrentAD_Value);
    uint16_t mappedVal = mapPotToServo((uint16_t)CurrentAD_Value[0], 0, 1023, 1250, 5000);
    ThisEvent.EventParam = mappedVal;
    //ThisEvent.EventParam = ES_Timer_GetTime();
    // this could be any of the service post functions, ES_PostListx or
    // ES_PostAll functions
    PostButtonService(ThisEvent);
    ReturnVal = true;
  }
  LastPinState = CurrentPinState; // update the state for next time

  return ReturnVal;
    
}

uint8_t getPoints(void) {
    return points;
}

uint32_t getLeftmostLEDs(void) {
    return leftmostLEDs;
}




/***************************************************************************
 private functions
 ***************************************************************************/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

