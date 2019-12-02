/* 
 * Operating Systems {2INCO} Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions which are commonly used by the farmer and the workers
 *
 */

#ifndef COMMON_H
#define COMMON_H
#include "uint128.h"

// maximum size for any message in the tests
#define MAX_MESSAGE_LENGTH	6
 

// TODO: put your definitions of the datastructures here

static mq_name1[80];
static mq_name2[80];

typedef struct
{
    // a data structure with 6 members
    int                     pid; //ID
    int 					length; //Message Length
    char					encryption[80]; //encryption 
    char					Fc; //First Char of the given alphabet
    char					Lc; //Last Char of the given alphabet
    uint128_t				md5; // MD encrytion tool
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // a data structure with 3 members

    int 					pid; //Id
    int                     length; //Rength of the sollution
    char                    message[MAX_MESSAGE_LENGTH]; //The sollution
} MQ_RESPONSE_MESSAGE;


#endif

