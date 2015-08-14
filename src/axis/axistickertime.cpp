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

#include "axistickertime.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPAxisTickerTime
////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \class QCPAxisTickerTime
  \brief 
  
  
*/

QCPAxisTickerTime::QCPAxisTickerTime() :
  mDateTimeFormat(QLatin1String("hh:mm:ss\ndd.MM.yy")),
  mDateTimeSpec(Qt::LocalTime)
{
}

void QCPAxisTickerTime::setDateTimeFormat(const QString &format)
{
  mDateTimeFormat = format;
}

void QCPAxisTickerTime::setDateTimeSpec(Qt::TimeSpec spec)
{
  mDateTimeSpec = spec;
}

double QCPAxisTickerTime::getTickStep(const QCPRange &range)
{
  double result = range.size()/(double)(mTickCount+1e-10); // mTickCount ticks on average, the small addition is to prevent jitter on exact integers
  
  
  
  if (result < 3) // ideal tick step is below 3 seconds -> use normal tickstep algorithm
  {
    double magnitudeFactor = qPow(10.0, qFloor(qLn(result)/qLn(10.0))); // get magnitude factor e.g. 0.01, 1, 10, 1000 etc.
    double tickStepMantissa = result/magnitudeFactor;
    if (tickStepMantissa < 5)
      result = (int)(tickStepMantissa*2)/2.0*magnitudeFactor; // round digit after decimal point to 0.5
    else
      result = (int)(tickStepMantissa/2.0)*2.0*magnitudeFactor; // round to first digit in multiples of 2
  } else if (result < 3600)
  {
    // pick the closest step in allowed steps:
    QVector<double> potentialSteps = QVector<double>() << 5 << 10 << 15 << 30 << 60 << 5*60 << 10*60 << 15*60 << 30*60 << 60*60;
    QVector<double>::const_iterator it = std::lower_bound(potentialSteps.constBegin(), potentialSteps.constEnd(), result);
    if (it == potentialSteps.constEnd())
      result = *(--it);
    else if (it == potentialSteps.constBegin())
      result = *it;
    else
      result = result-*(--it) < *it-result ? *(--it) : *it;
  }
  
  
  
      return result;
}

int QCPAxisTickerTime::getSubTickCount(double tickStep)
{
  // TODO
  
  
  
  int result = 1; // default to 1, if no proper value can be found
  
  // get mantissa of tickstep:
  double magnitudeFactor = qPow(10.0, qFloor(qLn(tickStep)/qLn(10.0))); // get magnitude factor e.g. 0.01, 1, 10, 1000 etc.
  double tickStepMantissa = tickStep/magnitudeFactor;
  
  // separate integer and fractional part of mantissa:
  double epsilon = 0.01;
  double intPartf;
  int intPart;
  double fracPart = modf(tickStepMantissa, &intPartf);
  intPart = intPartf;
  
  // handle cases with (almost) integer mantissa:
  if (fracPart < epsilon || 1.0-fracPart < epsilon)
  {
    if (1.0-fracPart < epsilon)
      ++intPart;
    switch (intPart)
    {
      case 1: result = 4; break; // 1.0 -> 0.2 substep
      case 2: result = 3; break; // 2.0 -> 0.5 substep
      case 3: result = 2; break; // 3.0 -> 1.0 substep
      case 4: result = 3; break; // 4.0 -> 1.0 substep
      case 5: result = 4; break; // 5.0 -> 1.0 substep
      case 6: result = 2; break; // 6.0 -> 2.0 substep
      case 7: result = 6; break; // 7.0 -> 1.0 substep
      case 8: result = 3; break; // 8.0 -> 2.0 substep
      case 9: result = 2; break; // 9.0 -> 3.0 substep
    }
  } else
  {
    // handle cases with significantly fractional mantissa:
    if (qAbs(fracPart-0.5) < epsilon) // *.5 mantissa
    {
      switch (intPart)
      {
        case 1: result = 2; break; // 1.5 -> 0.5 substep
        case 2: result = 4; break; // 2.5 -> 0.5 substep
        case 3: result = 4; break; // 3.5 -> 0.7 substep
        case 4: result = 2; break; // 4.5 -> 1.5 substep
        case 5: result = 4; break; // 5.5 -> 1.1 substep (won't occur with autoTickStep from here on)
        case 6: result = 4; break; // 6.5 -> 1.3 substep
        case 7: result = 2; break; // 7.5 -> 2.5 substep
        case 8: result = 4; break; // 8.5 -> 1.7 substep
        case 9: result = 4; break; // 9.5 -> 1.9 substep
      }
    }
    // if mantissa fraction isnt 0.0 or 0.5, don't bother finding good sub tick marks, leave default
  }
  
  return result;
}

QString QCPAxisTickerTime::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
  Q_UNUSED(precision)
  Q_UNUSED(formatChar)
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0) // use fromMSecsSinceEpoch function if available, to gain sub-second accuracy on tick labels (e.g. for format "hh:mm:ss:zzz")
  return locale.toString(QDateTime::fromTime_t(tick).toTimeSpec(mDateTimeSpec), mDateTimeFormat);
#else
  return locale.toString(QDateTime::fromMSecsSinceEpoch(tick*1000).toTimeSpec(mDateTimeSpec), mDateTimeFormat);
#endif
}


/*!
  assumes potentialSteps is not empty
*/

double QCPAxisTickerTime::pickClosestStep(double target, const QVector<double> &potentialSteps) const
{
  if (potentialSteps.size() == 1)
    return potentialSteps.first();
  QVector<double>::const_iterator it = std::lower_bound(potentialSteps.constBegin(), potentialSteps.constEnd(), target);
  if (it == potentialSteps.constEnd())
    return *(--it);
  else if (it == potentialSteps.constBegin())
    return *it;
  else
    return target-*(--it) < *it-target ? *(--it) : *it;
}
