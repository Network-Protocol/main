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
            L3_event_setEventFlag(L3_event_dataToSend);
            debug_if(DBGMSG_L3,"word is ready! ::: %s\n", originalWord);
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);
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
            if (L3_event_checkEventFlag(L3_event_resRcvd)){
                //Retrieving data info.
                char* dataPtr = L3_LLI_getMsgPtr();
                char size = L3_LLI_getSize();

                debug("\n -------------------------------------------------\nRCVD MSG : %s (length:%i)\n -------------------------------------------------\n", dataPtr, size);
                if(dataPtr[0] == 0) {
                    // if received message is "MSG_TYPE_QUA_REQ" --> change state to the "Communicate"
                    sdu = L3_msg_encodeData('Q',2, 1); // reject에 대한 value=0이기 때문에, 0을 message로 전송함 type은 MSG_TYPE_MSG_SEND==2
                    // L3_LLI_dataReqFunc(sdu, wordLen); // wordLen? --> sizeof(sdu)가 되어야하지않나?
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu)); // wordLen? --> sizeof(sdu)가 되어야하지않나?

                    main_state = L3STATE_COMMUNICATE;
                    //timer start
                    L3_timer_startTimer(); //wait Arb's res
                }
                L3_event_clearEventFlag(L3_event_resRcvd); 
            }
            else{
                // What?
            }
        // In communication
        case L3STATE_COMMUNICATE:
            if (L3_event_checkEventFlag(L3_event_resRcvd)){
                /*
                #define MSG_TYPE_QUA_REQ 0
                #define MSG_TYPE_QUA_RES 1
                #define MSG_TYPE_MSG_SEND 2
                #define MSG_TYPE_RLS_REQ 3
                #define MSG_TYPE_QUA_RLS 4
                #define MSG_TYPE_TIME_OUT 5
                #define MSG_TYPE_NO_GROUP 6
                #define MSG_TYPE_QUA_REJ 7
                // 7을 추가하려면 L3_msg.cpp를 수정해야함...
                
                */
                //Retrieving data info.
                char* dataPtr = L3_LLI_getMsgPtr();
                char size = L3_LLI_getSize();

                char* _msg;

                debug("\n -------------------------------------------------\nRCVD MSG : %s (length:%i)\n -------------------------------------------------\n", dataPtr, size);
                if(dataPtr[0] == 0) { // MSG_TYPE_QUA_REQ
                    //msg header setting
                    // sdu = L3_msg_encodeData('0',2, 7); // reject에 대한 value=0이기 때문에, 0을 message로 전송함 type은 MSG_TYPE_MSG_SEND==2
                    sdu = L3_msg_encodeData('Q',2, 0); // reject이려면, 값이 1만 아니면 되기때문에 그냥 0으로함. 
                    // L3_LLI_dataReqFunc(sdu, wordLen); // wordLen? --> sizeof(sdu)가 되어야하지않나?
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu)); // wordLen? --> sizeof(sdu)가 되어야하지않나?
                                    
                    main_state = L3STATE_COMMUNICATE;
                }
                else if(dataPtr[0] == 2) { // MSG_TYPE_QUA_REQ
                    memcpy(&dataPtr[L3_MSG_OFFSET_DATA], _msg, (size-1)*sizeof(char))
                    sdu = L3_msg_encodeData(_msg,sizeof(_msg)+1,2); //send message --> board 코드에 msg를 받는 코드가 추가되어야할 것 같음
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu));
                    // Arb가 보드에게 메세지를 쏘긴할텐데, 메세지를 보낸 보드를 제외하고 메세지를 보내는지?
                    // 여러명한테 동시에 보낸다면, 그런 process를 여기에서 처리해주는지 혹은 기계 내부 적으로 처리해주는지?
                    main_state = L3STATE_COMMUNICATE;
                }
                else if(dataPtr[0] == 3|| dataPtr[0] == 5) { // MSG_TYPE_RLS_REQ  or MSG_TYPE_TIME_OUT 
                    sdu = L3_msg_encodeData('R',2, 4); // reject에 대한 value=0이기 때문에, 0을 message로 전송함 type은 MSG_TYPE_MSG_SEND==2
                    L3_LLI_dataReqFunc(sdu, sizeof(sdu));
                    
                    main_state = L3STATE_IDLE;
                }
                else { 
                    // Nothing to do
                }
                L3_event_clearEventFlag(L3_event_resRcvd); 
            }
            else{
                // what?
            }
        default :
            break;
    }
}