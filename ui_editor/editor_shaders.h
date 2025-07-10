#include <Shader.h>
#include <glm/glm.hpp>
#include <editor_structs.h>
#include <vector>

class LayoutShader : public Shader {
public:
    size_t dynamicAlignment = 0;
    LayoutShader( 
        VkDevice vkd, 
        int sImgsCnt,
        VkRenderPass rp,
        std::vector<VkBuffer> ubos = {},
        std::vector<VkImageView> * tivs = nullptr,
        std::vector<VkSampler> ts = {}
    ) : Shader(vkd, sImgsCnt, rp, ubos, tivs, ts) {};


    VkVertexInputBindingDescription getBindingDescription() override {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = UINT32_MAX;
        return bindingDescription;
    };

    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        return attributeDescriptions;
    };

    void fillDescrInitData() override {
        descriptorsInitData = {
            {
                descriptor_type::dynamic_uniform,
                shader_stage::vertex,
                true,
                0,
                dynamicAlignment
            }, {
                descriptor_type::uniform,
                shader_stage::fragment,
                true,
                (uint32_t)swapImagesCount
            }
        };
    }
};

class SkinningShader : public Shader {
public:
    SkinningShader( 
        VkDevice vkd, 
        int sImgsCnt,
        VkRenderPass rp,
        std::vector<VkBuffer> ubos = {},
        std::vector<VkImageView> * tivs = nullptr,
        std::vector<VkSampler> ts = {}
    ) : Shader(vkd, sImgsCnt, rp, ubos, tivs, ts) {
        fillDescrInitData();
    };

    VkVertexInputBindingDescription getBindingDescription() override {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = UINT32_MAX;
        return bindingDescription;
    };

    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() override {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        return attributeDescriptions;
    };


    void fillDescrInitData() override {
        descriptorsInitData = {
            {
                descriptor_type::uniform,
                shader_stage::vertex,
                true,
                0,
                sizeof(LayoutRectUBO)
            }, {
                descriptor_type::sampler,
                shader_stage::fragment,
                false,
                0
            }
        };
    };
};


class ScreenQuadShader : public Shader {
public:
    ScreenQuadShader( 
        VkDevice vkd, 
        int sImgsCnt,
        VkRenderPass rp,
        std::vector<VkBuffer> ubos = {},
        std::vector<VkImageView> * tivs = nullptr,
        std::vector<VkSampler> ts = {}
    ) : Shader(vkd, sImgsCnt, rp, ubos, tivs, ts) {
        fillDescrInitData();
    };

    VkVertexInputBindingDescription getBindingDescription() override {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = UINT32_MAX;
        return bindingDescription;
    };

    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() override {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        return attributeDescriptions;
    };

    void setTextureImages() override;

    void fillDescrInitData() override {
        descriptorsInitData = {
            {
                descriptor_type::sampler,
                shader_stage::fragment,
                true,
                0
            }, {
                descriptor_type::sampler,
                shader_stage::fragment,
                true,
                0
            }, {
                descriptor_type::sampler,
                shader_stage::fragment,
                true,
                0
            }
        };
    };
};