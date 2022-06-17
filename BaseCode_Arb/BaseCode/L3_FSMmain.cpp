//#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
#define L3STATE_IDLE                0
#define L3STATE_COMMUNICATE         2


//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//SDU (input)
// static uint8_t originalWord[200];
// static uint8_t wordLen=0;

static uint8_t sdu[200];

//serial port interface
static Serial pc(USBTX, USBRX);



void L3_FSMrun(void)
{   
    if (prev_state != main_state)
    {
        prev_state = main_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        case L3STATE_IDLE: //IDLE state description
            if (L3_event_checkEventFlag(L3_event_reqRcvd)){
                uint8_t* dataPtr = L3_LLI_getMsgPtr();

                if(dataPtr[0] == '0') { // MSG_TYPE_QUA_REQ
                    //msg header setting
                    // if received message is "MSG_TYPE_QUA_REQ" --> change state to the "Communicate"
                    L3_msg_encodeQual(sdu, 1,1); 
                    // sdu = data, 1 = type, 0 = state
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu)/sizeof(uint8_t)); //res send

                    main_state = L3STATE_COMMUNICATE;
                    //timer start
                    L3_timer_startTimer();
                    L3_event_clearEventFlag(L3_event_reqRcvd); 
                }
                
                
            }
            break;

        // In communication
        case L3STATE_COMMUNICATE:
            uint8_t* dataPtr = L3_LLI_getMsgPtr();
            uint8_t size = L3_LLI_getSize();
            
            L3_event_e event_flag = L3_decode_EventFlag(dataPtr); //decodint data
            L3_event_setEventFlag(event_flag);
            if(L3_timer_getTimerStatus()==0) L3_event_setEventFlag(L3_event_Timeout);
            
            if (L3_event_checkEventFlag(L3_event_reqRcvd)){
                L3_msg_encodeQual(sdu,1,0); //reject
                L3_LLI_dataReqFunc(sdu, sizeof(sdu)/sizeof(uint8_t));
                main_state = L3STATE_COMMUNICATE;
            }
            if (L3_event_checkEventFlag(L3_event_MsgRcvd)){
                uint8_t* _msg;
                memcpy(_msg, &dataPtr[L3_MSG_OFFSET_DATA], (size-1)*sizeof(uint8_t));
                L3_msg_encodeMessage(sdu,_msg,2);//send message
                L3_LLI_dataReqFunc(sdu, sizeof(sdu)/sizeof(uint8_t));

                main_state = L3STATE_COMMUNICATE; //arb release
            }
            if(L3_event_checkEventFlag(L3_event_RlsRcvd)) { // MSG_TYPE_RLS_REQ && L3_event_RLSRcvd
                L3_msg_encodeData(sdu,4); //type-> MSG_TYPE_QUA_RLS==4
                L3_LLI_dataReqFunc(sdu, sizeof(sdu)/sizeof(uint8_t));
                L3_event_clearEventFlag(L3_event_RlsRcvd);
                main_state = L3STATE_IDLE;
            }
            if(L3_event_checkEventFlag(L3_event_Timeout)){
                L3_msg_encodeData(sdu,5); //type-> MSG_TYPE_TIME_OUT==5
                L3_LLI_dataReqFunc(sdu, sizeof(sdu)/sizeof(uint8_t));
                L3_event_clearEventFlag(L3_event_Timeout);
                main_state = L3STATE_IDLE;
            }
            break;
               
            
    }
}