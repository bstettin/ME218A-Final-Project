/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef GameCountDownService_H
#define GameCountDownService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitGameCountDownService(uint8_t Priority);
bool PostGameCountDownService(ES_Event_t ThisEvent);
ES_Event_t RunGameCountDownService(ES_Event_t ThisEvent);


#endif /* ServTemplate_H */

