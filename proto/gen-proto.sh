#/bin/sh

protoc-c --c_out=. ClientMessageDtos.proto
mv ClientMessageDtos.pb-c.c ../src/es_proto.c
mv ClientMessageDtos.pb-c.h ../src/es_proto.h
