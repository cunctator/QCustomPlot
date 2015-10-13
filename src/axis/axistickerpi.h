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

#ifndef QCP_AXISTICKERPI_H
#define QCP_AXISTICKERPI_H

#include "axisticker.h"

class QCP_LIB_DECL QCPAxisTickerPi : public QCPAxisTicker
{
public:
  QCPAxisTickerPi();
  
  enum FractionStyle
  {
    fsFloatingPoint
    ,fsAsciiFractions
    ,fsUnicodeFractions
  };
  
  // getters:
  QString piSymbol() const { return mPiSymbol; }
  double piValue() const { return mPiValue; }
  bool periodicity() const { return mPeriodicity; }
  FractionStyle fractionStyle() const { return mFractionStyle; }
  
  // setters:
  void setPiSymbol(QString symbol);
  void setPiValue(double pi);
  void setPeriodicity(int multiplesOfPi);
  void setFractionStyle(FractionStyle style);
  
protected:
  // property members:
  QString mPiSymbol;
  double mPiValue;
  int mPeriodicity;
  FractionStyle mFractionStyle;
  
  // non-property members:
  double mPiTickStep;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const QCPRange &range);
  virtual int getSubTickCount(double tickStep);
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision);
  
  // non-virtual methods:
  void simplifyFraction(int &numerator, int &denominator) const;
  QString fractionToString(int numerator, int denominator) const;
  QString unicodeFraction(int numerator, int denominator) const;
  QString unicodeSuperscript(int number) const;
  QString unicodeSubscript(int number) const;
};

#endif // QCP_AXISTICKERPI_H
