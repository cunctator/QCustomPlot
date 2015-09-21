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

#include "axistickerpi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPAxisTickerPi
////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \class QCPAxisTickerPi
  \brief 
  
  
*/

QCPAxisTickerPi::QCPAxisTickerPi() :
  mPiSymbol(QLatin1String(" ")+QChar(0x03C0)),
  mPiValue(M_PI),
  mPeriodicity(0),
  mFractionStyle(fsUnicodeFractions)
{
  setTickCount(4);
}

void QCPAxisTickerPi::setPiSymbol(QString symbol)
{
  mPiSymbol = symbol;
}

void QCPAxisTickerPi::setPiValue(double pi)
{
  mPiValue = pi;
}

void QCPAxisTickerPi::setPeriodicity(int multiplesOfPi)
{
  mPeriodicity = qAbs(multiplesOfPi);
}

void QCPAxisTickerPi::setFractionStyle(QCPAxisTickerPi::FractionStyle style)
{
  mFractionStyle = style;
}

double QCPAxisTickerPi::getTickStep(const QCPRange &range)
{
  mPiTickStep = range.size()/mPiValue/(double)(mTickCount+1e-10); // mTickCount ticks on average, the small addition is to prevent jitter on exact integers
  mPiTickStep = cleanMantissa(mPiTickStep);
  return mPiTickStep*mPiValue;
}

int QCPAxisTickerPi::getSubTickCount(double tickStep)
{
  return QCPAxisTicker::getSubTickCount(tickStep/mPiValue);
}

QString QCPAxisTickerPi::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
  double tickInPis = tick/mPiValue;
  if (mPeriodicity > 0)
    tickInPis = fmod(tickInPis, mPeriodicity);
  
  if (mFractionStyle != fsFloatingPoint && mPiTickStep > 0.09 && mPiTickStep < 50)
  {
    // simply construct fraction from decimal like 1.234 -> 1234/1000 and then simplify fraction, smaller digits are irrelevant due to mPiTickStep conditional above
    int denominator = 1000;
    int numerator = qRound(tickInPis*denominator);
    simplifyFraction(numerator, denominator);
    if (qAbs(numerator) == 1 && denominator == 1)
      return (numerator < 0 ? QLatin1String("-") : QLatin1String("")) + mPiSymbol.trimmed();
    else if (numerator == 0)
      return QLatin1String("0");
    else
      return fractionToString(numerator, denominator) + mPiSymbol;
  } else
  {
    if (qFuzzyIsNull(tickInPis))
      return QLatin1String("0");
    else if (qFuzzyCompare(qAbs(tickInPis), 1.0))
      return (tickInPis < 0 ? QLatin1String("-") : QLatin1String("")) + mPiSymbol.trimmed();
    else
      return QCPAxisTicker::getTickLabel(tickInPis, locale, formatChar, precision) + mPiSymbol;
  }
}

void QCPAxisTickerPi::simplifyFraction(int &numerator, int &denominator) const
{
  if (numerator == 0 || denominator == 0)
    return;
  
  int num = numerator;
  int denom = denominator;
  while (denom != 0) // euclidean gcd algorithm
  {
    int oldDenom = denom;
    denom = num % denom;
    num = oldDenom;
  }
  // num is now gcd of numerator and denominator
  numerator /= num;
  denominator /= num;
}

QString QCPAxisTickerPi::fractionToString(int numerator, int denominator) const
{
  if (denominator == 0)
  {
    qDebug() << Q_FUNC_INFO << "called with zero denominator";
    return QString();
  }
  if (mFractionStyle == fsFloatingPoint) // should never be the case when calling this function
  {
    qDebug() << Q_FUNC_INFO << "shouldn't be called with fraction style fsDecimal";
    return QString::number(numerator/(double)denominator); // failsafe
  }
  int sign = numerator*denominator < 0 ? -1 : 1;
  numerator = qAbs(numerator);
  denominator = qAbs(denominator);
  
  if (denominator == 1)
  {
    return QString::number(sign*numerator);
  } else
  {
    int integerPart = numerator/denominator;
    int remainder = numerator%denominator;
    if (remainder == 0)
    {
      return QString::number(sign*integerPart);
    } else
    {
      if (mFractionStyle == fsAsciiFractions)
      {
        return QString(QLatin1String("%1%2%3/%4"))
            .arg(sign == -1 ? QLatin1String("-") : QLatin1String(""))
            .arg(integerPart > 0 ? QString::number(integerPart)+QLatin1String(" ") : QLatin1String(""))
            .arg(remainder)
            .arg(denominator);
      } else if (mFractionStyle == fsUnicodeFractions)
      {
        return QString(QLatin1String("%1%2%3"))
            .arg(sign == -1 ? QLatin1String("-") : QLatin1String(""))
            .arg(integerPart > 0 ? QString::number(integerPart) : QLatin1String(""))
            .arg(unicodeFraction(remainder, denominator));
      }
    }
  }
  return QString();
}

QString QCPAxisTickerPi::unicodeFraction(int numerator, int denominator) const
{
  return unicodeSuperscript(numerator)+QChar(0x2044)+unicodeSubscript(denominator);
}

QString QCPAxisTickerPi::unicodeSuperscript(int number) const
{
  if (number == 0)
    return QString(QChar(0x2070));
  
  QString result;
  while (number > 0)
  {
    const int digit = number%10;
    switch (digit)
    {
      case 1: { result.prepend(QChar(0x00B9)); break; }
      case 2: { result.prepend(QChar(0x00B2)); break; }
      case 3: { result.prepend(QChar(0x00B3)); break; }
      default: { result.prepend(QChar(0x2070+digit)); break; }
    }
    number /= 10;
  }
  return result;
}

QString QCPAxisTickerPi::unicodeSubscript(int number) const
{
  if (number == 0)
    return QString(QChar(0x2080));
  
  QString result;
  while (number > 0)
  {
    result.prepend(QChar(0x2080+number%10));
    number /= 10;
  }
  return result;
}
