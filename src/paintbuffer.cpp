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
//////////////////// QCPAbstractPaintBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPAbstractPaintBuffer
  \brief 


*/

/* start documentation of pure virtual methods */

/*
  note: don't call clear while painters (returned from \ref createPainter) are active.
*/

/* end documentation of pure virtual methods */


QCPAbstractPaintBuffer::QCPAbstractPaintBuffer(const QSize &size, double devicePixelRatio) :
  mSize(size),
  mDevicePixelRatio(devicePixelRatio),
  mInvalidated(true)
{
}

QCPAbstractPaintBuffer::~QCPAbstractPaintBuffer()
{
}

/*
  note: invalidates painters previously created by \ref createPainter
*/

void QCPAbstractPaintBuffer::setSize(const QSize &size)
{
  if (mSize != size)
  {
    mSize = size;
    reallocateBuffer();
  }
}

void QCPAbstractPaintBuffer::setInvalidated(bool invalidated)
{
  mInvalidated = invalidated;
}

/*
  note: invalidates painters previously created by \ref createPainter
*/

void QCPAbstractPaintBuffer::setDevicePixelRatio(double ratio)
{
  if (!qFuzzyCompare(ratio, mDevicePixelRatio))
  {
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    mDevicePixelRatio = ratio;
    reallocateBuffer();
#else
    qDebug() << Q_FUNC_INFO << "Device pixel ratios not supported for Qt versions before 5.4";
    mDevicePixelRatio = 1.0;
#endif
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPPaintBufferPixmap
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPPaintBufferPixmap
  \brief 


*/

QCPPaintBufferPixmap::QCPPaintBufferPixmap(const QSize &size, double devicePixelRatio) :
  QCPAbstractPaintBuffer(size, devicePixelRatio)
{
  QCPPaintBufferPixmap::reallocateBuffer();
}

QCPPaintBufferPixmap::~QCPPaintBufferPixmap()
{
}

QCPPainter *QCPPaintBufferPixmap::startPainting()
{
  QCPPainter *result = new QCPPainter(&mBuffer);
  result->setRenderHint(QPainter::HighQualityAntialiasing);
  return result;
}

void QCPPaintBufferPixmap::draw(QCPPainter *painter) const
{
  if (painter && painter->isActive())
    painter->drawPixmap(0, 0, mBuffer);
  else
    qDebug() << Q_FUNC_INFO << "invalid or inactive painter passed";
}

void QCPPaintBufferPixmap::clear(const QColor &color)
{
  mBuffer.fill(color);
}

void QCPPaintBufferPixmap::reallocateBuffer()
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


#ifdef QCP_OPENGL_PBUFFER
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPPaintBufferGlPbuffer
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPPaintBufferGlPbuffer
  \brief

*/

QCPPaintBufferGlPbuffer::QCPPaintBufferGlPbuffer(const QSize &size, double devicePixelRatio, int multisamples) :
  QCPAbstractPaintBuffer(size, devicePixelRatio),
  mGlPBuffer(0),
  mMultisamples(qMax(0, multisamples))
{
  QCPPaintBufferGlPbuffer::reallocateBuffer();
}

QCPPaintBufferGlPbuffer::~QCPPaintBufferGlPbuffer()
{
  if (mGlPBuffer)
    delete mGlPBuffer;
}

QCPPainter *QCPPaintBufferGlPbuffer::startPainting()
{
  if (!mGlPBuffer->isValid())
  {
    qDebug() << Q_FUNC_INFO << "OpenGL frame buffer object doesn't exist, reallocateBuffer was not called?";
    return 0;
  }
  
  QCPPainter *result = new QCPPainter(mGlPBuffer);
  result->setRenderHint(QPainter::HighQualityAntialiasing);
  return result;
}

void QCPPaintBufferGlPbuffer::draw(QCPPainter *painter) const
{
  if (!painter || !painter->isActive())
  {
    qDebug() << Q_FUNC_INFO << "invalid or inactive painter passed";
    return;
  }
  if (!mGlPBuffer->isValid())
  {
    qDebug() << Q_FUNC_INFO << "OpenGL pbuffer isn't valid, reallocateBuffer was not called?";
    return;
  }
  painter->drawImage(0, 0, mGlPBuffer->toImage());
}

void QCPPaintBufferGlPbuffer::clear(const QColor &color)
{
  if (mGlPBuffer->isValid())
  {
    mGlPBuffer->makeCurrent();
    glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mGlPBuffer->doneCurrent();
  } else
    qDebug() << Q_FUNC_INFO << "OpenGL pbuffer invalid or context not current";
}

void QCPPaintBufferGlPbuffer::reallocateBuffer()
{
  if (mGlPBuffer)
    delete mGlPBuffer;
  
  QGLFormat format;
  format.setAlpha(true);
  format.setSamples(mMultisamples);
  mGlPBuffer = new QGLPixelBuffer(mSize, format);
}
#endif // QCP_OPENGL_PBUFFER


#ifdef QCP_OPENGL_FBO
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPPaintBufferGlFbo
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPPaintBufferGlFbo
  \brief 

*/
QCPPaintBufferGlFbo::QCPPaintBufferGlFbo(const QSize &size, double devicePixelRatio, QWeakPointer<QOpenGLContext> glContext, QWeakPointer<QOpenGLPaintDevice> glPaintDevice) :
  QCPAbstractPaintBuffer(size, devicePixelRatio),
  mGlContext(glContext),
  mGlPaintDevice(glPaintDevice),
  mGlFrameBuffer(0)
{
  QCPPaintBufferGlFbo::reallocateBuffer();
}

QCPPaintBufferGlFbo::~QCPPaintBufferGlFbo()
{
  if (mGlFrameBuffer)
    delete mGlFrameBuffer;
}

QCPPainter *QCPPaintBufferGlFbo::startPainting()
{
  if (mGlPaintDevice.isNull())
  {
    qDebug() << Q_FUNC_INFO << "OpenGL paint device doesn't exist";
    return 0;
  }
  if (!mGlFrameBuffer)
  {
    qDebug() << Q_FUNC_INFO << "OpenGL frame buffer object doesn't exist, reallocateBuffer was not called?";
    return 0;
  }
  
  if (QOpenGLContext::currentContext() != mGlContext.data())
    mGlContext.data()->makeCurrent(mGlContext.data()->surface());
  mGlFrameBuffer->bind();
  QCPPainter *result = new QCPPainter(mGlPaintDevice.data());
  result->setRenderHint(QPainter::HighQualityAntialiasing);
  return result;
}

void QCPPaintBufferGlFbo::donePainting()
{
  if (mGlFrameBuffer && mGlFrameBuffer->isBound())
    mGlFrameBuffer->release();
  else
    qDebug() << Q_FUNC_INFO << "Either OpenGL frame buffer not valid or was not bound";
}

void QCPPaintBufferGlFbo::draw(QCPPainter *painter) const
{
  if (!painter || !painter->isActive())
  {
    qDebug() << Q_FUNC_INFO << "invalid or inactive painter passed";
    return;
  }
  if (!mGlFrameBuffer)
  {
    qDebug() << Q_FUNC_INFO << "OpenGL frame buffer object doesn't exist, reallocateBuffer was not called?";
    return;
  }
  painter->drawImage(0, 0, mGlFrameBuffer->toImage());
}

void QCPPaintBufferGlFbo::clear(const QColor &color)
{
  if (mGlContext.isNull())
  {
    qDebug() << Q_FUNC_INFO << "OpenGL context doesn't exist";
    return;
  }
  if (!mGlFrameBuffer)
  {
    qDebug() << Q_FUNC_INFO << "OpenGL frame buffer object doesn't exist, reallocateBuffer was not called?";
    return;
  }
  
  if (QOpenGLContext::currentContext() != mGlContext.data())
    mGlContext.data()->makeCurrent(mGlContext.data()->surface());
  mGlFrameBuffer->bind();
  glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mGlFrameBuffer->release();
}

void QCPPaintBufferGlFbo::reallocateBuffer()
{
  // release and delete possibly existing framebuffer:
  if (mGlFrameBuffer)
  {
    if (mGlFrameBuffer->isBound())
      mGlFrameBuffer->release();
    delete mGlFrameBuffer;
    mGlFrameBuffer = 0;
  }
  
  if (mGlContext.isNull())
  {
    qDebug() << Q_FUNC_INFO << "OpenGL context doesn't exist";
    return;
  }
  if (mGlPaintDevice.isNull())
  {
    qDebug() << Q_FUNC_INFO << "OpenGL paint device doesn't exist";
    return;
  }
  
  // create new fbo with appropriate size:
  mGlContext.data()->makeCurrent(mGlContext.data()->surface());
  QOpenGLFramebufferObjectFormat frameBufferFormat;
  frameBufferFormat.setSamples(mGlContext.data()->format().samples());
  frameBufferFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  mGlFrameBuffer = new QOpenGLFramebufferObject(mSize*mDevicePixelRatio, frameBufferFormat);
  if (mGlPaintDevice.data()->size() != mSize*mDevicePixelRatio)
    mGlPaintDevice.data()->setSize(mSize*mDevicePixelRatio);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
  mGlPaintDevice.data()->setDevicePixelRatio(mDevicePixelRatio);
#endif
}
#endif // QCP_OPENGL_FBO
