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

#include "plottable-graph.h"

#include "../painter.h"
#include "../core.h"
#include "../axis/axis.h"
#include "../layoutelements/layoutelement-axisrect.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPGraphData
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPGraphData
  \brief Holds the data of one single data point for QCPGraph.
  
  The container for storing multiple data points is \ref QCPGraphDataContainer.
  
  The stored data is:
  \li \a key: coordinate on the key axis of this data point
  \li \a value: coordinate on the value axis of this data point
  
  \see QCPGraphDataContainer
*/

/*!
  Constructs a data point with key and value set to zero.
*/
QCPGraphData::QCPGraphData() :
  key(0),
  value(0)
{
}

/*!
  Constructs a data point with the specified \a key and \a value.
*/
QCPGraphData::QCPGraphData(double key, double value) :
  key(key),
  value(value)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPGraphDataContainer
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPGraphDataContainer
  \brief Container for storing \ref QCPGraphData objects, used by \ref QCPGraph.
  
  This is the container in which QCPGraph holds its data in a sorted fashion.
  \see QCPGraphData, QCPGraph::setData
*/

/*!
  Constructs a QCPGraphDataContainer used for QCPGraph
*/
QCPGraphDataContainer::QCPGraphDataContainer()
{
}

void QCPGraphDataContainer::add(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted)
{
  if (keys.size() != values.size())
    qDebug() << Q_FUNC_INFO << "keys and values have different sizes:" << keys.size() << values.size();
  const int n = qMin(keys.size(), values.size());
  if (n == 0)
    return;
  
  const int oldSize = size();
  mData.resize(oldSize+n);
  
  if (alreadySorted && oldSize > 0 && keys.last() <= constBegin()->key) // prepend if new data is sorted and keys are all smaller than existing ones
  {
    std::copy_backward(mData.begin(), mData.end()-n, mData.end());
    QCPGraphDataContainer::iterator it = mData.begin();
    for (int i=0; i<n; ++i)
    {
      it->key = keys[i];
      it->value = values[i];
      ++it;
    }
  } else // don't need to prepend, so append and then sort and merge if necessary
  {
    QCPGraphDataContainer::iterator it = mData.end()-n;
    for (int i=0; i<n; ++i)
    {
      it->key = keys[i];
      it->value = values[i];
      ++it;
    }
    if (!alreadySorted) // sort appended subrange if it wasn't already sorted
      std::sort(mData.end()-n, mData.end(), qcpLessThanKey);
    if (oldSize > 0 && !qcpLessThanKey(*(constEnd()-n-1), *(constEnd()-n))) // if appended range keys aren't all greater than existing ones, merge the two partitions
      std::inplace_merge(mData.begin(), mData.end()-n, mData.end(), qcpLessThanKey);
  }
}

void QCPGraphDataContainer::add(const QCPGraphDataContainer &data)
{
  if (data.isEmpty())
    return;
  
  const int n = data.size();
  const int oldSize = size();
  mData.resize(oldSize+n);
  
  if (oldSize > 0 && (data.constEnd()-1)->key <= constBegin()->key) // prepend if new data keys are all smaller than existing ones
  {
    std::copy_backward(mData.begin(), mData.end()-n, mData.end());
    std::copy(data.constBegin(), data.constEnd(), mData.begin());
  } else // don't need to prepend, so append and merge if necessary
  {
    std::copy(data.constBegin(), data.constEnd(), mData.end()-n);
    if (oldSize > 0 && !qcpLessThanKey(*(constEnd()-n-1), *(constEnd()-n))) // if appended range keys aren't all greater than existing ones, merge the two partitions
      std::inplace_merge(mData.begin(), mData.end()-n, mData.end(), qcpLessThanKey);
  }
}

void QCPGraphDataContainer::add(const QCPGraphData &data)
{
  if (isEmpty() || data.key >= (constEnd()-1)->key) // quickly handle appends
  {
    mData.append(data);
  } else // handle inserts, maintaining sorted keys
  {
    QCPGraphDataContainer::iterator insertionPoint = std::lower_bound(mData.begin(), mData.end(), data, qcpLessThanKey);
    mData.insert(insertionPoint, data);
  }
}

void QCPGraphDataContainer::removeBefore(double key)
{
  QCPGraphDataContainer::iterator it = mData.begin();
  QCPGraphDataContainer::iterator itEnd = std::lower_bound(mData.begin(), mData.end(), QCPGraphData(key, 0), qcpLessThanKey);
  mData.erase(it, itEnd);
}

void QCPGraphDataContainer::removeAfter(double key)
{
  QCPGraphDataContainer::iterator it = std::upper_bound(mData.begin(), mData.end(), QCPGraphData(key, 0), qcpLessThanKey);
  QCPGraphDataContainer::iterator itEnd = mData.end();
  mData.erase(it, itEnd);
}

void QCPGraphDataContainer::remove(double fromKey, double toKey)
{
  if (fromKey >= toKey || isEmpty())
    return;
  
  QCPGraphDataContainer::iterator it = std::lower_bound(mData.begin(), mData.end(), QCPGraphData(fromKey, 0), qcpLessThanKey);
  QCPGraphDataContainer::iterator itEnd = std::upper_bound(it, mData.end(), QCPGraphData(toKey, 0), qcpLessThanKey);
  mData.erase(it, itEnd);
}

void QCPGraphDataContainer::remove(double key)
{
  QCPGraphDataContainer::iterator it = std::lower_bound(mData.begin(), mData.end(), QCPGraphData(key, 0), qcpLessThanKey);
  if (it != mData.constEnd() && it->key == key)
    mData.erase(it);
}

void QCPGraphDataContainer::clear()
{
  mData.clear();
}

void QCPGraphDataContainer::sort()
{
  std::sort(mData.begin(), mData.end(), qcpLessThanKey);
}

QCPGraphDataContainer::const_iterator QCPGraphDataContainer::findBeginBelowKey(double key) const
{
  if (isEmpty())
    return constEnd();
  
  QCPGraphDataContainer::const_iterator it = std::lower_bound(constBegin(), constEnd(), QCPGraphData(key, 0), qcpLessThanKey);
  if (it != constBegin()) // also covers it == constEnd case, and we know --constEnd is valid because mData isn't empty
    --it;
  return it;
}

QCPGraphDataContainer::const_iterator QCPGraphDataContainer::findEndAboveKey(double key) const
{
  if (isEmpty())
    return constEnd();
  
  QCPGraphDataContainer::const_iterator it = std::upper_bound(constBegin(), constEnd(), QCPGraphData(key, 0), qcpLessThanKey);
  if (it != constEnd())
    ++it;
  return it;
}

QCPRange QCPGraphDataContainer::keyRange(bool &foundRange, QCP::SignDomain signDomain)
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
  
  QCPGraphDataContainer::const_iterator it = constBegin();
  QCPGraphDataContainer::const_iterator itEnd = constEnd();
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

QCPRange QCPGraphDataContainer::valueRange(bool &foundRange, QCP::SignDomain signDomain)
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
  
  QCPGraphDataContainer::const_iterator it = constBegin();
  QCPGraphDataContainer::const_iterator itEnd = constEnd();
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


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPGraph
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPGraph
  \brief A plottable representing a graph in a plot.

  \image html QCPGraph.png
  
  Usually you create new graphs by calling QCustomPlot::addGraph. The resulting instance can be
  accessed via QCustomPlot::graph.

  To plot data, assign it with the \ref setData or \ref addData functions. Alternatively, you can
  also access and modify the graph's data via the \ref data method, which returns a pointer to the
  internal \ref QCPDataMap.
  
  Graphs are used to display single-valued data. Single-valued means that there should only be one
  data point per unique key coordinate. In other words, the graph can't have \a loops. If you do
  want to plot non-single-valued curves, rather use the QCPCurve plottable.
  
  Gaps in the graph line can be created by adding data points with NaN as value
  (<tt>qQNaN()</tt> or <tt>std::numeric_limits<double>::quiet_NaN()</tt>) in between the two data points that shall be
  separated.
  
  \section appearance Changing the appearance
  
  The appearance of the graph is mainly determined by the line style, scatter style, brush and pen
  of the graph (\ref setLineStyle, \ref setScatterStyle, \ref setBrush, \ref setPen).
  
  \subsection filling Filling under or between graphs
  
  QCPGraph knows two types of fills: Normal graph fills towards the zero-value-line parallel to
  the key axis of the graph, and fills between two graphs, called channel fills. To enable a fill,
  just set a brush with \ref setBrush which is neither Qt::NoBrush nor fully transparent.
  
  By default, a normal fill towards the zero-value-line will be drawn. To set up a channel fill
  between this graph and another one, call \ref setChannelFillGraph with the other graph as
  parameter.

  \see QCustomPlot::addGraph, QCustomPlot::graph
