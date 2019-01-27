#!/bin/bash
cd "$( dirname "$0" )"

./amalgamate src/*.skeleton

cd src
mv qcustomplot.h ../qcustomplot.h
mv qcustomplot.cpp ../qcustomplot.cpp
cp qcppointer.h ../qcppointer.h
cp qcppointer_impl.h ../qcppointer_impl.h
cp qcplist.h ../qcplist.h
