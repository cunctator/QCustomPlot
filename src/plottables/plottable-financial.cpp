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

#include "plottable-financial.h"

#include "../painter.h"
#include "../core.h"
#include "../axis/axis.h"
#include "../layoutelements/layoutelement-axisrect.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPFinancialData
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPFinancialData
  \brief Holds the data of one single data point for QCPFinancial.
  
  The stored data is:
  \li \a key: coordinate on the key axis of this data point (this is the \a mainKey and the \a sortKey)
  \li \a open: The opening value at the data point (this is the \a mainValue)
  \li \a high: The high/maximum value at the data point
  \li \a low: The low/minimum value at the data point
  \li \a close: The closing value at the data point
  
  The container for storing multiple data points is \ref QCPFinancialDataContainer. It is a typedef
  for \ref QCPDataContainer with \ref QCPFinancialData as the DataType template parameter. See the
  documentation there for an explanation regarding the data type's generic methods.
  
  \see QCPFinancialDataContainer
*/

/* start documentation of inline functions */

/*! \fn double QCPFinancialData::sortKey() const
  
  Returns the \a key member of this data point.
  
  For a general explanation of what this method is good for in the context of the data container,
  see the documentation of \ref QCPDataContainer.
*/

/*! \fn static QCPFinancialData QCPFinancialData::fromSortKey(double sortKey)
  
  Returns a data point with the specified \a sortKey. All other members are set to zero.
  
  For a general explanation of what this method is good for in the context of the data container,
  see the documentation of \ref QCPDataContainer.
*/

/*! \fn static static bool QCPFinancialData::sortKeyIsMainKey()
  
  Since the member \a key is both the data point key coordinate and the data ordering parameter,
  this method returns true.
  
  For a general explanation of what this method is good for in the context of the data container,
  see the documentation of \ref QCPDataContainer.
*/

/*! \fn double QCPFinancialData::mainKey() const
  
  Returns the \a key member of this data point.
  
  For a general explanation of what this method is good for in the context of the data container,
  see the documentation of \ref QCPDataContainer.
*/

/*! \fn double QCPFinancialData::mainValue() const
  
  Returns the \a open member of this data point.
  
  For a general explanation of what this method is good for in the context of the data container,
  see the documentation of \ref QCPDataContainer.
*/

/*! \fn QCPRange QCPFinancialData::valueRange() const
  
  Returns a QCPRange spanning from the \a low to the \a high value of this data point.
  
  For a general explanation of what this method is good for in the context of the data container,
  see the documentation of \ref QCPDataContainer.
*/

/* end documentation of inline functions */

/*!
  Constructs a data point with key and all values set to zero.
*/
QCPFinancialData::QCPFinancialData() :
  key(0),
  open(0),
  high(0),
  low(0),
  close(0)
{
}

/*!
  Constructs a data point with the specified \a key and OHLC values.
*/
QCPFinancialData::QCPFinancialData(double key, double open, double high, double low, double close) :
  key(key),
  open(open),
  high(high),
  low(low),
  close(close)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPFinancial
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPFinancial
  \brief A plottable representing a financial stock chart

  \image html QCPFinancial.png
  
  This plottable represents time series data binned to certain intervals, mainly used for stock
  charts. The two common representations OHLC (Open-High-Low-Close) bars and Candlesticks can be
  set via \ref setChartStyle.
  
  The data is passed via \ref setData as a set of open/high/low/close values at certain keys
  (typically times). This means the data must be already binned appropriately. If data is only
  available as a series of values (e.g. \a price against \a time), you can use the static
  convenience function \ref timeSeriesToOhlc to generate binned OHLC-data which can then be passed
  to \ref setData.
  
  The width of the OHLC bars/candlesticks can be controlled with \ref setWidth and is given in plot
  key coordinates. A typical choice is to set it to (or slightly less than) one bin interval width.
  
  \section appearance Changing the appearance
  
  Charts can be either single- or two-colored (\ref setTwoColored). If set to be single-colored,
  lines are drawn with the plottable's pen (\ref setPen) and fills with the brush (\ref setBrush).
  
  If set to two-colored, positive changes of the value during an interval (\a close >= \a open) are
  represented with a different pen and brush than negative changes (\a close < \a open). These can
  be configured with \ref setPenPositive, \ref setPenNegative, \ref setBrushPositive, and \ref
  setBrushNegative. In two-colored mode, the normal plottable pen/brush is ignored. Upon selection
  however, the normal selected pen/brush (\ref setSelectedPen, \ref setSelectedBrush) is used,
  irrespective of whether the chart is single- or two-colored.
  
  \section usage Usage
  
  Like all data representing objects in QCustomPlot, the QCPFinancial is a plottable
  (QCPAbstractPlottable). So the plottable-interface of QCustomPlot applies
  (QCustomPlot::plottable, QCustomPlot::removePlottable, etc.)
  
  Usually, you first create an instance:
  
  \snippet documentation/doc-code-snippets/mainwindow.cpp qcpfinancial-creation-1
  which registers it with the QCustomPlot instance of the passed axes. Note that this QCustomPlot
  instance takes ownership of the plottable, so do not delete it manually but use
  QCustomPlot::removePlottable() instead. The newly created plottable can be modified, e.g.:
  
  \snippet documentation/doc-code-snippets/mainwindow.cpp qcpfinancial-creation-2
  Here we have used the static helper method \ref timeSeriesToOhlc, to turn a time-price data
  series into a 24-hour binned open-high-low-close data series as QCPFinancial uses.
