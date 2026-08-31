/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * GlView sample - draws a spinning cube with the application's own GLES code.
 *
 * The layout is deliberately awkward so that placement bugs are visible rather than
 * plausible: the GlView is inset, off-centre, and smaller than the window, and it sits
 * between two bands of ordinary DALi content.
 *
 *   - If the GL content escapes the frame, the view is ignoring
 *     GlViewRenderInfo::GetClippingBox(); if it lands in the wrong place or at the wrong
 *     scale, it is ignoring GetMvpMatrix() - or narrowing the viewport, which applies the
 *     view's placement twice.
 *   - If the *bottom* band of coloured DALi views breaks up, the GL code has corrupted
 *     DALi's GL state. That band is drawn after the GlView on purpose. Press C to see
 *     it happen deliberately.
 *   - The checkerboard on the cube comes from a DALi texture handed over with
 *     BindTextureResources(). Press X to unbind it and the cube falls back to flat
 *     per-face colours, which is also what happens on the first frames while the
 *     texture upload is still in flight.
 *
 * Keys:
 *   B   - rebuild the view with the other backend (DIRECT / UNSAFE_DIRECT)
 *   M   - toggle CONTINUOUS / ON_DEMAND rendering mode
 *   R   - RenderOnce() (only has a visible effect in ON_DEMAND)
 *   A   - animate the view's position, scale and rotation
 *   C   - stop restoring GL state after drawing (UNSAFE backend only - shows the hazard)
 *   X   - bind / unbind the DALi texture the cube is drawn with
 *   T   - Terminate() the view
 *   ESC - quit
 *
 * Threading: the GL callbacks run on the DALi render thread, so they must not touch
 * DALi APIs. Everything they want to report goes through atomics and is picked up by a
 * Timer on the event thread.
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

#include <GLES2/gl2.h>

#include <atomic>
#include <string>
#include <cmath>
#include <cstdio>
#include <memory>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float    GL_VIEW_INSET_X    = 40.0f;
constexpr float    GL_VIEW_TOP        = 184.0f;
constexpr float    BOTTOM_BAND_HEIGHT = 72.0f;
constexpr float    STATUS_HEIGHT      = 176.0f;
constexpr uint32_t STATUS_INTERVAL_MS = 200u;

const char* const VERTEX_SHADER =
  "attribute mediump vec3 aPosition;\n"
  "attribute mediump vec3 aColor;\n"
  "attribute mediump vec2 aTexCoord;\n"
  "uniform   mediump mat4 uMvp;\n"
  "uniform   mediump vec4 uWorldColor;\n"
  "varying   mediump vec4 vColor;\n"
  "varying   mediump vec2 vTexCoord;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = uMvp * vec4(aPosition, 1.0);\n"
  "  vColor      = vec4(aColor, 1.0) * uWorldColor;\n"
  "  vTexCoord   = aTexCoord;\n"
  "}\n";

const char* const FRAGMENT_SHADER =
  "precision mediump float;\n"
  "varying mediump vec4      vColor;\n"
  "varying mediump vec2      vTexCoord;\n"
  "uniform         sampler2D uTexture;\n"
  "uniform lowp    float     uTextureMix;\n"
  "void main()\n"
  "{\n"
  "  mediump vec4 texel = texture2D(uTexture, vTexCoord);\n"
  "  gl_FragColor = mix(vColor, texel * vColor, uTextureMix);\n"
  "}\n";

/**
 * Unit cube centred on the origin, one flat colour per face plus a full 0..1 UV per face.
 * Scaled to the view size by the caller, so the cube tracks the layout.
 */
