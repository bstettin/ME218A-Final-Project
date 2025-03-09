/****************************************************************************
 Module
   TestHarnessService0.c

 Revision
   1.0.1

 Description
   This is the first service for the Test Harness under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/26/17 18:26 jec     moves definition of ALL_BITS to ES_Port.h
 10/19/17 21:28 jec     meaningless change to test updating
 10/19/17 18:42 jec     removed referennces to driverlib and programmed the
                        ports directly
 08/21/17 21:44 jec     modified LED blink routine to only modify bit 3 so that
                        I can test the new new framework debugging lines on PF1-2
 08/16/17 14:13 jec      corrected ONE_SEC constant to match Tiva tick rate
 11/02/13 17:21 jec      added exercise of the event deferral/recall module
 08/05/13 20:33 jec      converted to test harness service
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/


// debugging printf()

// Hardware
#include <xc.h>
//#include <proc/p32mx170f256b.h>

// Event & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_Port.h"
#include "terminal.h"
#include "dbprintf.h"
#include "LEDService.h"
#include "DM_Display.h"
#include "PIC32_SPI_HAL.h"


/*----------------------------- Module Defines ----------------------------*/
// these times assume a 10.000mS/tick timing
#define ONE_SEC 1000
#define HALF_SEC (ONE_SEC / 2)
#define TWO_SEC (ONE_SEC * 2)
#define FIVE_SEC (ONE_SEC * 5)

#define ENTER_POST     ((MyPriority<<3)|0)
#define ENTER_RUN      ((MyPriority<<3)|1)
#define ENTER_TIMEOUT  ((MyPriority<<3)|2)

//#define TEST_INT_POST
//#define BLINK LED
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
#ifdef BLINK_LED
static void InitLED(void);
static void BlinkLED(void);
#endif

