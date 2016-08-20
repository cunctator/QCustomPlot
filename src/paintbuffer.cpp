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

#include "paintbuffer.h"

#include "painter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPPaintBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPPaintBuffer
  \brief 


*/

QCPPaintBuffer::QCPPaintBuffer(const QSize &size, double devicePixelRatio) :
  mSize(size),
  mDevicePixelRatio(devicePixelRatio),
  mInvalidated(true)
{
  reallocateBuffer();
}

QCPPaintBuffer::~QCPPaintBuffer()
{
}

/*
  note: invalidates painters previously created by \ref createPainter
*/
void QCPPaintBuffer::setSize(const QSize &size)
{
  if (mSize != size)
  {
    mSize = size;
    reallocateBuffer();
  }
}

void QCPPaintBuffer::setInvalidated(bool invalidated)
{
  mInvalidated = invalidated;
}

void QCPPaintBuffer::setDevicePixelRatio(double ratio)
{
  if (!qFuzzyCompare(ratio, mDevicePixelRatio))
  {
    mDevicePixelRatio = ratio;
    reallocateBuffer();
  }
}

QCPPainter *QCPPaintBuffer::createPainter()
{
  QCPPainter *result = new QCPPainter(&mBuffer);
  result->setRenderHint(QPainter::HighQualityAntialiasing);
  return result;
}

void QCPPaintBuffer::draw(QCPPainter *painter) const
{
  if (painter && painter->isActive())
    painter->drawPixmap(0, 0, mBuffer);
  else
    qDebug() << Q_FUNC_INFO << "invalid or inactive painter passed";
}

/*
  note: don't call while painters (returned from \ref createPainter) are active.
*/
void QCPPaintBuffer::fill(const QColor &color)
{
  mBuffer.fill(color);
}

void QCPPaintBuffer::reallocateBuffer()
{
  setInvalidated();
  if (!qFuzzyCompare(1.0, mDevicePixelRatio))
  {
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    mBuffer = QPixmap(mSize*mDevicePixelRatio);
    mBuffer.setDevicePixelRatio(mDevicePixelRatio);
#else
    qDebug() << Q_FUNC_INFO << "Device pixel ratios not supported for Qt versions before 5.4";
    mDevicePixelRatio = 1.0;
    mBuffer = QPixmap(mSize);
#endif
  } else
  {
    mBuffer = QPixmap(mSize);
  }
}
