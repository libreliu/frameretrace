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

#ifndef _GLFRAME_RETRACE_SKELETON_HPP_
#define _GLFRAME_RETRACE_SKELETON_HPP_

#include <string>
#include <vector>

#include "glframe_thread.hpp"
#include "glframe_retrace.hpp"

namespace ApiTrace {
class RetraceResponse;
}  // namespace ApiTrace

namespace glretrace {
class Socket;
class FrameRetrace;
class CancellationThread;

// handles retrace requests coming in through a socket, executes them,
// and formats responses back through the socket
class FrameRetraceSkeleton : public Thread,
                             public OnFrameRetrace {
 public:
  // call once, to set up the retrace socket, and shut it down at
  // exit
  FrameRetraceSkeleton(Socket *sock,
                       Socket *cancel_socket,
                       IFrameRetrace *frameretrace = NULL);
  virtual void Run();

  // callback responses, to be sent through the socket to the caller
  virtual void onShaderAssembly(RenderId renderId,
                                SelectionId selectionCount,
                                ExperimentId experimentCount,
                                const ShaderAssembly &vertex_shader,
                                const ShaderAssembly &fragment_shader,
                                const ShaderAssembly &tess_control_shader,
                                const ShaderAssembly &tess_eval_shader,
                                const ShaderAssembly &geom_shader,
                                const ShaderAssembly &comp_shader);
  virtual void onFileOpening(bool needs_upload,
                             bool finished,
                             uint32_t frame_count);
  virtual void onGLError(uint32_t frame_count,
                         const std::string &err,
                         const std::string &call_str);
  virtual void onRenderTarget(SelectionId selectionCount,
                              ExperimentId experimentCount,
                              const std::string &label,
                              const uvec & pngImageData);
  virtual void onShaderCompile(RenderId renderId,
                               ExperimentId experimentCount,
                               bool status,
                               const std::string &errorString);
  virtual void onMetricList(const std::vector<MetricId> &ids,
                            const std::vector<std::string> &names,
                            const std::vector<std::string> &desc);
  virtual void onMetrics(const MetricSeries &metricData,
                         ExperimentId experimentCount,
                         SelectionId selectionCount);
  virtual void onLastMetric();
  virtual void onApi(SelectionId selectionCount,
                     RenderId renderid,
                     const std::vector<std::string> &api_calls,
                     const std::vector<uint32_t> &error_indices,
                     const std::vector<std::string> &errors);
  virtual void onError(ErrorSeverity s, const std::string &message);

  virtual void onBatch(SelectionId selectionCount,
                       ExperimentId experimentCount,
                       RenderId renderId,
                       const std::string &batch);

  virtual void onUniform(SelectionId selectionCount,
                         ExperimentId experimentCount,
                         RenderId renderId,
                         const std::string &name,
                         UniformType type,
                         UniformDimension dimension,
                         const std::vector<unsigned char> &data);
  virtual void onState(SelectionId selectionCount,
                       ExperimentId experimentCount,
                       RenderId renderId,
                       StateKey item,
                       const std::vector<std::string> &value);
  virtual void onTextureData(ExperimentId experimentCount,
                             const std::string &md5sum,
                             const std::vector<unsigned char> &image);
  virtual void onTexture(SelectionId selectionCount,
                         ExperimentId experimentCount,
                         RenderId renderId,
                         TextureKey binding,
                         const std::vector<TextureData> &images);

 protected:
  bool m_force_upload;  // for unit test

 private:
  Socket *m_socket;
  std::vector<unsigned char> m_buf;
  IFrameRetrace *m_frame;
  CancellationThread *m_cancel;
  bool m_fatal_error;

  // For aggregating metrics callbacks on a series of requests.
  // retraceMetrics is called several times, calling the onMetrics
  // callback synchronously in the same thread.  Aggregated metrics
  // are returned through the socket in a single response.  The
  // calling stub will call onMetrics several times on the host
  // system.
  ApiTrace::RetraceResponse *m_multi_metrics_response;
};

}  // namespace glretrace

#endif  // _GLFRAME_RETRACE_SKELETON_HPP_
