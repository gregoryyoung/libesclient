/*  =========================================================================
    es_connection - class description

    Copyright (c) the Authors
    =========================================================================
*/

/*
@header
    es_connection - 
@discuss
@end
*/

#include "es_classes.h"

//  Structure of our class

struct _es_connection_t {
    int filler;     //  Declare class properties here
};


//  --------------------------------------------------------------------------
//  Create a new es_connection

es_connection_t *
es_connection_new (void)
{
    es_connection_t *self = (es_connection_t *) zmalloc (sizeof (es_connection_t));
    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the es_connection

void
es_connection_destroy (es_connection_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        es_connection_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Self test of this class

void
es_connection_test (bool verbose)
{
    printf (" * es_connection: ");

    //  @selftest
    //  Simple create/destroy test
    es_connection_t *self = es_connection_new ();
    assert (self);
    es_connection_destroy (&self);
    //  @end
    printf ("OK\n");
}
