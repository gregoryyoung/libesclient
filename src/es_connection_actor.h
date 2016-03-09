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

#ifndef ES_CONNECTION_ACTOR_H_INCLUDED
#define ES_CONNECTION_ACTOR_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _es_connection_actor_t es_connection_actor_t;

//  @interface
//  Create new es_connection_actor actor instance.
//  @TODO: Describe the purpose of this actor!
//
//      zactor_t *es_connection_actor = zactor_new (es_connection_actor, NULL);
//
//  Destroy es_connection_actor instance.
//
//      zactor_destroy (&es_connection_actor);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (es_connection_actor, "VERBOSE");
//
//  Start es_connection_actor actor.
//
//      zstr_sendx (es_connection_actor, "START", NULL);
//
//  Stop es_connection_actor actor.
//
//      zstr_sendx (es_connection_actor, "STOP", NULL);
//
//  This is the es_connection_actor constructor as a zactor_fn;
ES_EXPORT void
    es_connection_actor_actor (zsock_t *pipe, void *args);

//  Self test of this actor
ES_EXPORT void
    es_connection_actor_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
