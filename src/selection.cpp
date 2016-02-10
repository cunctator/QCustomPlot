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
  

*/

QCPDataRange::QCPDataRange() :
  mBegin(0),
  mEnd(0)
{
}

void QCPDataRange::setBegin(int begin)
{
  mBegin = begin;
}

void QCPDataRange::setEnd(int end)
{
  mEnd = end;
}

bool QCPDataRange::isValid() const
{
  return (mEnd >= mBegin) && (mBegin >= 0);
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

void QCPDataSelection::enforceType(SelectionType type)
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
  
  // enforce type:
  switch (type)
  {
    case QCPDataSelection::stWhole:
    {
      // whole selection isn't defined by data range, so don't change anything
      // TODO: really? (if we were to enforce whole here, QCPDataSelection would need to know the data size and keep it updated)
      break;
    }
    case QCPDataSelection::stSingleData:
    {
      // reduce all data ranges to the single first data point:
      if (mDataRanges.size() > 1)
        mDataRanges = QList<QCPDataRange>() << mDataRanges.first();
      if (mDataRanges.first().length() > 1)
        mDataRanges.first().setEnd(mDataRanges.first().begin()+1);
      break;
    }
    case QCPDataSelection::stDataRange:
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
    case QCPDataSelection::stMultipleDataRanges:
    {
      // this is the selection type that allows all concievable combinations of ranges, so do nothing
      break;
    }
  }
}




