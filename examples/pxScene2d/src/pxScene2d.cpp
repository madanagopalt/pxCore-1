// pxCore CopyRight 2007-2015 John Robinson
// pxScene2d.cpp

#include "pxScene2d.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <math.h>

#include "rtLog.h"
#include "rtRefT.h"
#include "rtString.h"
#include "rtPathUtils.h"

#include "pxCore.h"
#include "pxOffscreen.h"
#include "pxUtil.h"
#include "pxTimer.h"

GLuint textureId1, textureId2;

GLint attribute_coord;

struct point {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
};

static GLint u_matrix = -1;
static GLint u_alpha = -1;
static GLint u_resolution = -1;
GLint u_texture = -1;
GLint u_alphatexture = -1;
GLint u_color = -1;
GLint attr_pos = 0, attr_uv = 2;

static bool clip = false;


void draw9SliceRect(GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
  
  float ox1 = x;
  float ix1 = x+x1;
  float ox2 = x+w;
  float ix2 = x+w-x2;
  float oy1 = y;
  float iy1 = y+y1;
  float oy2 = y+h;
  float iy2 = y+h-y2;
  
  const GLfloat verts[22][2] = {
    { ox1,oy1 },
    { ix1,oy1 },
    { ox1,iy1 },
    { ix1,iy1 },
    { ox1,iy2 },
    { ix1,iy2 },
    { ox1,oy2 },
    { ix1,oy2 },
    { ix2,oy2 },
    { ix1,iy2 },
    { ix2,iy2 },
    { ix1,iy1 },
    { ix2,iy1 },
    { ix1,oy1 },
    { ix2,oy1 },
    { ox2,oy1 },
    { ix2,iy1 },
    { ox2,iy1 },
    { ix2,iy2 },
    { ox2,iy2 },
    { ix2,oy2 },
    { ox2,oy2 }
  };
#if 0
  const GLfloat colors[4][3] = {
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 }
  };
#endif
  const GLfloat uv[22][2] = {
    { 0, 0 },
    { 1, 0 },
    { 0, 1 },
    { 1, 1 }
  };
  
  
  {
    glUniform1f(u_alphatexture, 0.0);
    glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
    //      glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 0, colors);
    //    glVertexAttribPointer(attr_uv, 2, GL_FLOAT, GL_FALSE, 0, uv);
    glEnableVertexAttribArray(attr_pos);
    //      glEnableVertexAttribArray(attr_color);
    //glEnableVertexAttribArray(attr_uv);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 22);
    
    glDisableVertexAttribArray(attr_pos);
    //      glDisableVertexAttribArray(attr_color);
    //glDisableVertexAttribArray(attr_uv);
  }
  
}

void drawRectOutline(GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat lw) {
  
  float half = lw/2;
  float ox1 = x-half;
  float ix1 = x+half;
  float ox2 = x+w+half;
  float ix2 = x+w-half;
  float oy1 = y-half;
  float iy1 = y+half;
  float oy2 = y+h+half;
  float iy2 = y+h-half;
  
  const GLfloat verts[10][2] = {
    { ox1,oy1 },
    { ix1,iy1 },
    { ox2,oy1 },
    { ix2,iy1 },
    { ox2,oy2 },
    { ix2,iy2 },
    { ox1,oy2 },
    { ix1,iy2 },
    { ox1,oy1 },
    { ix1,iy1 }
  };
  
#if 0
  const GLfloat uv[4][2] = {
    { 0, 0 },
    { 1, 0 },
    { 0, 1 },
    { 1, 1 }
  };
#endif
  
  
  {
    glUniform1f(u_alphatexture, 0.0);

    glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
    //      glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 0, colors);
    //    glVertexAttribPointer(attr_uv, 2, GL_FLOAT, GL_FALSE, 0, uv);
    glEnableVertexAttribArray(attr_pos);
    //      glEnableVertexAttribArray(attr_color);
    //glEnableVertexAttribArray(attr_uv);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
    
    glDisableVertexAttribArray(attr_pos);
    //      glDisableVertexAttribArray(attr_color);
    //    glDisableVertexAttribArray(attr_uv);
  }
  
  
}


