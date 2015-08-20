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

#include "datacontainer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPDataContainer
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPDataContainer
  \brief 
  
*/

/*!
  Constructs a QCPDataContainer used for plottable classes that represent a series of key-sorted
  data
*/
template <class DataType>
QCPDataContainer<DataType>::QCPDataContainer() :
  mAutoSqueeze(true),
  mPreallocSize(0),
  mPreallocIteration(0)
{
}

template <class DataType>
void QCPDataContainer<DataType>::setAutoSqueeze(bool enabled)
{
  if (mAutoSqueeze != enabled)
  {
    mAutoSqueeze = enabled;
    if (mAutoSqueeze)
      performAutoSqueeze();
  }
}

template <class DataType>
void QCPDataContainer<DataType>::set(const QCPDataContainer<DataType> &data)
{
  set(data.mData, true);
}

template <class DataType>
void QCPDataContainer<DataType>::set(const QVector<DataType> &data, bool alreadySorted)
{
  mData = data;
  mPreallocSize = 0;
  mPreallocIteration = 0;
  if (!alreadySorted)
    sort();
}

template <class DataType>
void QCPDataContainer<DataType>::add(const QCPDataContainer<DataType> &data)
{
  if (data.isEmpty())
    return;
  
  const int n = data.size();
  const int oldSize = size();
  
  if (oldSize > 0 && (data.constEnd()-1)->key <= constBegin()->key) // prepend if new data keys are all smaller than existing ones
  {
    if (mPreallocSize < n)
      preallocateGrow(n);
    mPreallocSize -= n;
    std::copy(data.constBegin(), data.constEnd(), begin());
  } else // don't need to prepend, so append and merge if necessary
  {
    mData.resize(oldSize+n);
    std::copy(data.constBegin(), data.constEnd(), end()-n);
    if (oldSize > 0 && !qcpLessThanKey<DataType>(*(constEnd()-n-1), *(constEnd()-n))) // if appended range keys aren't all greater than existing ones, merge the two partitions
      std::inplace_merge(begin(), end()-n, end(), qcpLessThanKey<DataType>);
  }
}

template <class DataType>
void QCPDataContainer<DataType>::add(const QVector<DataType> &data, bool alreadySorted)
{
  if (data.isEmpty())
    return;
  if (isEmpty())
  {
    set(data, alreadySorted);
    return;
  }
  
  const int n = data.size();
  const int oldSize = size();
  
  if (alreadySorted && oldSize > 0 && (data.constEnd()-1)->key <= constBegin()->key) // prepend if new data is sorted and keys are all smaller than existing ones
  {
    if (mPreallocSize < n)
      preallocateGrow(n);
    mPreallocSize -= n;
    std::copy(data.constBegin(), data.constEnd(), begin());
  } else // don't need to prepend, so append and then sort and merge if necessary
  {
    mData.resize(oldSize+n);
    std::copy(data.constBegin(), data.constEnd(), end()-n);
    if (!alreadySorted) // sort appended subrange if it wasn't already sorted
      std::sort(end()-n, end(), qcpLessThanKey<DataType>);
    if (oldSize > 0 && !qcpLessThanKey<DataType>(*(constEnd()-n-1), *(constEnd()-n))) // if appended range keys aren't all greater than existing ones, merge the two partitions
      std::inplace_merge(begin(), end()-n, end(), qcpLessThanKey<DataType>);
  }
}

template <class DataType>
void QCPDataContainer<DataType>::add(const DataType &data)
{
  if (isEmpty() || data.key >= (constEnd()-1)->key) // quickly handle appends
  {
    mData.append(data);
  } else if (data.key < constBegin()->key)  // quickly handle prepends using preallocated space
  {
    if (mPreallocSize < 1)
      preallocateGrow(1);
    --mPreallocSize;
    *begin() = data;
  } else // handle inserts, maintaining sorted keys
  {
    QCPDataContainer<DataType>::iterator insertionPoint = std::lower_bound(begin(), end(), data, qcpLessThanKey<DataType>);
    mData.insert(insertionPoint, data);
  }
}

