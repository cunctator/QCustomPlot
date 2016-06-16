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

#include "selectionrect.h"

#include "axis/axis.h"
#include "painter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPSelectionRect
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPSelectionRect
  \brief
  
  document that it might return rect with negative width/height and null QMouseEvent in canceled
*/

QCPSelectionRect::QCPSelectionRect(QCustomPlot *parentPlot) :
  QCPLayerable(parentPlot),
  mPen(QBrush(Qt::gray), 0, Qt::DashLine),
  mBrush(Qt::NoBrush),
  mActive(false)
{
}

QCPSelectionRect::~QCPSelectionRect()
{
  cancel();
}

QCPRange QCPSelectionRect::range(const QCPAxis *axis) const
{
  if (axis)
  {
    if (axis->orientation() == Qt::Horizontal)
      return QCPRange(axis->pixelToCoord(mRect.left()), axis->pixelToCoord(mRect.left()+mRect.width()));
    else
      return QCPRange(axis->pixelToCoord(mRect.top()+mRect.height()), axis->pixelToCoord(mRect.top()));
  } else
  {
    qDebug() << Q_FUNC_INFO << "called with axis zero";
    return QCPRange();
  }
}

void QCPSelectionRect::setPen(const QPen &pen)
{
  mPen = pen;
}

void QCPSelectionRect::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

void QCPSelectionRect::cancel()
{
  if (mActive)
  {
    mActive = false;
    emit canceled(mRect, 0);
  }
}

void QCPSelectionRect::startSelection(QMouseEvent *event)
{
  mActive = true;
  mRect = QRect(event->pos(), event->pos());
  emit started(event);
}

void QCPSelectionRect::moveSelection(QMouseEvent *event)
{
  mRect.setBottomRight(event->pos());
  emit changed(mRect, event);
}

void QCPSelectionRect::endSelection(QMouseEvent *event)
{
  mRect.setBottomRight(event->pos());
  mActive = false;
  emit accepted(mRect, event);
}

void QCPSelectionRect::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Escape && mActive)
  {
    mActive = false;
    emit canceled(mRect, event);
  }
}

void QCPSelectionRect::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  applyAntialiasingHint(painter, mAntialiased, QCP::aeOther);
}

void QCPSelectionRect::draw(QCPPainter *painter)
{
  if (mActive)
  {
    painter->setPen(mPen);
    painter->setBrush(mBrush);
    painter->drawRect(mRect);
  }
}

