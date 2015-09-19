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

#ifndef QCP_AXISTICKERFIXED_H
#define QCP_AXISTICKERFIXED_H

#include "axisticker.h"

class QCP_LIB_DECL QCPAxisTickerFixed : public QCPAxisTicker
{
public:
  QCPAxisTickerFixed();
  
  enum ScaleStrategy {
     ssNone
    ,ssMultiples
    ,ssPowers
  };
  
  // getters:
  double tickStep() const { return mTickStep; }
  ScaleStrategy scaleStrategy() const { return mScaleStrategy; }
  
  // setters:
  void setTickStep(double step);
  void setScaleStrategy(ScaleStrategy strategy);
  
protected:
  // property members:
  double mTickStep;
  ScaleStrategy mScaleStrategy;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const QCPRange &range);
};

#endif // QCP_AXISTICKERFIXED_H