*/

/* start of documentation of inline functions */

/*! \fn QCPFinancialDataContainer *QCPFinancial::data() const
  
  Returns a pointer to the internal data storage of type \ref QCPFinancialDataContainer. You may
  use it to directly manipulate the data, which may be more convenient and faster than using the
  regular \ref setData or \ref addData methods, in certain situations.
*/

/* end of documentation of inline functions */

/*!
  Constructs a financial chart which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and not have
  the same orientation. If either of these restrictions is violated, a corresponding message is
  printed to the debug output (qDebug), the construction is not aborted, though.
  
  The created QCPFinancial is automatically registered with the QCustomPlot instance inferred from \a
  keyAxis. This QCustomPlot instance takes ownership of the QCPFinancial, so do not delete it manually
  but use QCustomPlot::removePlottable() instead.
*/
QCPFinancial::QCPFinancial(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis),
  mDataContainer(new QCPFinancialDataContainer),
  mChartStyle(csOhlc),
  mWidth(0.5),
  mTwoColored(false),
  mBrushPositive(QBrush(QColor(210, 210, 255))),
  mBrushNegative(QBrush(QColor(255, 210, 210))),
  mPenPositive(QPen(QColor(10, 40, 180))),
  mPenNegative(QPen(QColor(180, 40, 10)))
{
  setSelectedPen(QPen(QColor(80, 80, 255), 2.5));
  setSelectedBrush(QBrush(QColor(80, 80, 255)));
}

QCPFinancial::~QCPFinancial()
{
}

/*!
  TODO
  \see timeSeriesToOhlc
*/

void QCPFinancial::setData(QSharedPointer<QCPFinancialDataContainer> data)
{
  mDataContainer = data;
}

/*! \overload
  
  Replaces the current data with the provided open/high/low/close data. The provided vectors should
  have equal length. Else, the number of added points will be the size of the smallest vector.
  
  \see timeSeriesToOhlc
*/
void QCPFinancial::setData(const QVector<double> &keys, const QVector<double> &open, const QVector<double> &high, const QVector<double> &low, const QVector<double> &close, bool alreadySorted)
{
  mDataContainer->clear();
  addData(keys, open, high, low, close, alreadySorted);
}

/*!
  Sets which representation style shall be used to display the OHLC data.
*/
void QCPFinancial::setChartStyle(QCPFinancial::ChartStyle style)
{
  mChartStyle = style;
}

/*!
  Sets the width of the individual bars/candlesticks to \a width in plot key coordinates.
  
  A typical choice is to set it to (or slightly less than) one bin interval width.
*/
void QCPFinancial::setWidth(double width)
{
  mWidth = width;
}

/*!
  Sets whether this chart shall contrast positive from negative trends per data point by using two
  separate colors to draw the respective bars/candlesticks.
  
  If \a twoColored is false, the normal plottable's pen and brush are used (\ref setPen, \ref
  setBrush).
  
  \see setPenPositive, setPenNegative, setBrushPositive, setBrushNegative
*/
void QCPFinancial::setTwoColored(bool twoColored)
{
  mTwoColored = twoColored;
}

