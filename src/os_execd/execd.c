/*   $OSSEC, execd.c, v0.2, 2005/11/01, Daniel B. Cid$   */

/* Copyright (C) 2004,2005 Daniel B. Cid <dcid@ossec.net>
 * All right reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */



#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef ARGV0
   #define ARGV0 "ossec-execd"
#endif

#include "shared.h"

#include "os_regex/os_regex.h"
#include "os_net/os_net.h"

#include "execd.h"



/* Internal function */
void OS_Run(int q);


int main(int argc, char **argv)
{
    int c;
    int gid = 0,m_queue = 0;
    char *dir  = DEFAULTDIR;
    char *group = GROUPGLOBAL;
    char *cfg = DEFAULTARPATH;


    while((c = getopt(argc, argv, "dhu:g:D:c:")) != -1){
        switch(c){
            case 'h':
                help();
                break;
            case 'd':
                nowDebug();
                break;
            case 'g':
                if(!optarg)
                    ErrorExit("%s: -g needs an argument",ARGV0);
                group = optarg;
                break;
            case 'D':
                if(!optarg)
                    ErrorExit("%s: -D needs an argument",ARGV0);
                dir = optarg;
            case 'c':
                if(!optarg)
                    ErrorExit("%s: -c needs an argument",ARGV0);
                cfg = optarg;
                break;
            default:
                help();
                break;
        }

    }

    /* Starting daemon */
    verbose(STARTED_MSG,ARGV0);

    /* Check if the group given are valid */
    gid = Privsep_GetGroup(group);
    if(gid < 0)
        ErrorExit(USER_ERROR,ARGV0,"",group);


    /* Privilege separation */	
    if(Privsep_SetGroup(gid) < 0)
        ErrorExit(SETGID_ERROR,ARGV0,group);


    /* Starting queue (exec queue) */
    if((m_queue = StartMQ(EXECQUEUE,READ)) < 0)
        ErrorExit(QUEUE_ERROR,ARGV0,EXECQUEUE);


    /* Signal manipulation */
    StartSIG(ARGV0);


    /* Going daemon */
    nowDaemon();
    goDaemon();

    
    /* Creating the PID file */
    if(CreatePID(ARGV0, getpid()) < 0)
        merror(PID_ERROR,ARGV0);


    /* The real daemon Now */	
    OS_Run(m_queue);
    
    exit(0);
}


/* OS_Run: Read the queue and send the appropriate alerts */
void OS_Run(int q)
{
    char buffer[OS_MAXSTR + 1];

    memset(buffer, '\0', OS_MAXSTR +1);
    
    while(1)
    {
        if(recv(q, buffer, OS_MAXSTR, 0) == -1)
        {
            merror(QUEUE_ERROR, ARGV0, EXECQUEUE);
            continue;
        }

        merror("received: %s\n", buffer);
    }
}

/* EOF */
