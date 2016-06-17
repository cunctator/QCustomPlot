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

#include "selection.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPDataRange
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPDataRange
  \brief
  

  TODO: mention or \see addition/subtraction operator for QCPDataRanges defined in QCPDataSelection
*/

QCPDataRange::QCPDataRange() :
  mBegin(0),
  mEnd(0)
{
}

QCPDataRange::QCPDataRange(int begin, int end) :
  mBegin(begin),
  mEnd(end)
{
}

QCPDataRange QCPDataRange::bounded(const QCPDataRange &other) const
{
  QCPDataRange result(intersection(other));
  if (result.isEmpty()) // no intersection, preserve respective bounding side of otherRange as both begin and end of return value
  {
    if (mEnd <= other.mBegin)
      result = QCPDataRange(other.mBegin, other.mBegin);
    else
      result = QCPDataRange(other.mEnd, other.mEnd);
  }
  return result;
}

QCPDataRange QCPDataRange::intersection(const QCPDataRange &other) const
{
  QCPDataRange result(qMax(mBegin, other.mBegin), qMin(mEnd, other.mEnd));
  if (result.isValid())
    return result;
  else
    return QCPDataRange();
}

bool QCPDataRange::intersects(const QCPDataRange &other) const
{
   return !( (mBegin > other.mBegin && mBegin >= other.mEnd) ||
             (mEnd <= other.mBegin && mEnd < other.mEnd) );
}

