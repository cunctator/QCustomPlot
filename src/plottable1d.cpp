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

#include "plottable1d.h"

#include "painter.h"
#include "core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPAbstractPlottable1D
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPAbstractPlottable1D
  \brief 
  
*/

template <class DataType>
QCPAbstractPlottable1D<DataType>::QCPAbstractPlottable1D(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis),
  mDataContainer(new QCPDataContainer<DataType>)
{
}

template <class DataType>
QCPAbstractPlottable1D<DataType>::~QCPAbstractPlottable1D()
{
}

template <class DataType>
int QCPAbstractPlottable1D<DataType>::dataCount() const
{
  return mDataContainer->size();
}

template <class DataType>
double QCPAbstractPlottable1D<DataType>::dataMainKey(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    return (mDataContainer->constBegin()+index)->mainKey();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return 0;
  }
}

template <class DataType>
double QCPAbstractPlottable1D<DataType>::dataSortKey(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    return (mDataContainer->constBegin()+index)->sortKey();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return 0;
  }
}

template <class DataType>
double QCPAbstractPlottable1D<DataType>::dataMainValue(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    return (mDataContainer->constBegin()+index)->mainValue();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return 0;
  }
}

template <class DataType>
QCPRange QCPAbstractPlottable1D<DataType>::dataValueRange(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    return (mDataContainer->constBegin()+index)->valueRange();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return QCPRange(0, 0);
  }
}

template <class DataType>
QCPDataSelection QCPAbstractPlottable1D<DataType>::selectTestRect(const QRectF &rect) const
{
  QCPDataSelection result;
  
  // convert rect given in pixels to ranges given in plot coordinates:
  double key1, value1, key2, value2;
  pixelsToCoords(rect.topLeft(), key1, value1);
  pixelsToCoords(rect.bottomRight(), key2, value2);
  QCPRange keyRange(key1, key2); // QCPRange normalizes internally so we don't have to care about whether key1 < key2
  QCPRange valueRange(value1, value2);
  
  if (DataType::sortKeyIsMainKey())
  {
    // iterate over data points in key range and add contiguous batches of data to result:
    typename QCPDataContainer<DataType>::const_iterator begin = mDataContainer->findBegin(keyRange.lower, false);
    typename QCPDataContainer<DataType>::const_iterator end = mDataContainer->findEnd(keyRange.upper, false);
    if (begin == end)
      return result;
    
    int currentSegmentBegin = -1; // -1 means we're currently not in a segment that's contained in rect
    for (typename QCPDataContainer<DataType>::const_iterator it=begin; it!=end; ++it)
    {
      if (currentSegmentBegin == -1)
      {
        if (valueRange.contains(it->mainValue())) // start segment
          currentSegmentBegin = it-mDataContainer->constBegin();
      } else if (!valueRange.contains(it->mainValue())) // segment just ended
      {
        result.addDataRange(QCPDataRange(currentSegmentBegin, it-mDataContainer->constBegin()), false);
        currentSegmentBegin = -1;
      }
    }
    // process potential last segment:
    if (currentSegmentBegin != -1)
      result.addDataRange(QCPDataRange(currentSegmentBegin, end-mDataContainer->constBegin()), false);
  } else // we can't assume that data is sorted by main key, so have to use less optimized algorithm which also checks key coordinate
  {
    typename QCPDataContainer<DataType>::const_iterator begin = mDataContainer->constBegin();
    typename QCPDataContainer<DataType>::const_iterator end = mDataContainer->constEnd();
    if (begin == end)
      return result;
    
    int currentSegmentBegin = -1; // -1 means we're currently not in a segment that's contained in rect
    for (typename QCPDataContainer<DataType>::const_iterator it=begin; it!=end; ++it)
    {
      if (currentSegmentBegin == -1)
      {
        if (valueRange.contains(it->mainValue()) && keyRange.contains(it->mainKey())) // start segment
          currentSegmentBegin = it-mDataContainer->constBegin();
      } else if (!valueRange.contains(it->mainValue()) || !keyRange.contains(it->mainKey())) // segment just ended
      {
        result.addDataRange(QCPDataRange(currentSegmentBegin, it-mDataContainer->constBegin()), false);
        currentSegmentBegin = -1;
      }
    }
    // process potential last segment:
    if (currentSegmentBegin != -1)
      result.addDataRange(QCPDataRange(currentSegmentBegin, end-mDataContainer->constBegin()), false);
  }
  
  result.simplify();
  return result;
}

template <class DataType>
void QCPAbstractPlottable1D<DataType>::getDataSegments(QList<QCPDataRange> &selectedSegments, QList<QCPDataRange> &unselectedSegments) const
{
  QCPDataSelection sel(selection());
  sel.simplify();
  selectedSegments = sel.dataRanges();
  
  if (selectedSegments.isEmpty())
  {
    unselectedSegments.append(QCPDataRange(0, dataCount()));
  } else
  {
    // first unselected segment:
    if (selectedSegments.first().begin() != 0)
      unselectedSegments.append(QCPDataRange(0, selectedSegments.first().begin()));
    // intermediate unselected segments:
    for (int i=1; i<selectedSegments.size(); ++i)
      unselectedSegments.append(QCPDataRange(selectedSegments.at(i-1).end(), selectedSegments.at(i).begin()));
    // last unselected segment:
    if (selectedSegments.last().end() != dataCount())
      unselectedSegments.append(QCPDataRange(selectedSegments.last().end(), dataCount()));
  }
}

template <class DataType>
void QCPAbstractPlottable1D<DataType>::drawPolyline(QCPPainter *painter, const QVector<QPointF> &lineData) const
{
  // if drawing solid line and not in PDF, use much faster line drawing instead of polyline:
  if (mParentPlot->plottingHints().testFlag(QCP::phFastPolylines) &&
      painter->pen().style() == Qt::SolidLine &&
      !painter->modes().testFlag(QCPPainter::pmVectorized) &&
      !painter->modes().testFlag(QCPPainter::pmNoCaching))
  {
    int i = 0;
    bool lastIsNan = false;
    const int lineDataSize = lineData.size();
    while (i < lineDataSize && (qIsNaN(lineData.at(i).y()) || qIsNaN(lineData.at(i).x()))) // make sure first point is not NaN
      ++i;
    ++i; // because drawing works in 1 point retrospect
    while (i < lineDataSize)
    {
      if (!qIsNaN(lineData.at(i).y()) && !qIsNaN(lineData.at(i).x())) // NaNs create a gap in the line
      {
        if (!lastIsNan)
          painter->drawLine(lineData.at(i-1), lineData.at(i));
        else
          lastIsNan = false;
      } else
        lastIsNan = true;
      ++i;
    }
  } else
  {
    int segmentStart = 0;
    int i = 0;
    const int lineDataSize = lineData.size();
    while (i < lineDataSize)
    {
      if (qIsNaN(lineData.at(i).y()) || qIsNaN(lineData.at(i).x()) || qIsInf(lineData.at(i).y())) // NaNs create a gap in the line. Also filter Infs which make drawPolyline block
      {
        painter->drawPolyline(lineData.constData()+segmentStart, i-segmentStart); // i, because we don't want to include the current NaN point
        segmentStart = i+1;
      }
      ++i;
    }
    // draw last segment:
    painter->drawPolyline(lineData.constData()+segmentStart, lineDataSize-segmentStart);
  }
}
