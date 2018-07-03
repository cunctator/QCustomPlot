/*
 * QCustomPlot, an easy to use, modern plotting widget for Qt
 * This code is part of the improved version for traceshark
 * Copyright (C) 2017  Viktor Rosendahl <viktor.rosendahl@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QCPPOINTER_H
#define QCPPOINTER_H

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include "qcppointer_impl.h"
#else
#include <QPointer>
#define QCPPointer QPointer
#endif

#endif /* QCPPOINTER_H */
