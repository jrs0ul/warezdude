/*    Copyright (C) 2025 by jrs0ul                                          *
 *   jrs0ul@gmail.com                                                      *
*/

#include "ShaderProgram.h"
#include <cstdio>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#endif

void ShaderProgram::create(bool useVulkan)
{

    isVulkanShader = useVulkan;

    if (!useVulkan)
    {
        program = glCreateProgram();
    }
}

void ShaderProgram::destroy()
{
    glDeleteProgram(program);
}

void ShaderProgram::getLog(char* string, int len)
{
#ifndef __ANDROID__
    glGetInfoLogARB(program, len, 0, string);
#else
    glGetShaderInfoLog(program, len, 0, string);
#endif
}

void ShaderProgram::attach(Shader & sh)
{
    if (!isVulkanShader)
    {
        glAttachShader(program, sh.id);
    }
    else //VULKAN
    {
        VkPipelineShaderStageCreateInfo vkShaderStage{};
        vkShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkShaderStage.pNext = nullptr;
        vkShaderStage.stage = (sh.type == VERTEX_SHADER) ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        vkShaderStage.module = sh.vkShaderModule;
        vkShaderStage.pName = "main";


        vkShaderStages.push_back(vkShaderStage);
    }
}

void ShaderProgram::link()
{
    glLinkProgram(program);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE)
    {
#ifdef __ANDROID__
        LOGI("Linking failed!");
#else
        printf("Linking failed!!!1\n");
#endif

    }
    else
    {
        //printf("All good\n");
    }
}

void ShaderProgram::buildVkPipeline(VkDevice* device, VkRenderPass* pass)
{

    VkPipelineLayout           pipelineLayout;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    vkCreatePipelineLayout(*device, &pipelineLayoutInfo, nullptr, &pipelineLayout);


    VkPipelineVertexInputStateCreateInfo   vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;


    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext = nullptr;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;


    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = {640, 400};

    VkViewport viewport;
    viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = 640;
	viewport.height = 400;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

    VkPipelineViewportStateCreateInfo      viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.pNext = nullptr;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo   multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pNext = nullptr;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo    colorBlending = {};
    colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext           = nullptr;
    colorBlending.logicOpEnable   = VK_FALSE;
    colorBlending.logicOp         = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments    = &colorBlendAttachment;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext               = nullptr;
    pipelineInfo.stageCount          = vkShaderStages.size();
    pipelineInfo.pStages             = vkShaderStages.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.layout              = pipelineLayout;
    pipelineInfo.renderPass          = *pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(*device,
                                  VK_NULL_HANDLE,
                                  1,
                                  &pipelineInfo,
                                  nullptr,
                                  &vkPipeline) != VK_SUCCESS)
    {
        printf("Failed to create a pipeline\n");
    }
}

void ShaderProgram::use(VkCommandBuffer* vkCmd)
{
    if (!isVulkanShader)
    {
        glUseProgram(program);
    }
    else // VULKAN
    {
        vkCmdBindPipeline(*vkCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
    }
}

int ShaderProgram::getUniformID(const char* name)
{
    return  glGetUniformLocation(program, name);
}

int ShaderProgram::getAttributeID(const char* name)
{
    return glGetAttribLocation(program, name);
}
