 /*
 * bat.h
 *  Created on: 23 JAN 2022
 *   Author: NirK12
 */

#ifndef BAT_H_
#define BAT_H_


typedef struct b
{
    int rfid;				        // The number of the bat.
    String allowed;		 	    // "no" / "yes".
    int deserving_amount;   // (=1)*(per visits), up to infinity....
}Bat;

#endif
