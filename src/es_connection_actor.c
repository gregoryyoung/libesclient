/*  =========================================================================
    es_connection_actor - 

    The MIT License (MIT)                                                         
                                                                                  
    Copyright (c) 2016 Event Store LLP                                            
                                                                                  
    Permission is hereby granted, free of charge, to any person obtaining a copy  
    of this software and associated documentation files (the "Software"), to deal 
    in the Software without restriction, including without limitation the rights  
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell     
    copies of the Software, and to permit persons to whom the Software is         
    furnished to do so, subject to the following conditions:                      
                                                                                  
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.                               
                                                                                  
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE   
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER        
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
    SOFTWARE.                                                                     
    =========================================================================
*/

/*
@header
    es_connection_actor - 
@discuss
@end
*/

#include "es_classes.h"

//  Structure of our actor

struct _es_connection_actor_t {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
    //  TODO: Declare properties
};


//  --------------------------------------------------------------------------
//  Create a new es_connection_actor instance

static es_connection_actor_t *
es_connection_actor_new (zsock_t *pipe, void *args)
{
    es_connection_actor_t *self = (es_connection_actor_t *) zmalloc (sizeof (es_connection_actor_t));
    assert (self);

    self->pipe = pipe;
    self->terminated = false;
    self->poller = zpoller_new (self->pipe, NULL);

    //  TODO: Initialize properties

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the es_connection_actor instance

static void
es_connection_actor_destroy (es_connection_actor_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        es_connection_actor_t *self = *self_p;

        //  TODO: Free actor properties

        //  Free object itself
        zpoller_destroy (&self->poller);
        free (self);
        *self_p = NULL;
    }
}


//  Start this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
es_connection_actor_start (es_connection_actor_t *self)
{
    assert (self);

    //  TODO: Add startup actions

    return 0;
}


//  Stop this actor. Return a value greater or equal to zero if initialization
//  was successful. Otherwise -1.

static int
es_connection_actor_stop (es_connection_actor_t *self)
{
    assert (self);

    //  TODO: Add shutdown actions

    return 0;
}


//  Here we handle incoming message from the node

static void
es_connection_actor_recv_api (es_connection_actor_t *self)
{
    //  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return;        //  Interrupted

    char *command = zmsg_popstr (request);
    if (streq (command, "START"))
        es_connection_actor_start (self);
    else
    if (streq (command, "STOP"))
        es_connection_actor_stop (self);
    else
    if (streq (command, "VERBOSE"))
        self->verbose = true;
    else
    if (streq (command, "$TERM"))
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
}


//  --------------------------------------------------------------------------
//  This is the actor which runs in its own thread.

void
es_connection_actor_actor (zsock_t *pipe, void *args)
{
    es_connection_actor_t * self = es_connection_actor_new (pipe, args);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, 0);
        if (which == self->pipe)
            es_connection_actor_recv_api (self);
       //  Add other sockets when you need them.
    }
    es_connection_actor_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Self test of this actor.

void
es_connection_actor_test (bool verbose)
{
    printf (" * es_connection_actor: ");

    int rc = 0;
    //  @selftest
    //  Simple create/destroy test
    zactor_t *es_connection_actor = zactor_new (es_connection_actor_actor, NULL);

    zstr_send (es_connection_actor, "START");
    rc = zsock_wait (es_connection_actor);         //  Wait until actor started
    assert (rc == 0);

    zstr_send (es_connection_actor, "STOP");
    rc = zsock_wait (es_connection_actor);         //  Wait until actor stopped
    assert (rc == 0);

    zactor_destroy (&es_connection_actor);
    //  @end

    printf ("OK\n");
}
