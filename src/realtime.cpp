#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

#include "realtime.h"
#include "settings.h"
#include "utils/sceneparser.h"
#include "utils/shaderloader.h"
#include "shapes/sphere.h"

// ================== Project 6!

/**
 * @brief Realtime::Realtime
 *  - main portion of project that controls the openGL context and manages
 *  all important data to make the real-time rendering possible.
 *  - this constructor sets up mouse/keyboard interaction
 */
Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;
}
/**
 * @brief Realtime::finish - automatically called by GL on exiting the program
 * - Serves to free the VBOs created during the rendering process.
 * - Deletes the shader used by the program
 */
void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();
    // uses global, storedRenders, which is filled in updateVBO function below
    // Look at README for full explanation.
    for (RenderShapeData &shape : storedRenders) {
        glDeleteBuffers(1, &(shape.shape_vbo));
        glDeleteVertexArrays(1, &(shape.shape_vbo));
    }
    // freeing up allocated resources for the fbo
    glDeleteProgram(m_shader);
    glDeleteProgram(m_fbo_shader);
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    this->doneCurrent();
}

/**
 * @brief Realtime::initializeGL - called by GL on opening the program
 * - Initializes GL and enables useful settings.
 * - Creates the shader using functions from ShaderLoader class & GLSL files
 */
void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    // preparing important member variables that will be used to build fbo
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    m_defaultFBO = 2;

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " <<
                     glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " <<
                 glewGetString(GLEW_VERSION) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height()
                                                   * m_devicePixelRatio);

    m_shader = ShaderLoader::createShaderProgram(
                ":/resources/shaders/default.vert",
                ":/resources/shaders/default.frag");
    m_fbo_shader = ShaderLoader::createShaderProgram(
                ":/resources/shaders/fbo.vert", // shader for post-processing
                ":/resources/shaders/fbo.frag");

    m_skybox_shader = ShaderLoader::createShaderProgram(
                ":/resources/shaders/skybox.vert", // shader for skybox
                ":/resources/shaders/skybox.frag");


    // making the skybox vbo and vao


    glGenBuffers(1, &m_skybox_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_skybox_vbo);
    glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size()*sizeof(GLfloat), skyboxVertices.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_skybox_vao);
    glBindVertexArray(m_skybox_vao);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    //reset
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    glGenTextures(1, &m_skybox_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture);

    QString back_filepath = QString(":/resources/skybox/back.jpg");
    m_back = QImage(back_filepath);
    m_back = m_back.convertToFormat(QImage::Format_RGBA8888);

    QString bottom_filepath = QString(":/resources/skybox/bottom.jpg");
    m_bottom = QImage(bottom_filepath);
    m_bottom = m_bottom.convertToFormat(QImage::Format_RGBA8888);

    QString front_filepath = QString(":/resources/skybox/front.jpg");
    m_front = QImage(front_filepath);
    m_front = m_front.convertToFormat(QImage::Format_RGBA8888);

    QString left_filepath = QString(":/resources/skybox/left.jpg");
    m_left = QImage(left_filepath);
    m_left = m_left.convertToFormat(QImage::Format_RGBA8888);

    QString right_filepath = QString(":/resources/skybox/right.jpg");
    m_right = QImage(right_filepath);
    m_right = m_right.convertToFormat(QImage::Format_RGBA8888);

    QString top_filepath = QString(":/resources/skybox/top.jpg");
    m_top = QImage(top_filepath);
    m_top = m_top.convertToFormat(QImage::Format_RGBA8888);

    std::vector<QImage> faces =
    {
        m_right,
        m_left,
        m_top,
        m_bottom,
        m_front,
        m_back
    };
    for (unsigned int i = 0; i < faces.size(); i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGBA, faces[i].width(), faces[i].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, faces[i].bits());
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    //reset
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    makeFBO();
}

/**
 * @brief Realtime::makeFBO
 * Taken from lab 11 - generates an FBO used to do post-processing effects.
 * For this project, they'll be inverted colors and sharpening.
 */
