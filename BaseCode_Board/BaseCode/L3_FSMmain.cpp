#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
#define L3STATE_IDLE                0
#define L3STATE_CONNECT             1
#define L3STATE_COMMUNICATE       2


//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//SDU (input)
static uint8_t originalWord[20];
static int wordLen=0;

static uint8_t sdu[20];

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
            if (strncmp((const char*)originalWord,"REQ_Q",5) == 0){
                L3_event_setEventFlag(L3_event_reqToSend);
            } else L3_event_setEventFlag(L3_event_msgToSend);        
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_msgToSend);
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
        prev_state = main_state;
    }
    L3_event_checkEventFlag(L3_event_reqToSend);

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        case L3STATE_IDLE: //IDLE state description
            //qualitification request(A) -> send req_pdu & start res_timer

            if (L3_event_checkEventFlag(L3_event_reqToSend)) 
            {
                //msg header setting 
                L3_msg_encodeData(sdu, 0); //request qualitification
                L3_LLI_dataReqFunc(sdu,sizeof(sdu)/sizeof(uint8_t)); //sdu(received data), length, type

                //msg sending
            
                wordLen = 0;
                
                //timer start
                L3_timer_startTimer(); //wait Arb's res
                main_state = L3STATE_CONNECT;
                L3_event_clearEventFlag(L3_event_reqToSend); 
                L3_event_setEventFlag(L3_event_resRcvd);
                break;
            }
            
            else break;

        
        case L3STATE_CONNECT: //IDLE state description
        //qualitification's res reiceived
            if (L3_event_checkEventFlag(L3_event_resRcvd)) //if data reception event happens
            {
                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                // uint8_t size = L3_LLI_getSize();
                
                if(dataPtr[1] == '1' && L3_timer_getTimerStatus()==1) {
                    main_state = L3STATE_COMMUNICATE;
                }
                else { //rejected or time out
                    L3_timer_stopTimer();
                    main_state = L3STATE_IDLE;
                }
                                        
                L3_event_clearEventFlag(L3_event_resRcvd); 
                L3_event_setEventFlag(L3_event_msgToSend);
                
            } 
            break;


        case L3STATE_COMMUNICATE: //IDLE state description
        int msg_count=0;
            //message sendS -> send msg_pdu & count msg
            if (L3_event_checkEventFlag(L3_event_msgToSend)) //if data needs to be sent (keyboard input)
            {
                //msg header setting
                //L3_msg_encodeData(sdu,originalWord,2); 
                L3_msg_encodeMessage(sdu,originalWord,2); //send message
                L3_LLI_dataReqFunc(sdu,sizeof(sdu)/sizeof(uint8_t));            
                wordLen = 0;
                // msg reset
                for(int i=0; i< wordLen+1;i++){
                    originalWord[i]=NULL;
                }
                msg_count++;
                L3_event_clearEventFlag(L3_event_msgToSend);
                L3_event_setEventFlag(L3_event_msgToSend);
                if(msg_count >=10) L3_event_setEventFlag(L3_event_msgEnd);
                
                main_state = L3STATE_COMMUNICATE;
                
            }

            // request release
            else if(L3_event_checkEventFlag(L3_event_msgEnd)){
                L3_msg_encodeData(sdu,3); //send release request
                L3_LLI_dataReqFunc(sdu,sizeof(sdu)/sizeof(uint8_t));
                msg_count=0; //message count>=10 -> release
                L3_event_clearEventFlag(L3_event_msgEnd);
                L3_event_setEventFlag(L3_event_reqToSend);
                main_state = L3STATE_IDLE;

            }

            else if(L3_event_checkEventFlag(L3_event_rls_req)){
                msg_count =0;
                L3_event_clearEventFlag(L3_event_rls_req);
                L3_event_setEventFlag(L3_event_reqToSend);
                main_state = L3STATE_IDLE;

            }
            break;

        // default :
            // break;
    }
}

