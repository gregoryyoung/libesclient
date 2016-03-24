/*  =========================================================================
    es_frame - class description

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
    es_frame - 
@discuss
@end
*/

#include "es_classes.h"

//  Structure of our class

struct _es_frame_t {
    int filler;     //  Declare class properties here
};


//  --------------------------------------------------------------------------
//  Create a new es_frame

es_frame_t *
es_frame_new (void)
{
    es_frame_t *self = (es_frame_t *) zmalloc (sizeof (es_frame_t));
    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the es_frame

void
es_frame_destroy (es_frame_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        es_frame_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Self test of this class

void
es_frame_test (bool verbose)
{
    printf (" * es_frame: ");

    //  @selftest
    //  Simple create/destroy test
    es_frame_t *self = es_frame_new ();
    assert (self);
    es_frame_destroy (&self);
    //  @end
    printf ("OK\n");
}
