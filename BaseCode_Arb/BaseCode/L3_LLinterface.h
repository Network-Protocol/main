extern void (*L3_LLI_dataReqFunc)(char* msg, char size);
extern int (*L3_LLI_configReqFunc)(char type, char value);
//syntax : L3_LLI_configReqFunc(L2L3_CFGTYPE_SRCID, 2);

void L3_LLI_dataInd(char* dataPtr, char size, char snr, int16_t rssi);
char* L3_LLI_getMsgPtr();
char L3_LLI_getSize();
void L3_LLI_setDataReqFunc(void (*funcPtr)(char*, char));
void L3_LLI_setConfigReqFunc(int (*funcPtr)(char, char));