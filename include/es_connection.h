/*  =========================================================================
    es_connection - class description

    Copyright (c) the Authors
    =========================================================================
*/

#ifndef ES_CONNECTION_H_INCLUDED
#define ES_CONNECTION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new es_connection
ES_EXPORT es_connection_t *
    es_connection_new (void);

//  Destroy the es_connection
ES_EXPORT void
    es_connection_destroy (es_connection_t **self_p);

//  Self test of this class
ES_EXPORT void
    es_connection_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
