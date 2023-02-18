#!/bin/bash
cd "$( dirname "$0" )"

../Amalgamate/release/amalgamate src/*.skeleton

cd src
mv qcustomplot.h ../qcustomplot.h
mv qcustomplot.cpp ../qcustomplot.cpp
cp qcplist.h ../qcplist.h
cp qcppointer.h ../qcppointer.h
cp qcppointer_impl.h ../qcppointer_impl.h
