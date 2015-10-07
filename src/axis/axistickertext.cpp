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

#include "axistickertext.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPAxisTickerText
////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \class QCPAxisTickerText
  \brief 
  
  
*/

QCPAxisTickerText::QCPAxisTickerText() :
  mSubTickCount(0)
{
}

void QCPAxisTickerText::setTicks(const QMap<double, QString> &ticks)
{
  mTicks = ticks;
}

void QCPAxisTickerText::setTicks(const QVector<double> &positions, const QVector<QString> labels)
{
  clear();
  addTicks(positions, labels);
}

void QCPAxisTickerText::setSubTickCount(int subTicks)
{
  if (subTicks >= 0)
    mSubTickCount = subTicks;
  else
    qDebug() << Q_FUNC_INFO << "sub tick count can't be negative:" << subTicks;
}

void QCPAxisTickerText::clear()
{
  mTicks.clear();
}

void QCPAxisTickerText::addTick(double position, QString label)
{
  mTicks.insert(position, label);
}

void QCPAxisTickerText::addTicks(const QMap<double, QString> &ticks)
{
  mTicks.unite(ticks);
}

void QCPAxisTickerText::addTicks(const QVector<double> &positions, const QVector<QString> &labels)
{
  if (positions.size() != labels.size())
    qDebug() << Q_FUNC_INFO << "passed unequal length vectors for positions and labels:" << positions.size() << labels.size();
  int n = qMin(positions.size(), labels.size());
  for (int i=0; i<n; ++i)
    mTicks.insert(positions.at(i), labels.at(i));
}

double QCPAxisTickerText::getTickStep(const QCPRange &range)
{
  // text axis ticker has manual tick positions, so doesn't need this method
  Q_UNUSED(range)
  return 1.0;
}

int QCPAxisTickerText::getSubTickCount(double tickStep)
{
  Q_UNUSED(tickStep)
  return mSubTickCount;
}

QString QCPAxisTickerText::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
  Q_UNUSED(locale)
  Q_UNUSED(formatChar)
  Q_UNUSED(precision)
  return mTicks.value(tick);
}

QVector<double> QCPAxisTickerText::createTickVector(double tickStep, const QCPRange &range)
{
  Q_UNUSED(tickStep)
  QVector<double> result;
  if (mTicks.isEmpty())
    return result;
  
  QMap<double, QString>::const_iterator start = mTicks.lowerBound(range.lower);
  QMap<double, QString>::const_iterator end = mTicks.upperBound(range.upper);
  // this method should try to give one tick outside of range so proper subticks can be generated:
  if (start != mTicks.constBegin()) --start;
  if (end != mTicks.constEnd()) ++end;
  for (QMap<double, QString>::const_iterator it = start; it != end; ++it)
    result.append(it.key());
  
  return result;
}
