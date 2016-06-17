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

#ifndef QCP_PLOTTABLE_H
#define QCP_PLOTTABLE_H

#include "global.h"
#include "qcpvector2d.h"
#include "axis/range.h"
#include "layer.h"
#include "axis/axis.h"
#include "selection.h"
#include "scatterstyle.h"

class QCPPainter;
class QCPAbstractPlottable;
class QCPPlottableInterface1D;

class QCP_LIB_DECL QCPSelectionDecorator
{
  Q_GADGET
public:
  QCPSelectionDecorator();
  virtual ~QCPSelectionDecorator();
  
  // getters:
  QPen pen() const { return mPen; }
  QBrush brush() const { return mBrush; }
  QCPScatterStyle scatterStyle() const { return mScatterStyle; }
  QCPScatterStyle::ScatterProperties usedScatterProperties() const { return mUsedScatterProperties; }
  
  // setters:
  void setPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setScatterStyle(const QCPScatterStyle &scatterStyle, QCPScatterStyle::ScatterProperties usedProperties=QCPScatterStyle::spPen);
  void setUsedScatterProperties(const QCPScatterStyle::ScatterProperties &properties);
  
  // non-virtual methods:
  void copyFrom(const QCPSelectionDecorator *other);
  void applyPen(QCPPainter *painter) const;
  void applyBrush(QCPPainter *painter) const;
  QCPScatterStyle getFinalScatterStyle(const QCPScatterStyle &unselectedStyle) const;
  
  // introduced virtual methods:
  virtual void drawDecoration(QCPPainter *painter, QCPDataSelection selection);
  
protected:
  // property members:
  QPen mPen;
  QBrush mBrush;
  QCPScatterStyle mScatterStyle;
  QCPScatterStyle::ScatterProperties mUsedScatterProperties;
  // non-property members:
  QCPAbstractPlottable *mPlottable;
  
  // introduced virtual methods:
  virtual bool registerWithPlottable(QCPAbstractPlottable *plottable);
  
  friend class QCPAbstractPlottable;
};

class QCP_LIB_DECL QCPAbstractPlottable : public QCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(bool antialiasedFill READ antialiasedFill WRITE setAntialiasedFill)
  Q_PROPERTY(bool antialiasedScatters READ antialiasedScatters WRITE setAntialiasedScatters)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QCPAxis* keyAxis READ keyAxis WRITE setKeyAxis)
  Q_PROPERTY(QCPAxis* valueAxis READ valueAxis WRITE setValueAxis)
  Q_PROPERTY(QCP::SelectionType selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)
  Q_PROPERTY(bool selected READ selected NOTIFY selectionChanged)
  Q_PROPERTY(QCPSelectionDecorator* selectionDecorator READ selectionDecorator WRITE setSelectionDecorator)
  /// \endcond
public:
  QCPAbstractPlottable(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPAbstractPlottable();
  
  // getters:
  QString name() const { return mName; }
  bool antialiasedFill() const { return mAntialiasedFill; }
  bool antialiasedScatters() const { return mAntialiasedScatters; }
  QPen pen() const { return mPen; }
  QBrush brush() const { return mBrush; }
  QCPAxis *keyAxis() const { return mKeyAxis.data(); }
  QCPAxis *valueAxis() const { return mValueAxis.data(); }
  QCP::SelectionType selectable() const { return mSelectable; }
  bool selected() const { return !mSelection.isEmpty(); }
  QCPDataSelection selection() const { return mSelection; }
  QCPSelectionDecorator *selectionDecorator() const { return mSelectionDecorator; }
  
  // setters:
  void setName(const QString &name);
  void setAntialiasedFill(bool enabled);
  void setAntialiasedScatters(bool enabled);
  void setPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setKeyAxis(QCPAxis *axis);
  void setValueAxis(QCPAxis *axis);
  Q_SLOT void setSelectable(QCP::SelectionType selectable);
  Q_SLOT void setSelection(QCPDataSelection selection);
  void setSelectionDecorator(QCPSelectionDecorator *decorator);

  // introduced virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const = 0;
  virtual QCPPlottableInterface1D *interface1D() { return 0; }
  
  // non-property methods:
  void rescaleAxes(bool onlyEnlarge=false) const;
  void rescaleKeyAxis(bool onlyEnlarge=false) const;
  void rescaleValueAxis(bool onlyEnlarge=false) const;
  bool addToLegend();
  bool removeFromLegend() const;
  
signals:
  void selectionChanged(bool selected);
  void selectionChanged(QCPDataSelection selection);
  void selectableChanged(QCP::SelectionType selectable);
  
protected:
  // property members:
  QString mName;
  bool mAntialiasedFill, mAntialiasedScatters;
  QPen mPen;
  QBrush mBrush;
  QPointer<QCPAxis> mKeyAxis, mValueAxis;
  QCP::SelectionType mSelectable;
  QCPDataSelection mSelection;
  QCPSelectionDecorator *mSelectionDecorator;
  
  // reimplemented virtual methods:
  virtual QRect clipRect() const;
  virtual void draw(QCPPainter *painter) = 0;
  virtual QCP::Interaction selectionCategory() const;
  void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  
  // introduced virtual methods:
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const = 0;
  virtual QCPRange getKeyRange(bool &foundRange, QCP::SignDomain inSignDomain=QCP::sdBoth) const = 0;
  virtual QCPRange getValueRange(bool &foundRange, QCP::SignDomain inSignDomain=QCP::sdBoth) const = 0;
  
  // non-virtual methods:
  void coordsToPixels(double key, double value, double &x, double &y) const;
  const QPointF coordsToPixels(double key, double value) const;
  void pixelsToCoords(double x, double y, double &key, double &value) const;
  void pixelsToCoords(const QPointF &pixelPos, double &key, double &value) const;
  void applyFillAntialiasingHint(QCPPainter *painter) const;
  void applyScattersAntialiasingHint(QCPPainter *painter) const;

private:
  Q_DISABLE_COPY(QCPAbstractPlottable)
  
  friend class QCustomPlot;
  friend class QCPAxis;
  friend class QCPPlottableLegendItem;
};


#endif // QCP_PLOTTABLE_H