// clang-format off
const GLfloat CUBE_VERTICES[] = {
  //    x       y       z         r       g       b         u       v
   -0.50f,  -0.50f,   0.50f,     0.90f,   0.30f,   0.30f,     0.00f,   0.00f,
    0.50f,  -0.50f,   0.50f,     0.90f,   0.30f,   0.30f,     1.00f,   0.00f,
    0.50f,   0.50f,   0.50f,     0.90f,   0.30f,   0.30f,     1.00f,   1.00f,
   -0.50f,   0.50f,   0.50f,     0.90f,   0.30f,   0.30f,     0.00f,   1.00f,

   -0.50f,  -0.50f,  -0.50f,     0.30f,   0.60f,   0.90f,     0.00f,   0.00f,
    0.50f,  -0.50f,  -0.50f,     0.30f,   0.60f,   0.90f,     1.00f,   0.00f,
    0.50f,   0.50f,  -0.50f,     0.30f,   0.60f,   0.90f,     1.00f,   1.00f,
   -0.50f,   0.50f,  -0.50f,     0.30f,   0.60f,   0.90f,     0.00f,   1.00f,

   -0.50f,  -0.50f,  -0.50f,     0.40f,   0.85f,   0.40f,     0.00f,   0.00f,
   -0.50f,  -0.50f,   0.50f,     0.40f,   0.85f,   0.40f,     1.00f,   0.00f,
   -0.50f,   0.50f,   0.50f,     0.40f,   0.85f,   0.40f,     1.00f,   1.00f,
   -0.50f,   0.50f,  -0.50f,     0.40f,   0.85f,   0.40f,     0.00f,   1.00f,

    0.50f,  -0.50f,  -0.50f,     0.95f,   0.80f,   0.25f,     0.00f,   0.00f,
    0.50f,  -0.50f,   0.50f,     0.95f,   0.80f,   0.25f,     1.00f,   0.00f,
    0.50f,   0.50f,   0.50f,     0.95f,   0.80f,   0.25f,     1.00f,   1.00f,
    0.50f,   0.50f,  -0.50f,     0.95f,   0.80f,   0.25f,     0.00f,   1.00f,

   -0.50f,   0.50f,  -0.50f,     0.75f,   0.40f,   0.90f,     0.00f,   0.00f,
    0.50f,   0.50f,  -0.50f,     0.75f,   0.40f,   0.90f,     1.00f,   0.00f,
    0.50f,   0.50f,   0.50f,     0.75f,   0.40f,   0.90f,     1.00f,   1.00f,
   -0.50f,   0.50f,   0.50f,     0.75f,   0.40f,   0.90f,     0.00f,   1.00f,

   -0.50f,  -0.50f,  -0.50f,     0.55f,   0.55f,   0.60f,     0.00f,   0.00f,
    0.50f,  -0.50f,  -0.50f,     0.55f,   0.55f,   0.60f,     1.00f,   0.00f,
    0.50f,  -0.50f,   0.50f,     0.55f,   0.55f,   0.60f,     1.00f,   1.00f,
   -0.50f,  -0.50f,   0.50f,     0.55f,   0.55f,   0.60f,     0.00f,   1.00f,
};

const GLushort CUBE_INDICES[] = {
   0,  1,  2,   0,  2,  3,
   4,  6,  5,   4,  7,  6,
   8,  9, 10,   8, 10, 11,
  12, 14, 13,  12, 15, 14,
  16, 17, 18,  16, 18, 19,
  20, 22, 21,  20, 23, 22,
};
// clang-format on

const char* ToString(GlView::BackendMode mode)
{
  return (mode == GlView::BackendMode::UNSAFE_DIRECT_RENDERING) ? "UNSAFE_DIRECT_RENDERING"
                                                                : "DIRECT_RENDERING";
}

const char* ToString(GlView::RenderingMode mode)
{
  return (mode == GlView::RenderingMode::ON_DEMAND) ? "ON_DEMAND" : "CONTINUOUS";
}

/**
 * @brief Builds a checkerboard texture through DALi, not through GL.
 *
 * The point of BindTextureResources() is that the application never has to touch the
 * pixels from the rendering thread. A real application would let DALi load an image file
 * asynchronously here; this one just generates something recognisable.
 */
