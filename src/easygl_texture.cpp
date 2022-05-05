//
//  easygl_texture.cpp
//  EasyGLDemo
//
//  Created by 江派锋 on 2022/5/1.
//

#include "easygl_texture.hpp"
#include "easygl_resource.hpp"
#include "stb/stb_image.h"

NS_EASYGL_BEGIN

// MARK: - TextureDataProvider
class TextureDataFileProvider final : public ITextureDataProvider
{
public:
    TextureDataFileProvider(const std::string &filePath) : _filePath(filePath)
    {
        if (filePath.empty())
        {
            _status = Status::Invalid;
        }
    }
    
    bool loadData() override
    {
        if (_status == Status::Invalid)
        {
            return false;
        }
        
        if (_status == Status::DataLoaded)
        {
            return true;
        }
        
#ifdef USING_OPENGL_ES
        stbi_set_flip_vertically_on_load(true);
#endif

        int w = 0, h = 0, nrChannels = 0;
        auto data = stbi_load(_filePath.c_str(), &w, &h, &nrChannels, 0);
        if (!data)
        {
            return false;
        }
        
        _width = w;
        _height = h;
        _stride = w;
        _bytePreRow = _stride * nrChannels;
        _dataSize = h * _bytePreRow;
        _data.reset(new uint8_t[_dataSize]);
        memcpy(_data.get(), data, _dataSize);
        stbi_image_free(data);
        _status = Status::DataLoaded;
        
        struct __Format
        {
            GLenum internalFormat;
            GLenum format;
            GLenum type;
        };
        const __Format formats[] = {
            { GL_RED, GL_RED, GL_UNSIGNED_BYTE },
            { GL_RG, GL_RG, GL_UNSIGNED_BYTE },
            { GL_RGB, GL_RGB, GL_UNSIGNED_BYTE },
            { GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },
        };
        --nrChannels;
        _internalFormat = formats[nrChannels].internalFormat;
        _format = formats[nrChannels].format;
        _type = formats[nrChannels].type;
        return true;
    }
    
    Status status() const override { return _status; }
    GLsizei width() const override { return _width; }
    size_t bytePreRow() const override { return _bytePreRow; };
    GLsizei height() const override { return _height; }
    GLsizei stride() const override { return _stride; }
    GLenum internalFormat() const override { return _internalFormat; }
    GLenum format() const override { return _format; }
    GLenum type() const override { return _type; }
    
    size_t dataSize() const override { return _dataSize; }
    GLvoid * data() const override
    {
        if (_status != Status::DataLoaded)
        {
            return nullptr;
        }
        return (GLvoid *)_data.get();
    }
    GLvoid * releaseData() override
    {
        if (_status != Status::DataLoaded)
        {
            return nullptr;
        }
        _status = Status::Uninitialized;
        return (GLvoid *)_data.release();
    }

private:
    const std::string _filePath;
    
    Status _status = Status::Uninitialized;
    GLsizei _width = 0;
    GLsizei _height = 0;
    GLsizei _stride = 0;
    GLenum _internalFormat = 0;
    GLenum _format = 0;
    GLenum _type = 0;
    size_t _dataSize = 0;
    size_t _bytePreRow = 0;
    std::unique_ptr<uint8_t[]> _data;
};

ITextureDataProvider::Ptr ITextureDataProvider::CreateFromFile(const std::string &filePath)
{
    return std::make_shared<TextureDataFileProvider>(filePath);
}

// MARK: - TextureData
TextureData::TextureData()
{
    _status = Status::Invalid;
}
TextureData::TextureData(const Context::Ptr &ctx,
                         const ITextureDataProvider::Ptr &dataProvider)
{
    _ctx = ctx;
    _dataProvider = dataProvider;
    if (!_dataProvider || _dataProvider->status() == ITextureDataProvider::Status::Invalid)
    {
        _status = Status::Invalid;
    }
}

void TextureData::invalidate()
{
    // FIXME: call me!
    if (_texture > 0)
    {
        EASYGL_ASSERT(_ctx, "context invalid but texture has been created!")
        bool hasDelete = _ctx->resources()->tryDelete(Resources::Texture, _texture);
        EASYGL_ASSERT(hasDelete, "delete texture when context is not current");
        _texture = 0;
    }
}

bool TextureData::isInvalid() const { return status() == Status::Invalid; }
TextureData::Status TextureData::status() const { return _status; }
GLsizei TextureData::width() const
{
    if (isInvalid()) { return 0; }
    return _dataProvider->width();
}
GLsizei TextureData::height() const
{
    if (isInvalid()) { return 0; }
    return _dataProvider->height();
}
GLuint TextureData::texture() const { return _texture; }
bool TextureData::load()
{
    if (isInvalid()) { return false; }
    if (status() == Status::DataLoaded) { return true; }
    if (!_dataProvider->loadData())
    {
        return false;
    }
    
    EASYGL_CHECK_ERROR(glGenTextures(1, &_texture));
    EASYGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, _texture));
    EASYGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    EASYGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    EASYGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    EASYGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    EASYGL_CHECK_ERROR(glPixelStorei(GL_UNPACK_ROW_LENGTH, _dataProvider->stride()));
    GLint alignment = 1;
    auto bytePreRow = _dataProvider->bytePreRow();
    if (bytePreRow % 8 == 0)
    {
        alignment = 8;
    }
    else if (bytePreRow % 4 == 0)
    {
        alignment = 4;
    }
    else if (bytePreRow % 2 == 0)
    {
        alignment = 2;
    }
    else
    {
        alignment = 1;
    }
    EASYGL_CHECK_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, alignment));
    EASYGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, _dataProvider->internalFormat(),
                                    _dataProvider->width(), _dataProvider->height(), 0,
                                    _dataProvider->format(), _dataProvider->type(),
                                    _dataProvider->releaseData()));
    _status = Status::DataLoaded;
    
    // restore
    EASYGL_CHECK_ERROR(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
    EASYGL_CHECK_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    return true;
}

// MARK: - GLTexture2D
GLTexture2D::GLTexture2D(const type &textureData) : _value(textureData) {}
const GLTexture2D::type & GLTexture2D::value() const { return _value; }

void GLTexture2D::setUnit(GLenum unit)
{
    _unit = unit;
}
GLenum GLTexture2D::unit() const { return _unit; }
GLTexture2D::operator real_type() const
{
    return unit();
}
void GLTexture2D::syncUniform(GLint locaion)
{
    if (_value.status() != TextureData::Status::DataLoaded)
    {
        _value.load();
    }
    EASYGL_ASSERT(_value.status() == TextureData::Status::DataLoaded,
                  "Texture Data is not loaded");
    if (_value.status() != TextureData::Status::DataLoaded)
    {
        return;
    }
    EASYGL_CHECK_ERROR(glActiveTexture(unit() + GL_TEXTURE0));
    EASYGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, _value.texture()));
    
    real_type glValue = *this;
    GLUniformSetter::Set<type, 1>(locaion, &glValue);
}

NS_EASYGL_END
