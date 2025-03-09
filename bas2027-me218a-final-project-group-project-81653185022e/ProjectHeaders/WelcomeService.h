/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef WelcomeService_H
#define WelcomeService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitWelcomeService(uint8_t Priority);
bool PostWelcomeService(ES_Event_t ThisEvent);
ES_Event_t RunWelcomeService(ES_Event_t ThisEvent);
bool WelcomeorGame(void);

typedef enum {Welcome = 0, Game = 1, GameOver = 2, Timeout = 3
} WelcomeState_t;

WelcomeState_t getState(void);


#endif /* ServTemplate_H */

