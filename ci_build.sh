#!/usr/bin/env bash

git clean
git reset --hard HEAD
#./autogen.sh 2> /dev/null
#./configure --enable-drafts=no "${CONFIG_OPTS[@]}"
make -j4
#make check
make install
