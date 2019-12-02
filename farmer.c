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
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>    
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq

#include "settings.h"
#include "common.h"

#define STUDENT_NAME1        "Natalia"
#define STUDENT_NAME2	     "Hori"

static char                 mq_name1[80];
static char                 mq_name2[80];


static void childproccreate(pid_t childproc[]) {

    // get process IDs from fork()
    for (i = 0; i<NROF_WORKERS; i++) {
        childproc[i] = fork();

        if (childproc[i] < 0)
        {
           perror("fork() failed");
           exit (1);
        }
       else
       {
            if (childproc[i] == 0)
            {
                printf ("child  pid:%d\n", getpid());
                execlp ("ps", "ps", "-l", NULL);

                // we should never arrive here...
                perror ("execlp() failed");
            }
        }

    }

}

int main (int argc, char * argv[])
{
    // check if the user has started this program with valid arguments
    if (argc != 1)
    {
        fprintf (stderr, "%s: %d arguments:\n", argv[0], argc);
        for (i = 1; i < argc; i++)
        {
            fprintf (stderr, "     '%s'\n", argv[i]);
        }
        exit (1);
    }
    // else: parse the arguments...

    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;
    struct mq_attr      attr;
    
        
    //  * create the message queues (see message_queue_test()
    //       in interprocess_basic.c)
    sprintf (mq_name1, "/mq_request_%s_%d", STUDENT_NAME1, getpid());
    sprintf (mq_name2, "/mq_response_%s_%d", STUDENT_NAME2, getpid());

    attr.mq_maxmsg  = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
    mq_fd_request = mq_open (mq_name1, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);
    attr.mq_maxmsg  = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);
    mq_fd_response = mq_open (mq_name2, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

    getattr(mq_fd_request);
    getattr(mq_fd_response);

    //  * create the child processes (see process_test()
    //       and message_queue_test())
    pid_t childproc[NROF_WORKERS];
    childproccreate(childproc);

    //  * do the farming
    //  * declare counting variables
    int allhashes = 0;
    int currentalphabet = 0;


    while (allhashes < MD5_LIST_NROF) {

        //  * get the response message attributes
        mq_getattr(mq_fd_response);

        if (attr.mq_curmsgs != 0) {
            mq_receive(mq_fd_response, (char *)&rsp, sizeof(rsp), NULL);
	    allhashes++;
        }
        
	//  * get the request message attributes
        mq_getattr(mq_fd_request);

        if (attr.mq_curmsgs < MQ_MAX_MESSAGES && currentalphabet < ALPHABET_NROF_CHAR) {
            mq_send(mq_fd_request, (char *)&req, sizeof(req), 0);  
	    currentalphabet++;          
        }

	if (currentalphabet > ALPHABET_NROF_CHAR) {
	    currentalphabet = 0;
	}
        

    }
    



    //  * wait until the chilren have been 
    //      stopped (see process_test())    
    for (i = 0; i<NROF_WORKERS; i++) {
        waitpid (childproc[i], NULL, 0);   // wait for the child
        printf ("child %d has been finished\n\n", childproc[i]);
    }


    //  * clean up the message queues (see message_queue_test())
            mq_close (mq_fd_response);
            mq_close (mq_fd_request);
            mq_unlink (mq_name1);
            mq_unlink (mq_name2);      

    // Important notice: make sure 
    // that the names of the message queues contain your
    // student name and the process id 
    //(to ensure uniqueness during testing)
    
    return (0);
}

