#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>

void    button_init(void);
void    button_read(void);              /* scheduled task, period 1 tick */
uint8_t isButtonPressed(uint8_t i);     /* short press, returns 1 once */
uint8_t isButtonLongPressed(uint8_t i); /* held >= ~1 s, returns 1 once */

#endif /* INC_BUTTON_H_ */