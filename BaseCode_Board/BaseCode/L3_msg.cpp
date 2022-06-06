#include "mbed.h"
#include "L3_msg.h"
#include "string.h"


/*
#define MSG_TYPE_QUA_REQ 0
#define MSG_TYPE_QUA_RES 1
#define MSG_TYPE_MSG_SEND 2
#define MSG_TYPE_RLS_REQ 3
#define MSG_TYPE_QUA_RLS 4
#define MSG_TYPE_TIME_OUT 5
#define MSG_TYPE_NO_GROUP 6
*/

// int L3_msg_checkIfData(uint8_t* msg)
// {
//     return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_DATA);
// }

char L3_msg_encodeData(char* msg_data, int type)
{
    // L3_MSG_OFFSET_TYPE=0
    // L3_MSG_OFFSET_TYPE=1
    switch (type)
    {
    case 0:
        msg_data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_QUA_REQ;
        break;
    case 1:
        msg_data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_QUA_RES;
        break;
    case 2:
        msg_data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_MSG_SEND;
        break;
    case 3:
        msg_data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_RLS_REQ;
        break;
    case 4:
        msg_data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_QUA_RLS;
        break;
    
    default:
        break;
    }
    
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], msg_data, sizeof(msg_data));

    return *msg_data;
}

// char L3_msg_getWord(char* msg)
// {
//     return &msg[L3_MSG_OFFSET_DATA];
// }