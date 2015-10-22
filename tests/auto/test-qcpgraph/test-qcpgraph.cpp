#include "test-qcpgraph.h"
#include <QMainWindow>

void TestQCPGraph::init()
{
  mPlot = new QCustomPlot(0);
  mGraph = mPlot->addGraph();
}

void TestQCPGraph::cleanup()
{
  delete mPlot;
}

void TestQCPGraph::specializedGraphInterface()
{
  // for this test we don't use the graph created in init().
  mPlot->clearPlottables();
  
  // adding graphs:
  QCOMPARE(mPlot->plottableCount(), 0);
  QCOMPARE(mPlot->graphCount(), 0);
  
  QCPGraph *graph1 = mPlot->addGraph();
  QCOMPARE(mPlot->plottableCount(), 1);
  QCOMPARE(mPlot->graphCount(), 1);
  
  QCPGraph *graph2 = mPlot->addGraph();
  QCOMPARE(mPlot->plottableCount(), 2);
  QCOMPARE(mPlot->graphCount(), 2);
  
  QCPGraph *graph3 = new QCPGraph(mPlot->xAxis, mPlot->yAxis);
  QCOMPARE(mPlot->plottableCount(), 3);
  QCOMPARE(mPlot->graphCount(), 3);
  
  // accessing graphs:
  QCOMPARE(mPlot->graph(0), graph1);
  QCOMPARE(mPlot->graph(1), graph2);
  QCOMPARE(mPlot->graph(2), graph3);
  QCOMPARE(mPlot->plottable(0), graph1);
  QCOMPARE(mPlot->plottable(1), graph2);
  QCOMPARE(mPlot->plottable(2), graph3);

  // removing graphs:
  mPlot->removeGraph(0);
  QCOMPARE(mPlot->graphCount(), 2);
  QCOMPARE(mPlot->graph(0), graph2);
  QCOMPARE(mPlot->graph(1), graph3);
  
  mPlot->removeGraph(graph2);
  QCOMPARE(mPlot->graphCount(), 1);
  QCOMPARE(mPlot->graph(0), graph3);
  
  mPlot->removeGraph(0);
  QCOMPARE(mPlot->graphCount(), 0);
  QCOMPARE(mPlot->plottableCount(), 0);
}

