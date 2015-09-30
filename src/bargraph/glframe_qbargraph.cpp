/**************************************************************************
 *
 * Copyright 2015 Intel Corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include "glframe_qbargraph.hpp"

#include <QtOpenGL>

#include <vector>

using glretrace::QBarGraphRenderer;
using glretrace::BarGraphView;
using glretrace::BarMetrics;

QBarGraphRenderer::QBarGraphRenderer() : m_graph(true) {
  std::vector<BarMetrics> metrics(4);
  metrics[0].metric1 = 1;
  metrics[0].metric2 = 1;
  metrics[1].metric1 = 2;
  metrics[1].metric2 = 2;
  metrics[2].metric1 = 1;
  metrics[2].metric2 = 1;
  metrics[3].metric1 = 2;
  metrics[3].metric2 = 2;
  m_graph.setBars(metrics);
}

void
QBarGraphRenderer::render() {
  m_graph.render();
}

void
QBarGraphRenderer::synchronize(QQuickFramebufferObject * item) {
}

QOpenGLFramebufferObject *
QBarGraphRenderer::createFramebufferObject(const QSize & size) {
  QOpenGLFramebufferObjectFormat format;
  format.setSamples(20);
  return new QOpenGLFramebufferObject(size, format);
}


QQuickFramebufferObject::Renderer *
BarGraphView::createRenderer() const {
  return new QBarGraphRenderer();
}

BarGraphView::BarGraphView() {
}

