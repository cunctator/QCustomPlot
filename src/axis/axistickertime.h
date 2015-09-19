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

class QCPAxisTickerDateTime : public QCPAxisTicker
{
public:
  QCPAxisTickerDateTime();
  
  // getters:
  QString dateTimeFormat() const { return mDateTimeFormat; }
  Qt::TimeSpec dateTimeSpec() const { return mDateTimeSpec; }
  
  // setters:
  void setDateTimeFormat(const QString &format);
  void setDateTimeSpec(Qt::TimeSpec spec);
  void setTickOrigin(double origin); // hides base class method but calls baseclass implementation ("using" throws off IDEs and doxygen)
  void setTickOrigin(const QDateTime &origin);
  
protected:
  // property members:
  QString mDateTimeFormat;
  Qt::TimeSpec mDateTimeSpec;
  // non-property members:
  enum DateStrategy {dsNone, dsUniformTimeInDay, dsUniformDayInMonth} mDateStrategy;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const QCPRange &range);
  virtual int getSubTickCount(double tickStep);
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision);
  virtual QVector<double> createTickVector(double tickStep, const QCPRange &range);
  
  // non-virtual methods:
  QDateTime keyToDateTime(double key) const;
  double dateTimeToKey(const QDateTime dateTime) const;
};

#endif // QCP_AXISTICKERTIME_H
