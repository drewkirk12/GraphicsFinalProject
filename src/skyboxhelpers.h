#ifndef SKYBOXHELPERS_H
#define SKYBOXHELPERS_H

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

#include "realtime.h"
#include "settings.h"
#include "utils/sceneparser.h"


class SkyBox {

public:

    static void createSkyBoxVBOVAO(GLuint *m_skybox_vbo, GLuint *m_skybox_vao, std::vector<GLfloat> skyboxVertices) {
        glGenBuffers(1, m_skybox_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, *m_skybox_vbo);
        glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size()*sizeof(GLfloat), skyboxVertices.data(), GL_STATIC_DRAW);
        glGenVertexArrays(1, m_skybox_vao);
        glBindVertexArray(*m_skybox_vao);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

        //reset
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    static void loadSkyBoxImage(GLuint *m_skybox_texture, int type) {
        // order right, left, top, bottom, front, back

        glGenTextures(1, m_skybox_texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *m_skybox_texture);
        QString back_filepath;
        QString bottom_filepath;
        QString front_filepath;
        QString left_filepath;
        QString right_filepath;
        QString top_filepath;
        QImage m_back; QImage m_bottom; QImage m_front;QImage m_left; QImage m_right; QImage m_top;

        if (type) {
            back_filepath = QString(":/resources/skybox/sunsetback.png");
            m_back = QImage(back_filepath);
            m_back = m_back.convertToFormat(QImage::Format_RGBA8888);

            bottom_filepath = QString(":/resources/skybox/sunsetbottom.png");
            m_bottom = QImage(bottom_filepath);
            m_bottom = m_bottom.convertToFormat(QImage::Format_RGBA8888);

            front_filepath = QString(":/resources/skybox/sunsetfront.png");
            m_front = QImage(front_filepath);
            m_front = m_front.convertToFormat(QImage::Format_RGBA8888);

            left_filepath = QString(":/resources/skybox/sunsetleft.png");
            m_left = QImage(left_filepath);
            m_left = m_left.convertToFormat(QImage::Format_RGBA8888);

            right_filepath = QString(":/resources/skybox/sunsetright.png");
            m_right = QImage(right_filepath);
            m_right = m_right.convertToFormat(QImage::Format_RGBA8888);

            top_filepath = QString(":/resources/skybox/sunsettop.png");
            m_top = QImage(top_filepath);
            m_top = m_top.convertToFormat(QImage::Format_RGBA8888);
        }
        else {
            back_filepath = QString(":/resources/skybox/back.jpg");
            m_back = QImage(back_filepath);
            m_back = m_back.convertToFormat(QImage::Format_RGBA8888);

            bottom_filepath = QString(":/resources/skybox/bottom.jpg");
            m_bottom = QImage(bottom_filepath);
            m_bottom = m_bottom.convertToFormat(QImage::Format_RGBA8888);

            front_filepath = QString(":/resources/skybox/front.jpg");
            m_front = QImage(front_filepath);
            m_front = m_front.convertToFormat(QImage::Format_RGBA8888);

            left_filepath = QString(":/resources/skybox/left.jpg");
            m_left = QImage(left_filepath);
            m_left = m_left.convertToFormat(QImage::Format_RGBA8888);

            right_filepath = QString(":/resources/skybox/right.jpg");
            m_right = QImage(right_filepath);
            m_right = m_right.convertToFormat(QImage::Format_RGBA8888);

            top_filepath = QString(":/resources/skybox/top.jpg");
            m_top = QImage(top_filepath);
            m_top = m_top.convertToFormat(QImage::Format_RGBA8888);
        }



        std::vector<QImage> faces = {m_right, m_left, m_top, m_bottom, m_front, m_back };
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
    }
};

#endif // SKYBOXHELPERS_H