void Realtime::makeFBO() { // I can put my new stuff here!
    // generate texture, color attachment to paint to before applying kernel/per pixel effects
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // generate renderbuffer
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    // generate fbo
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // put it all together
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);
    // go back to default
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

/**
 * @brief Realtime::paintGL - called by GL when prompted to display a rendered
 *                            image (ie. after settingsChanged, sceneChanged)
 *  - For every shape, all shape-relevant information and lights are sent
 *    into the shader as uniform variables.
 *  - Every light type is now supported, each
 *  having its own uniform variables to set. To identify the light type
 *  once inside the GPU, the enum (0, 1, 2) for each type is sent in
 *  as the 0th element in the lights and colors uniform vector variables.
 */
void Realtime::paintGL() {


    // bind the fbo to paint to, first
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // set the view port to the fbo dimensions
    glViewport(0, 0, m_fbo_width, m_fbo_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // painting the skybox //////////////////////////////////////////////////////////////////////////
    glDepthMask(GL_FALSE);
    glUseProgram(m_skybox_shader);
    glm::mat4 view_skybox = glm::mat4(glm::mat3(m_view));
    glUniformMatrix4fv(glGetUniformLocation(m_skybox_shader, "view"),
                                     1, GL_FALSE, &view_skybox[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_skybox_shader, "projection"),
                                     1, GL_FALSE, &m_proj[0][0]);
    glBindVertexArray(m_skybox_vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // reset to default
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgram(0);

    glUseProgram(m_shader); // Bind the shader //////////////////////////////////////////////////////////////////////////
    for (RenderShapeData &shape : renderData.shapes) {
        glBindVertexArray(shape.shape_vao); // bind current shape vao

        glUniform4fv(glGetUniformLocation(m_shader, "cAmbient"),
                     1, &(shape.primitive.material.cAmbient[0]));
        glUniform4fv(glGetUniformLocation(m_shader, "cDiffuse"),
                     1, &(shape.primitive.material.cDiffuse[0]));
        glUniform4fv(glGetUniformLocation(m_shader, "cSpecular"),
                     1, &(shape.primitive.material.cSpecular[0]));
        glUniform1f(glGetUniformLocation(m_shader, "sh"),
                    shape.primitive.material.shininess);

        int numLights = renderData.lights.size();
        std::string str;
        GLint loc;
        int j = 0;
        for (j = 0; j < numLights; j++) {
            SceneLightData light = renderData.lights[j];
            switch (light.type) {
                case LightType::LIGHT_DIRECTIONAL:
                    str = "lights[" + std::to_string(j) + "]";
                    loc = glGetUniformLocation(m_shader, str.c_str());
                    glUniform4f(loc, 1, light.dir[0], light.dir[1], light.dir[2]);
                    break;
                case LightType::LIGHT_POINT:
                    str = "lights[" + std::to_string(j) + "]";
                    loc = glGetUniformLocation(m_shader, str.c_str());
                    glUniform4f(loc, 0, light.pos[0], light.pos[1], light.pos[2]);
                    break;

                case LightType::LIGHT_SPOT:
                    str = "lights[" + std::to_string(j) + "]";
                    loc = glGetUniformLocation(m_shader, str.c_str());
                    glUniform4f(loc, 2, light.pos[0], light.pos[1], light.pos[2]);

                    str = "spotDir[" + std::to_string(j) + "]";
                    loc = glGetUniformLocation(m_shader, str.c_str());
                    glUniform3f(loc, light.dir[0], light.dir[1], light.dir[2]);

                    // uniform variables for angles to calculate angular fall off
                    glUniform1f(glGetUniformLocation(m_shader, "thetaO"), light.angle);
                    glUniform1f(glGetUniformLocation(m_shader, "thetaI"),
                                (light.angle - light.penumbra));
                    break;
                default:
                    break;
            }
            str = "att[" + std::to_string(j) + "]";
            loc = glGetUniformLocation(m_shader, str.c_str());
            glUniform3f(loc, light.function[0], light.function[1], light.function[2]);

            str = "colors[" + std::to_string(j) + "]";
            loc = glGetUniformLocation(m_shader, str.c_str());
            glUniform4f(loc, 1, renderData.lights[j].color[0],
                                 renderData.lights[j].color[1],
                                 renderData.lights[j].color[2]);
        }
        glUniform1i(glGetUniformLocation(m_shader, "numLights"), j);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "ctm"),
                                1, GL_FALSE, &(shape.ctm[0][0]));
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "n_ctm"),
                              1, GL_FALSE, &(shape.normCTM[0][0]));
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "viewMat"),
                                         1, GL_FALSE, &m_view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "projMat"),
                                         1, GL_FALSE, &m_proj[0][0]);

        glUniform1f(glGetUniformLocation(m_shader, "ka"), m_ka);
        glUniform1f(glGetUniformLocation(m_shader, "kd"), m_kd);
        glUniform1f(glGetUniformLocation(m_shader, "ks"), m_ks);

        glUniform1i(glGetUniformLocation(m_shader, "fogType"), settings.fogType);
        float testFogVal = settings.fogValue;
        glUniform1f(glGetUniformLocation(m_shader, "fogIntensity"), (settings.fogValue)/100);

        glm::vec4 origin{0.0f,0.0f,0.0f, 1.0f};
        glm::vec4 camP = glm::inverse(m_view) * origin;
        glUniform4fv(glGetUniformLocation(m_shader, "camPos"), 1, &camP[0]);
        // actually draws the triangles
        glDrawArrays(GL_TRIANGLES, 0, shape.vertData.size() / 6);
        glBindVertexArray(0);

    }
    // Unbind the shader
    glUseProgram(0);

    // painting from framebuffer back to screen applying any effects
    // triggered by m_invert_bool or m_kernel_bool
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_fbo_shader);

    glUniform1i(glGetUniformLocation(m_fbo_shader, "invert"), m_invert_bool);
    glUniform1i(glGetUniformLocation(m_fbo_shader, "kernel"), m_kernel_bool);
    glUniform1f(glGetUniformLocation(m_fbo_shader, "im_height"), (float) m_fbo_height);
    glUniform1f(glGetUniformLocation(m_fbo_shader, "im_width"), (float) m_fbo_width);

    glBindVertexArray(m_fullscreen_vao);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

    // this will be my intercept point, let's see where m_fbo_texture lies

    glDrawArrays(GL_TRIANGLES, 0, 6); // paint to fullscreen squad
    // return to default state
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

