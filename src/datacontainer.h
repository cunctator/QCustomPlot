/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2015 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 25.04.15                                             **
**          Version: 1.3.1                                                **
****************************************************************************/
/*! \file */
#ifndef QCP_DATACONTAINER_H
#define QCP_DATACONTAINER_H

#include "global.h"
#include "axis/range.h"

template <class DataType>
inline bool qcpLessThanSortKey(const DataType &a, const DataType &b) { return a.sortKey() < b.sortKey(); }

template <class DataType>
class QCP_LIB_DECL QCPDataContainer
{
public:
  typedef typename QVector<DataType>::const_iterator const_iterator;
  typedef typename QVector<DataType>::iterator iterator;
  QCPDataContainer();
  
  void setAutoSqueeze(bool enabled);
  
  int size() const { return mData.size()-mPreallocSize; }
  bool isEmpty() const { return size() == 0; }
  bool autoSqueeze() const { return mAutoSqueeze; }
  
  void set(const QCPDataContainer<DataType> &data);
  void set(const QVector<DataType> &data, bool alreadySorted=false);
  void add(const QCPDataContainer<DataType> &data);
  void add(const QVector<DataType> &data, bool alreadySorted=false);
  void add(const DataType &data);
  void removeBefore(double sortKey);
  void removeAfter(double sortKey);
  void remove(double sortKeyFrom, double sortKeyTo);
  void remove(double sortKey);
  void clear();
  void sort();
  void squeeze(bool preAllocation=true, bool postAllocation=true);
  
  QCPDataContainer::const_iterator constBegin() const { return mData.constBegin()+mPreallocSize; }
  QCPDataContainer::const_iterator constEnd() const { return mData.constEnd(); }
  QCPDataContainer::iterator begin() { return mData.begin()+mPreallocSize; }
  QCPDataContainer::iterator end() { return mData.end(); }
  QCPDataContainer::const_iterator findBeginBelowKey(double key) const;
  QCPDataContainer::const_iterator findEndAboveKey(double key) const;
  QCPRange keyRange(bool &foundRange, QCP::SignDomain signDomain=QCP::sdBoth);
  QCPRange valueRange(bool &foundRange, QCP::SignDomain signDomain=QCP::sdBoth);
  
protected:
  bool mAutoSqueeze;
  
  QVector<DataType> mData;
  int mPreallocSize;
  int mPreallocIteration;
  
  void preallocateGrow(int minimumPreallocSize);
  void performAutoSqueeze();
};

// include implementation in header since it is a class template:
#include "datacontainer.cpp"

#endif // QCP_DATACONTAINER_H