/*!
  If \ref setTwoColored is set to true, this function controls the brush that is used to draw fills
  of data points with a positive trend (i.e. bars/candlesticks with close >= open).
  
  If \a twoColored is false, the normal plottable's pen and brush are used (\ref setPen, \ref
  setBrush).
  
  \see setBrushNegative, setPenPositive, setPenNegative
*/
void QCPFinancial::setBrushPositive(const QBrush &brush)
{
  mBrushPositive = brush;
}

/*!
  If \ref setTwoColored is set to true, this function controls the brush that is used to draw fills
  of data points with a negative trend (i.e. bars/candlesticks with close < open).
  
  If \a twoColored is false, the normal plottable's pen and brush are used (\ref setPen, \ref
  setBrush).
  
  \see setBrushPositive, setPenNegative, setPenPositive
*/
void QCPFinancial::setBrushNegative(const QBrush &brush)
{
  mBrushNegative = brush;
}

/*!
  If \ref setTwoColored is set to true, this function controls the pen that is used to draw
  outlines of data points with a positive trend (i.e. bars/candlesticks with close >= open).
  
  If \a twoColored is false, the normal plottable's pen and brush are used (\ref setPen, \ref
  setBrush).
  
  \see setPenNegative, setBrushPositive, setBrushNegative
*/
void QCPFinancial::setPenPositive(const QPen &pen)
{
  mPenPositive = pen;
}

/*!
  If \ref setTwoColored is set to true, this function controls the pen that is used to draw
  outlines of data points with a negative trend (i.e. bars/candlesticks with close < open).
  
  If \a twoColored is false, the normal plottable's pen and brush are used (\ref setPen, \ref
  setBrush).
  
  \see setPenPositive, setBrushNegative, setBrushPositive
*/
void QCPFinancial::setPenNegative(const QPen &pen)
{
  mPenNegative = pen;
}

/*! \overload
  
  Adds the provided open/high/low/close data to the current data.
  
  Alternatively, you can also access and modify the data via the \ref data method, which returns a
  pointer to the internal \ref QCPFinancialDataContainer.
*/
void QCPFinancial::addData(const QVector<double> &keys, const QVector<double> &open, const QVector<double> &high, const QVector<double> &low, const QVector<double> &close, bool alreadySorted)
{
  if (keys.size() != open.size() || open.size() != high.size() || high.size() != low.size() || low.size() != close.size() || close.size() != keys.size())
    qDebug() << Q_FUNC_INFO << "keys, open, high, low, close have different sizes:" << keys.size() << open.size() << high.size() << low.size() << close.size();
  const int n = qMin(keys.size(), qMin(open.size(), qMin(high.size(), qMin(low.size(), close.size()))));
  QVector<QCPFinancialData> tempData(n);
  QVector<QCPFinancialData>::iterator it = tempData.begin();
  const QVector<QCPFinancialData>::iterator itEnd = tempData.end();
  int i = 0;
  while (it != itEnd)
  {
    it->key = keys[i];
    it->open = open[i];
    it->high = high[i];
    it->low = low[i];
    it->close = close[i];
    ++it;
    ++i;
  }
  mDataContainer->add(tempData, alreadySorted); // don't modify tempData beyond this to prevent copy on write
}

/*! \overload
  
  Adds the provided single data point given by \a key, \a open, \a high, \a low, and \a close to
  the current data.
  
  Alternatively, you can also access and modify the data via the \ref data method, which returns a
  pointer to the internal \ref QCPFinancialDataContainer.
*/
void QCPFinancial::addData(double key, double open, double high, double low, double close)
{
  mDataContainer->add(QCPFinancialData(key, open, high, low, close));
}