#ifdef TEST_INT_POST
static void InitTMR2(void);
static void StartTMR2(void);
#endif
/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
// add a deferral queue for up to 3 pending deferrals +1 to allow for overhead
static ES_Event_t DeferralQueue[3 + 1];
static state_t current_state;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTestHarnessService0

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
bool InitLEDService(uint8_t Priority)
{
  ES_Event_t ThisEvent;
  MyPriority = Priority;

  // When doing testing, it is useful to announce just which program
  // is running.
  //clrScrn();
  DB_printf("\rInitializing LED service\r");
  

  /********************************************
   in here you write your initialization code
   *******************************************/
  //SPI Initialization 
  uint32_t baud = 10000; 
  SPISetup_BasicConfig(SPI_SPI1);
  SPISetup_SetLeader(SPI_SPI1, SPI_SMP_MID); 
  SPISetup_SetBitTime(SPI_SPI1, baud);
  SPISetup_MapSSOutput(SPI_SPI1, SPI_RPA0);
  SPISetup_MapSDOutput(SPI_SPI1, SPI_RPA1); 
  SPISetup_SetClockIdleState(SPI_SPI1, SPI_CLK_HI); 
  SPISetup_SetActiveEdge(SPI_SPI1, SPI_SECOND_EDGE);
  SPISetup_SetXferWidth(SPI_SPI1, SPI_16BIT);
  SPISetEnhancedBuffer(SPI_SPI1, true);
  SPISetup_EnableSPI(SPI_SPI1);

  IFS0CLR = _IFS0_INT4IF_MASK;

  //Initializing LED Display
  while (DM_TakeInitDisplayStep() == false){}
  current_state = waiting;
  
  ES_InitDeferralQueueWith(DeferralQueue, ARRAY_SIZE(DeferralQueue));
  
  // initialize LED drive for testing/debug output
#ifdef BLINK_LED
  InitLED();
#endif
#ifdef TEST_INT_POST
  InitTMR2();
#endif
  // initialize the Short timer system for channel A
  //ES_ShortTimerInit(MyPriority, SHORT_TIMER_UNUSED);
  

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
     PostTestHarnessService0

 Parameters
     ES_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostLEDService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTestHarnessService0

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes

 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event_t RunLEDService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ES_Event_t new_event;
  static int counter = 0;
  
  
  //static uint8_t row = 0;
  
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

#ifdef _INCLUDE_BYTE_DEBUG_
  _HW_ByteDebug_SetValueWithStrobe( ENTER_RUN );
#endif  
  
  switch (current_state)
  {
      //Only start moving a key while in the buffer
      case waiting:
      {
        
                    
        switch (ThisEvent.EventType)
        {
            case ES_NEW_KEY:   // announce
            {
              DB_printf("NEW CHARACTER RECEIVED -> %c <- LED Service\r\n",
                  (char)ThisEvent.EventParam); //what key was pressed
              
              
              //EVENT CHANGES TO UPDATE ROW
              if (counter != 0)
              {
                DM_ScrollDisplayBuffer(4); //scroll all characters by 4 columns
              }
              
              new_event.EventParam = ThisEvent.EventParam;
              new_event.EventType = ES_UPDATE_ROW;
              DM_AddChar2DisplayBuffer(ThisEvent.EventParam);
              PostLEDService(new_event);
              current_state = scrolling;
              counter++;
              
              
            }
            break;
            case ES_GAME_COUNTDOWN:
            {
                
                ThisEvent.EventType = ES_CLEAR_DISPLAY;
                PostGameCountDownService(ThisEvent);

                new_event.EventType = ES_UPDATE_ROW;
                current_state = scrolling;
                PostLEDService(ThisEvent); //posting this event to update the rows
                
            }
            break;
            
            case ES_RESET_COUNTER:
            {
                counter=0;
                current_state = scrolling;
            }
            break;
            
            
            break;
            default:
            {}
            break;
        }
      }
      break;
      case scrolling:
      {
          case ES_UPDATE_ROW:
          {
              
              bool CompleteUpdate = DM_TakeDisplayUpdateStep();
              if (CompleteUpdate)
              {
                  current_state = waiting;
                  if (true == ES_RecallEvents(MyPriority, DeferralQueue))
                  {
                      puts("Recalling Event\r");
                  }
                  //new_event.EventType = ES_NO_EVENT;
              }
              else
              {
                  PostLEDService(ThisEvent);
              }
                     
          }
          break;
          
          case ES_NEW_KEY:
          {
              if (ES_DeferEvent(DeferralQueue, ThisEvent))
              {
                puts("Deferring Character Print\r");
              }
          }
          break;

      }
      break;
      default: {} 
      break;
      
  }
              
              
      
              
      
     
  
#ifdef TEST_INT_POST
      if ('p' == ThisEvent.EventParam)
      {
        StartTMR2();
      }
#endif
    
  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
#ifdef BLINK_LED
#define LED LATBbits.LATB6
static void InitLED(void)
{
  LED = 0; //start with it off
  TRISBbits.TRISB6 = 0; // set RB6 as an output
}

static void BlinkLED(void)
{
  // toggle state of LED
  LED = ~LED;
}
#endif

#ifdef TEST_INT_POST
#include <sys/attribs.h> // for ISR macors

// for testing posting from interrupts.
// Intializes TMR2 to gerenate an interrupt at 100ms
static void InitTMR2(void)
{
  // turn timer off
  T2CONbits.ON = 0;
  // Use internal peripheral clock
  T2CONbits.TCS = 0;
  // setup for 16 bit mode
  T2CONbits.T32 = 0;
  // set prescale to 1:1
  T2CONbits.TCKPS = 0;
  // load period value
  PR2 = 2000-1; // creates a 100ms period with a 20MHz peripheral clock
  // set priority
  IPC2bits.T2IP = 2;
  // clear interrupt flag
  IFS0bits.T2IF = 0;
  // enable the timer interrupt
  IEC0bits.T2IE = 1;
}

// Clears and Starts TMR2
static void StartTMR2(void)
{
  // clear timer
  TMR2 = 0;
  // start timer
  //LATBbits.LATB14 = 0;
  T2CONbits.ON = 1;
}

void __ISR(_TIMER_2_VECTOR, IPL2AUTO) Timer2ISR(void)
{
  // clear flag
  IFS0bits.T2IF = 0;
  // post event
  static ES_Event_t interruptEvent = {ES_SHORT_TIMEOUT, 0};
  PostTestHarnessService0(interruptEvent);
  
  // stop timer
  T2CONbits.ON = 0;
  return;
}
#endif
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

