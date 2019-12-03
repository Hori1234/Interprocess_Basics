/* 
 * Operating Systems {2INCO} Practical Assignment
 * Interprocess Communication
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * Extra steps can lead to higher marks because we want students to take the initiative. 
 * Extra steps can be, for example, in the form of measurements added to your code, a formal 
 * analysis of deadlock freeness etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>          // for perror()
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include <time.h>           // for time()
#include <complex.h>
#include "uint128.h"
#include "common.h"
#include "md5s.h"

static void rsleep (int t);
static bool findHash(char Fc, char Lc, char word[], uint128_t encryptedWord, char* solution, int messageLength, int k);
static bool compare(uint128_t check[],uint128_t result[]);

int main (int argc, char * argv[])
{
    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * open the two message queues (whose names
    mqd_t m_com_request;
    mqd_t m_com_response;

    MQ_REQUEST_MESSAGE req;
    MQ_RESPONSE_MESSAGE res; 
    
    m_com_request = mq_open(mq_name1,O_RDONLY);
    m_com_response = mq_open(mq_name2,O_WRONLY);
    //      are provided in the arguments)
    bool havingJobs = true;
    //  * repeatingly:
    while(havingJobs){
    //      - read from a message queue the new job to do
            mq_receive(m_com_request,(char*) &req, sizeof(req),NULL);

    //      - wait a random amount of time (e.g. rsleep(10000);)
            rsleep(10000);
            
            if (req.length = 0){
                havingJobs = false;
            } else {
            
            //define a temp variable to store the first char
            
            char word[MAX_MESSAGE_LENGTH];
            word[0] = req.Fc;

                //initialize the sollution array
            char sollution[req.length];
    
    //      - do that job 
              bool hashFound = findHash(req.Fc,req.Lc,word,req.encryption,sollution,req.length,1);  

              if(hashFound){

                // Copying the solution in the new message
                for (int i=0; i< sizeof(req); i++){
                    res.message[i] = sollution[i];
                }

                // Filling the rest of the message in order to reach the 
                // final length
                for (int i = req.length; i < sizeof(res.message); i++)
                {
                    res.message[i] = (char) 0;
                }

                res.length = strlen(sollution);
              } else {
                
                res.length = 0;
              
              }
    //      - write the results to a message queue
              mq_send(m_com_response,(char *) &res,sizeof(res),0);

            }
    
            
    //    until there are no more tasks to do
    }
    
    //  * close the message queues
    mq_close(m_com_request);
    mq_close(m_com_response);
    
    return (0);
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;
    
    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}

static bool findHash(char Fc, char Lc, char word[], uint128_t encryptedWord, char* solution, int messageLength, int k){

    uint128_t hash;
    
    hash = md5s(word,strlen(word));

    bool hashIsFound = false;

    if(strlen(hash) == strlen(encryptedWord)){

        if (compare(hash,encryptedWord)) {
            strcpy(solution,word);
            hashIsFound = true;
        } 
    } else {

        if (strlen(word) < messageLength && hashIsFound == false){

            for (char c = Fc; c <= Lc; c++){

                  word[k] = c;
                  if (findHash(Fc,Lc,word,encryptedWord,solution,messageLength,k+1)){
                    hashIsFound = true;
                  }          

            }
        }
    }

    return hashIsFound;
}

// Comparing the hashed string with the original one
static bool compare(uint128_t check[], uint128_t result[]){
    
    for (int i=0; i < strlen(result); i++){
        
        if (check[i] != result[i]){
            return false;
        }
    }
    return true;
}