/* inherits documentation from base class */
double QCPFinancial::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
  Q_UNUSED(details)
  if (onlySelectable && !mSelectable)
    return -1;
  if (!mKeyAxis || !mValueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return -1; }
  
  if (mKeyAxis.data()->axisRect()->rect().contains(pos.toPoint()))
  {
    // get visible data range:
    QCPFinancialDataContainer::const_iterator lower, upperEnd;
    getVisibleDataBounds(lower, upperEnd);
    if (lower == upperEnd)
      return -1;
    // perform select test according to configured style:
    switch (mChartStyle)
    {
      case QCPFinancial::csOhlc:
        return ohlcSelectTest(pos, lower, upperEnd); break;
      case QCPFinancial::csCandlestick:
        return candlestickSelectTest(pos, lower, upperEnd); break;
    }
  }
  return -1;
}

/*!
  A convenience function that converts time series data (\a value against \a time) to OHLC binned
  data points. The return value can then be passed on to \ref QCPFinancialDataContainer::set(const
  QCPFinancialDataContainer&).
  
  The size of the bins can be controlled with \a timeBinSize in the same units as \a time is given.
  For example, if the unit of \a time is seconds and single OHLC/Candlesticks should span an hour
  each, set \a timeBinSize to 3600.
  
  \a timeBinOffset allows to control precisely at what \a time coordinate a bin should start. The
  value passed as \a timeBinOffset doesn't need to be in the range encompassed by the \a time keys.
  It merely defines the mathematical offset/phase of the bins that will be used to process the
  data.
*/
QCPFinancialDataContainer QCPFinancial::timeSeriesToOhlc(const QVector<double> &time, const QVector<double> &value, double timeBinSize, double timeBinOffset)
{
  QCPFinancialDataContainer data;
  int count = qMin(time.size(), value.size());
  if (count == 0)
    return QCPFinancialDataContainer();
  
  QCPFinancialData currentBinData(0, value.first(), value.first(), value.first(), value.first());
  int currentBinIndex = qFloor((time.first()-timeBinOffset)/timeBinSize+0.5);
  for (int i=0; i<count; ++i)
  {
    int index = qFloor((time.at(i)-timeBinOffset)/timeBinSize+0.5);
    if (currentBinIndex == index) // data point still in current bin, extend high/low:
    {
      if (value.at(i) < currentBinData.low) currentBinData.low = value.at(i);
      if (value.at(i) > currentBinData.high) currentBinData.high = value.at(i);
      if (i == count-1) // last data point is in current bin, finalize bin:
      {
        currentBinData.close = value.at(i);
        currentBinData.key = timeBinOffset+(index)*timeBinSize;
        data.add(currentBinData);
      }
    } else // data point not anymore in current bin, set close of old and open of new bin, and add old to map:
    {
      // finalize current bin:
      currentBinData.close = value.at(i-1);
      currentBinData.key = timeBinOffset+(index-1)*timeBinSize;
      data.add(currentBinData);
      // start next bin:
      currentBinIndex = index;
      currentBinData.open = value.at(i);
      currentBinData.high = value.at(i);
      currentBinData.low = value.at(i);
    }
  }
  
  return data;
}

/* inherits documentation from base class */
void QCPFinancial::draw(QCPPainter *painter)
{
  // get visible data range:
  QCPFinancialDataContainer::const_iterator lower, upperEnd;
  getVisibleDataBounds(lower, upperEnd);
  
  // draw visible data range according to configured style:
  switch (mChartStyle)
  {
    case QCPFinancial::csOhlc:
      drawOhlcPlot(painter, lower, upperEnd); break;
    case QCPFinancial::csCandlestick:
      drawCandlestickPlot(painter, lower, upperEnd); break;
  }
}

