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
#include "WelcomeService.h"
#include "ButtonService.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable

#define WELCOME_SCROLL_TIME_IN_MS 200 

static uint8_t MyPriority;
static char WelcomeMessage[] = "ASTEROID DESTROYER... ASTEROIDS = 1, ENEMY SHIPS = 2... LEFT SHOWS REMAINING TARGETS, RIGHT IS REMAINING TIME         ";
static char EndgameMessage[] = "FINAL SCORE: 00  ";
static char TimeoutMessage[] = "   GAME TIMED OUT DUE TO INACTIVITY: RESETING GAME  ";
static uint8_t endgame_location;
static uint8_t timeout_location;
static uint8_t current_location;
static WelcomeState_t current_state;

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
bool InitWelcomeService(uint8_t Priority)
{
    
    
  puts("\rStarting Welcome Service \r");
  DB_printf( "compiled at %s on %s\n", __TIME__, __DATE__);
  DB_printf( "\n\r\n");
    
  ES_Event_t ThisEvent;
  ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
  MyPriority = Priority;
  current_state = 0; //putting it in the welcome state
  current_location = 0;
  /********************************************
   in here you write your initialization code
   *******************************************/
  // post the initial transition event
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
bool PostWelcomeService(ES_Event_t ThisEvent)
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
ES_Event_t RunWelcomeService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  /********************************************
   in here you write your service code
   *******************************************/
  switch (current_state)
  {
      case Welcome:
      {
          switch (ThisEvent.EventType)
          {
              //timer goes off so next character needs to be displayed on the screen
              case ES_TIMEOUT:
              {
                  
                  
                  //Because the last letter spot in an array is marked with the \0 
                  //char, we need to increment above to not get that character displayed
                  if (current_location%sizeof(WelcomeMessage) == sizeof(WelcomeMessage)-1)
                  {
                      current_location++;
                  }
                  
                  //Because of the looping nature of the welcome message we need
                  //to use the modulo operator % to circle back
                  uint8_t index = current_location%sizeof(WelcomeMessage);
                  char letter = WelcomeMessage[index];
                  ThisEvent.EventType = ES_NEW_KEY; //Post the next letter to LEDService
                  ThisEvent.EventParam = letter;
                  PostLEDService(ThisEvent); //Post the next letter to LEDService
                  current_location++; //update the current location 
                  
                  //restart the timer
                  ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
              }
              break;
              
              //Timer needs to be stopped --> stop the timer and update the state
              case ES_STOP_WELCOME:
              {
                  puts("\rStopping the Welcome\r");
                  current_state++; //move on to the next state
                  ES_Timer_StopTimer(WELCOME_TIMER);
                  
                  ES_Event_t tempEvent;
                  tempEvent.EventType = ES_LEDS_OFF;
                  PostCoinLEDService(tempEvent);
              }
              break;
              
              case ES_NOINPUT: {
                  current_state = Timeout;
                  ES_Event_t tempEvent;
                  tempEvent.EventType = ES_CLEAR_DISPLAY;
                  PostGameCountDownService(tempEvent);
                  DB_printf("Timing out because of AFK");
                  ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
                  ES_Timer_InitTimer(RETURN_WELCOME_TIMER, 11000);
                  puts("\rStarting the Timeout Message\r");
              } break;
              
              default: {}
              break;
          }
      }
      break;
      
      case Game:
      {
          switch (ThisEvent.EventType)
          {
              //switch the state to the welcome state and then start printing 
              //when the timer expires
             
              
              
              case ES_WELCOME:
              {
                  current_state = Welcome;
                  ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
                  puts("\rStarting the Welcome Message\r");
              }
              break;
              
              case ES_GAMEOVER:
              {
                  current_state = GameOver;
                  uint8_t points = getPoints();
                  ES_Event_t tempEvent;
                  tempEvent.EventType = ES_CLEAR_DISPLAY;
                  PostGameCountDownService(tempEvent);
                  DB_printf("Number of points registered at end: %d\n", points);
                  if (points > 9) {
                      EndgameMessage[sizeof(EndgameMessage) - 5] = '1';
                      points -= 10;
                      EndgameMessage[sizeof(EndgameMessage) - 4] = (char)(points + '0');
                  } else {
                      EndgameMessage[sizeof(EndgameMessage) - 4] = (char)(points + '0');
                  }
                  DB_printf(EndgameMessage);
                  ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
                  ES_Timer_InitTimer(RETURN_WELCOME_TIMER, 10000);
                  puts("\rStarting the gameover Message\r");
                  
                  //reset points for next play of the game
                  tempEvent.EventType = ES_POINTRESET;
                  PostButtonService(tempEvent);
              }
              break;
              
              case ES_NOINPUT: {
                  current_state = Timeout;
                  ES_Event_t tempEvent;
                  tempEvent.EventType = ES_CLEAR_DISPLAY;
                  PostGameCountDownService(tempEvent);
                  tempEvent.EventType = ES_POINTRESET;
                  PostButtonService(tempEvent);
                  DB_printf("Timing out because of AFK");
                  ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
                  ES_Timer_InitTimer(RETURN_WELCOME_TIMER, 11000);
                  puts("\rStarting the Timeout Message\r");
                  EndgameMessage[sizeof(EndgameMessage) - 5] = '0';
                  EndgameMessage[sizeof(EndgameMessage) - 4] = '0';
              } break;
              default: {}
              break;
          }
      } break;
      
      case GameOver:
      {
          switch (ThisEvent.EventType)
          {
              //switch the state to the welcome state and then start printing 
              //when the timer expires
             
              case ES_TIMEOUT:
              {
                  
                  if (ThisEvent.EventParam == 14) {
                      //Because the last letter spot in an array is marked with the \0 
                    //char, we need to increment above to not get that character displayed
                    if (endgame_location%sizeof(EndgameMessage) == sizeof(EndgameMessage)-1)
                    {
                        endgame_location++;
                    }

                    //Because of the looping nature of the welcome message we need
                    //to use the modulo operator % to circle back
                    uint8_t index = endgame_location%sizeof(EndgameMessage);
                    char letter = EndgameMessage[index];
                    ThisEvent.EventType = ES_NEW_KEY; //Post the next letter to LEDService
                    ThisEvent.EventParam = letter;
                    PostLEDService(ThisEvent); //Post the next letter to LEDService
                    endgame_location++; //update the current location 

                    //restart the timer
                    ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
                  }
                  else if (ThisEvent.EventParam == 10) {
                      ES_Event_t tempEvent;
                      tempEvent.EventType = ES_LEDS_OFF;
                      PostCoinLEDService(tempEvent);
                      
                      current_location = 0;
                      endgame_location = 0;
                      current_state = Welcome;
                      ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
                      puts("\rStarting the Welcome Message\r");
                      EndgameMessage[sizeof(EndgameMessage) - 5] = '0';
                      EndgameMessage[sizeof(EndgameMessage) - 4] = '0';
                  }
                  
                  
              }
              break;
 
              
              
              default: {}
              break;
          }
      } break;
      
      case Timeout: {
          switch(ThisEvent.EventType) {
              case ES_TIMEOUT:
              {
                  
                  if (ThisEvent.EventParam == 14) {
                      //Because the last letter spot in an array is marked with the \0 
                    //char, we need to increment above to not get that character displayed
                    if (timeout_location%sizeof(TimeoutMessage) == sizeof(TimeoutMessage)-1)
                    {
                        timeout_location++;
                    }

                    //Because of the looping nature of the welcome message we need
                    //to use the modulo operator % to circle back
                    uint8_t index = timeout_location%sizeof(TimeoutMessage);
                    char letter = TimeoutMessage[index];
                    ThisEvent.EventType = ES_NEW_KEY; //Post the next letter to LEDService
                    ThisEvent.EventParam = letter;
                    PostLEDService(ThisEvent); //Post the next letter to LEDService
                    timeout_location++; //update the current location 

                    //restart the timer
                    ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
                  }
                  else if (ThisEvent.EventParam == 10) {
                      ES_Event_t tempEvent;
                      tempEvent.EventType = ES_LEDS_OFF;
                      PostCoinLEDService(tempEvent);
                      
                      current_location = 0;
                      timeout_location = 0;
                      current_state = Welcome;
                      ES_Timer_InitTimer(WELCOME_TIMER,WELCOME_SCROLL_TIME_IN_MS);
                      puts("\rStarting the Welcome Message\r");
                  }
                  
                  
              }
              break;
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
bool WelcomeorGame(void) {
    if (current_state == Welcome || current_state == GameOver || current_state == Timeout) {
        return false;
    } else {
        return true;
    }
}

WelcomeState_t getState(void) {
    return current_state;
}
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

