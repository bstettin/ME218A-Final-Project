/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef BuzzerService_H
#define BuzzerService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitBuzzerService(uint8_t Priority);
bool PostBuzzerService(ES_Event_t ThisEvent);
ES_Event_t RunBuzzerService(ES_Event_t ThisEvent);

#endif /* ServTemplate_H */