Texture MakeCheckerTexture()
{
  constexpr uint32_t SIDE  = 64u;
  constexpr uint32_t CELL  = 8u;
  constexpr uint32_t BYTES = SIDE * SIDE * 4u;

  auto* pixels = static_cast<uint8_t*>(malloc(BYTES));
  for(uint32_t y = 0u; y < SIDE; ++y)
  {
    for(uint32_t x = 0u; x < SIDE; ++x)
    {
      const bool     light = (((x / CELL) + (y / CELL)) % 2u) == 0u;
      const uint8_t  value = light ? 0xFF : 0x40;
      uint8_t* const texel = pixels + (y * SIDE + x) * 4u;
      texel[0]             = value;
      texel[1]             = value;
      texel[2]             = value;
      texel[3]             = 0xFF;
    }
  }

  PixelData pixelData = PixelData::New(pixels, BYTES, SIDE, SIDE, Pixel::RGBA8888, PixelData::ReleaseFunction::FREE);
  Texture   texture   = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, SIDE, SIDE);
  texture.Upload(pixelData);
  return texture;
}

GLuint CompileShader(GLenum type, const char* source)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  GLint compiled = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if(compiled != GL_TRUE)
  {
    char log[512] = {0};
    glGetShaderInfoLog(shader, sizeof(log) - 1, nullptr, log);
    printf("GlView sample: shader compile failed: %s\n", log);
    glDeleteShader(shader);
    return 0u;
  }
  return shader;
}
} // namespace

// ---------------------------------------------------------------------------
// The application's GL renderer. Every method here runs on the render thread.
// ---------------------------------------------------------------------------

class CubeRenderer
{
public:
  void OnGlInit(const GlViewRenderInfo& /*info*/)
  {
    GLuint vertexShader   = CompileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    if(vertexShader == 0u || fragmentShader == 0u)
    {
      return;
    }

    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertexShader);
    glAttachShader(mProgram, fragmentShader);
    glLinkProgram(mProgram);

    GLint linked = GL_FALSE;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
    if(linked != GL_TRUE)
    {
      char log[512] = {0};
      glGetProgramInfoLog(mProgram, sizeof(log) - 1, nullptr, log);
      printf("GlView sample: program link failed: %s\n", log);
      glDeleteProgram(mProgram);
      mProgram = 0u;
    }

    // The shaders are owned by the program from here on.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if(mProgram == 0u)
    {
      return;
    }

