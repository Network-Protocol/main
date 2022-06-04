#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
#define L3STATE_IDLE                0


//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//SDU (input)
static char originalWord[200];
static char wordLen=0;

static char sdu[200];

//serial port interface
static Serial pc(USBTX, USBRX);


//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_reqToSend))
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_reqToSend);
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
            //qualitification request(A) -> send req_pdu & start res_timer

            if (L3_event_checkEventFlag(L3_event_reqToSend)) 
            {
                //msg header setting 
                // strcpy((char*)sdu, "0Q");
                sdu = L3_msg_encodeData('Q',sizeof(sdu),0); //request qualitification
                L3_LLI_dataReqFunc(sdu, wordLen); //sdu(received data), length, type

                debug_if(DBGMSG_L3, "[L3] sending msg....\n");

                //msg sending
            
                wordLen = 0;

                pc.printf("Give a word to send : ");

                
                //timer start
                L3_timer_startTimer; //wait Arb's res
                main_state = L3STATE_CONNECT;
                L3_event_clearEventFlag(L3_event_reqToSend); 
            }

        
        case L3STATE_CONNECT: //IDLE state description
        //qualitification's res reiceived
            if (L3_event_checkEventFlag(L3_event_resRcvd)) //if data reception event happens
            {
                //Retrieving data info.
                char* dataPtr = L3_LLI_getMsgPtr();
                char size = L3_LLI_getSize();

                debug("\n -------------------------------------------------\nRCVD MSG : %s (length:%i)\n -------------------------------------------------\n", 
                            dataPtr, size);
                if(dataPtr[0] == 1 && L3_timer_getTimerStatus==1) {
                    main_state = L3STATE_COMMUNICATE;
                }
                else { //rejected or time out
                    L3_timer_stopTimer();
                    main_state = L3STATE_IDLE;
                }
                                        
                L3_event_clearEventFlag(L3_event_resRcvd); 
                
            } 


        case L3STATE_COMMUNICATE: //IDLE state description
        int msg_count=0;
            //message sendS -> send msg_pdu & count msg
            if (L3_event_checkEventFlag(L3_event_msgToSend)) //if data needs to be sent (keyboard input)
            {
                //msg header setting
                // strcpy((char*)sdu, (char*)originalWord);
                sdu = L3_msg_encodeData(originalWord,sizeof(originalWord)+1,2); //send message
                L3_LLI_dataReqFunc(sdu, wordLen);

                debug_if(DBGMSG_L3, "[L3] sending msg....\n");
            
                wordLen = 0;

                pc.printf("Give a word to send : ");
                msg_count++;
                L3_event_clearEventFlag(L3_event_msgToSend);
                if(msg_count >=10) L3_event_setEventFlag(L3_event_msgEnd);
                
                main_state = L3STATE_COMMUNICATE;
                
            }

            // request release
            if(L3_event_checkEventFlag(L3_event_msgEnd)){
                sdu = L3_msg_encodeData('R',sizeof(sdu)+1,3); //send release request
                L3_LLI_dataReqFunc(sdu, wordLen);
                msg_count=0; //message count>=10 -> release
                L3_event_clearEventFlag(L3_event_msgEnd);
                main_state = L3STATE_IDLE;

            }

            if(L3_event_checkEventFlag(L3_event_release)){
                msg_count =0;
                L3_event_clearEventFlag(L3_event_release);
                main_state = L3STATE_IDLE;

            }

        default :
            break;
    }
}

