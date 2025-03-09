/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServPot_H
#define ServPot_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitPotService(uint8_t Priority);
bool PostPotService(ES_Event_t ThisEvent);
ES_Event_t RunPotService(ES_Event_t ThisEvent);
bool CheckADEvent(void);
uint16_t mapPotToServo(uint16_t x, uint16_t x_min, uint16_t x_max, uint16_t y_min, uint16_t y_max);

#endif /* ServTemplate_H */

