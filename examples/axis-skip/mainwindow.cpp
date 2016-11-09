/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2016 Emanuel Eichhammer                            **
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
**             Date: 13.09.16                                             **
**          Version: 2.0.0-beta                                           **
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  setupPlot();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::setupPlot()
{
  QCPAxisRect *rightRect = new QCPAxisRect(ui->plot, false);
  ui->plot->plotLayout()->addElement(0, 1, rightRect);
  QCPAxis *xAxisRight = rightRect->addAxis(QCPAxis::atBottom);
  xAxisRight->grid()->setVisible(true);
  xAxisRight->grid()->setSubGridVisible(true);
  
  QCPGraph *g1 = ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis);
  QCPGraph *g2 = ui->plot->addGraph(xAxisRight, ui->plot->yAxis);

  g1->data()->set(generateData()); // fill data container of g1 with new generated data points
  g2->setData(g1->data()); // g2 shares data container with g1 now
  
  xAxisRight->setScaleType(QCPAxis::stLogarithmic);
  xAxisRight->setTicker(QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog));
  
  ui->plot->xAxis->setRange(1, 30);
  xAxisRight->setRange(30, 1200);
  
  ui->plot->yAxis->rescale();
  
  ui->plot->replot();
}

QVector<QCPGraphData> MainWindow::generateData()
{
  const int n = 750;
  QVector<QCPGraphData> result(n);
  for (int i=0; i<n; ++i)
  {
    result[i].key = 1.0 + i/(double)n * 1000.0;
    result[i].value = qLn(result.at(i).key*0.25)+qSin(result.at(i).key*0.25);
  }
  return result;
}

