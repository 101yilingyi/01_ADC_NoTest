#ifndef __DRV_NET_H
#define __DRV_NET_H

int Drv_4GModule_Init(void);
float Get_SignalStrength(void);
long long Get_IMEI(void);
int Get_MQTT_State(void);
int MQTT_Publish_Msg(unsigned char pubnum, const char* payload);
int MQTT_Get_Msg(unsigned char* pdata, unsigned int length);

#endif /* __DRV_NET_H */