void TestQCPGraph::dataManipulation()
{
  QVERIFY(mGraph->data()->isEmpty());
  
  QVector<double> x, y;
  x << -1 << 1 << -2 << 2;
  y <<  1 << 2 <<  0 << 3;
  
  mGraph->setData(x, y);
  QCOMPARE(mGraph->data()->size(), 4);
  // data should be sorted by x:
  QCOMPARE((mGraph->data()->constBegin()+0)->value, 0.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->value, 1.0);
  QCOMPARE((mGraph->data()->constBegin()+2)->value, 2.0);
  QCOMPARE((mGraph->data()->constBegin()+3)->value, 3.0);
  QCOMPARE((mGraph->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->key, -1.0);
  QCOMPARE((mGraph->data()->constBegin()+2)->key, 1.0);
  QCOMPARE((mGraph->data()->constBegin()+3)->key, 2.0);
  
  // data removal:
  mGraph->data()->removeBefore(0);
  QCOMPARE(mGraph->data()->size(), 2);
  QCOMPARE((mGraph->data()->begin()+0)->key, 1.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->key, 2.0);
  
  mGraph->setData(x, y);
  mGraph->data()->removeAfter(0);
  QCOMPARE(mGraph->data()->size(), 2);
  QCOMPARE((mGraph->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->key, -1.0);
  
  mGraph->setData(x, y);
  mGraph->data()->remove(-1.1, -0.9);
  QCOMPARE(mGraph->data()->size(), 3);
  QCOMPARE((mGraph->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->key, 1.0);
  QCOMPARE((mGraph->data()->constBegin()+2)->key, 2.0);
  
  mGraph->setData(x, y);
  mGraph->data()->remove(-2.1, -1.9);
  QCOMPARE(mGraph->data()->size(), 3);
  QCOMPARE((mGraph->data()->constBegin()+0)->key, -1.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->key, 1.0);
  QCOMPARE((mGraph->data()->constBegin()+2)->key, 2.0);
  
  mGraph->setData(x, y);
  mGraph->data()->remove(1.9, 2.1);
  QCOMPARE(mGraph->data()->size(), 3);
  QCOMPARE((mGraph->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->key, -1.0);
  QCOMPARE((mGraph->data()->constBegin()+2)->key, 1.0);
  
  mGraph->setData(x, y);
  mGraph->data()->remove(-1.1, 1.1);
  QCOMPARE(mGraph->data()->size(), 2);
  QCOMPARE((mGraph->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->key, 2.0);
  
  mGraph->setData(x, y);
  mGraph->data()->clear();
  QCOMPARE(mGraph->data()->size(), 0);
  
  // add single data points:
  mGraph->addData(5, 6);
  QCOMPARE(mGraph->data()->size(), 1);
  mGraph->addData(4, 5);
  QCOMPARE(mGraph->data()->size(), 2);
  QCOMPARE((mGraph->data()->constBegin()+0)->value, 5.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->value, 6.0);
  
  mGraph->setData(x, y);
  mGraph->addData(3, 4);
  QCOMPARE(mGraph->data()->size(), 5);
  QCOMPARE((mGraph->data()->constBegin()+0)->value, 0.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->value, 1.0);
  QCOMPARE((mGraph->data()->constBegin()+2)->value, 2.0);
  QCOMPARE((mGraph->data()->constBegin()+3)->value, 3.0);
  QCOMPARE((mGraph->data()->constBegin()+4)->value, 4.0);
  
  mGraph->setData(x, y);
  mGraph->addData(QVector<double>() << 3 << 4 << 5, QVector<double>() << 4 << 5 << 6);
  QCOMPARE(mGraph->data()->size(), 7);
  QCOMPARE((mGraph->data()->constBegin()+0)->value, 0.0);
  QCOMPARE((mGraph->data()->constBegin()+1)->value, 1.0);
  QCOMPARE((mGraph->data()->constBegin()+2)->value, 2.0);
  QCOMPARE((mGraph->data()->constBegin()+3)->value, 3.0);
  QCOMPARE((mGraph->data()->constBegin()+4)->value, 4.0);
  QCOMPARE((mGraph->data()->constBegin()+5)->value, 5.0);
  QCOMPARE((mGraph->data()->constBegin()+6)->value, 6.0);
}

void TestQCPGraph::dataSharing()
{
  QCPGraph *graph2 = new QCPGraph(mPlot->xAxis, mPlot->yAxis);
  
  QSharedPointer<QCPGraphDataContainer> data = graph2->data();
  mGraph->setData(data); // now shares data with curve2
  
  QVector<double> x, y;
  x << -1 << 1 << -2 << 2;
  y <<  10 << 9 <<  12 << 15;
  graph2->setData(x, y);
  
  QCOMPARE(mGraph->data()->size(), 4);
  QCOMPARE(graph2->data()->size(), 4);
  data->add(QCPGraphData(4, 5)); // modify from outside
  QCOMPARE(mGraph->data()->size(), 5);
  QCOMPARE(graph2->data()->size(), 5);
  data.clear(); // clear external reference, internals should live on
  QVERIFY(!data);
  QCOMPARE(mGraph->data()->size(), 5);
  QCOMPARE(graph2->data()->size(), 5);
  
  // decouple data again:
  mGraph->setData(QSharedPointer<QCPGraphDataContainer>(new QCPGraphDataContainer));
  mGraph->addData(1, 1);
  graph2->addData(4, 5);
  QCOMPARE(mGraph->data()->size(), 1);
  QCOMPARE(graph2->data()->size(), 6);
}

void TestQCPGraph::channelFill()
{
  QCPGraph *otherGraph = mPlot->addGraph();
  
  mGraph->setBrush(Qt::black);
  mGraph->setChannelFillGraph(otherGraph);
  mPlot->replot();
  
  mGraph->setData(QVector<double>()<<1<<2<<3<<4, QVector<double>()<<4<<2<<2.5<<6);
  mPlot->replot();
  
  otherGraph->setData(QVector<double>()<<-2<<-1.5<<-1<<-0.5, QVector<double>()<<1<<1.2<<1.3<<1.4);
  mPlot->replot();
  
  mGraph->data()->clear();
  mPlot->replot();
  
  mPlot->removeGraph(otherGraph);
  QCOMPARE(mGraph->channelFillGraph(), (QCPGraph*)0);
  mPlot->replot();
}


