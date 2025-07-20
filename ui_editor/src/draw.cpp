
#include <editor_renderers.h>
#include <editor_shaders.h>
#include <queue>
#include <functional>
#include <filesystem>
#include <memory>
#include <IndieGoUI.h>

using namespace IndieGo::UI;
using namespace IndieGo::vkI;
using namespace IndieGo::vkI::aux;
using namespace std;
namespace fs = filesystem;

unique_ptr<LayoutShader> layout_shader;
unique_ptr<LayoutRenderer> layout_renderer;

unique_ptr<SkinningShader> skinning_shader;
unique_ptr<SkinningRenderer> skinning_renderer;

unique_ptr<ScreenQuadShader> screen_quad_shader;
shared_ptr<ScreenQuadRenderer> screen_quad_renderer;
int maxFrames = 0;

#ifndef SHADERS_PREFIX
#define SHADERS_PREFIX ""
#endif

extern string winID;

void loadShader(const string & name) {
    string vertex = "", fragment = "";
    string load_path = SHADERS_PREFIX + name;
    for (auto f : fs::directory_iterator(load_path)) {
        if (f.symlink_status().type() == fs::file_type::regular) {
            // filter non-glsl files
            if (fs::path(f).extension() != ".spv") continue;
            // check name
            if (fs::path(f).filename() == "vert.spv") {
                vertex = fs::path(f).string();
            }
            if (fs::path(f).filename() == "frag.spv") {
                fragment = fs::path(f).string();
            }
        }
    }
    if (name == "layout_shader") {
        layout_shader->load(vertex.c_str(), fragment.c_str());
    } else if (name == "skinning_shader") {
        skinning_shader->load(vertex.c_str(), fragment.c_str());
    } else {
        screen_quad_shader->load(vertex.c_str(), fragment.c_str());
    }
};

void resizeUI() {
    Manager::resize(
        vkRenderer::swapChainExtent.width,
        vkRenderer::swapChainExtent.height
    );
}

static vector<VkImageView> pipeline_views = {};
void ScreenQuadShader::setTextureImages() {
    int i = 0;
    for (auto imageView : layout_renderer->textureImageViews) {
        pipeline_views[i] = imageView;
        i++;    
    }
    
    for (auto imageView : skinning_renderer->textureImageViews) {
        pipeline_views[i] = imageView;
        i++;    
    }

    // sceen quad renderer holds texture handlers for UI overlay
    for (auto imageView : screen_quad_renderer->textureImageViews) {
        pipeline_views[i] = imageView;
        i++;
    }
}

void initRenderers(GLFWwindow * w) {
    vkRenderer::resizeCallback = resizeUI;
    screen_quad_renderer = make_shared<ScreenQuadRenderer>(w);
    screen_quad_renderer->init();
    screen_quad_renderer->recreateNoFramebufferTexImages = true;

    skinning_renderer = make_unique<SkinningRenderer>(w, false);
    skinning_renderer->init();
    skinning_renderer->loadTextureImage();

    layout_renderer = make_unique<LayoutRenderer>(w, false);
    layout_renderer->init();

    maxFrames = vkRenderer::swapChainImagesCount;
    skinning_shader = make_unique<SkinningShader>(
        vkRenderer::device,
        vkRenderer::swapChainImagesCount,
        skinning_renderer->renderPass,
        skinning_renderer->uniformBuffers,

        // Here most imageViews are for framebuffers.
        // imageView for sampler was added as a last element in the array
        &skinning_renderer->textureImageViews,
        skinning_renderer->textureSamplers
    );
    skinning_shader->descriptorsInitData[1].bufferIdx = maxFrames;
    loadShader("skinning_shader");

    layout_shader = make_unique<LayoutShader>(
        vkRenderer::device,
        vkRenderer::swapChainImagesCount,
        layout_renderer->renderPass,
        layout_renderer->uniformBuffers
    );
    layout_shader->dynamicAlignment = layout_renderer->dynamicAlignment;
    layout_shader->dynamicOffsetsCount = 1;
    layout_shader->pipelineOutputs = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    layout_shader->fillDescrInitData();
    loadShader("layout_shader");

    pipeline_views = layout_renderer->textureImageViews;
    for (auto imageView : skinning_renderer->textureImageViews) {
        pipeline_views.push_back(imageView);
    }

    // sceen quad renderer holds texture handlers for UI overlay
    for (auto imageView : screen_quad_renderer->textureImageViews) {
        pipeline_views.push_back(imageView);
    }

    screen_quad_shader = make_unique<ScreenQuadShader>(
        vkRenderer::device,
        vkRenderer::swapChainImagesCount,
        screen_quad_renderer->renderPass,
        vector<VkBuffer>{},
        &pipeline_views,
        screen_quad_renderer->textureSamplers
    );
    screen_quad_shader->descriptorsInitData[1].bufferIdx = maxFrames;
    screen_quad_shader->descriptorsInitData[2].bufferIdx = maxFrames * 2 + 1;
    loadShader("screen_quad");
};


