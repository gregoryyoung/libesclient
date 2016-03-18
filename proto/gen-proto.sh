#/bin/sh

protoc-c --c_out=. ClientMessageDtos.proto
mv ClientMessageDtos.pb-c.c ../src/es-proto.c
mv ClientMessageDtos.pb-c.h ../src/es-proto.h
