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
#include "ServoService.h"
#include "PWM_PIC32.h"
#include "WelcomeService.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
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
bool InitServoService(uint8_t Priority)
{
    
    
  puts("\rStarting ServoService \r");
  DB_printf( "compiled at %s on %s\n", __TIME__, __DATE__);
  DB_printf( "\n\r\n");
  DB_printf( "Press any key to post key-stroke events to Service 0\n\r");
  DB_printf( "Press 'd' to test event deferral \n\r");
  DB_printf( "Press 'r' to test event recall \n\r");
  DB_printf( "Press 'p' to test posting from an interrupt \n\r");
    
    
  ES_Event_t ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  
  
   //setting RB15 to be a digital output
  
  ANSELBbits.ANSB15 = 0;
  TRISBbits.TRISB15 = 0; 
 
  //initializing PWM hardware @20ms rising edge period (i.e. 50,000 ticks)
  
  PWMSetup_BasicConfig(1);
  PWMSetup_SetPeriodOnTimer(50000, _Timer2_);
  PWMSetup_AssignChannelToTimer(1,_Timer2_);
  PWMSetup_MapChannelToOutputPin(1,PWM_RPB15);
  
  
  
  
  
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
bool PostServoService(ES_Event_t ThisEvent)
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
ES_Event_t RunServoService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
  {
    case ES_FORWARD:
    {
        PWMOperate_SetPulseWidthOnChannel(1250,1);
        puts("ES FORWARD\r");
    }
    break;
    case ES_REVERSE:
    {
        PWMOperate_SetPulseWidthOnChannel(5000,1);
        puts("ES REVERSE\r");
     }
    
    break;
    
      case ES_MOVE:
      {
          bool gameMode = WelcomeorGame();
          if (gameMode == true){
              PWMOperate_SetPulseWidthOnChannel(ThisEvent.EventParam,1);
          }
          
      }
      break;
    
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

