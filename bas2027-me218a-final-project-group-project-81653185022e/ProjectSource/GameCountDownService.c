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
#include "GameCountDownService.h"
#include "DM_Display.h"
#include "PIC32_SPI_HAL.h"
#include "WelcomeService.h"
#include "ButtonService.h"
/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable

#define _1SECOND_ 1000 
#define MAX_ROWS 8
static uint8_t MyPriority;
static char WelcomeMessage[] = "ASTROID DESTROYER... SHOOT THE ASTROIDS TO SCORE POINTS AND MAKE IT OUT ALIVE         ";
static uint8_t gameTime;
static uint32_t LEDS_On;
static uint8_t row;
static uint32_t fullRow = 0xFF;
static uint8_t current_row;
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
bool InitGameCountDownService(uint8_t Priority)
{
    
    
  puts("\rStarting Welcome Service \r");
  DB_printf( "compiled at %s on %s\n", __TIME__, __DATE__);
  DB_printf( "\n\r\n");
    
  ES_Event_t ThisEvent;
  //ES_Timer_InitTimer(FULL_GAME_TIMER,_1SECOND_);
  MyPriority = Priority;
  
  
  gameTime=61; //full game is 60 seconds
  row = 0; //8 rows in the LED matrix with zero indexing
  //LEDS_On = 10000000000000000000000011111111;
  LEDS_On = 0xFF; // 00000000 00000000 00000000 11111111 All LEDS in the bottom row on
  current_row = 0;
  
  
  //64 LEDS in a row --> subtract 4 since we have 60 seconds
  LEDS_On = LEDS_On >> 4; //bitshifting to the right by 4 to get the right layout

  //timer not initialized in here because we don't want the game to start 
  //when this service is initialized
  
  ThisEvent.EventType = ES_INIT;

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
bool PostGameCountDownService(ES_Event_t ThisEvent)
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
ES_Event_t RunGameCountDownService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (ThisEvent.EventType)
  {
      case ES_CLEAR_DISPLAY:
      {
        DM_ClearDisplayBuffer(); //sets all the buffers
        ThisEvent.EventType = ES_UPDATE_ROW; //update the cleared buffer
        PostLEDService(ThisEvent);
        current_row = row;
      }
      break;
      
      case ES_SHOW_INITIAL_TIME: //first event is special
      {
          if (current_row == 0)
          { 
              //DM_ScrollDisplayBuffer(1);
              ThisEvent.EventType = ES_RESET_COUNTER;
              PostLEDService(ThisEvent);
              ThisEvent.EventType = ES_SHOW_INITIAL_TIME;
              DB_printf("Loading Top Row into buffer\n");
              DM_PutDataIntoBufferRow(LEDS_On, current_row);
              current_row++;
              PostGameCountDownService(ThisEvent);
          }
          else if (current_row < MAX_ROWS)
          {
              DB_printf("Loading Row into buffer\n");
              DM_PutDataIntoBufferRow(fullRow, current_row);
              PostGameCountDownService(ThisEvent);
              current_row++;   
          }
          else
          {
              ThisEvent.EventType = ES_UPDATE_ROW;
              PostLEDService(ThisEvent);
              ES_Timer_InitTimer(FULL_GAME_TIMER,_1SECOND_);
          }
      }
      break;
      
      case ES_UPDATE_TIMER_ROW:
      {
          
          if (row < MAX_ROWS) //making sure there are still rows to update
          {
              if (current_row < MAX_ROWS)
              {
                  if (current_row == row)
                  {
                      uint32_t leftmostLEDs = getLeftmostLEDs();
                      if (row == 0) {
                          LEDS_On |= leftmostLEDs;
                          DM_PutDataIntoBufferRow(LEDS_On, row);
                          current_row++;
                          LEDS_On &= 0x003FFFFF;
                      } else {
                          DM_PutDataIntoBufferRow(LEDS_On, row);
                          current_row++;
                          DM_PutDataIntoBufferRow(leftmostLEDs, 0);
                      }
//                      ThisEvent.EventType = ES_UPDATE_TIMER_ROW;
//                      PostGameCountDownService(ThisEvent);
                  }
                  
                  else
                  {
                      DM_PutDataIntoBufferRow(fullRow, fullRow);
                      current_row++;
                  }
                  
                  ThisEvent.EventType = ES_UPDATE_TIMER_ROW;
                  PostGameCountDownService(ThisEvent);
              }

              else
              {
                ThisEvent.EventType = ES_GAME_COUNTDOWN; //Moves to LED scrolling which takes the values from the buffer and displays them
                PostLEDService(ThisEvent);
                current_row = row;
              }
          }
      }
      break;
      
      
      
      case ES_TIMEOUT:
      {
          DB_printf("INSIDE GAME TIMEOUT ES_TIMEOUT received from Timer %d in Service %d\r\n",
          ThisEvent.EventParam);
          ES_Timer_InitTimer(FULL_GAME_TIMER,_1SECOND_);
          gameTime--;
          
//          uint32_t leftmostLEDs = getLeftmostLEDs();
          
//          LEDS_On &= 0x003FFFFF;
          
          if (LEDS_On == 00000000000000000000000000000000)
          {
              LEDS_On = fullRow >> 1; 
              row++;
              current_row=row;
          }
          else
          {
              LEDS_On = LEDS_On >> 1;
          }
          
//          LEDS_On |= leftmostLEDs;
          
          ThisEvent.EventType = ES_UPDATE_TIMER_ROW;
          PostGameCountDownService(ThisEvent);
          
          if (gameTime == 0 || getPoints() == 12) { //send back to welcome and reset game
              LEDS_On = 0xFF; // 00000000 00000000 00000000 11111111 All LEDS in the bottom row on
              //64 LEDS in a row --> subtract 4 since we have 60 seconds
              LEDS_On = LEDS_On >> 4;

              current_row = 0;
              row = 0;
              gameTime = 61;
              ES_Event_t tempEvent;
              
              ES_Timer_StopTimer(FULL_GAME_TIMER);
              ES_Timer_StopTimer(AFK_TIMER);
              
              tempEvent.EventType = ES_CLEAR_DISPLAY;
              PostGameCountDownService(tempEvent);

              tempEvent.EventType = ES_GAMEOVER;
              PostWelcomeService(tempEvent);
             
          }
          
      }
      break;
      
      case ES_COUNTDOWNRESET: {
            LEDS_On = 0xFF; // 00000000 00000000 00000000 11111111 All LEDS in the bottom row on
            //64 LEDS in a row --> subtract 4 since we have 60 seconds
            LEDS_On = LEDS_On >> 4;

            current_row = 0;
            row = 0;
            gameTime = 61;
      } break;
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