void updateUniformBuffer() {
    LayoutRectUBO ubo;
    float pos_x = Manager::UIMap["w skin image x"]._data.f / UI_FLT_VAL_SCALE;
    float pos_y = Manager::UIMap["w skin image y"]._data.f / UI_FLT_VAL_SCALE;
    float scale = Manager::UIMap["w skin image scale"]._data.f / UI_FLT_VAL_SCALE * 0.1;
    ubo.transform = glm::translate(
        glm::mat4(1.f), { pos_x, pos_y, 0.f }
    );
    ubo.transform = glm::scale(
        ubo.transform, { scale, scale, 0.f }
    );

    ubo.proj = glm::ortho(-1 * ((float)(WIDTH) / (float)HEIGHT), ((float)(WIDTH) / (float)HEIGHT), -1.f, 1.f, -100.f, 100.f);
    ubo.color = glm::vec4(0);
    ubo.color.r = Manager::UIMap["w display skin image"]._data.b;
    memcpy(skinning_renderer->posSizeMappedMemory[vkRenderer::currFrame], &ubo, sizeof(LayoutRectUBO));

    region_size<unsigned int> screen_size = Manager::getWidget("Edit elements").screen_size;
    region<float> main_screen_region =      Manager::getWidget("Edit elements").screen_region;
    region<float> skinning_screen_region =  Manager::getWidget("Skinning").screen_region;
    region<float> fonts_screen_region =     Manager::getWidget("Fonts").screen_region;
    region<float> ep_screen_region =        Manager::getWidget("Element properties").screen_region;

    layout_renderer->editor_widgets_ubo.main_square = { 
        screen_size.w * main_screen_region.x,
        screen_size.h - screen_size.h * (main_screen_region.y + main_screen_region.h),
        screen_size.w * (main_screen_region.x + main_screen_region.w),
        screen_size.h - screen_size.h * main_screen_region.y
    };
    layout_renderer->editor_widgets_ubo.skinning_square = { 
        screen_size.w * skinning_screen_region.x,
        screen_size.h - screen_size.h * (skinning_screen_region.y + skinning_screen_region.h),
        screen_size.w * (skinning_screen_region.x + skinning_screen_region.w),
        screen_size.h - screen_size.h * skinning_screen_region.y
    };
    layout_renderer->editor_widgets_ubo.elements_square = { 
        screen_size.w * ep_screen_region.x,
        screen_size.h - screen_size.h * (ep_screen_region.y + ep_screen_region.h),
        screen_size.w * (ep_screen_region.x + ep_screen_region.w),
        screen_size.h - screen_size.h * ep_screen_region.y
    };
    layout_renderer->editor_widgets_ubo.fonts_square = { 
        screen_size.w * fonts_screen_region.x,
        screen_size.h - screen_size.h * (fonts_screen_region.y + fonts_screen_region.h),
        screen_size.w * (fonts_screen_region.x + fonts_screen_region.w),
        screen_size.h - screen_size.h * fonts_screen_region.y
    };

    if (Manager::UIMap["w display skin image"]._data.b) {
        glm::vec2 crop_size = {
            (Manager::UIMap["w crop w"]._data.f * 2),
            (Manager::UIMap["w crop h"]._data.f * 2)
        };

        glm::vec2 crop_loc = {
            (Manager::UIMap["w crop x"]._data.f * 2),
            (Manager::UIMap["w crop y"]._data.f * 2)
        };
        
        crop_size /= UI_FLT_VAL_SCALE;
        crop_loc /= UI_FLT_VAL_SCALE;
        float x_unit = ((float)(HEIGHT) / (float)WIDTH);

        layout_renderer->layout_rect_ubo[layout_renderer->layout_rect_idx].transform = glm::translate(
            glm::mat4(1.f), 
            { 
                pos_x - x_unit + crop_size.x * 0.5 * x_unit + crop_loc.x * x_unit, 
                pos_y + 1.f - crop_size.y * 0.5 - crop_loc.y, 
                0.f 
            }
        );
        layout_renderer->layout_rect_ubo[layout_renderer->layout_rect_idx].transform = glm::scale(
            layout_renderer->layout_rect_ubo[layout_renderer->layout_rect_idx].transform, { crop_size.x, crop_size.y, 0.f }
        );

        layout_renderer->layout_rect_ubo[layout_renderer->layout_rect_idx].proj = glm::ortho(-1 * ((float)(WIDTH) / (float)HEIGHT), ((float)(WIDTH) / (float)HEIGHT), -1.f, 1.f, -100.f, 100.f);
        layout_renderer->layout_rect_ubo[layout_renderer->layout_rect_idx].color = glm::vec4(1);
        layout_renderer->layout_rect_idx++;
    }

    memcpy(layout_renderer->layoutMappedMemory[vkRenderer::currFrame], &layout_renderer->layout_rect_ubo, sizeof(LayoutRectUBO) * layout_renderer->layout_rect_idx);
    memcpy(layout_renderer->editorWidgetsMappedMemory[vkRenderer::currFrame], &layout_renderer->editor_widgets_ubo, sizeof(EditorWidgetsUBO));
};