/* inherits documentation from base class */
void QCPFinancial::drawLegendIcon(QCPPainter *painter, const QRectF &rect) const
{
  painter->setAntialiasing(false); // legend icon especially of csCandlestick looks better without antialiasing
  if (mChartStyle == csOhlc)
  {
    if (mTwoColored)
    {
      // draw upper left half icon with positive color:
      painter->setBrush(mBrushPositive);
      painter->setPen(mPenPositive);
      painter->setClipRegion(QRegion(QPolygon() << rect.bottomLeft().toPoint() << rect.topRight().toPoint() << rect.topLeft().toPoint()));
      painter->drawLine(QLineF(0, rect.height()*0.5, rect.width(), rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.2, rect.height()*0.3, rect.width()*0.2, rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.8, rect.height()*0.5, rect.width()*0.8, rect.height()*0.7).translated(rect.topLeft()));
      // draw bottom right hald icon with negative color:
      painter->setBrush(mBrushNegative);
      painter->setPen(mPenNegative);
      painter->setClipRegion(QRegion(QPolygon() << rect.bottomLeft().toPoint() << rect.topRight().toPoint() << rect.bottomRight().toPoint()));
      painter->drawLine(QLineF(0, rect.height()*0.5, rect.width(), rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.2, rect.height()*0.3, rect.width()*0.2, rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.8, rect.height()*0.5, rect.width()*0.8, rect.height()*0.7).translated(rect.topLeft()));
    } else
    {
      painter->setBrush(mBrush);
      painter->setPen(mPen);
      painter->drawLine(QLineF(0, rect.height()*0.5, rect.width(), rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.2, rect.height()*0.3, rect.width()*0.2, rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.8, rect.height()*0.5, rect.width()*0.8, rect.height()*0.7).translated(rect.topLeft()));
    }
  } else if (mChartStyle == csCandlestick)
  {
    if (mTwoColored)
    {
      // draw upper left half icon with positive color:
      painter->setBrush(mBrushPositive);
      painter->setPen(mPenPositive);
      painter->setClipRegion(QRegion(QPolygon() << rect.bottomLeft().toPoint() << rect.topRight().toPoint() << rect.topLeft().toPoint()));
      painter->drawLine(QLineF(0, rect.height()*0.5, rect.width()*0.25, rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.75, rect.height()*0.5, rect.width(), rect.height()*0.5).translated(rect.topLeft()));
      painter->drawRect(QRectF(rect.width()*0.25, rect.height()*0.25, rect.width()*0.5, rect.height()*0.5).translated(rect.topLeft()));
      // draw bottom right hald icon with negative color:
      painter->setBrush(mBrushNegative);
      painter->setPen(mPenNegative);
      painter->setClipRegion(QRegion(QPolygon() << rect.bottomLeft().toPoint() << rect.topRight().toPoint() << rect.bottomRight().toPoint()));
      painter->drawLine(QLineF(0, rect.height()*0.5, rect.width()*0.25, rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.75, rect.height()*0.5, rect.width(), rect.height()*0.5).translated(rect.topLeft()));
      painter->drawRect(QRectF(rect.width()*0.25, rect.height()*0.25, rect.width()*0.5, rect.height()*0.5).translated(rect.topLeft()));
    } else
    {
      painter->setBrush(mBrush);
      painter->setPen(mPen);
      painter->drawLine(QLineF(0, rect.height()*0.5, rect.width()*0.25, rect.height()*0.5).translated(rect.topLeft()));
      painter->drawLine(QLineF(rect.width()*0.75, rect.height()*0.5, rect.width(), rect.height()*0.5).translated(rect.topLeft()));
      painter->drawRect(QRectF(rect.width()*0.25, rect.height()*0.25, rect.width()*0.5, rect.height()*0.5).translated(rect.topLeft()));
    }
  }
}

/* inherits documentation from base class */
QCPRange QCPFinancial::getKeyRange(bool &foundRange, QCP::SignDomain inSignDomain) const
{
  QCPRange range = mDataContainer->keyRange(foundRange, inSignDomain);
  // determine exact range by including width of bars/flags:
  if (foundRange)
  {
    if (inSignDomain != QCP::sdPositive || range.lower-mWidth*0.5 > 0)
      range.lower -= mWidth*0.5;
    if (inSignDomain != QCP::sdNegative || range.upper+mWidth*0.5 < 0)
      range.upper += mWidth*0.5;
  }
  return range;
}

/* inherits documentation from base class */
QCPRange QCPFinancial::getValueRange(bool &foundRange, QCP::SignDomain inSignDomain) const
{
  return mDataContainer->valueRange(foundRange, inSignDomain);
}

