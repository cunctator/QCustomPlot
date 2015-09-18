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
  mDateTimeSpec(Qt::LocalTime),
  mDateStrategy(dsNone)
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

void QCPAxisTickerTime::setTickOrigin(double origin)
{
  QCPAxisTicker::setTickOrigin(origin);
}

void QCPAxisTickerTime::setTickOrigin(const QDateTime &origin)
{
  setTickOrigin(dateTimeToKey(origin));
}

double QCPAxisTickerTime::getTickStep(const QCPRange &range)
{
  double result = range.size()/(double)(mTickCount+1e-10); // mTickCount ticks on average, the small addition is to prevent jitter on exact integers
  
  mDateStrategy = dsNone;
  if (result < 1) // ideal tick step is below 1 second -> use normal clean mantissa algorithm in units of seconds
  {
    result = cleanMantissa(result);
  } else if (result < 86400*30.4375*12) // below a year
  {
    result = pickClosest(result, QVector<double>()
                             << 1 << 2.5 << 5 << 10 << 15 << 30 << 60 << 2.5*60 << 5*60 << 10*60 << 15*60 << 30*60 << 60*60 // second, minute, hour range
                             << 3600 << 3600*2 << 3600*3 << 3600*6 << 3600*12 << 3600*24 // hour to day range
                             << 86400 << 86400*2 << 86400*5 << 86400*7 << 86400*14 << 86400*30.4375 << 86400*30.4375*2 << 86400*30.4375*3 << 86400*30.4375*6 << 86400*30.4375*12); // day, week, month range (avg. days per month includes leap years)
    if (result > 86400*30.4375-1) // month tick intervals or larger
      mDateStrategy = dsUniformDayInMonth;
    else if (result > 3600*24-1) // day tick intervals or larger
      mDateStrategy = dsUniformTimeInDay;
  } else // more than a year, go back to normal clean mantissa algorithm but in units of years
  {
    const double secondsPerYear = 86400*30.4375*12; // average including leap years
    result = cleanMantissa(result/secondsPerYear)*secondsPerYear;
    mDateStrategy = dsUniformDayInMonth;
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
  return locale.toString(keyToDateTime(tick).toTimeSpec(mDateTimeSpec), mDateTimeFormat);
}

QVector<double> QCPAxisTickerTime::createTickVector(double tickStep, const QCPRange &range)
{
  QVector<double> result = QCPAxisTicker::createTickVector(tickStep, range);
  if (!result.isEmpty())
  {  
    if (mDateStrategy == dsUniformTimeInDay)
    {
      QDateTime uniformDateTime = keyToDateTime(mTickOrigin); // the time of this datetime will be set for all other ticks, if possible
      QDateTime tickDateTime;
      for (int i=0; i<result.size(); ++i)
      {
        tickDateTime = keyToDateTime(result.at(i));
        tickDateTime.setTime(uniformDateTime.time());
        result[i] = dateTimeToKey(tickDateTime);
      }
    } else if (mDateStrategy == dsUniformDayInMonth)
    {
      QDateTime uniformDateTime = keyToDateTime(mTickOrigin); // this day (in month) and time will be set for all other ticks, if possible
      QDateTime tickDateTime;
      for (int i=0; i<result.size(); ++i)
      {
        tickDateTime = keyToDateTime(result.at(i));
        tickDateTime.setTime(uniformDateTime.time());
        int thisUniformDay = uniformDateTime.date().day() <= tickDateTime.date().daysInMonth() ? uniformDateTime.date().day() : tickDateTime.date().daysInMonth(); // don't exceed month (e.g. try to set day 31 in February)
        if (thisUniformDay-tickDateTime.date().day() < -15) // with leap years involved, date month may jump backwards or forwards, and needs to be corrected before setting day
          tickDateTime = tickDateTime.addMonths(1);
        else if (thisUniformDay-tickDateTime.date().day() > 15) // with leap years involved, date month may jump backwards or forwards, and needs to be corrected before setting day
          tickDateTime = tickDateTime.addMonths(-1);
        tickDateTime.setDate(QDate(tickDateTime.date().year(), tickDateTime.date().month(), thisUniformDay));
        result[i] = dateTimeToKey(tickDateTime);
      }
    }
  }
  return result;
}

QDateTime QCPAxisTickerTime::keyToDateTime(double key) const
{
# if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  return QDateTime::fromTime_t(key);
# else
  return QDateTime::fromMSecsSinceEpoch(key*1000.0);
# endif
}

double QCPAxisTickerTime::dateTimeToKey(const QDateTime dateTime) const
{
# if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  return dateTime.toTime_t();
# else
  return dateTime.toMSecsSinceEpoch()/1000.0;
# endif
}