/**
 * @brief Realtime::resizeGL - manages resizing screen & allows for
 * a call to settingsChanged() to be made to update the screen even as its
 * being resized.
 * @param w, h - dimensions of screen
 */
void Realtime::resizeGL(int w, int h) {
    // Resetting framebuffers when resizing occurs
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    makeFBO();
    glViewport(0, 0, size().width() * m_devicePixelRatio,
                   size().height() * m_devicePixelRatio);
    settingsChanged();
}

/**
 * @brief Realtime::sceneChanged - called when new scene is loaded in
 * - Calls the SceneParser to parse a given xml file
 * - Handles assigning global variables to values parsed by SceneParser
 */
void Realtime::sceneChanged() {
    makeCurrent();
    bool success = SceneParser::parse(settings.sceneFilePath, renderData);
    if (!success) {
        std::cerr << "Error loading scene" << std::endl;
    }

    camera.cameraUpdate(renderData, size().width(), size().height());
    m_view = camera.getViewMatrix();
    m_proj = camera.getPerspectiveMatrix();
    m_ka = renderData.globalData.ka;
    m_kd = renderData.globalData.kd;
    m_ks = renderData.globalData.ks;
    updateVBO();
    update(); // asks for a PaintGL() call to occur
}

/**
 * @brief Realtime::settingsChanged - called when a setting is changed
 * - Updates globals and calls updateVBO() and update() commmands
 */
void Realtime::settingsChanged() {
    makeCurrent();
    camera.cameraUpdate(renderData, size().width(), size().height());
    m_view = camera.getViewMatrix();
    m_proj = camera.getPerspectiveMatrix();
    m_invert_bool = settings.perPixelFilter;
    m_kernel_bool = settings.kernelBasedFilter;
    updateVBO();
    update();
}

