#include "drv_net.h"
#include "stm32f1xx_hal.h"
#include "drv_uart.h"
#include "drv_gpio.h"

#include <string.h>
#include <stdio.h>
//#include <stdint.h>

static unsigned char isCommandMode = 0; // 0-͸��ģʽ��1-ָ��ģʽ`

static int Enter_CommandMode(void);
static int Exit_CommandMode(void);
static int SaveConfig_and_Reset(void);
static int Function_ReShow_Ctrl(const char* string);
static int Function_WorkMode_Ctrl(const char* string);

static int Set_SerialLength(unsigned short len);
static int Set_SerialPacketTime(unsigned short time);
static int Function_Heart_Ctrl(unsigned char *string);
static int Set_MQTT_Heart(unsigned short keepalive, unsigned char cleansession);
static int Set_PublishTopic(uint8_t pubnum, uint8_t puben, const char *topic, uint8_t qos, uint8_t retained);
static int Set_SubscribeTopic(uint8_t subnum, uint8_t suben, int8_t *topic, uint8_t qos);
static int Set_MQTT_SerialCommunicationMode(unsigned char Mode);
static int Set_MQTT_Version(unsigned char version);
static int Set_MQTT_ServerIP(const char *server, unsigned short port);
static int Set_MQTT_ClientID(const char *client);
static int Set_MQTT_UserName(const char *name);
static int Set_MQTT_PassWord(const char *password);

int Drv_4GModule_Init(void)
{
	GPIO_4GModule_Init();
	Drv_USART2_Init();
	while(Enter_CommandMode() != 0);
	Function_WorkMode_Ctrl("MQTT");
	Set_SerialLength(4096);
	Set_SerialPacketTime(500);
	Function_Heart_Ctrl("OFF");
	Set_PublishTopic(1, 1, "/dev/update", 0, 0);
	Set_SubscribeTopic(1, 1, "/dev/get", 0);
	Set_MQTT_SerialCommunicationMode(1);
	Set_MQTT_Version(3);
	Set_MQTT_ServerIP("broker.emqx.io", 1883);
	long long id = Get_IMEI();
	char idstr[] = {0};
	sprintf(idstr, "%lld", id);
	Set_MQTT_ClientID(idstr);
	Set_MQTT_UserName("board");
	Set_MQTT_PassWord("123456");
	SaveConfig_and_Reset();
	return 0;
}

/* MCU ��ģ�鷢�� "+++", ģ����յ� "+++", �� MCU �ظ� 'a'
 * MCU ���յ� a ��, 3s �ڸ�ģ�鷢�� a , ģ���յ� a �󣬸��豸���� "+ok"
 */
static int Enter_CommandMode(void)
{
	char buf[8] = "+++";
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));	// ���� +++
	if(ret <= 0)	return -1;
	
	HAL_Delay(1000);
	char ack = 0;
	ret = Drv_USART2_Read((unsigned char *)&ack, 1);				// ���� a
	if(ret < 0)		return -1;
	if(ack == 'a'){
		ret =  Drv_USART2_Write((unsigned char *)ack, 1);			// ���� a
		if(ret < 0)		return -1;
		
		memset(buf, 0, strlen(buf));
		HAL_Delay(1000);
		ret = Drv_USART2_Read((unsigned char *)&buf, 8);			// ���� +ok
		if(ret < 0)		return -1;
		if(strcmp(buf, "+ok") == 0){
			isCommandMode = 1;
			return 0;
		}
	}
	return -1;
}

static int Exit_CommandMode(void)
{
	char buf[16] = "AT+ENTM\r\n";
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0)		return -1;
	
	HAL_Delay(1000);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 16);
	if(ret < 0)		return -1;
	if(strstr(buf, "\r\nOK\r\n") == 0){
		return 0;
		isCommandMode = 0;
	}
	return -1;
}

static int SaveConfig_and_Reset(void)
{
	char buf[16] = "AT+S\r\n";
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0)		return -1;
	
	HAL_Delay(500);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 16);
	if(ret < 0)		return -1;
	if(strcmp(buf, "\r\nOK\r\n") == 0){
		return 0;
	}
	return -1;
}

// ���Կ���
static int Function_ReShow_Ctrl(const char* string)
{
	if(strcmp(string, "ON") != 0 && strcmp(string, "OFF") != 0)		return -1;
	char buf[16] = {0};
	sprintf(buf, "AT+E=%s\r\n", string);
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0)		return -1;
	
	HAL_Delay(500);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 16);
	if(ret < 0)		return -1;
	if(strcmp(buf, "\r\nOK\r\n") == 0){
		return 0;
	}
	return -1;
} 

