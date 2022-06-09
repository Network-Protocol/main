#include "mbed.h"
#include "L3_msg.h"
<<<<<<< Updated upstream
#include "mbed.h"
#include "L3_msg.h"
=======
>>>>>>> Stashed changes
#include "string.h"


/*
#define MSG_TYPE_QUA_REQ 0
<<<<<<< Updated upstream
#define MSG_TYPE_QUA_RES 1 //발언권 승인(value = 1) 거절(value =0)
#define MSG_TYPE_MSG_SEND 2
#define MSG_TYPE_RLS_REQ 3
#define MSG_TYPE_QUA_RLS 4 //발언권 release 승인
=======
#define MSG_TYPE_QUA_RES 1
#define MSG_TYPE_MSG_SEND 2
#define MSG_TYPE_RLS_REQ 3
#define MSG_TYPE_QUA_RLS 4
>>>>>>> Stashed changes
#define MSG_TYPE_TIME_OUT 5
#define MSG_TYPE_NO_GROUP 6
*/

// int L3_msg_checkIfData(uint8_t* msg)
// {
//     return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_DATA);
// }

char L3_msg_encodeData(char* data, int type)
{
    // L3_MSG_OFFSET_TYPE=0
    // L3_MSG_OFFSET_TYPE=1
    switch (type)
    {
    case 1:
        data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_QUA_RES;
        memcpy(&data[L3_MSG_OFFSET_DATA],"REQ_QUA", sizeof(char)*7);
        break;
    case 4:
        data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_QUA_RLS;
        memcpy(&data[L3_MSG_OFFSET_DATA],"REQ_RLS", sizeof(char)*7);
        break;
    
    default:
        break;
    }
    
    return *data;
}

char L3_msg_encodeData(char* data,char* msg_data, int type)
{
    
    data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_MSG_SEND;
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], msg_data, sizeof(msg_data));

    return *data;
}