#include "mbed.h"
<<<<<<< Updated upstream
=======


>>>>>>> Stashed changes
#define MSG_TYPE_QUA_REQ 0
#define MSG_TYPE_QUA_RES 1
#define MSG_TYPE_MSG_SEND 2
#define MSG_TYPE_RLS_REQ 3
#define MSG_TYPE_QUA_RLS 4
#define MSG_TYPE_TIME_OUT 5
#define MSG_TYPE_NO_GROUP 6

#define L3_MSG_OFFSET_TYPE  0
#define L3_MSG_OFFSET_DATA  1

#define L3_MSG_MAXDATASIZE  26
#define L3_MSSG_MAX_SEQNUM  1024


int L2_msg_checkIfData(char* msg);
char L3_msg_encodeData(char* data, int type);
char L3_msg_encodeMessage(char* data,char* msg_data, int type);
char L3_msg_getSeq(char* msg);
char* L3_msg_getWord(char* msg);