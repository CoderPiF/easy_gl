//
//  Created by Pi on 2021/2/20.
//

#import "EasyGLView.h"
#import "easygl_ios.h"
#import "easygl_render_view.h"
#include "easygl_samples.hpp"
#include "easygl_samples_shaders.hpp"
#include "easygl_context.hpp"
#include "easygl_resource.hpp"
#include "easygl_program.hpp"

@interface EasyGLView ()
@property (nonatomic, assign) std::shared_ptr<EasyGLIOS> easygl;
@property (nonatomic, assign) std::shared_ptr<EasyGLRenderView> renderView;
@end

@implementation EasyGLView

+ (Class)layerClass
{
    return CAEAGLLayer.class;
}

- (instancetype) initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _easygl = std::make_shared<EasyGLIOS>();
        
        _renderView = std::make_shared<EasyGLRenderView>(_easygl->context());
        _renderView->setSize(frame.size.width, frame.size.height);
        _renderView->attachLayer((CAEAGLLayer *)super.layer);
        _easygl->addRenderer(_renderView);
        
//        [self helloTriangle1];
//        [self helloTriangle2];
        [self texturesCombined];
//        [self coordinateSystemsMultiple];
//        [self camera];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            self.easygl->run();
        });
    }
    return self;
}

- (void) layoutSubviews
{
    [super layoutSubviews];
    _renderView->setSize(self.bounds.size.width, self.bounds.size.height);
}

- (void) helloTriangle1
{
    using namespace easy_gl;
    using namespace samples;
    
    auto programMgr = _easygl->context()->resources()->programMgr();
    auto program = programMgr->makeFromFixed<PositionVSH, PositionVSHSrc, RGBFSH, UniformRGBFSHSrc>();

    auto shape = std::make_shared<ShapeBase<UniformRGBProgram>>(program, GL_TRIANGLES);
    shape->updateUniforms([&](RGBUniform *uniform){
        uniform->color(RGB(0xFF0000));
    });
    shape->updateVertexs(3, [&](PositionVertexAttr *vertex){
        vertex[0].pos = glm::vec3(0, 1, 0);
        vertex[1].pos = glm::vec3(-1, -1, 0);
        vertex[2].pos = glm::vec3(1, -1, 0);
    });
    _renderView->addShape(shape);
}

- (void) helloTriangle2
{
    using namespace easy_gl;
    using namespace samples;
    
    auto programMgr = _easygl->context()->resources()->programMgr();
    auto program = programMgr->makeFromFixed<PositionRGBVSH, PositionRGBVSHSrc, NoUniformFSH, VaryingRGBFSHSrc>();

    auto shape = std::make_shared<ShapeBase<VaryingRGBProgram>>(program, GL_TRIANGLES);
    shape->updateVertexs(3, [&](PositionRGBVertexAttr *vertex){
        vertex[0].pos = glm::vec3(0, 1, 0);
        vertex[0].color = RGB(0xFF0000);
        vertex[1].pos = glm::vec3(-1, -1, 0);
        vertex[1].color = RGB(0x00FF00);
        vertex[2].pos = glm::vec3(1, -1, 0);
        vertex[2].color = RGB(0x0000FF);
    });
    
    _renderView->addShape(shape);
}

- (void) texturesCombined
{
    using namespace easy_gl;
    using namespace samples;
    
    auto programMgr = _easygl->context()->resources()->programMgr();
    auto program = programMgr->makeFromFixed<TexCoordVSH, TexCoordVSHSrc, TextureFSH, TextureFSHSrc>();
    
    auto shape = std::make_shared<ShapeBase<TextureProgram>>(program, GL_TRIANGLE_STRIP);
    shape->updateVertexs(4, [&](TexCoordVertexAttr *vertex){
        vertex[0].pos = glm::vec2(-1, 1);
        vertex[0].texCoord = glm::vec2(0, 1);
        vertex[1].pos = glm::vec2(-1, -1);
        vertex[1].texCoord = glm::vec2(0, 0);
        vertex[2].pos = glm::vec2(1, 1);
        vertex[2].texCoord = glm::vec2(1, 1);
        vertex[3].pos = glm::vec2(1, -1);
        vertex[3].texCoord = glm::vec2(1, 0);
    });
    NSString *path = [NSBundle.mainBundle pathForResource:@"test.jpeg" ofType:@""];
    shape->updateUniforms([&](TexUniform *uniform){
        std::string filePath = path.UTF8String;
        auto dataProvider = ITextureDataProvider::CreateFromFile(filePath);
        TextureData data(_easygl->context(), dataProvider);
        uniform->texture1(data);
    });
    
    _renderView->addShape(shape);
}

- (void) coordinateSystemsMultiple
{
    
}

- (void) camera
{
    
}

@end