*/

/* start of documentation of inline functions */

/*! \fn QCPDataMap *QCPGraph::data() const
  
  Returns a pointer to the internal data storage of type \ref QCPDataMap. You may use it to
  directly manipulate the data, which may be more convenient and faster than using the regular \ref
  setData or \ref addData methods, in certain situations.
*/

/* end of documentation of inline functions */

/*!
  Constructs a graph which uses \a keyAxis as its key axis ("x") and \a valueAxis as its value
  axis ("y"). \a keyAxis and \a valueAxis must reside in the same QCustomPlot instance and not have
  the same orientation. If either of these restrictions is violated, a corresponding message is
  printed to the debug output (qDebug), the construction is not aborted, though.
  
  The created QCPGraph is automatically registered with the QCustomPlot instance inferred from \a
  keyAxis. This QCustomPlot instance takes ownership of the QCPGraph, so do not delete it manually
  but use QCustomPlot::removePlottable() instead.
  
  To directly create a graph inside a plot, you can also use the simpler QCustomPlot::addGraph function.
*/
QCPGraph::QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis) :
  QCPAbstractPlottable(keyAxis, valueAxis)
{
  // special handling for QCPGraphs to maintain the simple graph interface:
  mParentPlot->registerGraph(this);
  
  mDataContainer = new QCPGraphDataContainer;
  
  setPen(QPen(Qt::blue, 0));
  setBrush(Qt::NoBrush);
  setSelectedPen(QPen(QColor(80, 80, 255), 2.5));
  setSelectedBrush(Qt::NoBrush);
  
  setLineStyle(lsLine);
  setChannelFillGraph(0);
  setAdaptiveSampling(true);
}

QCPGraph::~QCPGraph()
{
  delete mDataContainer;
}

/*!
  Replaces the current data with the provided \a data.
  
  If \a copy is set to true, data points in \a data will only be copied. if false, the graph
  takes ownership of the passed data and replaces the internal data pointer with it. This is
  significantly faster than copying for large datasets.
  
  Alternatively, you can also access and modify the graph's data via the \ref data method, which
  returns a pointer to the internal \ref QCPDataMap.
*/
void QCPGraph::setData(QCPGraphDataContainer *data, bool copy)
{
  if (mDataContainer == data)
  {
    qDebug() << Q_FUNC_INFO << "The data pointer is already in (and owned by) this plottable" << reinterpret_cast<quintptr>(data);
    return;
  }
  if (copy)
  {
    *mDataContainer = *data;
  } else
  {
    delete mDataContainer;
    mDataContainer = data;
  }
}

/*! \overload
  
  Replaces the current data with the provided points in \a keys and \a values. The provided
  vectors should have equal length. Else, the number of added points will be the size of the
  smallest vector.
*/
void QCPGraph::setData(const QVector<double> &keys, const QVector<double> &values)
{
  mDataContainer->clear();
  addData(keys, values);
}

/*!
  Sets how the single data points are connected in the plot. For scatter-only plots, set \a ls to
  \ref lsNone and \ref setScatterStyle to the desired scatter style.
  
  \see setScatterStyle
*/
void QCPGraph::setLineStyle(LineStyle ls)
{
  mLineStyle = ls;
}

/*!
  Sets the visual appearance of single data points in the plot. If set to \ref QCPScatterStyle::ssNone, no scatter points
  are drawn (e.g. for line-only-plots with appropriate line style).
  
  \see QCPScatterStyle, setLineStyle
*/
void QCPGraph::setScatterStyle(const QCPScatterStyle &style)
{
  mScatterStyle = style;
}

/*!
  Sets the target graph for filling the area between this graph and \a targetGraph with the current
  brush (\ref setBrush).
  
  When \a targetGraph is set to 0, a normal graph fill to the zero-value-line will be shown. To
  disable any filling, set the brush to Qt::NoBrush.

  \see setBrush
*/
void QCPGraph::setChannelFillGraph(QCPGraph *targetGraph)
{
  // prevent setting channel target to this graph itself:
  if (targetGraph == this)
  {
    qDebug() << Q_FUNC_INFO << "targetGraph is this graph itself";
    mChannelFillGraph = 0;
    return;
  }
  // prevent setting channel target to a graph not in the plot:
  if (targetGraph && targetGraph->mParentPlot != mParentPlot)
  {
    qDebug() << Q_FUNC_INFO << "targetGraph not in same plot";
    mChannelFillGraph = 0;
    return;
  }
  
  mChannelFillGraph = targetGraph;
}

/*!
  Sets whether adaptive sampling shall be used when plotting this graph. QCustomPlot's adaptive
  sampling technique can drastically improve the replot performance for graphs with a larger number
  of points (e.g. above 10,000), without notably changing the appearance of the graph.
  
  By default, adaptive sampling is enabled. Even if enabled, QCustomPlot decides whether adaptive
  sampling shall actually be used on a per-graph basis. So leaving adaptive sampling enabled has no
  disadvantage in almost all cases.
  
  \image html adaptive-sampling-line.png "A line plot of 500,000 points without and with adaptive sampling"
  
  As can be seen, line plots experience no visual degradation from adaptive sampling. Outliers are
  reproduced reliably, as well as the overall shape of the data set. The replot time reduces
  dramatically though. This allows QCustomPlot to display large amounts of data in realtime.
  
  \image html adaptive-sampling-scatter.png "A scatter plot of 100,000 points without and with adaptive sampling"
  
  Care must be taken when using high-density scatter plots in combination with adaptive sampling.
  The adaptive sampling algorithm treats scatter plots more carefully than line plots which still
  gives a significant reduction of replot times, but not quite as much as for line plots. This is
  because scatter plots inherently need more data points to be preserved in order to still resemble
  the original, non-adaptive-sampling plot. As shown above, the results still aren't quite
  identical, as banding occurs for the outer data points. This is in fact intentional, such that
  the boundaries of the data cloud stay visible to the viewer. How strong the banding appears,
  depends on the point density, i.e. the number of points in the plot.
  
  For some situations with scatter plots it might thus be desirable to manually turn adaptive
  sampling off. For example, when saving the plot to disk. This can be achieved by setting \a
  enabled to false before issuing a command like \ref QCustomPlot::savePng, and setting \a enabled
  back to true afterwards.
*/
void QCPGraph::setAdaptiveSampling(bool enabled)
{
  mAdaptiveSampling = enabled;
}

/*!
  Adds the provided data points in \a dataMap to the current data.
  
  Alternatively, you can also access and modify the graph's data via the \ref data method, which
  returns a pointer to the internal \ref QCPDataMap.
  
  \see removeData
*/
void QCPGraph::addData(const QCPGraphDataContainer &data)
{
  mDataContainer->add(data);
}

/*! \overload
  Adds the provided single data point in \a data to the current data.
  
  Alternatively, you can also access and modify the graph's data via the \ref data method, which
  returns a pointer to the internal \ref QCPDataMap.
  
  \see removeData
*/
void QCPGraph::addData(const QCPGraphData &data)
{
  mDataContainer->add(data);
}

/*! \overload
  Adds the provided single data point as \a key and \a value pair to the current data.
  
  Alternatively, you can also access and modify the graph's data via the \ref data method, which
  returns a pointer to the internal \ref QCPDataMap.
  
  \see removeData
*/
void QCPGraph::addData(double key, double value)
{
  mDataContainer->add(QCPGraphData(key, value));
}

/*! \overload
  Adds the provided data points as \a key and \a value pairs to the current data.
  
  Alternatively, you can also access and modify the graph's data via the \ref data method, which
  returns a pointer to the internal \ref QCPDataMap.
  
  \see removeData
*/
void QCPGraph::addData(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted)
{
  mDataContainer->add(keys, values, alreadySorted);
}

/*!
  Removes all data points with keys smaller than \a key.
  \see addData, clearData
*/
void QCPGraph::removeDataBefore(double key)
{
  mDataContainer->removeBefore(key);
}

/*!
  Removes all data points with keys greater than \a key.
  \see addData, clearData
*/
void QCPGraph::removeDataAfter(double key)
{
  mDataContainer->removeAfter(key);
}

/*!
  Removes all data points with keys between \a fromKey and \a toKey.
  if \a fromKey is greater or equal to \a toKey, the function does nothing. To remove
  a single data point with known key, use \ref removeData(double key).
  
  \see addData, clearData
*/
void QCPGraph::removeData(double fromKey, double toKey)
{
  mDataContainer->remove(fromKey, toKey);
}

/*! \overload
  
  Removes a single data point at \a key. If the position is not known with absolute precision,
  consider using \ref removeData(double fromKey, double toKey) with a small fuzziness interval around
  the suspected position, depeding on the precision with which the key is known.

  \see addData, clearData
*/
void QCPGraph::removeData(double key)
{
  mDataContainer->remove(key);
}

