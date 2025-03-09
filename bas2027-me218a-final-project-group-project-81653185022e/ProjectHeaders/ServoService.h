/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServoService_H
#define ServoService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitServoService(uint8_t Priority);
bool PostServoService(ES_Event_t ThisEvent);
ES_Event_t RunServoService(ES_Event_t ThisEvent);

#endif /* ServTemplate_H */

