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

#ifndef QCP_SELECTIONRECT_H
#define QCP_SELECTIONRECT_H

#include "global.h"
#include "layer.h"
#include "selection.h"
#include "axis/range.h"

class QCPAxis;

class QCP_LIB_DECL QCPSelectionRect : public QCPLayerable
{
  Q_OBJECT
public:
  explicit QCPSelectionRect(QCustomPlot *parentPlot);
  ~QCPSelectionRect();
  
  // getters:
  QRect rect() const { return mRect; }
  QCPRange range(const QCPAxis *axis) const;
  QPen pen() const { return mPen; }
  QBrush brush() const { return mBrush; }
  bool isActive() const { return mActive; }
  
  // setters:
  void setPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  
signals:
  void started();
  void changed(QRect rect);
  void finished(QRect rect, bool accepted);
  
protected:
  // property members:
  QRect mRect;
  QPen mPen;
  QBrush mBrush;
  // non-property members:
  bool mActive;
  
  // introduced virtual methods (TODO: change to "reimplemented" when QCPLayerable gets mouse/key events):
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void keyPressEvent(QKeyEvent *event);
  
  // reimplemented virtual methods
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter);
  
};

#endif // QCP_SELECTIONRECT_H
