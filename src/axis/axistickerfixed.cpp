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

#include "axistickerfixed.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPAxisTickerFixed
////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \class QCPAxisTickerFixed
  \brief 
  
  
*/

QCPAxisTickerFixed::QCPAxisTickerFixed() :
  mTickStep(1.0),
  mScaleStrategy(ssNone)
{
}

void QCPAxisTickerFixed::setTickStep(double step)
{
  if (step > 0)
    mTickStep = step;
  else
    qDebug() << Q_FUNC_INFO << "tick step must be greater than zero:" << step;
}

void QCPAxisTickerFixed::setScaleStrategy(QCPAxisTickerFixed::ScaleStrategy strategy)
{
  mScaleStrategy = strategy;
}

double QCPAxisTickerFixed::getTickStep(const QCPRange &range)
{
  switch (mScaleStrategy)
  {
    case ssNone:
    {
      return mTickStep;
    }
    case ssMultiples:
    {
      double exactStep = range.size()/(double)(mTickCount+1e-10); // mTickCount ticks on average, the small addition is to prevent jitter on exact integers
      if (exactStep < mTickStep)
        return mTickStep;
      else
        return (qint64)(cleanMantissa(exactStep/mTickStep)+0.5)*mTickStep;
    }
    case ssPowers:
    {
      double exactStep = range.size()/(double)(mTickCount+1e-10); // mTickCount ticks on average, the small addition is to prevent jitter on exact integers
      return qPow(mTickStep, (int)(qLn(exactStep)/qLn(mTickStep)+0.5));
      break;
    }
  }
  return mTickStep;
}
