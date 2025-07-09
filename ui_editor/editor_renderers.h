#pragma once
#include <Renderer.h>
#include <editor_structs.h>
#include <array>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

#ifndef TEST_IMAGE_PATH
#define TEST_IMAGE_PATH ""
#endif

using namespace IndieGo::vkI;
using namespace IndieGo::vkI::aux;

class LayoutRenderer : public vkRenderer {
    size_t minUboAlignment = 16; // hardcoded, check = vulkanDevice->properties.limits.minUniformBufferOffsetAlignment;

    public:
        size_t dynamicAlignment = 0;
        std::array<LayoutRectUBO, MAX_WIDGET_ELEMENTS> layout_rect_ubo;
        EditorWidgetsUBO editor_widgets_ubo;
        // indx of max layout rectangle that should be drawn in current frame
        uint32_t layout_rect_idx = 0;
        LayoutRenderer(GLFWwindow * w, bool initRenderPass = true) : vkRenderer(w, initRenderPass) {};

        std::vector<void*> layoutMappedMemory;
        std::vector<void*> editorWidgetsMappedMemory;
        void init() override {
            renderPassAttachmentFormat = VK_FORMAT_R8G8B8A8_SRGB;
            renderPassFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            createRenderPass();

            createTextureImages(swapChainImagesCount);
            createTextureImageViews(textureImages);
            createTextureImageFramebuffers(textureImageViews);

            // Test option will draw to swapChainImage directly
            // createSwapChainFramebuffers();
            initFramebuffers(textureFramebuffers);

            // Vertices hardcoded, so we need only unifroms memory
            layoutMappedMemory.resize(swapChainImagesCount);
            dynamicAlignment = sizeof(LayoutRectUBO);
            dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
            size_t bufferSize = dynamicAlignment * MAX_WIDGET_ELEMENTS;
            createUnifromBuffers(layoutMappedMemory, bufferSize);

            editorWidgetsMappedMemory.resize(swapChainImagesCount);
            createUnifromBuffers(editorWidgetsMappedMemory, sizeof(EditorWidgetsUBO));

            createCommandBuffers(swapChainImagesCount);
            createSyncObjects(swapChainImagesCount * 2, swapChainImagesCount);

            // TODO : automatically detect renderers amount
            frameSemaphores.resize(swapChainImagesCount * 2);
            clearColor.color.float32[3] = 0.f;
        };

        void drawCommands(VkCommandBuffer commandBuffer) override {
            vkCmdDraw(commandBuffer, 5, 1, 0, 0);
        };

        uint32_t getDynamicOffset(int idx) {
            return idx * dynamicAlignment;
        }

        void drawLayout(LayoutRect element) {
            // create matix, update layout rectangle ubo
            layout_rect_ubo[layout_rect_idx].transform = glm::translate(
                glm::mat4(1.f), { element.x, element.y * -1, 0.f }
            );
            // layout_rect_ubo[layout_rect_idx].scale = { element.width, element.height };
            layout_rect_ubo[layout_rect_idx].transform = glm::scale(
                layout_rect_ubo[layout_rect_idx].transform, { element.width * ((float)(WIDTH) / (float)HEIGHT), element.height, 0.f }
            );

            layout_rect_ubo[layout_rect_idx].proj = glm::ortho(-1 * ((float)(WIDTH) / (float)HEIGHT), ((float)(WIDTH) / (float)HEIGHT), -1.f, 1.f, -100.f, 100.f);

            layout_rect_ubo[layout_rect_idx].color.r = element.red;
            layout_rect_ubo[layout_rect_idx].color.g = element.green;
            layout_rect_ubo[layout_rect_idx].color.b = element.blue;
            layout_rect_ubo[layout_rect_idx].color.a = element.alpha;


            layout_rect_idx++;
        };
};


class SkinningRenderer : public vkRenderer {
    public:
        SkinningRenderer(GLFWwindow * w, bool initRenderPass = true) : vkRenderer(w, initRenderPass) {};

        std::vector<void*> posSizeMappedMemory;
        int texWidth, texHeight, texChannels;
        void * skinImgID = nullptr;
        void loadTextureImage();
        void init() override {
            renderPassAttachmentFormat = VK_FORMAT_R8G8B8A8_SRGB;
            renderPassFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            createRenderPass();

            // Test option will draw to swapChainImage directly
            // createSwapChainFramebuffers();
            // initFramebuffers(swapChainFramebuffers);
            createTextureImages(swapChainImagesCount);
            createTextureImageViews(textureImages);
            createTextureImageFramebuffers(textureImageViews);
            initFramebuffers(textureFramebuffers);

            // Vertices hardcoded, so we need only unifroms memory
            posSizeMappedMemory.resize(swapChainImagesCount);
            createUnifromBuffers(posSizeMappedMemory, sizeof(LayoutRectUBO));

            createCommandBuffers(swapChainImagesCount);
            createSyncObjects(swapChainImagesCount * 2, swapChainImagesCount);

            frameSemaphores.resize(swapChainImagesCount * 2);
        };

        void drawCommands(VkCommandBuffer commandBuffer) override {
            vkCmdDraw(commandBuffer, 6, 1, 0, 0);
        };
};


class ScreenQuadRenderer : public vkRenderer {
    public:
        ScreenQuadRenderer(GLFWwindow * w, bool initRenderPass = true) : vkRenderer(w, initRenderPass) {};

        void init() override {
            // Test option will draw to swapChainImage directly
            createSwapChainFramebuffers();
            initFramebuffers(swapChainFramebuffers);

            createCommandBuffers(swapChainImagesCount);
            createSyncObjects(swapChainImagesCount * 2, swapChainImagesCount);
            createTextureImageSamplers(swapChainImagesCount * 3 + 1);

            imageCreateFormat = swapChainImageFormat;
            imageViewInitFormat = swapChainImageFormat;
            createTextureImages(swapChainImagesCount);
            createTextureImageViews(textureImages);
    
            frameSemaphores.resize(swapChainImagesCount * 2);
        };

        void drawCommands(VkCommandBuffer commandBuffer) override {
            vkCmdDraw(commandBuffer, 6, 1, 0, 0);
        };
};