// ����ģʽ
static int Function_WorkMode_Ctrl(const char* string)
{
	//if(strcmp(string, "ON") != 0 && strcmp(string, "OFF") != 0)		return -1;
	char buf[32] = {0};
	sprintf(buf, "AT+WKMOD=%s\r\n", string);
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0)		return -1;
	
	HAL_Delay(500);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 32);
	if(ret < 0)		return -1;
	if(strcmp(buf, "\r\nOK\r\n") == 0){
		return 0;
	}
	return -1;
}

// ��ѯ�ź�ǿ�� CSQ,  rssi-�ź�ǿ��, ber-������   
float Get_SignalStrength(void)
{
	if(isCommandMode == 0){
		while(Enter_CommandMode() != 0);
	}
	char buf[16] = "AT+CSQ\r\n";
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0){
		while(Exit_CommandMode() != 0);
		return -1;
	}
	
	HAL_Delay(100);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 16);
	if(ret < 0){
		while(Exit_CommandMode() != 0);
		return -1;
	}
	while(Exit_CommandMode() != 0);
	if(ret < 0)		return -1;
	if(strstr(buf, "+CSQ:") == NULL)	return -1;
	char *p = strstr(buf, "+CSQ: ");
	if(p == NULL)	return -1;
	p += strlen("+CSQ: ");
	unsigned char rssi = 0;
	while(*p != ',')
	{
		rssi = rssi * 10 + *p - '0';
		p++;
	}
	return (rssi+113)/2.0;
}

/* ��ѯ��Ϣָ�� */
// ��ѯ IMEI ��
long long Get_IMEI(void)
{
	char buf[32] = "AT+IMEI\r\n";
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0){
		while(Exit_CommandMode() != 0);
		return -1;
	}
	
	HAL_Delay(500);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 32);
	if(ret < 0){
		while(Exit_CommandMode() != 0);
		return -1;
	}
	while(Exit_CommandMode() != 0);
	char *p = strstr(buf, "+IMEI");
	if(p == NULL)	return -1;
	p += strlen("+IMEI: ");
	long long imei = 0;
	while(*p != 'r')
	{
		imei = imei * 10 + *p - '0';
		p++;
	}
	return imei;
}


// ��ѯICCID ��
// ��ѯ����IP
// ��ѯ SIM ���绰����

/* ���ڲ���ָ�� */
// ��ѯ/���ô��ڴ������ byte
static int Set_SerialLength(unsigned short len)
{
	if(len < 5 || len > 4096)	return -1;
	char buf[32] = {0};
	sprintf(buf, "AT+UARTFL%d\r\n", len);
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0)		return -1;
	
	HAL_Delay(500);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 32);
	if(ret < 0)		return -1;
	if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
	return -1;
}

// ��ѯ/���ô��ڴ��ʱ�� ms
static int Set_SerialPacketTime(unsigned short time)
{
	if(time < 20 || time > 500)		return -1;
	char buf[16] = {0};
	sprintf(buf, "AT+UARTFT=%d\r\n", time);
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0)		return -1;
	
	HAL_Delay(500);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 16);
	if(ret < 0)		return -1;
	if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
	return -1;
}

/* ������ָ�� */
// ��ѯ/����������ʹ��
static int Function_Heart_Ctrl(unsigned char *string)
{
	if(strcmp(string, "ON") != 0 && strcmp(string, "OFF") != 0)		return -1;
	char buf[16] = {0};
	sprintf(buf, "AT+HEARTEN=%s\r\n", string);
	int ret = Drv_USART2_Write((unsigned char *)buf, strlen(buf));
	if(ret < 0)		return -1;
	
	HAL_Delay(500);
	memset(buf, 0, strlen(buf));
	ret = Drv_USART2_Read((unsigned char *)&buf, 16);
	if(ret < 0)		return -1;
	if(strcmp(buf, "\r\nOK\r\n") == 0){
		return 0;
	}
	return -1;
}

/* ���� MQTT ���������������� 
 * keepalive: MQTT ������, cleansession: ��������־, 0���ر���������ʶ
 */
static int Set_MQTT_Heart(unsigned short keepalive, unsigned char cleansession)
{
    if(cleansession != 0 && cleansession != 1)    return -1;
    char buf[32] = {0};
    sprintf(buf, "AT+MQTTCFG=%d,%d\r\n", keepalive, cleansession);
    int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 32);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}