/*!
  Removes all data points.
  \see removeData, removeDataAfter, removeDataBefore
*/
void QCPGraph::clearData()
{
  mDataContainer->clear();
}

/* inherits documentation from base class */
double QCPGraph::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
  Q_UNUSED(details)
  if ((onlySelectable && !mSelectable) || mDataContainer->isEmpty())
    return -1;
  if (!mKeyAxis || !mValueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return -1; }
  
  if (mKeyAxis.data()->axisRect()->rect().contains(pos.toPoint()))
    return pointDistance(pos);
  else
    return -1;
}

/* inherits documentation from base class */
void QCPGraph::draw(QCPPainter *painter)
{
  if (!mKeyAxis || !mValueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  if (mKeyAxis.data()->range().size() <= 0 || mDataContainer->isEmpty()) return;
  if (mLineStyle == lsNone && mScatterStyle.isNone()) return;
  
  // allocate line and (if necessary) point vectors:
  QVector<QPointF> *lineData = new QVector<QPointF>;
  QVector<QCPGraphData> *scatterData = 0;
  if (!mScatterStyle.isNone())
    scatterData = new QVector<QCPGraphData>;
  
  // fill vectors with data appropriate to plot style:
  getPlotData(lineData, scatterData);
  
  // check data validity if flag set:
#ifdef QCUSTOMPLOT_CHECK_DATA
  QCPGraphDataContainer::const_iterator it;
  for (it = mDataContainer->constBegin(); it != mDataContainer->constEnd(); ++it)
  {
    if (QCP::isInvalidData(it->key, it->value))
      qDebug() << Q_FUNC_INFO << "Data point at" << it->key << "invalid." << "Plottable name:" << name();
  }
#endif

  // draw fill of graph:
  if (mLineStyle != lsNone)
    drawFill(painter, lineData);
  
  // draw line:
  if (mLineStyle == lsImpulse)
    drawImpulsePlot(painter, lineData);
  else if (mLineStyle != lsNone)
    drawLinePlot(painter, lineData); // also step plots can be drawn as a line plot
  
  // draw scatters:
  if (scatterData)
    drawScatterPlot(painter, scatterData);
  
  // free allocated line and point vectors:
  delete lineData;
  if (scatterData)
    delete scatterData;
}

/* inherits documentation from base class */
void QCPGraph::drawLegendIcon(QCPPainter *painter, const QRectF &rect) const
{
  // draw fill:
  if (mBrush.style() != Qt::NoBrush)
  {
    applyFillAntialiasingHint(painter);
    painter->fillRect(QRectF(rect.left(), rect.top()+rect.height()/2.0, rect.width(), rect.height()/3.0), mBrush);
  }
  // draw line vertically centered:
  if (mLineStyle != lsNone)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mPen);
    painter->drawLine(QLineF(rect.left(), rect.top()+rect.height()/2.0, rect.right()+5, rect.top()+rect.height()/2.0)); // +5 on x2 else last segment is missing from dashed/dotted pens
  }
  // draw scatter symbol:
  if (!mScatterStyle.isNone())
  {
    applyScattersAntialiasingHint(painter);
    // scale scatter pixmap if it's too large to fit in legend icon rect:
    if (mScatterStyle.shape() == QCPScatterStyle::ssPixmap && (mScatterStyle.pixmap().size().width() > rect.width() || mScatterStyle.pixmap().size().height() > rect.height()))
    {
      QCPScatterStyle scaledStyle(mScatterStyle);
      scaledStyle.setPixmap(scaledStyle.pixmap().scaled(rect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
      scaledStyle.applyTo(painter, mPen);
      scaledStyle.drawShape(painter, QRectF(rect).center());
    } else
    {
      mScatterStyle.applyTo(painter, mPen);
      mScatterStyle.drawShape(painter, QRectF(rect).center());
    }
  }
}

/*! \internal

  This function branches out to the line style specific "get(...)PlotData" functions, according to
  the line style of the graph.
  
  \a lineData will be filled with raw points that will be drawn with the according draw functions,
  e.g. \ref drawLinePlot and \ref drawImpulsePlot. These aren't necessarily the original data
  points, since for step plots for example, additional points are needed for drawing lines that
  make up steps. If the line style of the graph is \ref lsNone, the \a lineData vector will be left
  untouched.
  
  \a scatterData will be filled with the original data points so \ref drawScatterPlot can draw the
  scatter symbols accordingly. If no scatters need to be drawn, i.e. the scatter style's shape is
  \ref QCPScatterStyle::ssNone, pass 0 as \a scatterData, and this step will be skipped.
  
  \see getScatterPlotData, getLinePlotData, getStepLeftPlotData, getStepRightPlotData,
  getStepCenterPlotData, getImpulsePlotData
*/
void QCPGraph::getPlotData(QVector<QPointF> *lineData, QVector<QCPGraphData> *scatterData) const
{
  switch(mLineStyle)
  {
    case lsNone: getScatterPlotData(scatterData); break;
    case lsLine: getLinePlotData(lineData, scatterData); break;
    case lsStepLeft: getStepLeftPlotData(lineData, scatterData); break;
    case lsStepRight: getStepRightPlotData(lineData, scatterData); break;
    case lsStepCenter: getStepCenterPlotData(lineData, scatterData); break;
    case lsImpulse: getImpulsePlotData(lineData, scatterData); break;
  }
}

/*! \internal
  
  If line style is \ref lsNone and the scatter style's shape is not \ref QCPScatterStyle::ssNone,
  this function serves at providing the visible data points in \a scatterData, so the \ref
  drawScatterPlot function can draw the scatter points accordingly.
  
  If line style is not \ref lsNone, this function is not called and the data for the scatter points
  are (if needed) calculated inside the corresponding other "get(...)PlotData" functions.
  
  \see drawScatterPlot
*/
void QCPGraph::getScatterPlotData(QVector<QCPGraphData> *scatterData) const
{
  getPreparedData(0, scatterData);
}

/*! \internal
  
  Places the raw data points needed for a normal linearly connected graph in \a linePixelData.

  As for all plot data retrieval functions, \a scatterData just contains all unaltered data (scatter)
  points that are visible for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. the scatter style's shape is \ref QCPScatterStyle::ssNone), pass 0 as \a
  scatterData, and the function will skip filling the vector.
  
  \see drawLinePlot
*/
void QCPGraph::getLinePlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  if (!linePixelData) { qDebug() << Q_FUNC_INFO << "null pointer passed as linePixelData"; return; }
  
  QVector<QCPGraphData> lineData;
  getPreparedData(&lineData, scatterData);
  linePixelData->reserve(lineData.size()+2); // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
  linePixelData->resize(lineData.size());
  
  // transform lineData points to pixels:
  if (keyAxis->orientation() == Qt::Vertical)
  {
    for (int i=0; i<lineData.size(); ++i)
    {
      (*linePixelData)[i].setX(valueAxis->coordToPixel(lineData.at(i).value));
      (*linePixelData)[i].setY(keyAxis->coordToPixel(lineData.at(i).key));
    }
  } else // key axis is horizontal
  {
    for (int i=0; i<lineData.size(); ++i)
    {
      (*linePixelData)[i].setX(keyAxis->coordToPixel(lineData.at(i).key));
      (*linePixelData)[i].setY(valueAxis->coordToPixel(lineData.at(i).value));
    }
  }
}

/*!
  \internal
  Places the raw data points needed for a step plot with left oriented steps in \a lineData.

  As for all plot data retrieval functions, \a scatterData just contains all unaltered data (scatter)
  points that are visible for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. the scatter style's shape is \ref QCPScatterStyle::ssNone), pass 0 as \a
  scatterData, and the function will skip filling the vector.
  
  \see drawLinePlot
*/
void QCPGraph::getStepLeftPlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  if (!linePixelData) { qDebug() << Q_FUNC_INFO << "null pointer passed as lineData"; return; }
  
  QVector<QCPGraphData> lineData;
  getPreparedData(&lineData, scatterData);
  linePixelData->reserve(lineData.size()*2+2); // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
  linePixelData->resize(lineData.size()*2);
  
  // calculate steps from lineData and transform to pixel coordinates:
  if (keyAxis->orientation() == Qt::Vertical)
  {
    double lastValue = valueAxis->coordToPixel(lineData.first().value);
    double key;
    for (int i=0; i<lineData.size(); ++i)
    {
      key = keyAxis->coordToPixel(lineData.at(i).key);
      (*linePixelData)[i*2+0].setX(lastValue);
      (*linePixelData)[i*2+0].setY(key);
      lastValue = valueAxis->coordToPixel(lineData.at(i).value);
      (*linePixelData)[i*2+1].setX(lastValue);
      (*linePixelData)[i*2+1].setY(key);
    }
  } else // key axis is horizontal
  {
    double lastValue = valueAxis->coordToPixel(lineData.first().value);
    double key;
    for (int i=0; i<lineData.size(); ++i)
    {
      key = keyAxis->coordToPixel(lineData.at(i).key);
      (*linePixelData)[i*2+0].setX(key);
      (*linePixelData)[i*2+0].setY(lastValue);
      lastValue = valueAxis->coordToPixel(lineData.at(i).value);
      (*linePixelData)[i*2+1].setX(key);
      (*linePixelData)[i*2+1].setY(lastValue);
    }
  }
}