    mPositionLocation   = glGetAttribLocation(mProgram, "aPosition");
    mColorLocation      = glGetAttribLocation(mProgram, "aColor");
    mTexCoordLocation   = glGetAttribLocation(mProgram, "aTexCoord");
    mMvpLocation        = glGetUniformLocation(mProgram, "uMvp");
    mWorldColorLocation = glGetUniformLocation(mProgram, "uWorldColor");
    mTextureLocation    = glGetUniformLocation(mProgram, "uTexture");
    mTextureMixLocation = glGetUniformLocation(mProgram, "uTextureMix");

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);

    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CUBE_INDICES), CUBE_INDICES, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    mInitialised = true;
    mInitCount.fetch_add(1u);
  }

  bool OnGlRenderFrame(const GlViewRenderInfo& info)
  {
    mFrameCount.fetch_add(1u);

    const BoundsInteger& box = info.GetClippingBox();
    const Size&          size = info.GetSize();

    mReportedWidth.store(box.width);
    mReportedHeight.store(box.height);
    mReportedX.store(box.x);
    mReportedY.store(box.y);

    if(!mInitialised)
    {
      return false;
    }

    // Save the pieces of GL state the framework does not restore for us. Without this
    // the UNSAFE backend leaves DALi clipped to our scissor box for the rest of the frame.
    GLint     savedScissorBox[4]  = {0, 0, 0, 0};
    GLboolean savedScissorEnabled = GL_FALSE;
    GLboolean savedDepthEnabled   = GL_FALSE;
    if(mRestoreState)
    {
      glGetIntegerv(GL_SCISSOR_BOX, savedScissorBox);
      savedScissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
      savedDepthEnabled   = glIsEnabled(GL_DEPTH_TEST);
    }

    // Confine drawing to the region GlView told us about. GetClippingBox() is already in
    // the convention glScissor() expects.
    //
    // The viewport is deliberately left as it is. GetMvpMatrix() maps the view's local
    // space to whatever viewport is current when the callback runs, so narrowing it to
    // the clipping box would apply the view's placement a second time.
    glScissor(box.x, box.y, box.width, box.height);
    glEnable(GL_SCISSOR_TEST);
    // DALi requests a depth buffer by default (DALI_DISABLE_DEPTH_BUFFER turns it off, in
    // which case the cube's faces will draw in the wrong order). Note that on the UNSAFE
    // backend this shares DALi's depth buffer, so the clear below is visible to DALi for
    // the rest of the frame - inside the scissor box only.
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.06f, 0.07f, 0.10f, 1.0f);
    glClear(static_cast<GLbitfield>(GL_COLOR_BUFFER_BIT) | static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT));

    glUseProgram(mProgram);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

    const GLsizei stride = static_cast<GLsizei>(8 * sizeof(GLfloat));
    glEnableVertexAttribArray(static_cast<GLuint>(mPositionLocation));
    glVertexAttribPointer(static_cast<GLuint>(mPositionLocation), 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(static_cast<GLuint>(mColorLocation));
    glVertexAttribPointer(static_cast<GLuint>(mColorLocation), 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<const void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(static_cast<GLuint>(mTexCoordLocation));
    glVertexAttribPointer(static_cast<GLuint>(mTexCoordLocation), 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<const void*>(6 * sizeof(GLfloat)));

    // The texture DALi loaded for us, if there is one this frame. An entry reads 0 until
    // its upload has been processed, and backends that render into an isolated context
    // report no textures at all, so both have to be handled.
    const auto&    handles    = info.GetBoundTextureNativeHandles();
    const uint32_t boundCount = static_cast<uint32_t>(handles.Count());
    const GLuint   texture    = (boundCount > 0u) ? static_cast<GLuint>(handles[0]) : 0u;

    mBoundTextureCount.store(boundCount);
    mBoundTextureHandle.store(texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    if(texture != 0u)
    {
      // Filtering is texture object state, so this is shared with DALi. Setting it here is
      // safe only because DALi does not draw this texture itself in this sample - it re-applies
      // its own sampler state whenever it does use a texture.
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glUniform1i(mTextureLocation, 0);
    glUniform1f(mTextureMixLocation, (texture != 0u) ? 1.0f : 0.0f);

    // Local space is the view's own pixel space, origin at the centre, exactly as DALi
    // shaders see it. Scaling the unit cube by the view size and letting the MVP do the
    // rest is what makes the cube follow the layout, animation and parent transforms.
    const float extent = std::min(size.width, size.height) * 0.45f;

    Matrix model(false);
    model.SetIdentityAndScale(Vector3(extent, extent, extent));
    Matrix rotation(false);
    rotation.SetTransformComponents(Vector3::ONE,
                                    Quaternion(Radian(mAngle * 0.6f), Vector3::YAXIS) *
                                      Quaternion(Radian(mAngle), Vector3::XAXIS),
                                    Vector3::ZERO);

    // Dali::Matrix::Multiply(result, lhs, rhs) computes result = rhs * lhs, so these read
    // "scale first, then rotate" and "local transform first, then the view's own MVP".
    Matrix local(false);
    Matrix::Multiply(local, model, rotation);

    Matrix mvp(false);
    Matrix::Multiply(mvp, local, info.GetMvpMatrix());

    glUniformMatrix4fv(mMvpLocation, 1, GL_FALSE, mvp.AsFloat());

    const Vector4& worldColorMultiplier = info.GetWorldColorMultiplier();
    glUniform4f(mWorldColorLocation, worldColorMultiplier.r, worldColorMultiplier.g, worldColorMultiplier.b, worldColorMultiplier.a);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sizeof(CUBE_INDICES) / sizeof(CUBE_INDICES[0])),
                   GL_UNSIGNED_SHORT, nullptr);

    mAngle += 0.02f;

    if(mRestoreState)
    {
      glDisableVertexAttribArray(static_cast<GLuint>(mPositionLocation));
      glDisableVertexAttribArray(static_cast<GLuint>(mColorLocation));
      glDisableVertexAttribArray(static_cast<GLuint>(mTexCoordLocation));
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glUseProgram(0);

      glScissor(savedScissorBox[0], savedScissorBox[1], savedScissorBox[2], savedScissorBox[3]);
      if(savedScissorEnabled == GL_FALSE)
      {
        glDisable(GL_SCISSOR_TEST);
      }
      if(savedDepthEnabled == GL_FALSE)
      {
        glDisable(GL_DEPTH_TEST);
      }
    }

    return true;
  }

  void OnGlTerminate()
  {
    if(mVertexBuffer != 0u)
    {
      glDeleteBuffers(1, &mVertexBuffer);
      mVertexBuffer = 0u;
    }
    if(mIndexBuffer != 0u)
    {
      glDeleteBuffers(1, &mIndexBuffer);
      mIndexBuffer = 0u;
    }
    if(mProgram != 0u)
    {
      glDeleteProgram(mProgram);
      mProgram = 0u;
    }
    mInitialised = false;
    mTerminateCount.fetch_add(1u);
  }

  /**
   * Invoked on the event thread once GlView::Terminate() has run to completion. Unlike
   * OnGlTerminate() above it is always invoked, so it is the point at which this renderer
   * is known to be unreachable from the rendering thread.
   */
  void OnTerminateCompleted()
  {
    mTerminateCompleted = true;
  }
  bool IsTerminateCompleted() const { return mTerminateCompleted; }

  // Read from the event thread.
  void SetRestoreState(bool restore)
  {
    mRestoreState.store(restore);
  }
  bool     GetRestoreState() const { return mRestoreState.load(); }
  uint32_t GetInitCount() const { return mInitCount.load(); }
  uint32_t GetFrameCount() const { return mFrameCount.load(); }
  uint32_t GetTerminateCount() const { return mTerminateCount.load(); }
  int32_t  GetReportedX() const { return mReportedX.load(); }
  int32_t  GetReportedY() const { return mReportedY.load(); }
  int32_t  GetReportedWidth() const { return mReportedWidth.load(); }
  int32_t  GetReportedHeight() const { return mReportedHeight.load(); }
  uint32_t GetBoundTextureCount() const { return mBoundTextureCount.load(); }
  uint32_t GetBoundTextureHandle() const { return mBoundTextureHandle.load(); }

private:
  // Render thread only.
  GLuint mProgram{0u};
  GLuint mVertexBuffer{0u};
  GLuint mIndexBuffer{0u};
  GLint  mPositionLocation{-1};
  GLint  mColorLocation{-1};
  GLint  mTexCoordLocation{-1};
  GLint  mMvpLocation{-1};
  GLint  mWorldColorLocation{-1};
  GLint  mTextureLocation{-1};
  GLint  mTextureMixLocation{-1};
  bool   mInitialised{false};
  float  mAngle{0.0f};

  // Shared with the event thread.
  std::atomic_bool     mRestoreState{true};
  std::atomic_uint32_t mInitCount{0u};
  std::atomic_uint32_t mFrameCount{0u};
  std::atomic_uint32_t mTerminateCount{0u};

  /// Event thread only.
  bool mTerminateCompleted{false};
  std::atomic_int32_t  mReportedX{0};
  std::atomic_int32_t  mReportedY{0};
  std::atomic_int32_t  mReportedWidth{0};
  std::atomic_int32_t  mReportedHeight{0};
  std::atomic_uint32_t mBoundTextureCount{0u};
  std::atomic_uint32_t mBoundTextureHandle{0u};
};

