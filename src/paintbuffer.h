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

#ifndef QCP_PAINTBUFFER_H
#define QCP_PAINTBUFFER_H

#include "global.h"

class QCPPainter;

class QCP_LIB_DECL QCPPaintBuffer
{
public:
  explicit QCPPaintBuffer(const QSize &size, double devicePixelRatio);
  virtual ~QCPPaintBuffer();
  
  // getters:
  QSize size() const { return mBuffer.size(); }
  bool invalidated() const { return mInvalidated; }
  double devicePixelRatio() const { return mDevicePixelRatio; }
  
  // setters:
  void setSize(const QSize &size);
  void setInvalidated(bool invalidated=true);
  void setDevicePixelRatio(double ratio);
  
  // non-property methods:
  virtual QCPPainter *createPainter();
  virtual void draw(QCPPainter *painter) const;
  void fill(const QColor &color);
  
protected:
  // property members:
  QPixmap mBuffer;
  QSize mSize;
  double mDevicePixelRatio;
  
  // non-property members:
  bool mInvalidated;
  
  // non-virtual methods:
  void reallocateBuffer();
  
};

#endif // QCP_PAINTBUFFER_H