void drawRect(float x, float y, float w, float h, pxOffscreen& offscreen) {
  
  glActiveTexture(GL_TEXTURE0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
	       offscreen.width(), offscreen.height(), 0, GL_BGRA_EXT,
	       GL_UNSIGNED_BYTE, offscreen.base());

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(u_texture, 0);
  //  glUniform1f(u_alphatexture, 1.0);

  const float verts[4][2] = {
    { x,y },
    {  x+w, y },
    {  x,  y+h },
    {  x+w, y+h }
  };

  const float uv[4][2] = {
    { 0, 0 },
    { 1, 0 },
    { 0, 1 },
    { 1, 1 }
  };
  
  {
    glUniform1f(u_alphatexture, 1.0);
    glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glVertexAttribPointer(attr_uv, 2, GL_FLOAT, GL_FALSE, 0, uv);
    glEnableVertexAttribArray(attr_pos);
    glEnableVertexAttribArray(attr_uv);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(attr_pos);
    glDisableVertexAttribArray(attr_uv);
  }
}

double pxInterpLinear(double i) {
  return pxClamp<double>(i, 0, 1);
}

static const char *fShaderText =
  "uniform float u_alphatexture;\n"
  "uniform float u_alpha;\n"
  "uniform vec4 a_color;\n"
  "uniform sampler2D s_texture;\n"
  "varying vec2 v_uv;\n"
  "void main() {\n"
  "if (u_alphatexture < 1.0) {"
  // solid color
  " gl_FragColor = a_color;"
  "} else "
  "if (u_alphatexture < 2.0) {\n"
  // image
  "gl_FragColor = texture2D(s_texture, v_uv);\n"
  "} else {\n"
  // text
  "gl_FragColor = vec4(1, 1, 1, texture2D(s_texture, v_uv).a) * vec4(0, 0, 0, 1);"
  "}\n"
  "gl_FragColor.a *= u_alpha;"
  "}\n";

static const char *vShaderText =
  "uniform vec2 u_resolution;\n"
  "uniform mat4 amymatrix;\n"
  "attribute vec2 pos;\n"
  "attribute vec2 uv;\n"
  "varying vec2 v_uv;\n"
  "void main() {\n"
  // map to "pixel coordinates"
  " vec4 p = amymatrix * vec4(pos, 0, 1);\n"
  " vec4 zeroToOne = p / vec4(u_resolution, u_resolution.x, 1);\n"
  " vec4 zeroToTwo = zeroToOne * vec4(2.0, 2.0, 1, 1);\n"
  " vec4 clipSpace = zeroToTwo - vec4(1.0, 1.0, 0, 0);\n"
  " clipSpace.w = 1.0+clipSpace.z;\n"
  " gl_Position =  clipSpace * vec4(1, -1, 1, 1);\n"
  "v_uv = uv;\n"
  "}\n";

GLuint createShaderProgram(const char* vShaderTxt, const char* fShaderTxt) {

  GLuint fragShader, vertShader, program = 0;
  GLint stat;
  
  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, (const char **) &fShaderTxt, NULL);
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &stat);
  if (!stat) {
    rtLog("Error: fragment shader did not compile!\n");
    
    GLint maxLength = 0;
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);
    
    //The maxLength includes the NULL character
    std::vector<char> errorLog(maxLength);
    glGetShaderInfoLog(fragShader, maxLength, &maxLength, &errorLog[0]);
    
    //Provide the infolog in whatever manor you deem best.
    rtLog("%s\n", &errorLog[0]);
    //Exit with failure.
    glDeleteShader(fragShader); //Don't leak the shader.
    exit(1);
  }
  
  vertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertShader, 1, (const char **) &vShaderTxt, NULL);
  glCompileShader(vertShader);
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &stat);
  if (!stat) {
    rtLog("Error: vertex shader did not compile!\n");
    exit(1);
  }
  
  program = glCreateProgram();
  glAttachShader(program, fragShader);
  glAttachShader(program, vertShader);
  glLinkProgram(program);
  
  glGetProgramiv(program, GL_LINK_STATUS, &stat);
  if (!stat) {
    char log[1000];
    GLsizei len;
    glGetProgramInfoLog(program, 1000, &len, log);
    rtLog("Error: linking:\n%s\n", log);
    exit(1);
  }
  
  /* test setting attrib locations */
  glBindAttribLocation(program, attr_pos, "pos");
  glBindAttribLocation(program, attr_uv, "uv");
  glLinkProgram(program);  /* needed to put attribs into effect */
  
  return program;
}

void pxObject::set(const char* prop, float v) {
  if (strcmp(prop, "x") == 0) x = v;
  else if (strcmp(prop, "y") == 0) y = v;
  else if (strcmp(prop, "w") == 0) w = v;
  else if (strcmp(prop, "h") == 0) h = v;
  else if (strcmp(prop, "sx") == 0) sx = v;
  else if (strcmp(prop, "sy") == 0) sy = v;
  else if (strcmp(prop, "cx") == 0) cx = v;
  else if (strcmp(prop, "cy") == 0) cy = v;
  else if (strcmp(prop, "a") == 0) a = v;
  else if (strcmp(prop, "r") == 0) r = v;
}

