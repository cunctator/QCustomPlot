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

#ifndef QCP_AXISTICKERTIME_H
#define QCP_AXISTICKERTIME_H

#include "axisticker.h"

class QCP_LIB_DECL QCPAxisTickerTime : public QCPAxisTicker
{
  Q_GADGET
public:
  enum TimeUnit {
    tuMilliseconds
    ,tuSeconds
    ,tuMinutes
    ,tuHours
    ,tuDays
  };
  Q_ENUMS(TimeUnit)
  
  QCPAxisTickerTime();

  // getters:
  QString timeFormat() const { return mTimeFormat; }
  int fieldWidth(TimeUnit unit) const { return mFieldWidth.value(unit); }
  
  // setters:
  void setTimeFormat(const QString &format);
  void setFieldWidth(TimeUnit unit, int width);
  
protected:
  // property members:
  QString mTimeFormat;
  QHash<TimeUnit, int> mFieldWidth;
  
  // non-property members:
  TimeUnit mSmallestUnit, mBiggestUnit;
  QHash<TimeUnit, QString> mFormatPattern;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const QCPRange &range);
  virtual int getSubTickCount(double tickStep);
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision);
  
  // non-virtual methods:
  void replaceUnit(QString &text, TimeUnit unit, int value) const;
};
Q_DECLARE_METATYPE(QCPAxisTickerTime::TimeUnit)

#endif // QCP_AXISTICKERTIME_H
