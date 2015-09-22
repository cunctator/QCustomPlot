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

#include "axistickerlog.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPAxisTickerLog
////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \class QCPAxisTickerLog
  \brief 
  
  
*/

QCPAxisTickerLog::QCPAxisTickerLog() :
  mLogBase(10.0),
  mSubTickCount(8), // generates 10 intervals
  mLogBaseLnInv(1.0/qLn(mLogBase))
{
}

void QCPAxisTickerLog::setLogBase(double base)
{
  if (base > 0)
  {
    mLogBase = base;
    mLogBaseLnInv = 1.0/qLn(mLogBase);
  } else
    qDebug() << Q_FUNC_INFO << "log base has to be greater than zero:" << base;
}

void QCPAxisTickerLog::setSubTickCount(int subTicks)
{
  if (subTicks >= 0)
    mSubTickCount = subTicks;
  else
    qDebug() << Q_FUNC_INFO << "sub tick count can't be negative:" << subTicks;
}

double QCPAxisTickerLog::getTickStep(const QCPRange &range)
{
  // Logarithmic axis ticker has unequal tick spacing, so doesn't need this method
  Q_UNUSED(range)
  return 1.0;
}

int QCPAxisTickerLog::getSubTickCount(double tickStep)
{
  Q_UNUSED(tickStep)
  return mSubTickCount;
}

QVector<double> QCPAxisTickerLog::createTickVector(double tickStep, const QCPRange &range)
{
  Q_UNUSED(tickStep)
  QVector<double> result;
  if (range.lower > 0 && range.upper > 0) // positive range
  {
    double currentTick = qPow(mLogBase, qFloor(qLn(range.lower)*mLogBaseLnInv));
    result.append(currentTick);
    while (currentTick < range.upper && currentTick > 0) // currentMag might be zero for ranges ~1e-300, just cancel in that case
    {
      currentTick *= mLogBase;
      result.append(currentTick);
    }
  } else if (range.lower < 0 && range.upper < 0) // negative range
  {
    double currentTick = -qPow(mLogBase, qCeil(qLn(-range.lower)*mLogBaseLnInv));
    result.append(currentTick);
    while (currentTick < range.upper && currentTick < 0) // currentMag might be zero for ranges ~1e-300, just cancel in that case
    {
      currentTick /= mLogBase;
      result.append(currentTick);
    }
  } else // invalid range for logarithmic scale, because lower and upper have different sign
  {
    qDebug() << Q_FUNC_INFO << "Invalid range for logarithmic plot: " << range.lower << ".." << range.upper;
  }
  
  return result;
}