// ---------------------------------------------------------------------------
// Sample application. Everything here runs on the event thread.
// ---------------------------------------------------------------------------

class GlViewSample : public ConnectionTracker
{
public:
  explicit GlViewSample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &GlViewSample::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::BLACK);
    window.KeyEventSignal().Connect(this, &GlViewSample::OnKeyEvent);

    auto posSize = window.GetPositionSize();
    mW           = static_cast<float>(posSize.width);
    mH           = static_cast<float>(posSize.height);

    mStatus = Label::New("");
    mStatus.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(8.0f, 4.0f, mW - 16.0f, STATUS_HEIGHT)));
    mStatus.SetTextColor(UiColor(1.0f, 1.0f, 1.0f, 1.0f));
    mStatus.SetFontSize(22.0f);
    mStatus.SetMultiLine(true);

    // The slot both frames the GL region and owns the GlView, so rebuilding the view on a
    // backend switch never disturbs the sibling order that the canary band depends on.
    // Giving the GlView a non-identity parent transform also makes the MVP less trivial.
    const LayoutRect slotBounds = GlViewBounds();
    mGlSlot                     = AbsoluteLayout::New();
    mGlSlot.SetBackgroundColor(UiColor(0x303848));
    mGlSlot.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(slotBounds));

    mRoot = AbsoluteLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.Add(mGlSlot);

    // Added after the GlView's slot, so it is drawn after it. If the GL code corrupts
    // DALi's state this band is what visibly breaks.
    mRoot.Add(MakeCanaryBand());
    mRoot.Add(mStatus);

    mTexture = MakeCheckerTexture();

    BuildGlView();

    window.Add(mRoot);

    mStatusTimer = Timer::New(STATUS_INTERVAL_MS);
    mStatusTimer.TickSignal().Connect(this, &GlViewSample::OnStatusTick);
    mStatusTimer.Start();

    UpdateStatus();
  }

  CubeRenderer& Renderer() const
  {
    return *mRenderers.back();
  }

  /**
   * A retired renderer is only unreachable from the rendering thread once its view has
   * reported the terminate back, so this is what waits for that. The last one is always
   * kept: it belongs to the view on screen, and the status line reads its counters.
   */
  void ReclaimRetiredRenderers()
  {
    const CubeRenderer* current = mRenderers.empty() ? nullptr : mRenderers.back().get();

    for(auto iter = mRenderers.begin(); iter != mRenderers.end();)
    {
      if(iter->get() != current && (*iter)->IsTerminateCompleted())
      {
        ++mReclaimedCount;
        iter = mRenderers.erase(iter);
      }
      else
      {
        ++iter;
      }
    }
  }

  /**
   * How many Terminate() completion callbacks have been seen, retired renderers included.
   */
  uint32_t CompletedCount() const
  {
    return mReclaimedCount + ((!mRenderers.empty() && mRenderers.back()->IsTerminateCompleted()) ? 1u : 0u);
  }

  /**
   * Deliberately inset and off-centre: the horizontal inset is asymmetric so that GL
   * content drawn at the window origin instead of the view's own position is obvious.
   */
  LayoutRect GlViewBounds() const
  {
    return LayoutRect(GL_VIEW_INSET_X,
                      GL_VIEW_TOP,
                      mW - GL_VIEW_INSET_X * 3.0f,
                      mH - GL_VIEW_TOP - BOTTOM_BAND_HEIGHT - 24.0f);
  }

  View MakeCanaryBand() const
  {
    StackLayout band = StackLayout::New(StackOrientation::HORIZONTAL);
    band.SetSpacing(6.0f);
    band.SetLayoutParams(AbsoluteLayoutParams::New()
                           .SetBounds(LayoutRect(0.0f, mH - BOTTOM_BAND_HEIGHT, mW, BOTTOM_BAND_HEIGHT)));

    static const uint32_t COLORS[] = {0xE05050, 0xE0A030, 0xD8D040, 0x50C060, 0x4090D0, 0x8060D0};
    for(uint32_t color : COLORS)
    {
      View swatch = View::New();
      swatch.SetBackgroundColor(UiColor(color));
      swatch.SetRequestedHeight(MATCH_PARENT);
      swatch.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
      band.Add(swatch);
    }
    return band;
  }

  /**
   * The backend is fixed at construction, so switching it means building a new view.
   */
  void BuildGlView()
  {
    if(mGlView)
    {
      mGlView.Terminate(Ui::Callback<void()>::New(&Renderer(), &CubeRenderer::OnTerminateCompleted));
      mGlSlot.Remove(mGlView);
      mGlView.Reset();
    }

    ReclaimRetiredRenderers();

    // Inset inside the slot so the slot's background shows as a frame around the GL area.
    const LayoutRect slot = GlViewBounds();
    const float      pad  = 3.0f;

    const bool restoreState = mRenderers.empty() ? true : Renderer().GetRestoreState();
    mRenderers.push_back(std::make_unique<CubeRenderer>());
    CubeRenderer* renderer = mRenderers.back().get();
    renderer->SetRestoreState(restoreState);

    mGlView = GlView::New(mBackendMode);
    mGlView.SetLayoutParams(AbsoluteLayoutParams::New()
                              .SetBounds(LayoutRect(pad, pad, slot.width - pad * 2.0f, slot.height - pad * 2.0f)));
    mGlView.RegisterGlCallbacks(
      Ui::Callback<void(const GlViewRenderInfo&)>::New(renderer, &CubeRenderer::OnGlInit),
      Ui::Callback<bool(const GlViewRenderInfo&)>::New(renderer, &CubeRenderer::OnGlRenderFrame),
      Ui::Callback<void()>::New(renderer, &CubeRenderer::OnGlTerminate));
    mGlView.SetRenderingMode(mRenderingMode);

    mGlSlot.Add(mGlView);

    ApplyTextureBinding();

    mTerminated = false;
  }

  /**
   * Rebinding is cheap and idempotent, so the same call covers "bind" and "unbind".
   */
  void ApplyTextureBinding()
  {
    Dali::Vector<Texture> textures;
    if(mTextureBound)
    {
      textures.PushBack(mTexture);
    }
    mGlView.BindTextureResources(std::move(textures));
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    const auto key = event.GetKeyName();

    if(key == "b" || key == "B")
    {
      mBackendMode = (mBackendMode == GlView::BackendMode::DIRECT_RENDERING)
                       ? GlView::BackendMode::UNSAFE_DIRECT_RENDERING
                       : GlView::BackendMode::DIRECT_RENDERING;
      BuildGlView();
    }
    else if(key == "m" || key == "M")
    {
      mRenderingMode = (mRenderingMode == GlView::RenderingMode::CONTINUOUS)
                         ? GlView::RenderingMode::ON_DEMAND
                         : GlView::RenderingMode::CONTINUOUS;
      if(mGlView)
      {
        mGlView.SetRenderingMode(mRenderingMode);
      }
    }
    else if(key == "r" || key == "R")
    {
      if(mGlView)
      {
        mGlView.RenderOnce();
      }
    }
    else if(key == "a" || key == "A")
    {
      ToggleAnimation();
    }
    else if(key == "c" || key == "C")
    {
      Renderer().SetRestoreState(!Renderer().GetRestoreState());
    }
    else if(key == "x" || key == "X")
    {
      mTextureBound = !mTextureBound;
      ApplyTextureBinding();
    }
    else if(key == "t" || key == "T")
    {
      if(mGlView && !mTerminated)
      {
        mGlView.Terminate(Ui::Callback<void()>::New(&Renderer(), &CubeRenderer::OnTerminateCompleted));
        mTerminated = true;
      }
    }

    UpdateStatus();
  }

  /**
   * Moves, scales and rotates the view so the GL content has to follow a non-trivial
   * MVP. If the cube stays put while the frame behind it moves, the MVP is being ignored.
   */
  void ToggleAnimation()
  {
    if(mAnimation)
    {
      mAnimation.Stop();
      mAnimation.Clear();
      mAnimation.Reset();
      if(mGlView)
      {
        mGlView.SetProperty(Actor::Property::SCALE, Vector3::ONE);
        mGlView.SetProperty(Actor::Property::ORIENTATION, Quaternion());
      }
      mAnimating = false;
      return;
    }

    if(!mGlView)
    {
      return;
    }

    mAnimation = Animation::New(4.0f);
    mAnimation.AnimateTo(Property(mGlView, Actor::Property::SCALE), Vector3(0.65f, 0.65f, 1.0f),
                         AlphaFunction::SIN, TimePeriod(0.0f, 2.0f));
    mAnimation.AnimateTo(Property(mGlView, Actor::Property::SCALE), Vector3::ONE,
                         AlphaFunction::SIN, TimePeriod(2.0f, 2.0f));
    mAnimation.AnimateTo(Property(mGlView, Actor::Property::ORIENTATION),
                         Quaternion(Radian(Degree(20.0f)), Vector3::ZAXIS),
                         AlphaFunction::SIN, TimePeriod(0.0f, 2.0f));
    mAnimation.AnimateTo(Property(mGlView, Actor::Property::ORIENTATION), Quaternion(),
                         AlphaFunction::SIN, TimePeriod(2.0f, 2.0f));
    mAnimation.SetLooping(true);
    mAnimation.Play();
    mAnimating = true;
  }

  bool OnStatusTick()
  {
    ReclaimRetiredRenderers();
    UpdateStatus();
    return true;
  }

  void UpdateStatus()
  {
    const bool unsafe = (mBackendMode == GlView::BackendMode::UNSAFE_DIRECT_RENDERING);

    char text[640];
    snprintf(text, sizeof(text),
             "backend %s   mode %s%s\n"
             "init %u   frames %u   terminate %u   completed %u   renderers %u%s\n"
             "clippingBox  x %d  y %d  w %d  h %d      textures %u  handle %u%s\n"
             "B backend   M mode   R renderOnce   A animate%s   X texture   T terminate   ESC quit",
             ToString(mBackendMode),
             ToString(mRenderingMode),
             mAnimating ? "   animating" : "",
             Renderer().GetInitCount(),
             Renderer().GetFrameCount(),
             Renderer().GetTerminateCount(),
             CompletedCount(),
             static_cast<uint32_t>(mRenderers.size()),
             mTerminated ? "   TERMINATED" : "",
             Renderer().GetReportedX(),
             Renderer().GetReportedY(),
             Renderer().GetReportedWidth(),
             Renderer().GetReportedHeight(),
             Renderer().GetBoundTextureCount(),
             Renderer().GetBoundTextureHandle(),
             (Renderer().GetBoundTextureCount() > 0u && Renderer().GetBoundTextureHandle() == 0u) ? " (not ready)" : "",
             (unsafe && !Renderer().GetRestoreState()) ? "   C restore OFF" : "   C restore on");

    // Setting the same text again still dirties the label, which wakes the update thread
    // every tick and hides whether anything is actually still rendering. Once the view has
    // been terminated the counters stop moving, and so should the application.
    if(mStatusText != text)
    {
      mStatusText = text;
      mStatus.SetText(text);
      mStatus.SetTextColor((unsafe && !Renderer().GetRestoreState())
                             ? UiColor(1.0f, 0.45f, 0.45f, 1.0f)
                             : UiColor(1.0f, 1.0f, 1.0f, 1.0f));
    }
  }

private:
  Application& mApplication;

  // One renderer per GlView. A retired view's callbacks still hold a raw pointer to its
  // renderer and Terminate() completes asynchronously, so retired renderers are kept until
  // their view reports back - reusing one across rebuilds would let the old view's
  // terminate delete the new view's GL objects.
  std::vector<std::unique_ptr<CubeRenderer>> mRenderers;
  uint32_t                                   mReclaimedCount{0u};

  AbsoluteLayout mRoot;
  AbsoluteLayout mGlSlot;
  GlView         mGlView;
  Label          mStatus;
  std::string    mStatusText;
  Timer          mStatusTimer;
  Animation      mAnimation;

  Texture mTexture;

  GlView::BackendMode   mBackendMode{GlView::BackendMode::UNSAFE_DIRECT_RENDERING};
  GlView::RenderingMode mRenderingMode{GlView::RenderingMode::CONTINUOUS};

  float mW{0.0f};
  float mH{0.0f};
  bool  mAnimating{false};
  bool  mTerminated{false};
  bool  mTextureBound{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  GlViewSample sample(application);
  application.MainLoop();
  return 0;
}
