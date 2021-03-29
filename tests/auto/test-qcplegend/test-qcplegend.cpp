#include "test-qcplegend.h"

void TestQCPLegend::init()
{
  mPlot = new QCustomPlot(0);
  mPlot->legend->setVisible(true);
  mPlot->show();
  //QTest::qWait(150);
}


void TestQCPLegend::cleanup()
{
  delete mPlot;
}

void TestQCPLegend::autoAddPlottables()
{
  // legend items shouldn't be added if auto-add is off:
  QCOMPARE(mPlot->legend->itemCount(), 0);
  mPlot->setAutoAddPlottableToLegend(false);
  mPlot->addGraph();
  QCOMPARE(mPlot->legend->itemCount(), 0);
  new QCPBars(mPlot->xAxis, mPlot->yAxis);
  QCOMPARE(mPlot->legend->itemCount(), 0);
  
  // legend items should be added if auto-add is on:
  mPlot->setAutoAddPlottableToLegend(true);
  mPlot->addGraph();
  QCOMPARE(mPlot->legend->itemCount(), 1);
  mPlot->addGraph();
  QCOMPARE(mPlot->legend->itemCount(), 2);
  new QCPBars(mPlot->xAxis, mPlot->yAxis);
  QCOMPARE(mPlot->legend->itemCount(), 3);
}

void TestQCPLegend::addAndRemove()
{
  mPlot->setAutoAddPlottableToLegend(false);
  
  QCPGraph *g1 = new QCPGraph(mPlot->xAxis, mPlot->yAxis);
  QCPGraph *g2 = new QCPGraph(mPlot->xAxis, mPlot->yAxis);
  QCPBars *b1 = new QCPBars(mPlot->xAxis, mPlot->yAxis);
  QCOMPARE(mPlot->legend->itemCount(), 0);
  
  g1->addToLegend();
  QCOMPARE(mPlot->legend->itemCount(), 1);
  mPlot->legend->addItem(new QCPPlottableLegendItem(mPlot->legend, g2)); // explicit way of adding to legend
  QCOMPARE(mPlot->legend->itemCount(), 2);
  b1->addToLegend();
  QCOMPARE(mPlot->legend->itemCount(), 3);
  
  g1->removeFromLegend();
  QCOMPARE(mPlot->legend->itemCount(), 2);
  
  mPlot->legend->clearItems();
  QCOMPARE(mPlot->legend->itemCount(), 0);
}