float pxObject::get(const char* prop) const {
  if (strcmp(prop, "x") == 0) return x;
  else if (strcmp(prop, "y") == 0) return y;
  else if (strcmp(prop, "w") == 0) return w;
  else if (strcmp(prop, "h") == 0) return h;
  else if (strcmp(prop, "sx") == 0) return sx;
  else if (strcmp(prop, "sy") == 0) return sy;
  else if (strcmp(prop, "cx") == 0) return cx;
  else if (strcmp(prop, "cy") == 0) return cy;
  else if (strcmp(prop, "a") == 0) return a;
  else if (strcmp(prop, "r") == 0) return r;
  return 0;
}

void pxObject::setParent(rtRefT<pxObject>& parent) {
  mParent = parent;
  parent->mChildren.push_back(this);
}

void pxObject::animateTo(const char* prop, double to, double duration, pxInterp interp,
 pxAnimationType at, pxAnimationEnded e, void* c) {
  animation a;
  a.prop = prop;
  a.from = get(prop);
  a.to = to;
  a.start = -1;
  a.duration = duration;
  a.interp = interp?interp:pxInterpLinear;
  a.at = at;
  a.ended = e;
  a.ctx = c;
  
  animation b;
  b = a;
  mAnimations.push_back(a);
}

void pxObject::update(double t) {
  // Update animations
  vector<animation>::iterator it = mAnimations.begin();
  while (it != mAnimations.end()) {
    animation& a = (*it);
    if (a.start < 0) a.start = t;
    double end = a.start + a.duration;
    
    // if duration has elapsed
    if (t >= end) {
      set(a.prop, a.to);
      if (a.at == stop) {
	if (a.ended)
	  a.ended(a.ctx);
	it = mAnimations.erase(it);
	continue;
      }
#if 0
      else if (a.at == seesaw) {
	// flip
	double t;
	t = a.from;
	a.from = a.to;
	a.to = t;
      }
#endif
    }
    
    double t1 = (t-a.start)/a.duration; // Some of this could be pushed into the end handling
    double t2 = floor(t1);
    t1 = t1-t2; // 0-1
    double d = a.interp(t1);
    float from, to;
    from = a.from;
    to = a.to;
    if (a.at == seesaw) {
      if (fmod(t2,2) != 0) {  // perf chk ?
	from = a.to;
	to = a.from;
      }
    }
    
    float v = from + (to - from) * d;
      set(a.prop, v);
      ++it;
  }
  
  // Recursively update children
  for(vector<rtRefT<pxObject> >::iterator it = mChildren.begin(); it != mChildren.end(); ++it) {
    (*it)->update(t);
  }
}

void pxObject::drawInternal(pxMatrix4f m) {

  m.translate(x+cx, y+cy);
  m.rotateInDegrees(r);
  m.scale(sx, sy);
  m.translate(-cx, -cy);
  
  // set up uniforms
  glUniformMatrix4fv(u_matrix, 1, GL_FALSE,m.data());
  glUniform1f(u_alpha, a);
  
  draw();
  
  for(vector<rtRefT<pxObject> >::iterator it = mChildren.begin(); it != mChildren.end(); ++it) {
    (*it)->drawInternal(m);
  }
}


void rectangle::draw() {
  //glLineWidth(mLineWidth);
  //    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  
  glUniform4fv(u_color, 1, mFillColor);
  float half = mLineWidth/2;
  //    drawRect(half, half, w-mLineWidth, h-mLineWidth);
  if (mLineWidth > 0) {
    //glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POINT_SMOOTH);
    //      glEnable(GL_POLYGON_SMOOTH);
    glUniform4fv(u_color, 1, mLineColor);
    drawRectOutline(0, 0, w, h, mLineWidth);
  }
}

void rectangle9::draw() {
  //glLineWidth(mLineWidth);
  //    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  
  glUniform4fv(u_color, 1, mFillColor);
  float half = mLineWidth/2;
  draw9SliceRect(half, half, w-mLineWidth, h-mLineWidth, 10, 10, 10, 10);
  if (mLineWidth > 0) {
    //    glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POINT_SMOOTH);
    //      glEnable(GL_POLYGON_SMOOTH);
    glUniform4fv(u_color, 1, mLineColor);
    drawRectOutline(0, 0, w, h, mLineWidth);
  }
}
  
