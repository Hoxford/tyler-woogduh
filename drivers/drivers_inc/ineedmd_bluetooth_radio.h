/*
 * ineedmd_bluetooth_radio.h
 *
 *  Created on: Jul 7, 2014
 *      Author: BrianS
 */

#ifndef __INEEDMD_BLUETOOTH_RADIO_H__
#define __INEEDMD_BLUETOOTH_RADIO_H__

void        ineedmd_radio_power(bool);
void        ineedmd_radio_reset(void);
//void ineedmd_radio_soft_reset(void);
ERROR_CODE  eIneedmd_radio_rfd(void);
void        ineedmd_radio_send_string(char *send_string, uint16_t uiBuff_size);
int         ineedmd_radio_send_frame(uint8_t *send_frame, uint16_t uiFrame_size);
int         iIneedmd_radio_que_frame(uint8_t *send_frame, uint16_t uiFrame_size);
int         iIneedmd_radio_rcv_string(char *cRcv_string, uint16_t uiBuff_size);
int         iIneedmd_radio_rcv_byte(uint8_t *uiRcv_byte);
int         iIneedmd_radio_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size);
ERROR_CODE  eIneedmd_radio_int_rcv_frame(uint8_t *uiRcv_frame, uint16_t uiBuff_size);
int         iIneedMD_radio_setup(void);
int         iIneedMD_radio_check_for_connection(void);
int         iIneedMD_radio_process(void);

#endif /* INEEDMD_BLUETOOTH_RADIO_H_ */
