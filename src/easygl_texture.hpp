//
//  Created by Pi on 2022/5/1.
//

#ifndef easygl_texture_hpp
#define easygl_texture_hpp

#include "easygl_utils.hpp"
#include "easygl_anytype.hpp"
#include "easygl_context.hpp"
#include <memory>

NS_EASYGL_BEGIN

class ITextureDataProvider
{
public:
    using Ptr = std::shared_ptr<ITextureDataProvider>;
    enum class Status
    {
        Invalid = -1,
        Uninitialized,
        DataLoaded,
    };
    
    virtual Status status() const = 0;
    virtual GLsizei width() const = 0;
    virtual GLsizei height() const = 0;
    virtual GLsizei stride() const = 0;
    virtual GLenum internalFormat() const = 0;
    virtual GLenum format() const = 0;
    virtual GLenum type() const = 0;
    
    virtual bool loadData() = 0;
    virtual size_t dataSize() const = 0;
    virtual size_t bytePreRow() const = 0;
    virtual GLvoid * data() const = 0;
    virtual GLvoid * releaseData() = 0;
    
    virtual ~ITextureDataProvider() {};
    
    static Ptr CreateFromFile(const std::string &filePath);
};

class TextureData final
{
public:
    enum class Status
    {
        Invalid = -1,
        Uninitialized,
        DataLoaded,
    };
    
    TextureData();
    TextureData(const Context::Ptr &ctx, const ITextureDataProvider::Ptr &dataProvider);

    void invalidate();
    
    bool isInvalid() const;
    Status status() const;
    GLsizei width() const;
    GLsizei height() const;
    GLuint texture() const;
    bool load();

private:
    Status _status{Status::Uninitialized};
    ITextureDataProvider::Ptr _dataProvider;
    GLuint _texture = 0;
    Context::Ptr _ctx;
};

class GLTexture2D final : public GLAnyType
{
public:
    using type = TextureData;
    using real_type = GLint;

    GLTexture2D(const type &textureData);
    const type & value() const;
    operator real_type() const;
    
    void setUnit(GLenum unit);
    GLenum unit() const;
    void syncUniform(GLint locaion) override;
    
private:
    GLenum _unit = 0;
    type _value;
};

NS_EASYGL_END

#endif /* easygl_texture_hpp */
