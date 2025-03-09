/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef CoinLEDService_H
#define CoinLEDService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitCoinLEDService(uint8_t Priority);
bool PostCoinLEDService(ES_Event_t ThisEvent);
ES_Event_t RunCoinLEDService(ES_Event_t ThisEvent);
bool Check4Coin(void);






#endif /* ServTemplate_H */

