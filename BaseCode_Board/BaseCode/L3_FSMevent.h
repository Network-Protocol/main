typedef enum L3_event
{
    L3_event_reqToSend = 1, //A
    L3_event_rls_req =2, //B
    L3_event_resRcvd = 3, //C Answer for Requst qualitification
    L3_event_msgToSend = 4, //D
    L3_event_msgEnd =5, //E message count>=10
    
} L3_event_e;


void L3_event_setEventFlag(L3_event_e event);
void L3_event_clearEventFlag(L3_event_e event);
void L3_event_clearAllEventFlag(void);
int L3_event_checkEventFlag(L3_event_e event);