/*!
  \internal
  Places the raw data points needed for a step plot with right oriented steps in \a lineData.

  As for all plot data retrieval functions, \a scatterData just contains all unaltered data (scatter)
  points that are visible for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. the scatter style's shape is \ref QCPScatterStyle::ssNone), pass 0 as \a
  scatterData, and the function will skip filling the vector.
  
  \see drawLinePlot
*/
void QCPGraph::getStepRightPlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  if (!linePixelData) { qDebug() << Q_FUNC_INFO << "null pointer passed as lineData"; return; }
  
  QVector<QCPGraphData> lineData;
  getPreparedData(&lineData, scatterData);
  linePixelData->reserve(lineData.size()*2+2); // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
  linePixelData->resize(lineData.size()*2);
  
  // calculate steps from lineData and transform to pixel coordinates:
  if (keyAxis->orientation() == Qt::Vertical)
  {
    double lastKey = keyAxis->coordToPixel(lineData.first().key);
    double value;
    for (int i=0; i<lineData.size(); ++i)
    {
      value = valueAxis->coordToPixel(lineData.at(i).value);
      (*linePixelData)[i*2+0].setX(value);
      (*linePixelData)[i*2+0].setY(lastKey);
      lastKey = keyAxis->coordToPixel(lineData.at(i).key);
      (*linePixelData)[i*2+1].setX(value);
      (*linePixelData)[i*2+1].setY(lastKey);
    }
  } else // key axis is horizontal
  {
    double lastKey = keyAxis->coordToPixel(lineData.first().key);
    double value;
    for (int i=0; i<lineData.size(); ++i)
    {
      value = valueAxis->coordToPixel(lineData.at(i).value);
      (*linePixelData)[i*2+0].setX(lastKey);
      (*linePixelData)[i*2+0].setY(value);
      lastKey = keyAxis->coordToPixel(lineData.at(i).key);
      (*linePixelData)[i*2+1].setX(lastKey);
      (*linePixelData)[i*2+1].setY(value);
    }
  }
}

/*!
  \internal
  Places the raw data points needed for a step plot with centered steps in \a lineData.

  As for all plot data retrieval functions, \a scatterData just contains all unaltered data (scatter)
  points that are visible for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. the scatter style's shape is \ref QCPScatterStyle::ssNone), pass 0 as \a
  scatterData, and the function will skip filling the vector.
  
  \see drawLinePlot
*/
void QCPGraph::getStepCenterPlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  if (!linePixelData) { qDebug() << Q_FUNC_INFO << "null pointer passed as lineData"; return; }
  
  QVector<QCPGraphData> lineData;
  getPreparedData(&lineData, scatterData);
  linePixelData->reserve(lineData.size()*2+2); // added 2 to reserve memory for lower/upper fill base points that might be needed for fill
  linePixelData->resize(lineData.size()*2);
  // calculate steps from lineData and transform to pixel coordinates:
  if (keyAxis->orientation() == Qt::Vertical)
  {
    double lastKey = keyAxis->coordToPixel(lineData.first().key);
    double lastValue = valueAxis->coordToPixel(lineData.first().value);
    double key;
    (*linePixelData)[0].setX(lastValue);
    (*linePixelData)[0].setY(lastKey);
    for (int i=1; i<lineData.size(); ++i)
    {
      key = (keyAxis->coordToPixel(lineData.at(i).key)+lastKey)*0.5;
      (*linePixelData)[i*2-1].setX(lastValue);
      (*linePixelData)[i*2-1].setY(key);
      lastValue = valueAxis->coordToPixel(lineData.at(i).value);
      lastKey = keyAxis->coordToPixel(lineData.at(i).key);
      (*linePixelData)[i*2+0].setX(lastValue);
      (*linePixelData)[i*2+0].setY(key);
    }
    (*linePixelData)[lineData.size()*2-1].setX(lastValue);
    (*linePixelData)[lineData.size()*2-1].setY(lastKey);
  } else // key axis is horizontal
  {
    double lastKey = keyAxis->coordToPixel(lineData.first().key);
    double lastValue = valueAxis->coordToPixel(lineData.first().value);
    double key;
    (*linePixelData)[0].setX(lastKey);
    (*linePixelData)[0].setY(lastValue);
    for (int i=1; i<lineData.size(); ++i)
    {
      key = (keyAxis->coordToPixel(lineData.at(i).key)+lastKey)*0.5;
      (*linePixelData)[i*2-1].setX(key);
      (*linePixelData)[i*2-1].setY(lastValue);
      lastValue = valueAxis->coordToPixel(lineData.at(i).value);
      lastKey = keyAxis->coordToPixel(lineData.at(i).key);
      (*linePixelData)[i*2+0].setX(key);
      (*linePixelData)[i*2+0].setY(lastValue);
    }
    (*linePixelData)[lineData.size()*2-1].setX(lastKey);
    (*linePixelData)[lineData.size()*2-1].setY(lastValue);
  }

}

/*!
  \internal
  Places the raw data points needed for an impulse plot in \a lineData.

  As for all plot data retrieval functions, \a scatterData just contains all unaltered data (scatter)
  points that are visible for drawing scatter points, if necessary. If drawing scatter points is
  disabled (i.e. the scatter style's shape is \ref QCPScatterStyle::ssNone), pass 0 as \a
  scatterData, and the function will skip filling the vector.
  
  \see drawImpulsePlot
*/
void QCPGraph::getImpulsePlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  if (!linePixelData) { qDebug() << Q_FUNC_INFO << "null pointer passed as linePixelData"; return; }
  
  QVector<QCPGraphData> lineData;
  getPreparedData(&lineData, scatterData);
  linePixelData->resize(lineData.size()*2); // no need to reserve 2 extra points because impulse plot has no fill
  
  // transform lineData points to pixels:
  if (keyAxis->orientation() == Qt::Vertical)
  {
    double zeroPointX = valueAxis->coordToPixel(0);
    double key;
    for (int i=0; i<lineData.size(); ++i)
    {
      key = keyAxis->coordToPixel(lineData.at(i).key);
      (*linePixelData)[i*2+0].setX(zeroPointX);
      (*linePixelData)[i*2+0].setY(key);
      (*linePixelData)[i*2+1].setX(valueAxis->coordToPixel(lineData.at(i).value));
      (*linePixelData)[i*2+1].setY(key);
    }
  } else // key axis is horizontal
  {
    double zeroPointY = valueAxis->coordToPixel(0);
    double key;
    for (int i=0; i<lineData.size(); ++i)
    {
      key = keyAxis->coordToPixel(lineData.at(i).key);
      (*linePixelData)[i*2+0].setX(key);
      (*linePixelData)[i*2+0].setY(zeroPointY);
      (*linePixelData)[i*2+1].setX(key);
      (*linePixelData)[i*2+1].setY(valueAxis->coordToPixel(lineData.at(i).value));
    }
  }
}

/*! \internal
  
  Draws the fill of the graph with the specified brush.

  If the fill is a normal fill towards the zero-value-line, only the \a lineData is required (and
  two extra points at the zero-value-line, which are added by \ref addFillBasePoints and removed by
  \ref removeFillBasePoints after the fill drawing is done).
  
  If the fill is a channel fill between this QCPGraph and another QCPGraph (mChannelFillGraph), the
  more complex polygon is calculated with the \ref getChannelFillPolygon function.
  
  \see drawLinePlot
*/
void QCPGraph::drawFill(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  if (mLineStyle == lsImpulse) return; // fill doesn't make sense for impulse plot
  if (mainBrush().style() == Qt::NoBrush || mainBrush().color().alpha() == 0) return;
  
  applyFillAntialiasingHint(painter);
  if (!mChannelFillGraph)
  {
    // draw base fill under graph, fill goes all the way to the zero-value-line:
    addFillBasePoints(lineData);
    painter->setPen(Qt::NoPen);
    painter->setBrush(mainBrush());
    painter->drawPolygon(QPolygonF(*lineData));
    removeFillBasePoints(lineData);
  } else
  {
    // draw channel fill between this graph and mChannelFillGraph:
    painter->setPen(Qt::NoPen);
    painter->setBrush(mainBrush());
    painter->drawPolygon(getChannelFillPolygon(lineData));
  }
}

