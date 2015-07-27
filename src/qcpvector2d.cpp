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

#include "qcpvector2d.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPVector2D
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPVector2D
  \brief Represents two doubles as a mathematical 2D vector


*/

/*!
  Creates a QCPVector2D object and initializes the x and y coordinates to 0.
*/
QCPVector2D::QCPVector2D() :
  mX(0),
  mY(0)
{
}

QCPVector2D::QCPVector2D(double x, double y) :
  mX(x),
  mY(y)
{
}

QCPVector2D::QCPVector2D(const QPoint &point) :
  mX(point.x()),
  mY(point.y())
{
}

QCPVector2D::QCPVector2D(const QPointF &point) :
  mX(point.x()),
  mY(point.y())
{
}

void QCPVector2D::normalize()
{
  double len = length();
  mX /= len;
  mY /= len;
}

QCPVector2D QCPVector2D::normalized() const
{
  QCPVector2D result(mX, mY);
  result.normalize();
  return result;
}

double QCPVector2D::distanceSquaredToLine(const QCPVector2D &start, const QCPVector2D &end) const
{
  QCPVector2D v(end-start);
  double vLengthSqr = v.lengthSquared();
  if (!qFuzzyIsNull(vLengthSqr))
  {
    double mu = v.dot(*this-start)/vLengthSqr;
    if (mu < 0)
      return (*this-start).lengthSquared();
    else if (mu > 1)
      return (*this-end).lengthSquared();
    else
      return ((start + mu*v)-*this).lengthSquared();
  } else
    return (*this-start).lengthSquared();
}

double QCPVector2D::distanceToStraightLine(const QCPVector2D &base, const QCPVector2D &direction) const
{
  return qAbs((*this-base).dot(direction.perpendicular()))/direction.length();
}

QCPVector2D &QCPVector2D::operator*=(double factor)
{
  mX *= factor;
  mY *= factor;
  return *this;
}

QCPVector2D &QCPVector2D::operator/=(double divisor)
{
  mX /= divisor;
  mY /= divisor;
  return *this;
}

QCPVector2D &QCPVector2D::operator+=(const QCPVector2D &vector)
{
  mX += vector.mX;
  mY += vector.mY;
  return *this;
}

QCPVector2D &QCPVector2D::operator-=(const QCPVector2D &vector)
{
  mX -= vector.mX;
  mY -= vector.mY;
  return *this;
}

