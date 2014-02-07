/* 
 * File:   data_utils.h
 * Author: jesse
 *
 * Created on November 7, 2013, 11:16 AM
 */

#ifndef __DATA_UTILS_H__
#define	__DATA_UTILS_H__

#include <eigen3/Eigen/Dense>
#include <cstdio>
#include <string>

namespace glw {
void File2Str(std::string &str, const std::string &file_name);
void File2Str(std::string &str, FILE * fp);
}



#endif	/* DATA_UTILS_H */