/*! \internal
  
  Draws scatter symbols at every data point passed in \a scatterData. scatter symbols are independent
  of the line style and are always drawn if the scatter style's shape is not \ref
  QCPScatterStyle::ssNone. Hence, the \a scatterData vector is outputted by all "get(...)PlotData"
  functions, together with the (line style dependent) line data.
  
  \see drawLinePlot, drawImpulsePlot
*/
void QCPGraph::drawScatterPlot(QCPPainter *painter, QVector<QCPGraphData> *scatterData) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  
  // draw scatter point symbols:
  applyScattersAntialiasingHint(painter);
  mScatterStyle.applyTo(painter, mPen);
  if (keyAxis->orientation() == Qt::Vertical)
  {
    for (int i=0; i<scatterData->size(); ++i)
      if (!qIsNaN(scatterData->at(i).value))
        mScatterStyle.drawShape(painter, valueAxis->coordToPixel(scatterData->at(i).value), keyAxis->coordToPixel(scatterData->at(i).key));
  } else
  {
    for (int i=0; i<scatterData->size(); ++i)
      if (!qIsNaN(scatterData->at(i).value))
        mScatterStyle.drawShape(painter, keyAxis->coordToPixel(scatterData->at(i).key), valueAxis->coordToPixel(scatterData->at(i).value));
  }
}

