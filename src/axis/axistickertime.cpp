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
  mTimeFormat(QLatin1String("%h:%m:%s")),
  mSmallestUnit(tuSeconds),
  mBiggestUnit(tuHours)
{
  setTickCount(4);
  mFieldWidth[tuMilliseconds] = 3;
  mFieldWidth[tuSeconds] = 2;
  mFieldWidth[tuMinutes] = 2;
  mFieldWidth[tuHours] = 2;
  mFieldWidth[tuDays] = 1;
  
  mFormatPattern[tuMilliseconds] = QLatin1String("%z");
  mFormatPattern[tuSeconds] = QLatin1String("%s");
  mFormatPattern[tuMinutes] = QLatin1String("%m");
  mFormatPattern[tuHours] = QLatin1String("%h");
  mFormatPattern[tuDays] = QLatin1String("%d");
}

void QCPAxisTickerTime::setTimeFormat(const QString &format)
{
  mTimeFormat = format;
  
  // determine smallest and biggest unit in format, to optimize unit replacement and allow biggest
  // unit to consume remaining time of a tick value and grow beyond its modulo (e.g. min > 59)
  mSmallestUnit = tuMilliseconds;
  mBiggestUnit = tuMilliseconds;
  bool hasSmallest = false;
  for (int i = tuMilliseconds; i <= tuDays; ++i)
  {
    TimeUnit unit = static_cast<TimeUnit>(i);
    if (mTimeFormat.contains(mFormatPattern.value(unit)))
    {
      if (!hasSmallest)
      {
        mSmallestUnit = unit;
        hasSmallest = true;
      }
      mBiggestUnit = unit;
    }
  }
}

void QCPAxisTickerTime::setFieldWidth(QCPAxisTickerTime::TimeUnit unit, int width)
{
  mFieldWidth[unit] = qMax(width, 1);
}

double QCPAxisTickerTime::getTickStep(const QCPRange &range)
{
  double result = range.size()/(double)(mTickCount+1e-10); // mTickCount ticks on average, the small addition is to prevent jitter on exact integers
  
  if (result < 1) // ideal tick step is below 1 second -> use normal clean mantissa algorithm in units of seconds
  {
    if (mSmallestUnit == tuMilliseconds)
      result = qMax(cleanMantissa(result), 0.001); // smallest tick step is 1 millisecond
    else // have no milliseconds available in format, so stick with 1 second tickstep
      result = 1.0;
  } else if (result < 3600*24) // below a day
  {
    // the filling of availableSteps seems a bit contorted but it fills in a sorted fashion and thus saves a post-fill sorting run
    QVector<double> availableSteps;
    // seconds range:
    if (mSmallestUnit <= tuSeconds)
      availableSteps << 1;
    if (mSmallestUnit == tuMilliseconds)
      availableSteps << 2.5; // only allow half second steps if milliseconds are there to display it
    else if (mSmallestUnit == tuSeconds)
      availableSteps << 2;
    if (mSmallestUnit <= tuSeconds)
      availableSteps << 5 << 10 << 15 << 30;
    // minutes range:
    if (mSmallestUnit <= tuMinutes)
      availableSteps << 1*60;
    if (mSmallestUnit <= tuSeconds)
      availableSteps << 2.5*60; // only allow half minute steps if seconds are there to display it
    else if (mSmallestUnit == tuMinutes)
      availableSteps << 2*60;
    if (mSmallestUnit <= tuMinutes)
      availableSteps << 5*60 << 10*60 << 15*60 << 30*60;
    // hours range:
    if (mSmallestUnit <= tuHours)
      availableSteps << 1*3600 << 2*3600 << 3*3600 << 6*3600 << 12*3600 << 24*3600;
    // pick available step that is most appropriate to approximate ideal step:
    result = pickClosest(result, availableSteps);
  } else // more than a day, go back to normal clean mantissa algorithm but in units of days
  {
    const double secondsPerDay = 3600*24;
    result = cleanMantissa(result/secondsPerDay)*secondsPerDay;
  }
  return result;
}

int QCPAxisTickerTime::getSubTickCount(double tickStep)
{
  int result = QCPAxisTicker::getSubTickCount(tickStep);
  switch (qRound(tickStep)) // hand chosen subticks for specific minute/hour/day range (as specified in getTickStep)
  {
    case 5*60: result = 4; break;
    case 10*60: result = 1; break;
    case 15*60: result = 2; break;
    case 30*60: result = 1; break;
    case 60*60: result = 3; break;
    case 3600*2: result = 3; break;
    case 3600*3: result = 2; break;
    case 3600*6: result = 1; break;
    case 3600*12: result = 3; break;
    case 3600*24: result = 3; break;
  }
  return result;
}

QString QCPAxisTickerTime::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
  Q_UNUSED(precision)
  Q_UNUSED(formatChar)
  Q_UNUSED(locale)
  bool negative = tick < 0;
  if (negative) tick *= -1;
  double values[tuDays+1]; // contains the msec/sec/min/... value with its respective modulo (e.g. minute 0..59)
  double restValues[tuDays+1]; // contains the msec/sec/min/... value as if it's the largest available unit and thus consumes the remaining time
  
  restValues[tuMilliseconds] = tick*1000;
  values[tuMilliseconds] = modf(restValues[tuMilliseconds]/1000, &restValues[tuSeconds])*1000;
  values[tuSeconds] = modf(restValues[tuSeconds]/60, &restValues[tuMinutes])*60;
  values[tuMinutes] = modf(restValues[tuMinutes]/60, &restValues[tuHours])*60;
  values[tuHours] = modf(restValues[tuHours]/24, &restValues[tuDays])*24;
  // no need to set values[tuDays] because days are always a rest value (there is no higher unit so it consumes all remaining time)
  
  QString result = mTimeFormat;
  for (int i = mSmallestUnit; i <= mBiggestUnit; ++i)
  {
    TimeUnit iUnit = static_cast<TimeUnit>(i);
    replaceUnit(result, iUnit, qRound(iUnit == mBiggestUnit ? restValues[iUnit] : values[iUnit]));
  }
  if (negative)
    result.prepend('-');
  return result;
}

void QCPAxisTickerTime::replaceUnit(QString &text, QCPAxisTickerTime::TimeUnit unit, int value) const
{
  QString valueStr = QString::number(value);
  while (valueStr.size() < mFieldWidth.value(unit))
    valueStr.prepend('0');
  
  text.replace(mFormatPattern.value(unit), valueStr);
}


