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

#include "axisticker.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPAxisTicker
////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \class QCPAxisTicker
  \brief 
  
  
*/

QCPAxisTicker::QCPAxisTicker() :
  mTickStepStrategy(tssReadability),
  mTickCount(6),
  mTickOrigin(0)
{
}

QCPAxisTicker::~QCPAxisTicker()
{
  
}

void QCPAxisTicker::setTickStepStrategy(QCPAxisTicker::TickStepStrategy strategy)
{
  mTickStepStrategy = strategy;
}

void QCPAxisTicker::setTickCount(int count)
{
  if (count > 0)
    mTickCount = count;
  else
    qDebug() << Q_FUNC_INFO << "tick count must be greater than zero:" << count;
}

void QCPAxisTicker::setTickOrigin(double origin)
{
  mTickOrigin = origin;
}

void QCPAxisTicker::generate(const QCPRange &range, const QLocale &locale, QChar formatChar, int precision, QVector<double> &ticks, QVector<double> &subTicks, QVector<QString> &tickLabels)
{
  // generate (major) ticks:
  double tickStep = getTickStep(range);
  ticks = createTickVector(tickStep, range);
  trimTicks(range, ticks, true); // trim ticks to visible range plus one outer tick on each side (incase a subclass createTickVector creates more)
  
  // generate sub ticks between major ticks:
  if (ticks.size() > 0)
    subTicks = createSubTickVector(getSubTickCount(tickStep), ticks);
  else
    subTicks = QVector<double>();
  
  // finally trim also outliers (no further clipping happens in axis drawing):
  trimTicks(range, ticks, false); 
  trimTicks(range, subTicks, false);
  // generate labels for visible ticks:
  tickLabels = createLabelVector(ticks, locale, formatChar, precision);
}

double QCPAxisTicker::getTickStep(const QCPRange &range)
{
  double exactStep = range.size()/(double)(mTickCount+1e-10); // mTickCount ticks on average, the small addition is to prevent jitter on exact integers
  return cleanMantissa(exactStep);
}

int QCPAxisTicker::getSubTickCount(double tickStep)
{
  int result = 1; // default to 1, if no proper value can be found
  
  // separate integer and fractional part of mantissa:
  double epsilon = 0.01;
  double intPartf;
  int intPart;
  double fracPart = modf(getMantissa(tickStep), &intPartf);
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
        case 5: result = 4; break; // 5.5 -> 1.1 substep (won't occur with default getTickStep from here on)
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

QString QCPAxisTicker::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
  return locale.toString(tick, formatChar.toLatin1(), precision);
}

/*! \internal
  
  Called by generateAutoTicks when \ref setAutoSubTicks is set to true. Depending on the \a
  tickStep between two major ticks on the axis, a different number of sub ticks is appropriate. For
  Example taking 4 sub ticks for a \a tickStep of 1 makes more sense than taking 5 sub ticks,
  because this corresponds to a sub tick step of 0.2, instead of the less intuitive 0.16667. Note
  that a subtick count of 4 means dividing the major tick step into 5 sections.
  
  This is implemented by a hand made lookup for integer tick steps as well as fractional tick steps
  with a fractional part of (approximately) 0.5. If a tick step is different (i.e. has no
  fractional part close to 0.5), the currently set sub tick count (\ref setSubTickCount) is
  returned.
*/

QVector<double> QCPAxisTicker::createSubTickVector(int subTickCount, const QVector<double> &ticks)
{
  QVector<double> result;
  if (subTickCount <= 0 || ticks.size() < 2)
    return result;
  
  result.reserve((ticks.size()-1)*subTickCount);
  for (int i=1; i<ticks.size(); ++i)
  {
    double subTickStep = (ticks.at(i)-ticks.at(i-1))/(double)(subTickCount+1);
    for (int k=1; k<=subTickCount; ++k)
      result.append(ticks.at(i-1) + k*subTickStep);
  }
  return result;
}

QVector<double> QCPAxisTicker::createTickVector(double tickStep, const QCPRange &range)
{
  QVector<double> result;
  // Generate tick positions according to tickStep:
  qint64 firstStep = floor((range.lower-mTickOrigin)/tickStep); // do not use qFloor here, or we'll lose 64 bit precision
  qint64 lastStep = ceil((range.upper-mTickOrigin)/tickStep); // do not use qCeil here, or we'll lose 64 bit precision
  int tickcount = lastStep-firstStep+1;
  if (tickcount < 0) tickcount = 0;
  result.resize(tickcount);
  for (int i=0; i<tickcount; ++i)
    result[i] = mTickOrigin + (firstStep+i)*tickStep;
  return result;
}

QVector<QString> QCPAxisTicker::createLabelVector(const QVector<double> &ticks, const QLocale &locale, QChar formatChar, int precision)
{
  QVector<QString> result;
  result.reserve(ticks.size());
  for (int i=0; i<ticks.size(); ++i)
    result.append(getTickLabel(ticks.at(i), locale, formatChar, precision));
  return result;
}

void QCPAxisTicker::trimTicks(const QCPRange &range, QVector<double> &ticks, bool keepOneOutlier) const
{
  bool lowFound = false;
  bool highFound = false;
  int lowIndex = 0;
  int highIndex = -1;
  
  for (int i=0; i < ticks.size(); ++i)
  {
    if (ticks.at(i) >= range.lower)
    {
      lowFound = true;
      lowIndex = i;
      break;
    }
  }
  for (int i=ticks.size()-1; i >= 0; --i)
  {
    if (ticks.at(i) <= range.upper)
    {
      highFound = true;
      highIndex = i;
      break;
    }
  }
  
  if (highFound && lowFound)
  {
    int trimFront = qMax(0, lowIndex-(keepOneOutlier ? 1 : 0));
    int trimBack = qMax(0, ticks.size()-(keepOneOutlier ? 2 : 1)-highIndex);
    if (trimFront > 0 || trimBack > 0)
      ticks = ticks.mid(trimFront, ticks.size()-trimFront-trimBack);
  } else // all ticks are either all below or all above the range
    ticks.clear();
}

/*!
  assumes potentialSteps is not empty
*/

double QCPAxisTicker::pickClosest(double target, const QVector<double> &candidates) const
{
  if (candidates.size() == 1)
    return candidates.first();
  QVector<double>::const_iterator it = std::lower_bound(candidates.constBegin(), candidates.constEnd(), target);
  if (it == candidates.constEnd())
    return *(it-1);
  else if (it == candidates.constBegin())
    return *it;
  else
    return target-*(it-1) < *it-target ? *(it-1) : *it;
}

double QCPAxisTicker::getMantissa(double input, double *magnitude) const
{
  const double mag = qPow(10.0, qFloor(qLn(input)/qLn(10.0)));
  if (magnitude) *magnitude = mag;
  return input/mag;
}

double QCPAxisTicker::cleanMantissa(double input) const
{
  double magnitude;
  const double mantissa = getMantissa(input, &magnitude);
  switch (mTickStepStrategy)
  {
    case tssReadability:
    {
      return pickClosest(mantissa, QVector<double>() << 1.0 << 2.0 << 2.5 << 5.0)*magnitude;
    }
    case tssMeetTickCount:
    {
      // this gives effectively a mantissa of 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 8.0
      if (mantissa <= 5.0)
        return (int)(mantissa*2)/2.0*magnitude; // round digit after decimal point to 0.5
      else
        return (int)(mantissa/2.0)*2.0*magnitude; // round to first digit in multiples of 2
    }
  }
  return input;
}