/*! \internal
  
  Draws the data from \a begin to \a end-1 as OHLC bars with the provided \a painter.

  This method is a helper function for \ref draw. It is used when the chart style is \ref csOhlc.
*/
void QCPFinancial::drawOhlcPlot(QCPPainter *painter, const QCPFinancialDataContainer::const_iterator &begin, const QCPFinancialDataContainer::const_iterator &end)
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  
  QPen linePen;
  
  if (keyAxis->orientation() == Qt::Horizontal)
  {
    for (QCPFinancialDataContainer::const_iterator it = begin; it != end; ++it)
    {
      if (mSelected)
        linePen = mSelectedPen;
      else if (mTwoColored)
        linePen = it->close >= it->open ? mPenPositive : mPenNegative;
      else
        linePen = mPen;
      painter->setPen(linePen);
      double keyPixel = keyAxis->coordToPixel(it->key);
      double openPixel = valueAxis->coordToPixel(it->open);
      double closePixel = valueAxis->coordToPixel(it->close);
      // draw backbone:
      painter->drawLine(QPointF(keyPixel, valueAxis->coordToPixel(it->high)), QPointF(keyPixel, valueAxis->coordToPixel(it->low)));
      // draw open:
      double keyWidthPixels = keyPixel-keyAxis->coordToPixel(it->key-mWidth*0.5); // sign of this makes sure open/close are on correct sides
      painter->drawLine(QPointF(keyPixel-keyWidthPixels, openPixel), QPointF(keyPixel, openPixel));
      // draw close:
      painter->drawLine(QPointF(keyPixel, closePixel), QPointF(keyPixel+keyWidthPixels, closePixel));
    }
  } else
  {
    for (QCPFinancialDataContainer::const_iterator it = begin; it != end; ++it)
    {
      if (mSelected)
        linePen = mSelectedPen;
      else if (mTwoColored)
        linePen = it->close >= it->open ? mPenPositive : mPenNegative;
      else
        linePen = mPen;
      painter->setPen(linePen);
      double keyPixel = keyAxis->coordToPixel(it->key);
      double openPixel = valueAxis->coordToPixel(it->open);
      double closePixel = valueAxis->coordToPixel(it->close);
      // draw backbone:
      painter->drawLine(QPointF(valueAxis->coordToPixel(it->high), keyPixel), QPointF(valueAxis->coordToPixel(it->low), keyPixel));
      // draw open:
      double keyWidthPixels = keyPixel-keyAxis->coordToPixel(it->key-mWidth*0.5); // sign of this makes sure open/close are on correct sides
      painter->drawLine(QPointF(openPixel, keyPixel-keyWidthPixels), QPointF(openPixel, keyPixel));
      // draw close:
      painter->drawLine(QPointF(closePixel, keyPixel), QPointF(closePixel, keyPixel+keyWidthPixels));
    }
  }
}

