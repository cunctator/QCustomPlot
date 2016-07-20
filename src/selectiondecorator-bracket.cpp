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

#include "selectiondecorator-bracket.h"

#include "plottable1d.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPSelectionDecoratorBracket
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPSelectionDecoratorBracket
  \brief 
  
*/

QCPSelectionDecoratorBracket::QCPSelectionDecoratorBracket() :
  mBracketPen(QPen(Qt::black)),
  mBracketBrush(Qt::NoBrush),
  mBracketWidth(5),
  mBracketHeight(50),
  mBracketStyle(bsSquareBracket),
  mTangentToData(false),
  mTangentAverage(2)
{
  
}

QCPSelectionDecoratorBracket::~QCPSelectionDecoratorBracket()
{
}

void QCPSelectionDecoratorBracket::setBracketPen(const QPen &pen)
{
  mBracketPen = pen;
}

void QCPSelectionDecoratorBracket::setBracketBrush(const QBrush &brush)
{
  mBracketBrush = brush;
}

void QCPSelectionDecoratorBracket::setBracketWidth(int width)
{
  mBracketWidth = width;
}

void QCPSelectionDecoratorBracket::setBracketHeight(int height)
{
  mBracketHeight = height;
}

void QCPSelectionDecoratorBracket::setBracketStyle(QCPSelectionDecoratorBracket::BracketStyle style)
{
  mBracketStyle = style;
}

void QCPSelectionDecoratorBracket::setTangentToData(bool enabled)
{
  mTangentToData = enabled;
}

void QCPSelectionDecoratorBracket::setTangentAverage(int pointCount)
{
  mTangentAverage = pointCount;
  if (mTangentAverage < 1)
    mTangentAverage = 1;
}

void QCPSelectionDecoratorBracket::drawBracket(QCPPainter *painter, int direction) const
{
  switch (mBracketStyle)
  {
    case bsSquareBracket:
    {
      painter->drawLine(QLineF(mBracketWidth*direction, -mBracketHeight*0.5, 0, -mBracketHeight*0.5));
      painter->drawLine(QLineF(mBracketWidth*direction, mBracketHeight*0.5, 0, mBracketHeight*0.5));
      painter->drawLine(QLineF(0, -mBracketHeight*0.5, 0, mBracketHeight*0.5));
      break;
    }
    case bsHalfEllipse:
    {
      painter->drawArc(-mBracketWidth*0.5, -mBracketHeight*0.5, mBracketWidth, mBracketHeight, -90*16, -180*16*direction);
      break;
    }
    case bsEllipse:
    {
      painter->drawEllipse(-mBracketWidth*0.5, -mBracketHeight*0.5, mBracketWidth, mBracketHeight);
      break;
    }
    case bsPlus:
    {
      painter->drawLine(QLineF(0, -mBracketHeight*0.5, 0, mBracketHeight*0.5));
      painter->drawLine(QLineF(-mBracketWidth*0.5, 0, mBracketWidth*0.5, 0));
      break;
    }
    default:
    {
      qDebug() << Q_FUNC_INFO << "unknown/custom bracket style can't be handeld by default implementation:" << static_cast<int>(mBracketStyle);
      break;
    }
  }
}

void QCPSelectionDecoratorBracket::drawDecoration(QCPPainter *painter, QCPDataSelection selection)
{
  if (!mPlottable || selection.isEmpty()) return;
  
  if (QCPPlottableInterface1D *interface1d = mPlottable->interface1D())
  {
    foreach (const QCPDataRange &dataRange, selection.dataRanges())
    {
      // determine position and (if tangent mode is enabled) angle of brackets:
      int openBracketDir = (mPlottable->keyAxis() && !mPlottable->keyAxis()->rangeReversed()) ? 1 : -1;
      int closeBracketDir = -openBracketDir;
      QPointF openBracketPos = getPixelCoordinates(interface1d, dataRange.begin());
      QPointF closeBracketPos = getPixelCoordinates(interface1d, dataRange.end()-1);
      double openBracketAngle = 0;
      double closeBracketAngle = 0;
      if (mTangentToData)
      {
        openBracketAngle = getTangentAngle(interface1d, dataRange.begin(), openBracketDir);
        closeBracketAngle = getTangentAngle(interface1d, dataRange.end()-1, closeBracketDir);
      }
      // draw opening bracket:
      QTransform oldTransform = painter->transform();
      painter->setPen(mBracketPen);
      painter->setBrush(mBracketBrush);
      painter->translate(openBracketPos);
      painter->rotate(openBracketAngle/M_PI*180.0);
      drawBracket(painter, openBracketDir);
      painter->setTransform(oldTransform);
      // draw closing bracket:
      painter->setPen(mBracketPen);
      painter->setBrush(mBracketBrush);
      painter->translate(closeBracketPos);
      painter->rotate(closeBracketAngle/M_PI*180.0);
      drawBracket(painter, closeBracketDir);
      painter->setTransform(oldTransform);
    }
  }
}

double QCPSelectionDecoratorBracket::getTangentAngle(const QCPPlottableInterface1D *interface1d, int dataIndex, int direction) const
{
  if (!interface1d || dataIndex < 0 || dataIndex >= interface1d->dataCount())
    return 0;
  direction = direction < 0 ? -1 : 1; // enforce direction is either -1 or 1
  
  // how many steps we can actually go from index in the given direction without exceeding data bounds:
  int averageCount;
  if (direction < 0)
    averageCount = qMin(mTangentAverage, dataIndex);
  else
    averageCount = qMin(mTangentAverage, interface1d->dataCount()-1-dataIndex);
  qDebug() << averageCount;
  // calculate point average of averageCount points:
  QVector<QPointF> points(averageCount);
  QPointF pointsAverage;
  int currentIndex = dataIndex;
  for (int i=0; i<averageCount; ++i)
  {
    points[i] = getPixelCoordinates(interface1d, currentIndex);
    pointsAverage += points[i];
    currentIndex += direction;
  }
  pointsAverage /= (double)averageCount;
  
  // calculate slope of linear regression through points:
  double numSum = 0;
  double denomSum = 0;
  for (int i=0; i<averageCount; ++i)
  {
    const double dx = points.at(i).x()-pointsAverage.x();
    const double dy = points.at(i).y()-pointsAverage.y();
    numSum += dx*dy;
    denomSum += dx*dx;
  }
  if (!qFuzzyIsNull(denomSum) && !qFuzzyIsNull(numSum))
  {
    return qAtan2(numSum, denomSum);
  } else // undetermined angle, probably mTangentAverage == 1, so using only one data point
    return 0;
}

QPointF QCPSelectionDecoratorBracket::getPixelCoordinates(const QCPPlottableInterface1D *interface1d, int dataIndex) const
{
  QCPAxis *keyAxis = mPlottable->keyAxis();
  QCPAxis *valueAxis = mPlottable->valueAxis();
  if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return QPointF(0, 0); }
  
  if (keyAxis->orientation() == Qt::Horizontal)
    return QPointF(keyAxis->coordToPixel(interface1d->dataMainKey(dataIndex)), valueAxis->coordToPixel(interface1d->dataMainValue(dataIndex)));
  else
    return QPointF(valueAxis->coordToPixel(interface1d->dataMainValue(dataIndex)), keyAxis->coordToPixel(interface1d->dataMainKey(dataIndex)));
}








