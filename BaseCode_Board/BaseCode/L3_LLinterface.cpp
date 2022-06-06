#include "mbed.h"
#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "protocol_parameters.h"
#include "time.h"

static char rcvdMsg[L3_MAXDATASIZE];
static char rcvdSize;
static int16_t rcvdRssi; //type: short
static char rcvdSnr;

//TX function
void (*L3_LLI_dataReqFunc)(char* msg, char size);
//configFunction
int (*L3_LLI_configReqFunc)(char type, char value);

//interface event : DATA_IND, RX data has arrived
void L3_LLI_dataInd(char* dataPtr, char size, char snr, int16_t rssi)
{
    memcpy(rcvdMsg, dataPtr, size*sizeof(char));
    rcvdSize = size;
    rcvdSnr = snr;
    rcvdRssi = rssi;

    L3_event_setEventFlag(L3_event_resRcvd);
}

char* L3_LLI_getMsgPtr()
{
    return rcvdMsg;
}
char L3_LLI_getSize()
{
    return rcvdSize;
}

void L3_LLI_setDataReqFunc(void (*funcPtr)(char*, char))
{
    L3_LLI_dataReqFunc = funcPtr;
}

void L3_LLI_setConfigReqFunc(int (*funcPtr)(char, char))
{
    L3_LLI_configReqFunc = funcPtr;
}