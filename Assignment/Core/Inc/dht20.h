

#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include <stdint.h>

void    dht20_init(void);
uint8_t dht20_trigger(void); 
uint8_t dht20_read(void);    
void    dht20_task(void);    

#endif /* INC_DHT20_H_ */
