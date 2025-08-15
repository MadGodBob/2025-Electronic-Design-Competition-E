#ifndef __KEY_H
#define __KEY_H

int readKeys(void);
void displayKeys(void);
float get_change_k_sign(void);
    
extern char keys[4];
extern volatile int ready_key_state;

#endif