pxScene2d::pxScene2d():start(0),frameCount(0) { 
  mRoot = new pxObject(); 
}

void pxScene2d::draw() {
    
  if (clip) {
    glPushAttrib(GL_SCISSOR_BIT);
    glEnable(GL_SCISSOR_TEST);
    glScissor(256,256,64,64);
    
  }
  
  glClear(GL_COLOR_BUFFER_BIT);
  glUniform2f(u_resolution, mWidth, mHeight);
  
  if (mRoot) {
    pxMatrix4f m;
    mRoot->drawInternal(m);
  }
  
  if (clip) {
    glDisable(GL_SCISSOR_TEST);
    glPopAttrib();
  }
  
}
  
void pxScene2d::getMatrixFromObjectToScene(pxObject* o, pxMatrix4f& m) {
    
}
  
void pxScene2d::getMatrixFromSceneToObject(pxObject* o, pxMatrix4f& m) {
    
}
  
void pxScene2d::getMatrixFromObjectToObject(pxObject* from, pxObject* to, pxMatrix4f& m) {
    
}
  
void pxScene2d::transformPointFromObjectToScene(pxObject* o, const pxPoint2f& from, pxPoint2f& to) {
    
}
  
void pxScene2d::transformPointFromObjectToObject(pxObject* fromObject, pxObject* toObject, pxPoint2f& from, pxPoint2f& to) {
  
}
  
void pxScene2d::hitTest(pxPoint2f p, vector<rtRefT<pxObject> > hitList) {
    
}

void pxScene2d::onDraw() {
  if (start == 0)
    start = pxSeconds();
  
  update(pxSeconds());
  draw();
  if (frameCount >= 60) {
    end2 = pxSeconds();
    rtLog("elapsed no clip %g\n", (double)frameCount/(end2-start));
    start = end2;
    frameCount = 0;
  }
  frameCount++;
}

// Does not draw updates scene to time t
// t is assumed to be monotonically increasing
void pxScene2d::update(double t) {
  if (mRoot)
    mRoot->update(t);
}

pxObject* pxScene2d::getRoot() { 
  return mRoot; 
}

void initGL() {
#ifndef __APPLE__
  GLenum err = glewInit();
  if (err != GLEW_OK)
    exit(1); // or handle the error in a nicer way
  if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
    exit(1); // or handle the error in a nicer way
  
#endif

  glClearColor(0.4, 0.4, 0.4, 0.0);

  GLuint program = createShaderProgram(vShaderText, fShaderText);

  u_resolution = glGetUniformLocation(program, "u_resolution");
  u_texture = glGetUniformLocation(program, "s_texture");
  u_matrix = glGetUniformLocation(program, "amymatrix");
  u_alpha = glGetUniformLocation(program, "u_alpha");
  u_color = glGetUniformLocation(program, "a_color");
  u_alphatexture = glGetUniformLocation(program, "u_alphatexture");

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &textureId1);
  glBindTexture(GL_TEXTURE_2D, textureId1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glActiveTexture(GL_TEXTURE1);
  glGenTextures(1, &textureId2);
  glBindTexture(GL_TEXTURE_2D, textureId2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glEnable(GL_BLEND);

  // assume non-premultiplied for now... 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  glUseProgram(program);

}

int pxScene2d::width() {
  return mWidth;
}

int pxScene2d::height() {
  return mHeight;
}

void pxScene2d::onSize(int w, int h) {
  clip = false;
  glViewport(0, 0, (GLint)w, (GLint)h);
  mWidth = w;
  mHeight = h;
}

void pxScene2d::onMouseDown(int x, int y, unsigned long flags) {
}

void pxScene2d::onMouseUp(int x, int y, unsigned long flags) {
}

void pxScene2d::onMouseLeave() {
}

void pxScene2d::onMouseMove(int x, int y) {
#if 0
  rtLog("onMousePassiveMotion x: %d y: %d\n", x, y);
  
  //  pxMatrix4f m1, m2;
  pxVector4f from(x, y);
  pxVector4f to;
  
  pxObject::transformPointFromSceneToObject(target, from, to);
  
  rtLog("in target coords x: %f y: %f\n", to.mX, to.mY);
  
  pxObject::transformPointFromObjectToScene(target, to, from);
  
  rtLog("in sceme coords x: %f y: %f\n", from.mX, from.mY);
#endif
}

void pxScene2d::onKeyDown(int keycode, unsigned long flags) {
}

void pxScene2d::onKeyUp(int keycode, unsigned long flags) {
}