/*! \internal
  
  Draws the data from \a begin to \a end-1 as Candlesticks with the provided \a painter.

  This method is a helper function for \ref draw. It is used when the chart style is \ref csCandlestick.
*/
void QCPFinancial::drawCandlestickPlot(QCPPainter *painter, const QCPFinancialDataContainer::const_iterator &begin, const QCPFinancialDataContainer::const_iterator &end)
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  
  QPen linePen;
  QBrush boxBrush;
  
  if (keyAxis->orientation() == Qt::Horizontal)
  {
    for (QCPFinancialDataContainer::const_iterator it = begin; it != end; ++it)
    {
      if (mSelected)
      {
        linePen = mSelectedPen;
        boxBrush = mSelectedBrush;
      } else if (mTwoColored)
      {
        if (it->close >= it->open)
        {
          linePen = mPenPositive;
          boxBrush = mBrushPositive;
        } else
        {
          linePen = mPenNegative;
          boxBrush = mBrushNegative;
        }
      } else
      {
        linePen = mPen;
        boxBrush = mBrush;
      }
      painter->setPen(linePen);
      painter->setBrush(boxBrush);
      double keyPixel = keyAxis->coordToPixel(it->key);
      double openPixel = valueAxis->coordToPixel(it->open);
      double closePixel = valueAxis->coordToPixel(it->close);
      // draw high:
      painter->drawLine(QPointF(keyPixel, valueAxis->coordToPixel(it->high)), QPointF(keyPixel, valueAxis->coordToPixel(qMax(it->open, it->close))));
      // draw low:
      painter->drawLine(QPointF(keyPixel, valueAxis->coordToPixel(it->low)), QPointF(keyPixel, valueAxis->coordToPixel(qMin(it->open, it->close))));
      // draw open-close box:
      double keyWidthPixels = keyPixel-keyAxis->coordToPixel(it->key-mWidth*0.5);
      painter->drawRect(QRectF(QPointF(keyPixel-keyWidthPixels, closePixel), QPointF(keyPixel+keyWidthPixels, openPixel)));
    }
  } else // keyAxis->orientation() == Qt::Vertical
  {
    for (QCPFinancialDataContainer::const_iterator it = begin; it != end; ++it)
    {
      if (mSelected)
      {
        linePen = mSelectedPen;
        boxBrush = mSelectedBrush;
      } else if (mTwoColored)
      {
        if (it->close >= it->open)
        {
          linePen = mPenPositive;
          boxBrush = mBrushPositive;
        } else
        {
          linePen = mPenNegative;
          boxBrush = mBrushNegative;
        }
      } else
      {
        linePen = mPen;
        boxBrush = mBrush;
      }
      painter->setPen(linePen);
      painter->setBrush(boxBrush);
      double keyPixel = keyAxis->coordToPixel(it->key);
      double openPixel = valueAxis->coordToPixel(it->open);
      double closePixel = valueAxis->coordToPixel(it->close);
      // draw high:
      painter->drawLine(QPointF(valueAxis->coordToPixel(it->high), keyPixel), QPointF(valueAxis->coordToPixel(qMax(it->open, it->close)), keyPixel));
      // draw low:
      painter->drawLine(QPointF(valueAxis->coordToPixel(it->low), keyPixel), QPointF(valueAxis->coordToPixel(qMin(it->open, it->close)), keyPixel));
      // draw open-close box:
      double keyWidthPixels = keyPixel-keyAxis->coordToPixel(it->key-mWidth*0.5);
      painter->drawRect(QRectF(QPointF(closePixel, keyPixel-keyWidthPixels), QPointF(openPixel, keyPixel+keyWidthPixels)));
    }
  }
}

/*! \internal
  
  This method is a helper function for \ref selectTest. It is used to test for selection when the
  chart style is \ref csOhlc. It only tests against the data points between \a begin and \a end.
*/
double QCPFinancial::ohlcSelectTest(const QPointF &pos, const QCPFinancialDataContainer::const_iterator &begin, const QCPFinancialDataContainer::const_iterator &end) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return -1; }

  double minDistSqr = std::numeric_limits<double>::max();
  QCPFinancialDataContainer::const_iterator it;
  if (keyAxis->orientation() == Qt::Horizontal)
  {
    for (it = begin; it != end; ++it)
    {
      double keyPixel = keyAxis->coordToPixel(it->key);
      // calculate distance to backbone:
      double currentDistSqr = QCPVector2D(pos).distanceSquaredToLine(QCPVector2D(keyPixel, valueAxis->coordToPixel(it->high)), QCPVector2D(keyPixel, valueAxis->coordToPixel(it->low)));
      if (currentDistSqr < minDistSqr)
        minDistSqr = currentDistSqr;
    }
  } else // keyAxis->orientation() == Qt::Vertical
  {
    for (it = begin; it != end; ++it)
    {
      double keyPixel = keyAxis->coordToPixel(it->key);
      // calculate distance to backbone:
      double currentDistSqr = QCPVector2D(pos).distanceSquaredToLine(QCPVector2D(valueAxis->coordToPixel(it->high), keyPixel), QCPVector2D(valueAxis->coordToPixel(it->low), keyPixel));
      if (currentDistSqr < minDistSqr)
        minDistSqr = currentDistSqr;
    }
  }
  return qSqrt(minDistSqr);
}