/**
 * @brief Realtime::updateVBO - Highly important function that handles
 * the most important task: making just the right number of VBOs For more info,
 * please read the README. The jist is that I never prepare more than 4 VBO/VAO
 * pairs, (1) for each unit primitive.
 * Since the function is hefty: here's step by step
 * 1. For each shape, check its type.
 * 2. Using shapeCheck<>, check if the given shape is the first of its kind
 *    to come through the loop. If so, store th pointer of the shape in the
 *    primitive shape class (Cube, Cylinder, etc.). Also, prepare vertex data
 *    using methods in the primitive's shape class. O/W get the true
 *    first of a shape to come through (stored in primitive shape's class)
 * 3.  If the current shape is the first of its kind, allocate/bind, & arrange
 *     memory for the VBO and the VAO for a shape.
 * 4. Otherwise, using the stored pointer to the first shape to come through
 *    Find the first shape's vbo/vao and copy it.
 */
void Realtime::updateVBO() {
    makeCurrent(); // allows GL context to be updated here
    // tracks which shapes have had their "first" of their kind come through
    std::vector<int> shapeCheck{1, 1, 1, 1};
    for (RenderShapeData &shape : renderData.shapes) {

        boolean makeNewVBO = false; // used below to see whether VBO is needed
        RenderShapeData *firstRender; // pointer used to access prior VAO/VBOs

        switch (shape.primitive.type) {
            case PrimitiveType::PRIMITIVE_SPHERE:
                if (shapeCheck[0]) {
                    makeNewVBO = true;
                    shapeCheck[0] = 0;
                    sphere.storeRender(&shape);
                    shape.vertData = sphere.updateParams(
                                  settings.shapeParameter1,
                                  settings.shapeParameter2);
                    storedRenders.push_back(shape);
                }
                else {
                    firstRender = sphere.getRender();
                }
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                if (shapeCheck[1]) {
                    makeNewVBO = true;
                    shapeCheck[1] = 0;
                    cylinder.storeRender(&shape);
                    shape.vertData = cylinder.updateParams(
                                   settings.shapeParameter1,
                                   settings.shapeParameter2);
                    storedRenders.push_back(shape);
                }
                else {
                    firstRender = cylinder.getRender();
                }

                break;
            case PrimitiveType::PRIMITIVE_CONE:
                if (shapeCheck[2]) {
                    makeNewVBO = true;
                    shapeCheck[2] = 0;
                    cone.storeRender(&shape);
                    shape.vertData = cone.updateParams(
                               settings.shapeParameter1,
                               settings.shapeParameter2);
                    storedRenders.push_back(shape);
                }
                else {
                    firstRender = cone.getRender();
                }

                break;
            case PrimitiveType::PRIMITIVE_CUBE:
                if (shapeCheck[3]) {
                    makeNewVBO = true;
                    shapeCheck[3] = 0;
                    cube.storeRender(&shape);
                    storedRenders.push_back(shape);
                    shape.vertData = cube.updateParams(
                                settings.shapeParameter1);
                }
                else {
                    firstRender = cube.getRender();
                }
                break;
            default:
                break;
        }
        if (makeNewVBO) {
            glGenBuffers(1, &(shape.shape_vbo));
            glBindBuffer(GL_ARRAY_BUFFER, shape.shape_vbo);
            glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) *
                                           shape.vertData.size()),
                         (shape.vertData.data()), GL_STATIC_DRAW);

            // Vertex Array Objects

            glGenVertexArrays(1, &(shape.shape_vao));
            glBindVertexArray(shape.shape_vao);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            // two sets of three floats, vertices, norms
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24,
                                  reinterpret_cast<void*>(0));
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24,
                                  reinterpret_cast<void*>((3 * sizeof(GLfloat))));
            // Returning to Default State
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        else {
            shape.shape_vbo = firstRender->shape_vbo;
            shape.shape_vao = firstRender->shape_vao;
            shape.vertData = firstRender->vertData;
        }
    }
}

// updates m_keyMap according to key presses
void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

// updates m_keyMap according to key releases
void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