// ���÷���topic
static int Set_PublishTopic(uint8_t pubnum, uint8_t puben, const char *topic, uint8_t qos, uint8_t retained)
{
	if(pubnum==0 || pubnum>10)  return -1;
    if(puben!=0 && puben!=1)    return -1;
    if(NULL == topic)           return -1;
    if(qos > 2)                 return -1;
    if(retained!=0 && retained!=1)    return -1;
	
	char buf[64] = {0};
	sprintf(buf, "AT+MQTTPUBTP=%d,%d,%s,%d,%d\r\n", pubnum, puben, topic, qos, retained);
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 64);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}

// ���ö���topic
static int Set_SubscribeTopic(uint8_t subnum, uint8_t suben, int8_t *topic, uint8_t qos)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MQTTSUBTP=%d,%d,%s,%d\r\n", subnum, suben, topic, qos);
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 64);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}

/* ���� MQTT ���ڴ���ģʽ 
 * 0��͸��ģʽ��Ĭ�ϣ�,1���ַ�ģʽ
 */
static int Set_MQTT_SerialCommunicationMode(unsigned char Mode)
{
	if(Mode != 0 && Mode != 1)	return -1;
	char buf[16] = {0};
	sprintf(buf, "AT+MQTTMOD=%d\r\n", Mode);
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 16);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}

// ���� MQTT �汾
static int Set_MQTT_Version(unsigned char version)
{
	if(version != 3 && version != 4)	return -1;
	char buf[16] = {0};
	sprintf(buf, "AT+MQTTVER=%d\r\n", version);
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 16);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}

// ���� MQTT ���������ӵ�ַ
static int Set_MQTT_ServerIP(const char *server, unsigned short port)
{
	if(server == NULL)		return -1;
	if(strlen(server) > 128)  return -1;
	if(port == 0)	return -1;
	char buf[32] = {0};
	sprintf(buf, "AT+MQTTSVR=%s,%d\r\n", server, port);
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 32);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}

// ���� MQTT �ͻ���ID, ID ��Χ: 1~128
static int Set_MQTT_ClientID(const char *client)
{
	if(client == NULL || strlen(client) == 128)	return -1;
	char buf[256] = {0};
	sprintf(buf, "AT+MQTTCID=%s\r\n", client);
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 32);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}
// ���� MQTT �ͻ��û���
static int Set_MQTT_UserName(const char *name)
{
	if(name == NULL || strlen(name) == 128)	return -1;
	char buf[32] = {0};
	sprintf(buf, "AT+MQTTUSER=%s\r\n", name);
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 32);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}

// ���� MQTT �ͻ��û�����
static int Set_MQTT_PassWord(const char *password)
{
	if(password == NULL || strlen(password) == 128)	return -1;
	char buf[256] = {0};
	sprintf(buf, "AT+MQTTPSW=%s\r\n", password);
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return -1;
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 32);
    if(ret < 0) return -1;
    if(strcmp(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
        return 0;
    }
    return -1;
}

// ��ѯ MQTT ������״̬
int Get_MQTT_State(void)
{
	if(isCommandMode == 0)
	{
		while(Enter_CommandMode() != 0);
	}
	char buf[256] = "AT+MQTTSTA\r\n";
	int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0){
		while(Exit_CommandMode() != 0);
		return -1;
	}
    
    HAL_Delay(10);      // �ȴ�ģ�����Ӧ
    memset(buf, 0, strlen(buf));
    ret = Drv_USART2_Read((unsigned char*)buf, 32);
	while(Exit_CommandMode() != 0);
    if(ret < 0) return -1;
    if(strstr(buf, "\r\nOK\r\n") == 0)  // �յ�����ȷ��ָ����Ӧ��
    {
		if(strstr(buf, "+MQTTSTA:\"DISCONNECTION\"") == 0)	return -1;
		else if(strstr(buf, "+MQTTSTA:\"CONNECTION\"")==0) 	return 0;
        return 0;
    }
    return -1;
}

// ������Ϣ
int MQTT_Publish_Msg(unsigned char pubnum, const char* payload)
{
    char buf[8] = {0};
    sprintf(buf, "%d,", pubnum);
    int ret = Drv_USART2_Write((unsigned char*)buf, strlen(buf));
    if(ret < 0) return ret;
    
    ret = Drv_USART2_Read((unsigned char*)payload, strlen(payload));
    return ret;
}

int MQTT_Get_Msg(unsigned char* pdata, unsigned int length)
{
    return Drv_USART2_Read(pdata, length);
}

