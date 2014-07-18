/*
 * ineedmd_bluetooth_radio.h
 *
 *  Created on: Jul 7, 2014
 *      Author: BrianS
 */

#ifndef __INEEDMD_BLUETOOTH_RADIO_H__
#define __INEEDMD_BLUETOOTH_RADIO_H__

void ineedmd_radio_power(bool);
void ineedmd_radio_reset(void);
void ineedmd_radio_soft_reset(void);
void ineedmd_radio_send_string(char *send_string);
void bluetooth_setup(void);
#endif /* INEEDMD_BLUETOOTH_RADIO_H_ */
