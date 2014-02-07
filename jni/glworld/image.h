#include "platform_gl.h"
#include <string>

struct RawImageData {
    const GLsizei width;
    const GLsizei height;
    const GLint size;
    const GLenum gl_color_format;
    const void* data;
};

/* Returns the decoded image data, or aborts if there's an error during decoding. */
RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size);
void release_raw_image_data(const RawImageData* data);
GLuint load_texture(
    const GLsizei width, const GLsizei height,
    const GLenum type, const GLvoid* pixels);
GLuint load_png_memory_into_texture(const std::string &png_file);
GLuint load_png_into_texture(const char* path);
GLuint load_png_into_texture(const std::string &path);
