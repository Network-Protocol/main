#include "L3_FSMevent.h"
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
static uint8_t originalWord[200];
static uint8_t wordLen=0;

static uint8_t sdu[200];

//serial port interface
static Serial pc(USBTX, USBRX);


//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_dataToSend))
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            if (originalWord == "REQ_Q"){
                L3_event_setEventFlag(L3_event_dataToSend);    //질문
            } else L3_event_setEventFlag(L3_event_msgToSend);  //질문

            debug_if(DBGMSG_L3,"word is ready! ::: %s\n", originalWord);
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);  //질문
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
        }
    }
}



void L3_initFSM()
{
    //initialize service layer
    pc.attach(&L3service_processInputWord, Serial::RxIrq);

    pc.printf("Give a word to send : ");
}

void L3_FSMrun(void)
{   
    if (prev_state != main_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %i to %i\n", prev_state, main_state);
        prev_state = main_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        case L3STATE_IDLE: //IDLE state description
            if (L3_event_checkEventFlag(L3_event_reqRcvd)){
                
                 /*
                #define MSG_TYPE_QUA_REQ 0
                #define MSG_TYPE_QUA_RES 1
                #define MSG_TYPE_MSG_SEND 2
                #define MSG_TYPE_RLS_REQ 3
                #define MSG_TYPE_QUA_RLS 4
                #define MSG_TYPE_TIME_OUT 5
                #define MSG_TYPE_NO_GROUP 6
                
                */
                //Retrieving data info.
                char* dataPtr = L3_LLI_getMsgPtr();
                char size = L3_LLI_getSize();

                char* _msg;

                debug("\n -------------------------------------------------\nRCVD MSG : %s (length:%i)\n -------------------------------------------------\n", dataPtr, size);

                if(dataPtr[0] == 0) { // MSG_TYPE_QUA_REQ
                    //msg header setting
                    // if received message is "MSG_TYPE_QUA_REQ" --> change state to the "Communicate"
                    L3_msg_encodeData(sdu, 1); // permit value=1 , type--> MSG_TYPE_QUA_RES==1 
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu)); 
                    main_state = L3STATE_COMMUNICATE;
                    //timer start
                    L3_timer_startTimer();
                    L3_event_clearEventFlag(L3_event_reqRcvd); 
                }
                break;
                
            }
        // In communication
        case L3STATE_COMMUNICATE:
            if (L3_event_checkEventFlag(L3_event_reqRcvd)){
               
                char* dataPtr = L3_LLI_getMsgPtr();
                char size = L3_LLI_getSize();

                char* _msg;

                debug("\n -------------------------------------------------\nRCVD MSG : %s (length:%i)\n -------------------------------------------------\n", dataPtr, size);
                if(dataPtr[0] == 0) { // MSG_TYPE_QUA_REQ
                    
                    main_state = L3STATE_COMMUNICATE;// reject.
                }
                else if(dataPtr[0] == 2 && L3_event_checkEventFlag(L3_event_MSGRcvd)) { // MSG_TYPE_MSG_SEND && L3_event_MSGRcvd
                    memcpy(_msg, &dataPtr[L3_MSG_OFFSET_DATA], (size-1)*sizeof(char))
                    //sdu = L3_msg_encodeData(_msg,sizeof(_msg)+1,2);  
                    L3_msg_encodeMessage(sdu,originalWord,2);//send message
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu));
                    //질문 if pdu type = 2 && current_board == data sended borad's number->arb send data to group
                    // if arq_count>10(group is not exist or disconnected) -> act stop ->massage timer stop -> send PDU type = 6 to board 
                    //  ->arb release
                    main_state = L3STATE_COMMUNICATE;
                }
                 else if(dataPtr[0] == 3 && L3_event_checkEventFlag(L3_event_RLSRcvd)) { // MSG_TYPE_RLS_REQ && L3_event_RLSRcvd
                    //sdu = L3_msg_encodeData(sdu,2, 4);
                    L3_msg_encodeData(sdu,4); //type-> MSG_TYPE_QUA_RLS==4
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu));
                    
                    main_state = L3STATE_IDLE;
                }
                 else if(L3_event_checkEventFlag(L3_event_Timeout)){
                    L3_msg_encodeData(sdu,5); //type-> MSG_TYPE_TIME_OUT==5
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu));

                    L3_event_clearEventFlag(L3_event_Timeout);
                    main_state = L3STATE_IDLE;
                }
               
            }
        default :
            break;
    }
}