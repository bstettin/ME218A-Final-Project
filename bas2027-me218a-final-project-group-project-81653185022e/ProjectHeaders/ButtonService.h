/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ButtonService_H
#define ButtonService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitButtonService(uint8_t Priority);
bool PostButtonService(ES_Event_t ThisEvent);
ES_Event_t RunButtonService(ES_Event_t ThisEvent);
bool CheckButton(void);
uint8_t getPoints(void);
uint32_t getLeftmostLEDs(void);
#endif /* ServTemplate_H */