// updates m_mouseDown and m_prev_mouse_pos according to mouse presses
void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(),
                                     event->position().y());
    }
}

// same as above but for mouse releases
void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

/*
 * Calculates the rodrigues matrix or matrix for rotation of
 * theta radians about an arbitrary axis, (ux, uy, uz).
 * Helper function for mouseMoveEvent()
 */
glm::mat3 rodriguesMatrix(float theta, float ux, float uy, float uz) {
    float tl = glm::cos(theta) + (pow(ux, 2) * (1 - glm::cos(theta)));
    float tm = (ux * uy * (1 - glm::cos(theta))) - (uz * glm::sin(theta));
    float tr = (ux * uz * (1 - glm::cos(theta)) + (uy *glm::sin(theta)));
    float cl = (ux * uy * (1 - glm::cos(theta)) + (uz * glm::sin(theta)));
    float cm = glm::cos(theta) + (pow(uy, 2) * (1 - glm::cos(theta)));
    float cr = (uy * uz * (1 - glm::cos(theta)) - (ux * glm::sin(theta)));
    float bl = (ux * uz * (1 - glm::cos(theta)) - (uy * glm::sin(theta)));
    float bm = (uy * uz * (1 - glm::cos(theta)) + (ux * glm::sin(theta)));
    float br = glm::cos(theta) + (pow(uz, 2) * (1 - glm::cos(theta)));

    glm::mat3 ret = glm::mat3(
                tl, cl, bl,
                tm, cm, bm,
                tr, cr, br);
    return ret;
}

/**
 * Rotates the camera's look vector using  deltaX and deltaY, the
 * change in mouse position between mouse clicks.
*/
void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);
        float thetaX = 0.0123f * deltaX; //0.0123 ~ PI/256 yielded an acceptable mouse sensitivity
        float thetaY = 0.0123f * deltaY;
        glm::mat3 rodriMatX = rodriguesMatrix(-thetaX, 0, 1, 0);
        glm::vec3 perp = glm::normalize(glm::cross(camera.up, camera.look));
        glm::mat3 rodriMatY = rodriguesMatrix(thetaY, perp[0], perp[1], perp[2]);
        if (m_mouseDown) {
            camera.look = rodriMatX * rodriMatY * camera.look;
        }
        m_view = camera.getViewMatrix();
        m_proj = camera.getPerspectiveMatrix();
        update(); // asks for a PaintGL() call to occur
    }
}





/**
 * @brief Realtime::timerEvent
 * Translates the camera's position according to how long
 * WASD, SPACE, or CTRL are held down for, yielding movements
 * forward, backward, left, right, up, and down, respectively.
 */
void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f; // deltaTime in seconds
    m_elapsedTimer.restart();
    // corresponds to how much of 1 Unit distance I'll travel to ultimately move at 5unit/sec
    float distScale = 5.0f * deltaTime;
    glm::vec3 look = glm::normalize(camera.look);
    glm::vec3 up = glm::normalize(camera.up);
    if (m_keyMap[Qt::Key_W]) {
        camera.pos = camera.pos + (distScale * look);
    }
    if (m_keyMap[Qt::Key_A]) {
        glm::vec3 left = glm::normalize(glm::cross(up, look));
        camera.pos = camera.pos + (distScale * left);
    }
    if (m_keyMap[Qt::Key_S]) {
        camera.pos = camera.pos - (distScale * look);
    }
    if (m_keyMap[Qt::Key_D]) {
        glm::vec3 right = glm::normalize(glm::cross(look, up));
        camera.pos = camera.pos + (distScale * right);
    }
    if (m_keyMap[Qt::Key_Space]) {
        glm::vec3 worldUp{0, 1, 0};
        camera.pos = camera.pos + (distScale * worldUp);
    }
    if (m_keyMap[Qt::Key_Control]) {
        glm::vec3 worldDown{0, -1, 0};
        camera.pos = camera.pos + (distScale * worldDown);
    }
    m_view = camera.getViewMatrix();
    m_proj = camera.getPerspectiveMatrix();
    update(); // asks for a PaintGL() call to occur
}