/*!  \internal
  
  Draws line graphs from the provided data. It connects all points in \a lineData, which was
  created by one of the "get(...)PlotData" functions for line styles that require simple line
  connections between the point vector they create. These are for example \ref getLinePlotData,
  \ref getStepLeftPlotData, \ref getStepRightPlotData and \ref getStepCenterPlotData.
  
  \see drawScatterPlot, drawImpulsePlot
*/
void QCPGraph::drawLinePlot(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  // draw line of graph:
  if (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
  {
    applyDefaultAntialiasingHint(painter);
    painter->setPen(mainPen());
    painter->setBrush(Qt::NoBrush);
    
    /* Draws polyline in batches, currently not used:
    int p = 0;
    while (p < lineData->size())
    {
      int batch = qMin(25, lineData->size()-p);
      if (p != 0)
      {
        ++batch;
        --p; // to draw the connection lines between two batches
      }
      painter->drawPolyline(lineData->constData()+p, batch);
      p += batch;
    }
    */
    
    // if drawing solid line and not in PDF, use much faster line drawing instead of polyline:
    if (mParentPlot->plottingHints().testFlag(QCP::phFastPolylines) &&
        painter->pen().style() == Qt::SolidLine &&
        !painter->modes().testFlag(QCPPainter::pmVectorized) &&
        !painter->modes().testFlag(QCPPainter::pmNoCaching))
    {
      int i = 0;
      bool lastIsNan = false;
      const int lineDataSize = lineData->size();
      while (i < lineDataSize && (qIsNaN(lineData->at(i).y()) || qIsNaN(lineData->at(i).x()))) // make sure first point is not NaN
        ++i;
      ++i; // because drawing works in 1 point retrospect
      while (i < lineDataSize)
      {
        if (!qIsNaN(lineData->at(i).y()) && !qIsNaN(lineData->at(i).x())) // NaNs create a gap in the line
        {
          if (!lastIsNan)
            painter->drawLine(lineData->at(i-1), lineData->at(i));
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
      const int lineDataSize = lineData->size();
      while (i < lineDataSize)
      {
        if (qIsNaN(lineData->at(i).y()) || qIsNaN(lineData->at(i).x())) // NaNs create a gap in the line
        {
          painter->drawPolyline(lineData->constData()+segmentStart, i-segmentStart); // i, because we don't want to include the current NaN point
          segmentStart = i+1;
        }
        ++i;
      }
      // draw last segment:
      painter->drawPolyline(lineData->constData()+segmentStart, lineDataSize-segmentStart);
    }
  }
}

/*! \internal
  
  Draws impulses from the provided data, i.e. it connects all line pairs in \a lineData, which was
  created by \ref getImpulsePlotData.
  
  \see drawScatterPlot, drawLinePlot
*/
void QCPGraph::drawImpulsePlot(QCPPainter *painter, QVector<QPointF> *lineData) const
{
  // draw impulses:
  if (mainPen().style() != Qt::NoPen && mainPen().color().alpha() != 0)
  {
    applyDefaultAntialiasingHint(painter);
    QPen pen = mainPen();
    pen.setCapStyle(Qt::FlatCap); // so impulse line doesn't reach beyond zero-line
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawLines(*lineData);
  }
}

/*! \internal
  
  Returns the \a lineData and \a scatterData that need to be plotted for this graph taking into
  consideration the current axis ranges and, if \ref setAdaptiveSampling is enabled, local point
  densities.
  
  0 may be passed as \a lineData or \a scatterData to indicate that the respective dataset isn't
  needed. For example, if the scatter style (\ref setScatterStyle) is \ref QCPScatterStyle::ssNone, \a
  scatterData should be 0 to prevent unnecessary calculations.
  
  This method is used by the various "get(...)PlotData" methods to get the basic working set of data.
*/
void QCPGraph::getPreparedData(QVector<QCPGraphData> *lineData, QVector<QCPGraphData> *scatterData) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
  // get visible data range:
  QCPGraphDataContainer::const_iterator lower, upperEnd;
  getVisibleDataBounds(lower, upperEnd);
  if (lower == mDataContainer->constEnd())
    return;
  
  // count points in visible range, taking into account that we only need to count to the limit maxCount if using adaptive sampling:
  int maxCount = std::numeric_limits<int>::max();
  if (mAdaptiveSampling)
  {
    int keyPixelSpan = qAbs(keyAxis->coordToPixel(lower->key)-keyAxis->coordToPixel((upperEnd-1)->key));
    maxCount = 2*keyPixelSpan+2;
  }
  int dataCount = (upperEnd-lower)-1;
  
  if (mAdaptiveSampling && dataCount >= maxCount) // use adaptive sampling only if there are at least two points per pixel on average
  {
    if (lineData)
    {
      QCPGraphDataContainer::const_iterator it = lower;
      double minValue = it->value;
      double maxValue = it->value;
      QCPGraphDataContainer::const_iterator currentIntervalFirstPoint = it;
      int reversedFactor = keyAxis->rangeReversed() != (keyAxis->orientation()==Qt::Vertical) ? -1 : 1; // is used to calculate keyEpsilon pixel into the correct direction
      int reversedRound = keyAxis->rangeReversed() != (keyAxis->orientation()==Qt::Vertical) ? 1 : 0; // is used to switch between floor (normal) and ceil (reversed) rounding of currentIntervalStartKey
      double currentIntervalStartKey = keyAxis->pixelToCoord((int)(keyAxis->coordToPixel(lower->key)+reversedRound));
      double lastIntervalEndKey = currentIntervalStartKey;
      double keyEpsilon = qAbs(currentIntervalStartKey-keyAxis->pixelToCoord(keyAxis->coordToPixel(currentIntervalStartKey)+1.0*reversedFactor)); // interval of one pixel on screen when mapped to plot key coordinates
      bool keyEpsilonVariable = keyAxis->scaleType() == QCPAxis::stLogarithmic; // indicates whether keyEpsilon needs to be updated after every interval (for log axes)
      int intervalDataCount = 1;
      ++it; // advance iterator to second data point because adaptive sampling works in 1 point retrospect
      while (it != upperEnd)
      {
        if (it->key < currentIntervalStartKey+keyEpsilon) // data point is still within same pixel, so skip it and expand value span of this cluster if necessary
        {
          if (it->value < minValue)
            minValue = it->value;
          else if (it->value > maxValue)
            maxValue = it->value;
          ++intervalDataCount;
        } else // new pixel interval started
        {
          if (intervalDataCount >= 2) // last pixel had multiple data points, consolidate them to a cluster
          {
            if (lastIntervalEndKey < currentIntervalStartKey-keyEpsilon) // last point is further away, so first point of this cluster must be at a real data point
              lineData->append(QCPGraphData(currentIntervalStartKey+keyEpsilon*0.2, currentIntervalFirstPoint->value));
            lineData->append(QCPGraphData(currentIntervalStartKey+keyEpsilon*0.25, minValue));
            lineData->append(QCPGraphData(currentIntervalStartKey+keyEpsilon*0.75, maxValue));
            if (it->key > currentIntervalStartKey+keyEpsilon*2) // new pixel started further away from previous cluster, so make sure the last point of the cluster is at a real data point
              lineData->append(QCPGraphData(currentIntervalStartKey+keyEpsilon*0.8, (it-1)->value));
          } else
            lineData->append(QCPGraphData(currentIntervalFirstPoint->key, currentIntervalFirstPoint->value));
          lastIntervalEndKey = (it-1)->key;
          minValue = it->value;
          maxValue = it->value;
          currentIntervalFirstPoint = it;
          currentIntervalStartKey = keyAxis->pixelToCoord((int)(keyAxis->coordToPixel(it->key)+reversedRound));
          if (keyEpsilonVariable)
            keyEpsilon = qAbs(currentIntervalStartKey-keyAxis->pixelToCoord(keyAxis->coordToPixel(currentIntervalStartKey)+1.0*reversedFactor));
          intervalDataCount = 1;
        }
        ++it;
      }
      // handle last interval:
      if (intervalDataCount >= 2) // last pixel had multiple data points, consolidate them to a cluster
      {
        if (lastIntervalEndKey < currentIntervalStartKey-keyEpsilon) // last point wasn't a cluster, so first point of this cluster must be at a real data point
          lineData->append(QCPGraphData(currentIntervalStartKey+keyEpsilon*0.2, currentIntervalFirstPoint->value));
        lineData->append(QCPGraphData(currentIntervalStartKey+keyEpsilon*0.25, minValue));
        lineData->append(QCPGraphData(currentIntervalStartKey+keyEpsilon*0.75, maxValue));
      } else
        lineData->append(QCPGraphData(currentIntervalFirstPoint->key, currentIntervalFirstPoint->value));
    }
    
    if (scatterData)
    {
      double valueMaxRange = valueAxis->range().upper;
      double valueMinRange = valueAxis->range().lower;
      QCPGraphDataContainer::const_iterator it = lower;
      double minValue = it->value;
      double maxValue = it->value;
      QCPGraphDataContainer::const_iterator minValueIt = it;
      QCPGraphDataContainer::const_iterator maxValueIt = it;
      QCPGraphDataContainer::const_iterator currentIntervalStart = it;
      int reversedFactor = keyAxis->rangeReversed() ? -1 : 1; // is used to calculate keyEpsilon pixel into the correct direction
      int reversedRound = keyAxis->rangeReversed() ? 1 : 0; // is used to switch between floor (normal) and ceil (reversed) rounding of currentIntervalStartKey
      double currentIntervalStartKey = keyAxis->pixelToCoord((int)(keyAxis->coordToPixel(lower->key)+reversedRound));
      double keyEpsilon = qAbs(currentIntervalStartKey-keyAxis->pixelToCoord(keyAxis->coordToPixel(currentIntervalStartKey)+1.0*reversedFactor)); // interval of one pixel on screen when mapped to plot key coordinates
      bool keyEpsilonVariable = keyAxis->scaleType() == QCPAxis::stLogarithmic; // indicates whether keyEpsilon needs to be updated after every interval (for log axes)
      int intervalDataCount = 1;
      ++it; // advance iterator to second data point because adaptive sampling works in 1 point retrospect
      while (it != upperEnd)
      {
        if (it->key < currentIntervalStartKey+keyEpsilon) // data point is still within same pixel, so skip it and expand value span of this pixel if necessary
        {
          if (it->value < minValue && it->value > valueMinRange && it->value < valueMaxRange)
          {
            minValue = it->value;
            minValueIt = it;
          } else if (it->value > maxValue && it->value > valueMinRange && it->value < valueMaxRange)
          {
            maxValue = it->value;
            maxValueIt = it;
          }
          ++intervalDataCount;
        } else // new pixel started
        {
          if (intervalDataCount >= 2) // last pixel had multiple data points, consolidate them
          {
            // determine value pixel span and add as many points in interval to maintain certain vertical data density (this is specific to scatter plot):
            double valuePixelSpan = qAbs(valueAxis->coordToPixel(minValue)-valueAxis->coordToPixel(maxValue));
            int dataModulo = qMax(1, qRound(intervalDataCount/(valuePixelSpan/4.0))); // approximately every 4 value pixels one data point on average
            QCPGraphDataContainer::const_iterator intervalIt = currentIntervalStart;
            int c = 0;
            while (intervalIt != it)
            {
              if ((c % dataModulo == 0 || intervalIt == minValueIt || intervalIt == maxValueIt) && intervalIt->value > valueMinRange && intervalIt->value < valueMaxRange)
                scatterData->append(*intervalIt);
              ++c;
              ++intervalIt;
            }
          } else if (currentIntervalStart->value > valueMinRange && currentIntervalStart->value < valueMaxRange)
            scatterData->append(*currentIntervalStart);
          minValue = it->value;
          maxValue = it->value;
          currentIntervalStart = it;
          currentIntervalStartKey = keyAxis->pixelToCoord((int)(keyAxis->coordToPixel(it->key)+reversedRound));
          if (keyEpsilonVariable)
            keyEpsilon = qAbs(currentIntervalStartKey-keyAxis->pixelToCoord(keyAxis->coordToPixel(currentIntervalStartKey)+1.0*reversedFactor));
          intervalDataCount = 1;
        }
        ++it;
      }
      // handle last interval:
      if (intervalDataCount >= 2) // last pixel had multiple data points, consolidate them
      {
        // determine value pixel span and add as many points in interval to maintain certain vertical data density (this is specific to scatter plot):
        double valuePixelSpan = qAbs(valueAxis->coordToPixel(minValue)-valueAxis->coordToPixel(maxValue));
        int dataModulo = qMax(1, qRound(intervalDataCount/(valuePixelSpan/4.0))); // approximately every 4 value pixels one data point on average
        QCPGraphDataContainer::const_iterator intervalIt = currentIntervalStart;
        int c = 0;
        while (intervalIt != it)
        {
          if ((c % dataModulo == 0 || intervalIt == minValueIt || intervalIt == maxValueIt) && intervalIt->value > valueMinRange && intervalIt->value < valueMaxRange)
            scatterData->append(*intervalIt);
          ++c;
          ++intervalIt;
        }
      } else if (currentIntervalStart->value > valueMinRange && currentIntervalStart->value < valueMaxRange)
        scatterData->append(*currentIntervalStart);
    }
  } else // don't use adaptive sampling algorithm, transfer points one-to-one from the map into the output parameters
  {
    QVector<QCPGraphData> *dataVector = 0;
    if (lineData)
      dataVector = lineData;
    else if (scatterData)
      dataVector = scatterData;
    if (dataVector)
    {
      QCPGraphDataContainer::const_iterator it = lower;
      dataVector->reserve(dataCount+2); // +2 for possible fill end points
      while (it != upperEnd)
      {
        dataVector->append(*it);
        ++it;
      }
    }
    if (lineData && scatterData)
      *scatterData = *dataVector;
  }
}

/*!  \internal
  
  called by \ref getPreparedData to determine which data (key) range is visible at the current key
  axis range setting, so only that needs to be processed.
  
  \a lower returns an iterator to the lowest data point that needs to be taken into account when
  plotting. Note that in order to get a clean plot all the way to the edge of the axis rect, \a
  lower may still be just outside the visible range.
  
  \a upper returns the iterator just above the highest data point that needs to
  be taken into account. Same as before, \a upper may also lie just outside of
  the visible range.

  if the graph contains no data, both \a lower and \a upper point to \c constEnd.
*/
void QCPGraph::getVisibleDataBounds(QCPGraphDataContainer::const_iterator &begin, QCPGraphDataContainer::const_iterator &end) const
{
  if (!mKeyAxis) { qDebug() << Q_FUNC_INFO << "invalid key axis"; return; }
  begin = mDataContainer->findBeginBelowKey(mKeyAxis.data()->range().lower);
  end = mDataContainer->findEndAboveKey(mKeyAxis.data()->range().upper);
}

/*! \internal
  
  The line data vector generated by e.g. getLinePlotData contains only the line that connects the
  data points. If the graph needs to be filled, two additional points need to be added at the
  value-zero-line in the lower and upper key positions of the graph. This function calculates these
  points and adds them to the end of \a lineData. Since the fill is typically drawn before the line
  stroke, these added points need to be removed again after the fill is done, with the
  removeFillBasePoints function.
  
  The expanding of \a lineData by two points will not cause unnecessary memory reallocations,
  because the data vector generation functions (getLinePlotData etc.) reserve two extra points when
  they allocate memory for \a lineData.
  
  \see removeFillBasePoints, lowerFillBasePoint, upperFillBasePoint
*/
void QCPGraph::addFillBasePoints(QVector<QPointF> *lineData) const
{
  if (!mKeyAxis) { qDebug() << Q_FUNC_INFO << "invalid key axis"; return; }
  if (!lineData) { qDebug() << Q_FUNC_INFO << "passed null as lineData"; return; }
  if (lineData->isEmpty()) return;
  
  // append points that close the polygon fill at the key axis:
  if (mKeyAxis.data()->orientation() == Qt::Vertical)
  {
    *lineData << upperFillBasePoint(lineData->last().y());
    *lineData << lowerFillBasePoint(lineData->first().y());
  } else
  {
    *lineData << upperFillBasePoint(lineData->last().x());
    *lineData << lowerFillBasePoint(lineData->first().x());
  }
}

/*! \internal
  
  removes the two points from \a lineData that were added by \ref addFillBasePoints.
  
  \see addFillBasePoints, lowerFillBasePoint, upperFillBasePoint
*/
void QCPGraph::removeFillBasePoints(QVector<QPointF> *lineData) const
{
  if (!lineData) { qDebug() << Q_FUNC_INFO << "passed null as lineData"; return; }
  if (lineData->isEmpty()) return;
  
  lineData->remove(lineData->size()-2, 2);
}

/*! \internal
  
  called by \ref addFillBasePoints to conveniently assign the point which closes the fill polygon
  on the lower side of the zero-value-line parallel to the key axis. The logarithmic axis scale
  case is a bit special, since the zero-value-line in pixel coordinates is in positive or negative
  infinity. So this case is handled separately by just closing the fill polygon on the axis which
  lies in the direction towards the zero value.
  
  \a lowerKey will be the the key (in pixels) of the returned point. Depending on whether the key
  axis is horizontal or vertical, \a lowerKey will end up as the x or y value of the returned
  point, respectively.
  
  \see upperFillBasePoint, addFillBasePoints
*/
QPointF QCPGraph::lowerFillBasePoint(double lowerKey) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return QPointF(); }
  
  QPointF point;
  if (valueAxis->scaleType() == QCPAxis::stLinear)
  {
    if (keyAxis->axisType() == QCPAxis::atLeft)
    {
      point.setX(valueAxis->coordToPixel(0));
      point.setY(lowerKey);
    } else if (keyAxis->axisType() == QCPAxis::atRight)
    {
      point.setX(valueAxis->coordToPixel(0));
      point.setY(lowerKey);
    } else if (keyAxis->axisType() == QCPAxis::atTop)
    {
      point.setX(lowerKey);
      point.setY(valueAxis->coordToPixel(0));
    } else if (keyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(lowerKey);
      point.setY(valueAxis->coordToPixel(0));
    }
  } else // valueAxis->mScaleType == QCPAxis::stLogarithmic
  {
    // In logarithmic scaling we can't just draw to value zero so we just fill all the way
    // to the axis which is in the direction towards zero
    if (keyAxis->orientation() == Qt::Vertical)
    {
      if ((valueAxis->range().upper < 0 && !valueAxis->rangeReversed()) ||
          (valueAxis->range().upper > 0 && valueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setX(keyAxis->axisRect()->right());
      else
        point.setX(keyAxis->axisRect()->left());
      point.setY(lowerKey);
    } else if (keyAxis->axisType() == QCPAxis::atTop || keyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(lowerKey);
      if ((valueAxis->range().upper < 0 && !valueAxis->rangeReversed()) ||
          (valueAxis->range().upper > 0 && valueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setY(keyAxis->axisRect()->top());
      else
        point.setY(keyAxis->axisRect()->bottom());
    }
  }
  return point;
}

/*! \internal
  
  called by \ref addFillBasePoints to conveniently assign the point which closes the fill
  polygon on the upper side of the zero-value-line parallel to the key axis. The logarithmic axis
  scale case is a bit special, since the zero-value-line in pixel coordinates is in positive or
  negative infinity. So this case is handled separately by just closing the fill polygon on the
  axis which lies in the direction towards the zero value.

  \a upperKey will be the the key (in pixels) of the returned point. Depending on whether the key
  axis is horizontal or vertical, \a upperKey will end up as the x or y value of the returned
  point, respectively.
  
  \see lowerFillBasePoint, addFillBasePoints
*/
QPointF QCPGraph::upperFillBasePoint(double upperKey) const
{
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return QPointF(); }
  
  QPointF point;
  if (valueAxis->scaleType() == QCPAxis::stLinear)
  {
    if (keyAxis->axisType() == QCPAxis::atLeft)
    {
      point.setX(valueAxis->coordToPixel(0));
      point.setY(upperKey);
    } else if (keyAxis->axisType() == QCPAxis::atRight)
    {
      point.setX(valueAxis->coordToPixel(0));
      point.setY(upperKey);
    } else if (keyAxis->axisType() == QCPAxis::atTop)
    {
      point.setX(upperKey);
      point.setY(valueAxis->coordToPixel(0));
    } else if (keyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(upperKey);
      point.setY(valueAxis->coordToPixel(0));
    }
  } else // valueAxis->mScaleType == QCPAxis::stLogarithmic
  {
    // In logarithmic scaling we can't just draw to value 0 so we just fill all the way
    // to the axis which is in the direction towards 0
    if (keyAxis->orientation() == Qt::Vertical)
    {
      if ((valueAxis->range().upper < 0 && !valueAxis->rangeReversed()) ||
          (valueAxis->range().upper > 0 && valueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setX(keyAxis->axisRect()->right());
      else
        point.setX(keyAxis->axisRect()->left());
      point.setY(upperKey);
    } else if (keyAxis->axisType() == QCPAxis::atTop || keyAxis->axisType() == QCPAxis::atBottom)
    {
      point.setX(upperKey);
      if ((valueAxis->range().upper < 0 && !valueAxis->rangeReversed()) ||
          (valueAxis->range().upper > 0 && valueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
        point.setY(keyAxis->axisRect()->top());
      else
        point.setY(keyAxis->axisRect()->bottom());
    }
  }
  return point;
}

/*! \internal
  
  Generates the polygon needed for drawing channel fills between this graph (data passed via \a
  lineData) and the graph specified by mChannelFillGraph (data generated by calling its \ref
  getPlotData function). May return an empty polygon if the key ranges have no overlap or fill
  target graph and this graph don't have same orientation (i.e. both key axes horizontal or both
  key axes vertical). For increased performance (due to implicit sharing), keep the returned
  QPolygonF const.
*/
const QPolygonF QCPGraph::getChannelFillPolygon(const QVector<QPointF> *lineData) const
{
  if (!mChannelFillGraph)
    return QPolygonF();
  
  QCPAxis *keyAxis = mKeyAxis.data();
  QCPAxis *valueAxis = mValueAxis.data();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return QPolygonF(); }
  if (!mChannelFillGraph.data()->mKeyAxis) { qDebug() << Q_FUNC_INFO << "channel fill target key axis invalid"; return QPolygonF(); }
  
  if (mChannelFillGraph.data()->mKeyAxis.data()->orientation() != keyAxis->orientation())
    return QPolygonF(); // don't have same axis orientation, can't fill that (Note: if keyAxis fits, valueAxis will fit too, because it's always orthogonal to keyAxis)
  
  if (lineData->isEmpty()) return QPolygonF();
  QVector<QPointF> otherData;
  mChannelFillGraph.data()->getPlotData(&otherData, 0);
  if (otherData.isEmpty()) return QPolygonF();
  QVector<QPointF> thisData;
  thisData.reserve(lineData->size()+otherData.size()); // because we will join both vectors at end of this function
  for (int i=0; i<lineData->size(); ++i) // don't use the vector<<(vector),  it squeezes internally, which ruins the performance tuning with reserve()
    thisData << lineData->at(i);
  
  // pointers to be able to swap them, depending which data range needs cropping:
  QVector<QPointF> *staticData = &thisData;
  QVector<QPointF> *croppedData = &otherData;
  
  // crop both vectors to ranges in which the keys overlap (which coord is key, depends on axisType):
  if (keyAxis->orientation() == Qt::Horizontal)
  {
    // x is key
    // if an axis range is reversed, the data point keys will be descending. Reverse them, since following algorithm assumes ascending keys:
    if (staticData->first().x() > staticData->last().x())
    {
      int size = staticData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*staticData)[i], (*staticData)[size-1-i]);
    }
    if (croppedData->first().x() > croppedData->last().x())
    {
      int size = croppedData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*croppedData)[i], (*croppedData)[size-1-i]);
    }
    // crop lower bound:
    if (staticData->first().x() < croppedData->first().x()) // other one must be cropped
      qSwap(staticData, croppedData);
    int lowBound = findIndexBelowX(croppedData, staticData->first().x());
    if (lowBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(0, lowBound);
    // set lowest point of cropped data to fit exactly key position of first static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    double slope;
    if (croppedData->at(1).x()-croppedData->at(0).x() != 0)
      slope = (croppedData->at(1).y()-croppedData->at(0).y())/(croppedData->at(1).x()-croppedData->at(0).x());
    else
      slope = 0;
    (*croppedData)[0].setY(croppedData->at(0).y()+slope*(staticData->first().x()-croppedData->at(0).x()));
    (*croppedData)[0].setX(staticData->first().x());
    
    // crop upper bound:
    if (staticData->last().x() > croppedData->last().x()) // other one must be cropped
      qSwap(staticData, croppedData);
    int highBound = findIndexAboveX(croppedData, staticData->last().x());
    if (highBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
    // set highest point of cropped data to fit exactly key position of last static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    int li = croppedData->size()-1; // last index
    if (croppedData->at(li).x()-croppedData->at(li-1).x() != 0)
      slope = (croppedData->at(li).y()-croppedData->at(li-1).y())/(croppedData->at(li).x()-croppedData->at(li-1).x());
    else
      slope = 0;
    (*croppedData)[li].setY(croppedData->at(li-1).y()+slope*(staticData->last().x()-croppedData->at(li-1).x()));
    (*croppedData)[li].setX(staticData->last().x());
  } else // mKeyAxis->orientation() == Qt::Vertical
  {
    // y is key
    // similar to "x is key" but switched x,y. Further, lower/upper meaning is inverted compared to x,
    // because in pixel coordinates, y increases from top to bottom, not bottom to top like data coordinate.
    // if an axis range is reversed, the data point keys will be descending. Reverse them, since following algorithm assumes ascending keys:
    if (staticData->first().y() < staticData->last().y())
    {
      int size = staticData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*staticData)[i], (*staticData)[size-1-i]);
    }
    if (croppedData->first().y() < croppedData->last().y())
    {
      int size = croppedData->size();
      for (int i=0; i<size/2; ++i)
        qSwap((*croppedData)[i], (*croppedData)[size-1-i]);
    }
    // crop lower bound:
    if (staticData->first().y() > croppedData->first().y()) // other one must be cropped
      qSwap(staticData, croppedData);
    int lowBound = findIndexAboveY(croppedData, staticData->first().y());
    if (lowBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(0, lowBound);
    // set lowest point of cropped data to fit exactly key position of first static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    double slope;
    if (croppedData->at(1).y()-croppedData->at(0).y() != 0) // avoid division by zero in step plots
      slope = (croppedData->at(1).x()-croppedData->at(0).x())/(croppedData->at(1).y()-croppedData->at(0).y());
    else
      slope = 0;
    (*croppedData)[0].setX(croppedData->at(0).x()+slope*(staticData->first().y()-croppedData->at(0).y()));
    (*croppedData)[0].setY(staticData->first().y());
    
    // crop upper bound:
    if (staticData->last().y() < croppedData->last().y()) // other one must be cropped
      qSwap(staticData, croppedData);
    int highBound = findIndexBelowY(croppedData, staticData->last().y());
    if (highBound == -1) return QPolygonF(); // key ranges have no overlap
    croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
    // set highest point of cropped data to fit exactly key position of last static data
    // point via linear interpolation:
    if (croppedData->size() < 2) return QPolygonF(); // need at least two points for interpolation
    int li = croppedData->size()-1; // last index
    if (croppedData->at(li).y()-croppedData->at(li-1).y() != 0) // avoid division by zero in step plots
      slope = (croppedData->at(li).x()-croppedData->at(li-1).x())/(croppedData->at(li).y()-croppedData->at(li-1).y());
    else
      slope = 0;
    (*croppedData)[li].setX(croppedData->at(li-1).x()+slope*(staticData->last().y()-croppedData->at(li-1).y()));
    (*croppedData)[li].setY(staticData->last().y());
  }
  
  // return joined:
  for (int i=otherData.size()-1; i>=0; --i) // insert reversed, otherwise the polygon will be twisted
    thisData << otherData.at(i);
  return QPolygonF(thisData);
}

/*! \internal
  
  Finds the smallest index of \a data, whose points x value is just above \a x. Assumes x values in
  \a data points are ordered ascending, as is the case when plotting with horizontal key axis.

  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexAboveX(const QVector<QPointF> *data, double x) const
{
  for (int i=data->size()-1; i>=0; --i)
  {
    if (data->at(i).x() < x)
    {
      if (i<data->size()-1)
        return i+1;
      else
        return data->size()-1;
    }
  }
  return -1;
}

/*! \internal
  
  Finds the highest index of \a data, whose points x value is just below \a x. Assumes x values in
  \a data points are ordered ascending, as is the case when plotting with horizontal key axis.
  
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexBelowX(const QVector<QPointF> *data, double x) const
{
  for (int i=0; i<data->size(); ++i)
  {
    if (data->at(i).x() > x)
    {
      if (i>0)
        return i-1;
      else
        return 0;
    }
  }
  return -1;
}

/*! \internal
  
  Finds the smallest index of \a data, whose points y value is just above \a y. Assumes y values in
  \a data points are ordered descending, as is the case when plotting with vertical key axis.
  
  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexAboveY(const QVector<QPointF> *data, double y) const
{
  for (int i=0; i<data->size(); ++i)
  {
    if (data->at(i).y() < y)
    {
      if (i>0)
        return i-1;
      else
        return 0;
    }
  }
  return -1;
}

/*! \internal
  
  Calculates the (minimum) distance (in pixels) the graph's representation has from the given \a
  pixelPoint in pixels. This is used to determine whether the graph was clicked or not, e.g. in
  \ref selectTest.
  
  If either the graph has no data or if the line style is \ref lsNone and the scatter style's shape
  is \ref QCPScatterStyle::ssNone (i.e. there is no visual representation of the graph), returns -1.0.
*/
double QCPGraph::pointDistance(const QPointF &pixelPoint) const
{
  if (mDataContainer->isEmpty())
    return -1.0;
  if (mLineStyle == lsNone && mScatterStyle.isNone())
    return -1.0;
  
  // calculate minimum distances to graph representation:
  if (mLineStyle == lsNone)
  {
    // no line displayed, only calculate distance to scatter points:
    QVector<QCPGraphData> scatterData;
    getScatterPlotData(&scatterData);
    if (scatterData.size() > 0)
    {
      double minDistSqr = std::numeric_limits<double>::max();
      for (int i=0; i<scatterData.size(); ++i)
      {
        double currentDistSqr = QCPVector2D(coordsToPixels(scatterData.at(i).key, scatterData.at(i).value)-pixelPoint).lengthSquared();
        if (currentDistSqr < minDistSqr)
          minDistSqr = currentDistSqr;
      }
      return qSqrt(minDistSqr);
    } else // no data available in view to calculate distance to
      return -1.0;
  } else
  {
    // line displayed, calculate distance to line segments:
    QVector<QPointF> lineData;
    getPlotData(&lineData, 0); // unlike with getScatterPlotData we get pixel coordinates here
    if (lineData.size() > 1) // at least one line segment, compare distance to line segments
    {
      double minDistSqr = std::numeric_limits<double>::max();
      if (mLineStyle == lsImpulse)
      {
        // impulse plot differs from other line styles in that the lineData points are only pairwise connected:
        QCPVector2D p(pixelPoint);
        for (int i=0; i<lineData.size()-1; i+=2) // iterate pairs
        {
          double currentDistSqr = p.distanceSquaredToLine(lineData.at(i), lineData.at(i+1));
          if (currentDistSqr < minDistSqr)
            minDistSqr = currentDistSqr;
        }
      } else
      {
        // all other line plots (line and step) connect points directly:
        QCPVector2D p(pixelPoint);
        for (int i=0; i<lineData.size()-1; ++i)
        {
          double currentDistSqr = p.distanceSquaredToLine(lineData.at(i), lineData.at(i+1));
          if (currentDistSqr < minDistSqr)
            minDistSqr = currentDistSqr;
        }
      }
      return qSqrt(minDistSqr);
    } else if (lineData.size() > 0) // only single data point, calculate distance to that point
    {
      return QCPVector2D(lineData.at(0)-pixelPoint).length();
    } else // no data available in view to calculate distance to
      return -1.0;
  }
}

/*! \internal
  
  Finds the highest index of \a data, whose points y value is just below \a y. Assumes y values in
  \a data points are ordered descending, as is the case when plotting with vertical key axis (since
  keys are ordered ascending).

  Used to calculate the channel fill polygon, see \ref getChannelFillPolygon.
*/
int QCPGraph::findIndexBelowY(const QVector<QPointF> *data, double y) const
{
  for (int i=data->size()-1; i>=0; --i)
  {
    if (data->at(i).y() > y)
    {
      if (i<data->size()-1)
        return i+1;
      else
        return data->size()-1;
    }
  }
  return -1;
}

/* inherits documentation from base class */
QCPRange QCPGraph::getKeyRange(bool &foundRange, QCP::SignDomain inSignDomain) const
{
  return mDataContainer->keyRange(foundRange, inSignDomain);
}

/* inherits documentation from base class */
QCPRange QCPGraph::getValueRange(bool &foundRange, QCP::SignDomain inSignDomain) const
{
  return mDataContainer->valueRange(foundRange, inSignDomain);
}
