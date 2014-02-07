#include "image.h"
#include "data_utils.h"
//#include "platform_log.h"
#include <assert.h>
#include <png.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace glw;

struct DataHandle {
    const png_byte* data;
    const png_size_t size;
};

struct ReadDataHandle {
    const DataHandle data;
    png_size_t offset;
};

struct PngInfo {
    const png_uint_32 width;
    const png_uint_32 height;
    const int color_type;
};

static void read_png_data_callback(
        png_structp png_ptr, png_byte* png_data, png_size_t read_length);
static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr);
static DataHandle read_entire_png_image(
        const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height);
static GLenum get_gl_color_format(const int png_color_format);

RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size)
{
    assert(png_data != NULL && png_data_size > 8);
    assert(png_sig_cmp((const unsigned char *) png_data, 0, 8) == 0);

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png_ptr != NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr != NULL);

    ReadDataHandle png_data_handle = (ReadDataHandle){
        {(const png_byte*) png_data, (const size_t) png_data_size}, 0};
    png_set_read_fn(png_ptr, &png_data_handle, read_png_data_callback);

    if (setjmp(png_jmpbuf(png_ptr))) {
        //CRASH("Error reading PNG file!");
        cout << ("Error reading PNG file!") << endl;
        exit(1);
    }

    const PngInfo png_info = read_and_update_info(png_ptr, info_ptr);
    const DataHandle raw_image = read_entire_png_image(png_ptr, info_ptr, png_info.height);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return (RawImageData)
    {
        (GLsizei) png_info.width,
                (GLsizei) png_info.height,
                (GLint) raw_image.size,
                get_gl_color_format(png_info.color_type),
                raw_image.data
    };
}

void release_raw_image_data(const RawImageData* data)
{
    assert(data != NULL);
    free((void*) data->data);
}

static void read_png_data_callback(png_structp png_ptr, png_byte* raw_data, png_size_t read_length)
{
    ReadDataHandle* handle = (ReadDataHandle *) png_get_io_ptr(png_ptr);
    const png_byte* png_src = handle->data.data + handle->offset;

    memcpy(raw_data, png_src, read_length);
    handle->offset += read_length;
}

static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr)
{
    png_uint_32 width, height;
    int bit_depth, color_type;

    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    // Convert transparency to full alpha
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    // Convert grayscale, if needed.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    // Convert paletted images, if needed.
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // Add alpha channel, if there is none (rationale: GL_RGBA is faster than GL_RGB on many GPUs)
    if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB)
        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

    // Ensure 8-bit packing
    if (bit_depth < 8)
        png_set_packing(png_ptr);
    else if (bit_depth == 16)
        png_set_scale_16(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    // Read the new color type after updates have been made.
    color_type = png_get_color_type(png_ptr, info_ptr);

    return (PngInfo)
    {
        width, height, color_type
    };
}

static DataHandle read_entire_png_image(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height)
{
    const png_size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
    const int data_length = row_size * height;
    assert(row_size > 0);

    png_byte* raw_image = (png_byte *) malloc(data_length);
    assert(raw_image != NULL);

    png_byte * row_ptrs[height];

    png_uint_32 i;
//    //flip rows, as opengl or all the images are stored up side down.
//    //turn this off, flip uv in assimp importer
//    for (i = 0; i < height; i++) {
//        row_ptrs[i] = raw_image + (height - 1 - i) * row_size;
//    }
    
    for (i = 0; i < height; i++) {
        row_ptrs[i] = raw_image + i * row_size;
    }

    png_read_image(png_ptr, &row_ptrs[0]);

    return (DataHandle)
    {
        raw_image, (png_size_t) data_length
    };
}

static GLenum get_gl_color_format(const int png_color_format)
{
    assert(png_color_format == PNG_COLOR_TYPE_GRAY
            || png_color_format == PNG_COLOR_TYPE_RGB_ALPHA
            || png_color_format == PNG_COLOR_TYPE_GRAY_ALPHA);

    switch (png_color_format) {
    case PNG_COLOR_TYPE_GRAY:
        return GL_LUMINANCE;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        return GL_RGBA;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        return GL_LUMINANCE_ALPHA;
    }

    return 0;
}

GLuint load_texture(
    const GLsizei width, const GLsizei height,
    const GLenum type, const GLvoid* pixels) {
    GLuint textureObjectId;
    glGenTextures(1, &textureObjectId);
    assert(textureObjectId != 0);
 
    glBindTexture(GL_TEXTURE_2D, textureObjectId);
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
 
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureObjectId;
}

GLuint load_png_memory_into_texture(const string &png_file) {
    //png_file could be either file name or file content block in the memory;
    const RawImageData raw_image_data =
        get_raw_image_data_from_png((void *)png_file.c_str(), (const int)png_file.size());
    const GLuint textureObjectId = load_texture(
        raw_image_data.width, raw_image_data.height,
        raw_image_data.gl_color_format, raw_image_data.data);
    release_raw_image_data(&raw_image_data);
    //release_asset_data(&png_file);
 
    return textureObjectId;
}

GLuint load_png_into_texture(const char* path) {
    assert(path != NULL);
    string png_file;
    FILE * fp = fopen(path, "r");
    File2Str(png_file, fp);
    fclose(fp); 
    assert(1 < png_file.size());
    return load_png_memory_into_texture(png_file);
}

GLuint load_png_into_texture(const string &path) {
    string png_file;
    File2Str(png_file, path);
    assert(1 < png_file.size());
    return load_png_memory_into_texture(png_file);
}