template <class DataType>
void QCPDataContainer<DataType>::removeBefore(double key)
{
  QCPDataContainer<DataType>::iterator it = begin();
  QCPDataContainer<DataType>::iterator itEnd = std::lower_bound(begin(), end(), DataType(key, 0), qcpLessThanKey<DataType>);
  mPreallocSize += itEnd-it; // don't actually delete, just add it to the preallocated block (if it gets too large, squeeze will take care of it)
  if (mAutoSqueeze)
    performAutoSqueeze();
}

template <class DataType>
void QCPDataContainer<DataType>::removeAfter(double key)
{
  QCPDataContainer<DataType>::iterator it = std::upper_bound(begin(), end(), DataType(key, 0), qcpLessThanKey<DataType>);
  QCPDataContainer<DataType>::iterator itEnd = end();
  mData.erase(it, itEnd); // typically adds it to the postallocated block
  if (mAutoSqueeze)
    performAutoSqueeze();
}

template <class DataType>
void QCPDataContainer<DataType>::remove(double fromKey, double toKey)
{
  if (fromKey >= toKey || isEmpty())
    return;
  
  QCPDataContainer<DataType>::iterator it = std::lower_bound(begin(), end(), DataType(fromKey, 0), qcpLessThanKey<DataType>);
  QCPDataContainer<DataType>::iterator itEnd = std::upper_bound(it, end(), DataType(toKey, 0), qcpLessThanKey<DataType>);
  mData.erase(it, itEnd);
  if (mAutoSqueeze)
    performAutoSqueeze();
}

template <class DataType>
void QCPDataContainer<DataType>::remove(double key)
{
  QCPDataContainer::iterator it = std::lower_bound(begin(), end(), DataType(key, 0), qcpLessThanKey<DataType>);
  if (it != end() && it->key == key)
  {
    if (it == begin())
      ++mPreallocSize; // don't actually delete, just add it to the preallocated block (if it gets too large, squeeze will take care of it)
    else
      mData.erase(it);
  }
  if (mAutoSqueeze)
    performAutoSqueeze();
}

template <class DataType>
void QCPDataContainer<DataType>::clear()
{
  mData.clear();
  mPreallocIteration = 0;
  mPreallocSize = 0;
}

template <class DataType>
void QCPDataContainer<DataType>::sort()
{
  std::sort(begin(), end(), qcpLessThanKey<DataType>);
}

template <class DataType>
void QCPDataContainer<DataType>::squeeze(bool preAllocation, bool postAllocation)
{
  if (preAllocation)
  {
    std::copy(begin(), end(), mData.begin());
    mData.resize(size());
    mPreallocSize = 0;
    mPreallocIteration = 0;
  }
  if (postAllocation)
    mData.squeeze();
}

template <class DataType>
typename QCPDataContainer<DataType>::const_iterator QCPDataContainer<DataType>::findBeginBelowKey(double key) const
{
  if (isEmpty())
    return constEnd();
  
  QCPDataContainer<DataType>::const_iterator it = std::lower_bound(constBegin(), constEnd(), DataType(key, 0), qcpLessThanKey<DataType>);
  if (it != constBegin()) // also covers it == constEnd case, and we know --constEnd is valid because mData isn't empty
    --it;
  return it;
}

template <class DataType>
typename QCPDataContainer<DataType>::const_iterator QCPDataContainer<DataType>::findEndAboveKey(double key) const
{
  if (isEmpty())
    return constEnd();
  
  QCPDataContainer<DataType>::const_iterator it = std::upper_bound(constBegin(), constEnd(), DataType(key, 0), qcpLessThanKey<DataType>);
  if (it != constEnd())
    ++it;
  return it;
}