bool QCPDataRange::contains(const QCPDataRange &other) const
{
  return mBegin <= other.mBegin && mEnd >= other.mEnd;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPDataSelection
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPDataSelection
  \brief
  

*/

QCPDataSelection::QCPDataSelection()
{
}

QCPDataSelection::QCPDataSelection(const QCPDataRange &range)
{
  mDataRanges.append(range);
}

bool QCPDataSelection::operator==(const QCPDataSelection &other) const
{
  if (mDataRanges.size() != other.mDataRanges.size())
    return false;
  for (int i=0; i<mDataRanges.size(); ++i)
  {
    if (mDataRanges.at(i) != other.mDataRanges.at(i))
      return false;
  }
  return true;
}

QCPDataSelection &QCPDataSelection::operator+=(const QCPDataSelection &other)
{
  mDataRanges << other.mDataRanges;
  simplify();
  return *this;
}

QCPDataSelection &QCPDataSelection::operator+=(const QCPDataRange &other)
{
  addDataRange(other);
  return *this;
}

QCPDataSelection &QCPDataSelection::operator-=(const QCPDataSelection &other)
{
  for (int i=0; i<other.dataRangeCount(); ++i)
    *this -= other.dataRange(i);
  
  return *this;
}

QCPDataSelection &QCPDataSelection::operator-=(const QCPDataRange &other)
{
  if (other.isEmpty() || isEmpty())
    return *this;
  
  simplify();
  int i=0;
  while (i < mDataRanges.size())
  {
    const int thisBegin = mDataRanges.at(i).begin();
    const int thisEnd = mDataRanges.at(i).end();
    if (thisBegin >= other.end())
      break; // since data ranges are sorted after the simplify() call, no ranges which contain other will come after this
    
    if (thisEnd > other.begin()) // ranges which don't fulfill this are entirely before other and can be ignored
    {
      if (thisBegin >= other.begin()) // range leading segment is encompassed
      {
        if (thisEnd <= other.end()) // range fully encompassed, remove completely
        {
          mDataRanges.removeAt(i);
          continue;
        } else // only leading segment is encompassed, trim accordingly
          mDataRanges[i].setBegin(other.end());
      } else // leading segment is not encompassed
      {
        if (thisEnd <= other.end()) // only trailing segment is encompassed, trim accordingly
        {
          mDataRanges[i].setEnd(other.begin());
        } else // other lies inside this range, so split range
        {
          mDataRanges[i].setEnd(other.begin());
          mDataRanges.insert(i+1, QCPDataRange(other.end(), thisEnd));
          break; // since data ranges are sorted (and don't overlap) after simplify() call, we're done here
        }
      }
    }
    ++i;
  }
  
  return *this;
}

int QCPDataSelection::dataPointCount() const
{
  int result = 0;
  for (int i=0; i<mDataRanges.size(); ++i)
    result += mDataRanges.at(i).length();
  return result;
}

QCPDataRange QCPDataSelection::dataRange(int index) const
{
  if (index >= 0 && index < mDataRanges.size())
  {
    return mDataRanges.at(index);
  } else
  {
    qDebug() << Q_FUNC_INFO << "index out of range:" << index;
    return QCPDataRange();
  }
}

/*
  Same as += operator but allows disabling immediate simplification which can improve performance if
  adding a very large amount of data ranges successively.
*/

void QCPDataSelection::addDataRange(const QCPDataRange &dataRange, bool simplify)
{
  mDataRanges.append(dataRange);
  if (simplify)
    this->simplify();
}

void QCPDataSelection::clear()
{
  mDataRanges.clear();
}

void QCPDataSelection::simplify()
{
  // remove any empty ranges:
  for (int i=mDataRanges.size()-1; i>=0; --i)
  {
    if (mDataRanges.at(i).isEmpty())
      mDataRanges.removeAt(i);
  }
  if (mDataRanges.isEmpty())
    return;
  
  // sort ranges by starting value, ascending:
  std::sort(mDataRanges.begin(), mDataRanges.end(), lessThanDataRangeBegin);
  
  // join overlapping/contiguous ranges:
  int i = 1;
  while (i < mDataRanges.size())
  {
    if (mDataRanges.at(i-1).end() >= mDataRanges.at(i).begin()) // range i overlaps/joins with i-1, so expand range i-1 appropriately and remove range i from list
    {
      mDataRanges[i-1].setEnd(qMax(mDataRanges.at(i-1).end(), mDataRanges.at(i).end()));
      mDataRanges.removeAt(i);
    } else
      ++i;
  }
}

void QCPDataSelection::enforceType(QCP::SelectionType type)
{
  simplify();
  switch (type)
  {
    case QCP::stNone:
    {
      mDataRanges.clear();
      break;
    }
    case QCP::stWhole:
    {
      // whole selection isn't defined by data range, so don't change anything
      // TODO: really? (if we were to enforce whole here, QCPDataSelection would need to know the data size and keep it updated)
      break;
    }
    case QCP::stSingleData:
    {
      // reduce all data ranges to the single first data point:
      if (!mDataRanges.isEmpty())
      {
        if (mDataRanges.size() > 1)
          mDataRanges = QList<QCPDataRange>() << mDataRanges.first();
        if (mDataRanges.first().length() > 1)
          mDataRanges.first().setEnd(mDataRanges.first().begin()+1);
      }
      break;
    }
    case QCP::stDataRange:
    {
      if (mDataRanges.size() > 1)
      {
        QCPDataRange newDataRange;
        newDataRange.setBegin(mDataRanges.first().begin());
        newDataRange.setEnd(mDataRanges.last().end());
        mDataRanges = QList<QCPDataRange>() << newDataRange;
      }
      break;
    }
    case QCP::stMultipleDataRanges:
    {
      // this is the selection type that allows all concievable combinations of ranges, so do nothing
      break;
    }
  }
}

bool QCPDataSelection::contains(const QCPDataSelection &other) const
{
  if (other.isEmpty()) return false;
  
  int otherIndex = 0;
  int thisIndex = 0;
  while (thisIndex < mDataRanges.size() && otherIndex < other.mDataRanges.size())
  {
    if (mDataRanges.at(thisIndex).contains(other.mDataRanges.at(otherIndex)))
      ++otherIndex;
    else
      ++thisIndex;
  }
  return thisIndex < mDataRanges.size(); // if thisIndex ran all the way to the end to find a containing range for the current otherIndex, other is not contained in this
}




