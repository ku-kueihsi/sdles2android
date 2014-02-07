/* 
 * File:   gl_tools.h
 * Author: jesse
 *
 * Created on November 9, 2013, 11:10 AM
 */

#ifndef __GL_TOOLS_H__
#define	__GL_TOOLS_H__

#include "platform_gl.h"
#include <eigen3/Eigen/Dense>
#include <string>

namespace glw{

GLuint LoadShaderMemory(std::string &vstr, std::string &fstr);

#define GLMAT44 Eigen::Matrix<Tfloat, 4, 4, Eigen::ColMajor >
#define GLVEC3 Eigen::Matrix<Tfloat, 3, 1, Eigen::ColMajor >

template<class Tfloat>
void rotation_matrix(GLMAT44 &mat, Tfloat u, Tfloat v, Tfloat w, Tfloat theta);

template<class Tfloat>
void scale_matrix(GLMAT44 &mat, Tfloat s);

template<class Tfloat>
void scale_matrix(GLMAT44 &mat, Tfloat xs, Tfloat ys, Tfloat zs);

template<class Tfloat>
void translation_matrix(GLMAT44 &mat, Tfloat x, Tfloat y, Tfloat z);

template<class Tfloat>
void perspective_matrix(GLMAT44 &mat, Tfloat angleOfView, Tfloat aspectRatio, Tfloat near, Tfloat far);

template<class Tfloat>
void camera_matrix(GLMAT44 &mat, const GLVEC3 &direction, const GLVEC3 &camera, const GLVEC3 &camera_up);

//extern template <> void scale_matrix<GLfloat>(Eigen::Matrix<GLfloat, 4, 4, Eigen::ColMajor > &mat, GLfloat xs, GLfloat ys, GLfloat zs);
//extern template <> void scale_matrix<GLfloat>(Eigen::Matrix<GLfloat, 4, 4, Eigen::ColMajor > &mat, GLfloat s);

#undef GLVEC3
#undef GLMAT44
}

#endif	/* GL_TOOLS_H */