template <class DataType>
QCPRange QCPDataContainer<DataType>::keyRange(bool &foundRange, QCP::SignDomain signDomain)
{
  if (isEmpty())
  {
    foundRange = false;
    return QCPRange();
  }
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  double current;
  
  QCPDataContainer<DataType>::const_iterator it = constBegin();
  QCPDataContainer<DataType>::const_iterator itEnd = constEnd();
  if (signDomain == QCP::sdBoth) // range may be anywhere, just find first and last non-NaN key (because QCPGraph is sorted by key)
  {
    while (it != itEnd) // find first non-nan going up from left
    {
      if (!qIsNaN(it->value))
      {
        range.lower = it->key;
        haveLower = true;
        break;
      }
      ++it;
    }
    it = itEnd;
    while (it != constBegin()) // find first non-nan going down from right
    {
      --it;
      if (!qIsNaN(it->value))
      {
        range.upper = it->key;
        haveUpper = true;
        break;
      }
    }
  } else if (signDomain == QCP::sdNegative) // range may only be in the negative sign domain
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->value))
      {
        current = it->key;
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (signDomain == QCP::sdPositive) // range may only be in the positive sign domain
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->value))
      {
        current = it->key;
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  }
  
  foundRange = haveLower && haveUpper;
  return range;
}

template <class DataType>
QCPRange QCPDataContainer<DataType>::valueRange(bool &foundRange, QCP::SignDomain signDomain)
{
  if (isEmpty())
  {
    foundRange = false;
    return QCPRange();
  }
  QCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  double current;
  
  QCPDataContainer<DataType>::const_iterator it = constBegin();
  QCPDataContainer<DataType>::const_iterator itEnd = constEnd();
  if (signDomain == QCP::sdBoth) // range may be anywhere
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->value))
      {
        current = it->value;
        if (current < range.lower || !haveLower)
        {
          range.lower = current;
          haveLower = true;
        }
        if (current > range.upper || !haveUpper)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (signDomain == QCP::sdNegative) // range may only be in the negative sign domain
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->value))
      {
        current = it->value;
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (signDomain == QCP::sdPositive) // range may only be in the positive sign domain
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->value))
      {
        current = it->value;
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  }
  
  foundRange = haveLower && haveUpper;
  return range;
}

template <class DataType>
void QCPDataContainer<DataType>::preallocateGrow(int minimumPreallocSize)
{
  if (minimumPreallocSize <= mPreallocSize)
    return;
  
  int newPreallocSize = minimumPreallocSize;
  newPreallocSize += (1u<<qBound(4, mPreallocIteration+4, 15)) - 12; // do 4 up to 32768-12 preallocation, doubling in each intermediate iteration
  ++mPreallocIteration;
  
  int sizeDifference = newPreallocSize-mPreallocSize;
  mData.resize(mData.size()+sizeDifference);
  std::copy_backward(mData.begin()+mPreallocSize, mData.end()-sizeDifference, mData.end());
  mPreallocSize = newPreallocSize;
}

template <class DataType>
void QCPDataContainer<DataType>::performAutoSqueeze()
{
  const int totalAlloc = mData.capacity();
  const int postAllocSize = totalAlloc-mData.size();
  const int usedSize = size();
  bool shrinkPostAllocation = false;
  bool shrinkPreAllocation = false;
  if (totalAlloc > 650000) // if allocation is larger, shrink earlier with respect to total used size
  {
    shrinkPostAllocation = postAllocSize > usedSize*1.5; // QVector grow strategy is 2^n for static data. Watch out not to oscillate!
    shrinkPreAllocation = mPreallocSize*10 > usedSize;
  } else if (totalAlloc > 1000) // below 10 MiB raw data be generous with preallocated memory, below 1k points don't even bother
  {
    shrinkPostAllocation = postAllocSize > usedSize*5;
    shrinkPreAllocation = mPreallocSize > usedSize*1.5; // preallocation can grow into postallocation, so can be smaller
  }
  
  if (shrinkPreAllocation || shrinkPostAllocation)
    squeeze(shrinkPreAllocation, shrinkPostAllocation);
}