void drawFrame() {
    updateUniformBuffer();

    vkRenderer::acquireImageFromSwapChain(screen_quad_renderer.get());
    // Render UI
    Manager::currFrame = vkRenderer::currFrame;
    Manager::drawFrameEnd();

    // Skin image rendering
    VkCommandBuffer frameCB = skinning_renderer->commandBuffers[vkRenderer::currFrame];
    skinning_renderer->beginRecordCommandBuffer(frameCB);
    skinning_renderer->beginRenderPass(frameCB, vkRenderer::imageIndex, skinning_shader.get());
    skinning_renderer->drawCommands(frameCB);
    vkCmdEndRenderPass(frameCB);

    // layout rendering
    layout_renderer->beginRenderPass(frameCB, vkRenderer::imageIndex);
    for (int i = 0; i < layout_renderer->layout_rect_idx; i++) {
        uint32_t offset = layout_renderer->getDynamicOffset(i);
        layout_shader->dynamicOffsets = &offset;
        layout_shader->use(frameCB, vkRenderer::currFrame);
        layout_renderer->drawCommands(frameCB);
    }
    vkCmdEndRenderPass(frameCB);

    // rendering of final image
    screen_quad_renderer->beginRenderPass(frameCB, vkRenderer::imageIndex, screen_quad_shader.get());
    screen_quad_renderer->drawCommands(frameCB);
    vkCmdEndRenderPass(frameCB);
    screen_quad_renderer->endRecordCommandBuffer(frameCB);
    screen_quad_renderer->submitQueue(frameCB, true);

    vkRenderer::presentImageToScreen(screen_quad_renderer.get());
}