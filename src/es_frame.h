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

#ifndef ES_FRAME_H_INCLUDED
#define ES_FRAME_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _es_frame_t es_frame_t;

//  @interface
//  Create a new es_frame
ES_EXPORT es_frame_t *
    es_frame_new (void);

//  Destroy the es_frame
ES_EXPORT void
    es_frame_destroy (es_frame_t **self_p);

//  Self test of this class
ES_EXPORT void
    es_frame_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
