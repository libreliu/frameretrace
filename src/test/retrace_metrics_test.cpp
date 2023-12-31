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
 * Authors:
 *   Mark Janes <mark.a.janes@intel.com>
 **************************************************************************/

#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

#include "retrace_test.hpp"
#include "glframe_glhelper.hpp"
#include "glframe_metrics.hpp"
#include "glframe_retrace.hpp"
#include "test_bargraph_ctx.hpp"

namespace glretrace {
class MetricsCallback : public OnFrameRetrace {
 public:
  void onFileOpening(bool needUpload,
                     bool finished,
                     uint32_t frame_count) {}
  void onGLError(uint32_t frame_count,
                 const std::string &err,
                 const std::string &call_str) { assert(false); }
  void onShaderAssembly(RenderId renderId,
                        SelectionId sc,
                        ExperimentId experimentCount,
                        const ShaderAssembly &vertex,
                        const ShaderAssembly &fragment,
                        const ShaderAssembly &tess_control,
                        const ShaderAssembly &tess_eval,
                        const ShaderAssembly &geom,
                        const ShaderAssembly &comp) {}
  void onRenderTarget(SelectionId selectionCount,
                      ExperimentId experimentCount,
                      const std::string &label,
                      const uvec & pngImageData) {}
  void onShaderCompile(RenderId renderId, ExperimentId experimentCount,
                       bool status,
                       const std::string &errorString) {}
  void onMetricList(const std::vector<MetricId> &i,
                    const std::vector<std::string> &n,
                    const std::vector<std::string> &desc) {
    ids = i;
    names = n;
  }
  void onMetrics(const MetricSeries &metricData,
                 ExperimentId experimentCount,
                 SelectionId selectionCount) {
    data.push_back(metricData);
    experiment_count = experimentCount;
    selection_count = selectionCount;
  }
  void onLastMetric() {}
  void onApi(SelectionId selectionCount,
             RenderId renderId,
             const std::vector<std::string> &api_calls,
             const std::vector<uint32_t> &error_indices,
             const std::vector<std::string> &errors) {}
  void onError(ErrorSeverity s, const std::string &message) {}
  void onBatch(SelectionId selectionCount,
               ExperimentId experimentCount,
               RenderId renderId,
               const std::string &batch) {}
  void onUniform(SelectionId selectionCount,
                 ExperimentId experimentCount,
                 RenderId renderId,
                 const std::string &name,
                 UniformType type,
                 UniformDimension dimension,
                 const std::vector<unsigned char> &data) {}
  void onState(SelectionId selectionCount,
               ExperimentId experimentCount,
               RenderId renderId,
               StateKey item,
               const std::vector<std::string> &value) {}
  void onTextureData(ExperimentId experimentCount,
                     const std::string &md5sum,
                     const std::vector<unsigned char> &image) {}
  void onTexture(SelectionId selectionCount,
                 ExperimentId experimentCount,
                 RenderId renderId,
                 TextureKey binding,
                 const std::vector<TextureData> &images) {}
  std::vector<MetricId> ids;
  std::vector<std::string> names;
  std::vector<MetricSeries> data;
  ExperimentId experiment_count;
  SelectionId selection_count;
};

TEST_F(RetraceTest, ReadMetrics) {
  TestContext c;

  MetricsCallback cb;
  PerfMetrics *p = PerfMetrics::Create(&cb);
  EXPECT_EQ(cb.ids.size(), cb.names.size());
  // check ids are unique
  std::set<MetricId> mets;
  for (auto id : cb.ids) {
    EXPECT_EQ(mets.find(id), mets.end());
    mets.insert(id);
  }
  delete p;
}

static const char *test_file = CMAKE_CURRENT_SOURCE_DIR "/simple.trace";

TEST_F(RetraceTest, SingleMetricData) {
  retrace::setUp();
  GlFunctions::Init();
  TestContext c;
  MetricsCallback cb;

  FrameRetrace rt;
  rt.openFile(test_file, md5, fileSize, 7, 1, &cb);

  PerfMetrics *p = PerfMetrics::Create(&cb);
  if (!cb.ids.size())
    return;
  bool found = false;

  for (size_t i = 0; i < cb.ids.size(); ++i) {
    if (cb.names[i] == "GPU Time Elapsed") {
      found = true;
      p->selectMetric(cb.ids[i]);
      break;
    }
    if (cb.names[i] == "GPUTime") {
      found = true;
      p->selectMetric(cb.ids[i]);
      break;
    }
  }
  if (!found)
    p->selectMetric(cb.ids[0]);
  EXPECT_TRUE(found);

  p->beginContext();
  p->begin(RenderId(0));
  RenderSelection s;
  s.id = SelectionId(0);
  s.series.push_back(RenderSequence(RenderId(0), RenderId(2)));
  rt.retraceRenderTarget(ExperimentId(0), s,
                         glretrace::NORMAL_RENDER,
                         glretrace::STOP_AT_RENDER, &cb);
  p->end();
  p->endContext();
  p->publish(ExperimentId(1), SelectionId(0), &cb);
  EXPECT_EQ(cb.experiment_count.count(), 1);
  EXPECT_EQ(cb.data.size(), 1);
  for (float d : cb.data[0].data) {
    EXPECT_GT(d, 0.0);
  }
  delete p;
  retrace::cleanUp();
}

TEST_F(RetraceTest, FrameMetricData) {
  GlFunctions::Init();
  MetricsCallback cb;

  FrameRetrace rt;
  rt.openFile(test_file, md5, fileSize, 7, 1, &cb);
  if (!cb.ids.size()) {
    retrace::cleanUp();
    return;
  }
  MetricId id;
  bool found = false;
  for (size_t i = 0; i < cb.ids.size(); ++i) {
    if (cb.names[i] == "GPU Time Elapsed") {
      found = true;
      id = cb.ids[i];
      break;
    }
    if (cb.names[i] == "GPUTime") {
      found = true;
      id = cb.ids[i];
      break;
    }
  }
  EXPECT_TRUE(found);
  // EXPECT_GT(id(), 0);
  const std::vector<MetricId> mets = { id };
  const ExperimentId experiment(1);
  rt.retraceMetrics(mets, experiment, &cb);
  EXPECT_EQ(cb.experiment_count.count(), 1);
  EXPECT_EQ(cb.data.size(), 1);
  for (float d : cb.data[0].data) {
    EXPECT_GT(d, 0.0);
  }
  retrace::cleanUp();
}

TEST_F(RetraceTest, AllMetricData) {
  GlFunctions::Init();
  MetricsCallback cb;

  FrameRetrace rt;
  rt.openFile(test_file, md5, fileSize, 7, 1, &cb);
  if (!cb.ids.size()) {
    retrace::cleanUp();
    return;
  }

  // get all metrics for render 0 and 1
  RenderSelection sel;
  sel.id = SelectionId(777);
  sel.series.resize(2);
  sel.series[0].begin = RenderId(0);
  sel.series[0].end = RenderId(1);
  sel.series[1].begin = RenderId(1);
  sel.series[1].end = RenderId(2);
  const ExperimentId experiment(1);
  rt.retraceAllMetrics(sel, experiment, &cb);
  EXPECT_EQ(cb.experiment_count.count(), 1);
  EXPECT_EQ(cb.selection_count.count(), 777);
  EXPECT_GT(cb.data.size(), 1);  // one callback for each metric
  for (const MetricSeries &s : cb.data) {
    for (float d : s.data) {
      EXPECT_GT(d, -0.1);
    }
  }
  retrace::cleanUp();
}
}  // namespace glretrace