/*! \internal
  
  This method is a helper function for \ref selectTest. It is used to test for selection when the
  chart style is \ref csCandlestick. It only tests against the data points between \a begin and \a
  end.
*/
double QCPFinancial::candlestickSelectTest(const QPointF &pos, const QCPFinancialDataContainer::const_iterator &begin, const QCPFinancialDataContainer::const_iterator &end) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return -1; }

  double minDistSqr = std::numeric_limits<double>::max();
  QCPFinancialDataContainer::const_iterator it;
  if (keyAxis->orientation() == Qt::Horizontal)
  {
    for (it = begin; it != end; ++it)
    {
      double currentDistSqr;
      // determine whether pos is in open-close-box:
      QCPRange boxKeyRange(it->key-mWidth*0.5, it->key+mWidth*0.5);
      QCPRange boxValueRange(it->close, it->open);
      double posKey, posValue;
      pixelsToCoords(pos, posKey, posValue);
      if (boxKeyRange.contains(posKey) && boxValueRange.contains(posValue)) // is in open-close-box
      {
        currentDistSqr = mParentPlot->selectionTolerance()*0.99 * mParentPlot->selectionTolerance()*0.99;
      } else
      {
        // calculate distance to high/low lines:
        double keyPixel = keyAxis->coordToPixel(it->key);
        double highLineDistSqr = QCPVector2D(pos).distanceSquaredToLine(QCPVector2D(keyPixel, valueAxis->coordToPixel(it->high)), QCPVector2D(keyPixel, valueAxis->coordToPixel(qMax(it->open, it->close))));
        double lowLineDistSqr = QCPVector2D(pos).distanceSquaredToLine(QCPVector2D(keyPixel, valueAxis->coordToPixel(it->low)), QCPVector2D(keyPixel, valueAxis->coordToPixel(qMin(it->open, it->close))));
        currentDistSqr = qMin(highLineDistSqr, lowLineDistSqr);
      }
      if (currentDistSqr < minDistSqr)
        minDistSqr = currentDistSqr;
    }
  } else // keyAxis->orientation() == Qt::Vertical
  {
    for (it = begin; it != end; ++it)
    {
      double currentDistSqr;
      // determine whether pos is in open-close-box:
      QCPRange boxKeyRange(it->key-mWidth*0.5, it->key+mWidth*0.5);
      QCPRange boxValueRange(it->close, it->open);
      double posKey, posValue;
      pixelsToCoords(pos, posKey, posValue);
      if (boxKeyRange.contains(posKey) && boxValueRange.contains(posValue)) // is in open-close-box
      {
        currentDistSqr = mParentPlot->selectionTolerance()*0.99 * mParentPlot->selectionTolerance()*0.99;
      } else
      {
        // calculate distance to high/low lines:
        double keyPixel = keyAxis->coordToPixel(it->key);
        double highLineDistSqr = QCPVector2D(pos).distanceSquaredToLine(QCPVector2D(valueAxis->coordToPixel(it->high), keyPixel), QCPVector2D(valueAxis->coordToPixel(qMax(it->open, it->close)), keyPixel));
        double lowLineDistSqr = QCPVector2D(pos).distanceSquaredToLine(QCPVector2D(valueAxis->coordToPixel(it->low), keyPixel), QCPVector2D(valueAxis->coordToPixel(qMin(it->open, it->close)), keyPixel));
        currentDistSqr = qMin(highLineDistSqr, lowLineDistSqr);
      }
      if (currentDistSqr < minDistSqr)
        minDistSqr = currentDistSqr;
    }
  }
  return qSqrt(minDistSqr);
}

/*! \internal
  
  called by the drawing methods to determine which data (key) range is visible at the current key
  axis range setting, so only that needs to be processed.
  
  \a begin returns an iterator to the lowest data point that needs to be taken into account when
  plotting. Note that in order to get a clean plot all the way to the edge of the axis rect, \a
  begin may still be just outside the visible range.
  
  \a end returns the iterator just above the highest data point that needs to be taken into
  account. Same as before, \a end may also lie just outside of the visible range
  
  if the plottable contains no data, both \a begin and \a end point to \c constEnd.
*/
void QCPFinancial::getVisibleDataBounds(QCPFinancialDataContainer::const_iterator &begin, QCPFinancialDataContainer::const_iterator &end) const
{
  if (!mKeyAxis) { qDebug() << Q_FUNC_INFO << "invalid key axis"; return; }
  begin = mDataContainer->findBeginBelowKey(mKeyAxis.data()->range().lower-mWidth*0.5); // subtract half width of ohlc/candlestick to include partially visible data points
  end = mDataContainer->findEndAboveKey(mKeyAxis.data()->range().upper+mWidth*0.5); // add half width of ohlc/candlestick to include partially visible data points
}
