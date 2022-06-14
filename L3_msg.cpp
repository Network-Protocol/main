#include "mbed.h"
#include "L3_msg.h"
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
#define MSG_TYPE_REJECT 7
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
    case 0:
        data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_QUA_REQ;
        memcpy(&data[L3_MSG_OFFSET_DATA],"REQ_QUA", sizeof(char)*7);
        break;
    case 3:
        data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_RLS_REQ;
        memcpy(&data[L3_MSG_OFFSET_DATA],"REQ_RLS", sizeof(char)*7);
        break;
    case 1:
        data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_QUA_RES;
        memcpy(&data[L3_MSG_OFFSET_DATA],"QUA_RES", sizeof(char)*7);
        break;
    case 4:
        data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_QUA_RLS;
        memcpy(&data[L3_MSG_OFFSET_DATA],"QUA_RLS", sizeof(char)*7);
        break;
    case 7:
        data[L3_MSG_OFFSET_TYPE] = MSG_TYPE_REJECT;
        memcpy(&data[L3_MSG_OFFSET_DATA],"REJECT", sizeof(char)*7);
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