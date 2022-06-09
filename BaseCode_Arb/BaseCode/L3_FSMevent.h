typedef enum L3_event
{
    L3_event_reqRcvd =1, //A
    L3_event_MSGRcvd =2, //B
    L3_event_RLSRcvd =3, //C
    L3_event_Timeout =4, //D
    
} L3_event_e;

void L3_event_setEventFlag(L3_event_e event);
void L3_event_clearEventFlag(L3_event_e event);
void L3_event_clearAllEventFlag(void);
int L3_event_checkEventFlag(L3_event_e event);