/*
Copyright (C) 2025  Soulgamer <SOsoulgamer@outlook.com>.

This file is part of CGUI.

CGUI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CGUI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cg_render.h"
#include "cg_log.h"
#include <stdio.h>
#include <stdlib.h>

static bool cg_read_shader(const char *path, uint32_t **code, size_t *size) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr || fseek(file, 0, SEEK_END) != 0) {
        if (file != nullptr) {
            fclose(file);
        }
        return false;
    }
    long file_size = ftell(file);
    if (file_size <= 0 || (file_size % sizeof(uint32_t)) != 0) {
        fclose(file);
        return false;
    }
    rewind(file);
    *code = malloc((size_t)file_size);
    if (*code == nullptr || fread(*code, 1, (size_t)file_size, file) != (size_t)file_size) {
        free(*code);
        *code = nullptr;
        fclose(file);
        return false;
    }
    fclose(file);
    *size = (size_t)file_size;
    return true;
}

static bool cg_create_shader_module(cg_info_t *p_info, const char *path, VkShaderModule *module) {
    uint32_t *code = nullptr;
    size_t size = 0;
    if (!cg_read_shader(path, &code, &size)) {
        return false;
    }
    PFN_vkCreateShaderModule create_shader_module = (PFN_vkCreateShaderModule)p_info->library.vk_get_device_proc_addr(
        p_info->logic_device.vk_logic_device, "vkCreateShaderModule");
    if (create_shader_module == nullptr) {
        free(code);
        return false;
    }
    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = code};
    p_info->library.vk_result = create_shader_module(
        p_info->logic_device.vk_logic_device, &create_info, nullptr, module);
    free(code);
    return p_info->library.vk_result == VK_SUCCESS;
}

bool cg_create_render_resources(cg_info_t *p_info) {
    VkDevice device = p_info->logic_device.vk_logic_device;
    PFN_vkCreateImageView create_image_view = (PFN_vkCreateImageView)p_info->library.vk_get_device_proc_addr(device, "vkCreateImageView");
    PFN_vkCreateRenderPass create_render_pass = (PFN_vkCreateRenderPass)p_info->library.vk_get_device_proc_addr(device, "vkCreateRenderPass");
    PFN_vkCreateFramebuffer create_framebuffer = (PFN_vkCreateFramebuffer)p_info->library.vk_get_device_proc_addr(device, "vkCreateFramebuffer");
    PFN_vkCreatePipelineLayout create_pipeline_layout = (PFN_vkCreatePipelineLayout)p_info->library.vk_get_device_proc_addr(device, "vkCreatePipelineLayout");
    PFN_vkCreateGraphicsPipelines create_graphics_pipelines = (PFN_vkCreateGraphicsPipelines)p_info->library.vk_get_device_proc_addr(device, "vkCreateGraphicsPipelines");
    PFN_vkDestroyShaderModule destroy_shader_module = (PFN_vkDestroyShaderModule)p_info->library.vk_get_device_proc_addr(device, "vkDestroyShaderModule");
    if (create_image_view == nullptr || create_render_pass == nullptr || create_framebuffer == nullptr || create_pipeline_layout == nullptr || create_graphics_pipelines == nullptr || destroy_shader_module == nullptr) {
        PRINT_ERROR("load render functions fail!\n");
        return false;
    }

    p_info->wsi.swapchain_image_view_array = cg_alloc_memory(p_info->p_memory_pool, p_info->wsi.swapchain_image_count * sizeof(VkImageView));
    p_info->wsi.framebuffer_array = cg_alloc_memory(p_info->p_memory_pool, p_info->wsi.swapchain_image_count * sizeof(VkFramebuffer));
    if (p_info->wsi.swapchain_image_view_array == nullptr || p_info->wsi.framebuffer_array == nullptr) {
        return false;
    }

    for (uint32_t i = 0; i < p_info->wsi.swapchain_image_count; i++) {
        p_info->wsi.swapchain_image_view_array[i] = VK_NULL_HANDLE;
        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = p_info->wsi.swapchain_image_array[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = p_info->wsi.enabled_surface_format.format,
            .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
        if (create_image_view(device, &view_info, nullptr, &p_info->wsi.swapchain_image_view_array[i]) != VK_SUCCESS) {
            return false;
        }
    }

    VkAttachmentDescription attachment = {.format = p_info->wsi.enabled_surface_format.format, .samples = VK_SAMPLE_COUNT_1_BIT, .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE, .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};
    VkAttachmentReference color_reference = {.attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkSubpassDescription subpass = {.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS, .colorAttachmentCount = 1, .pColorAttachments = &color_reference};
    VkRenderPassCreateInfo render_pass_info = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, .attachmentCount = 1, .pAttachments = &attachment, .subpassCount = 1, .pSubpasses = &subpass};
    if (create_render_pass(device, &render_pass_info, nullptr, &p_info->wsi.render_pass) != VK_SUCCESS) {
        return false;
    }

    VkShaderModule vertex_module = VK_NULL_HANDLE;
    VkShaderModule fragment_module = VK_NULL_HANDLE;
    if (!cg_create_shader_module(p_info, "content/shader/shader.vert.spv", &vertex_module) || !cg_create_shader_module(p_info, "content/shader/shader_test.frag.spv", &fragment_module)) {
        return false;
    }
    VkPipelineShaderStageCreateInfo stages[2] = {{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = vertex_module, .pName = "main"}, {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = fragment_module, .pName = "main"}};
    VkPipelineVertexInputStateCreateInfo vertex_input = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    VkPipelineViewportStateCreateInfo viewport_state = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, .viewportCount = 1, .scissorCount = 1};
    VkPipelineRasterizationStateCreateInfo rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, .polygonMode = VK_POLYGON_MODE_FILL, .cullMode = VK_CULL_MODE_NONE, .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE, .lineWidth = 1.0f};
    VkPipelineMultisampleStateCreateInfo multisample = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT};
    VkPipelineColorBlendAttachmentState blend_attachment = {.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
    VkPipelineColorBlendStateCreateInfo blend = {.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, .attachmentCount = 1, .pAttachments = &blend_attachment};
    VkDynamicState dynamic_states[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, .dynamicStateCount = 2, .pDynamicStates = dynamic_states};
    if (create_pipeline_layout(device, &(VkPipelineLayoutCreateInfo){.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO}, nullptr, &p_info->wsi.pipeline_layout) != VK_SUCCESS) {
        return false;
    }
    VkGraphicsPipelineCreateInfo pipeline_info = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, .stageCount = 2, .pStages = stages, .pVertexInputState = &vertex_input, .pInputAssemblyState = &input_assembly, .pViewportState = &viewport_state, .pRasterizationState = &rasterizer, .pMultisampleState = &multisample, .pColorBlendState = &blend, .pDynamicState = &dynamic_state, .layout = p_info->wsi.pipeline_layout, .renderPass = p_info->wsi.render_pass, .subpass = 0};
    VkResult pipeline_result = create_graphics_pipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &p_info->wsi.graphics_pipeline);
    destroy_shader_module(device, vertex_module, nullptr);
    destroy_shader_module(device, fragment_module, nullptr);
    if (pipeline_result != VK_SUCCESS) {
        return false;
    }

    for (uint32_t i = 0; i < p_info->wsi.swapchain_image_count; i++) {
        VkImageView attachment_view = p_info->wsi.swapchain_image_view_array[i];
        VkFramebufferCreateInfo framebuffer_info = {.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, .renderPass = p_info->wsi.render_pass, .attachmentCount = 1, .pAttachments = &attachment_view, .width = p_info->wsi.enabled_image_extent_size.width, .height = p_info->wsi.enabled_image_extent_size.height, .layers = 1};
        if (create_framebuffer(device, &framebuffer_info, nullptr, &p_info->wsi.framebuffer_array[i]) != VK_SUCCESS) {
            return false;
        }
    }
    return true;
}

bool cg_draw_frame(cg_info_t *p_info) {
    VkDevice device = p_info->logic_device.vk_logic_device;
    VkQueue queue = p_info->logic_device.queue_family_handle;
    VkSwapchainKHR swapchain = p_info->wsi.swapchain;
    VkCommandBuffer command_buffer = p_info->command_pool.command_buffer_array[0];
    VkSemaphore image_available = p_info->sync.semaphore_array[0];
    VkSemaphore render_finished = p_info->sync.semaphore_array[1];
    VkFence in_flight = p_info->sync.fence_array[0];

    PFN_vkWaitForFences wait_for_fences = (PFN_vkWaitForFences)p_info->library.vk_get_device_proc_addr(device, "vkWaitForFences");
    PFN_vkResetFences reset_fences = (PFN_vkResetFences)p_info->library.vk_get_device_proc_addr(device, "vkResetFences");
    PFN_vkAcquireNextImageKHR acquire_next_image = (PFN_vkAcquireNextImageKHR)p_info->library.vk_get_device_proc_addr(device, "vkAcquireNextImageKHR");
    PFN_vkResetCommandBuffer reset_command_buffer = (PFN_vkResetCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkResetCommandBuffer");
    PFN_vkBeginCommandBuffer begin_command_buffer = (PFN_vkBeginCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkBeginCommandBuffer");
    PFN_vkEndCommandBuffer end_command_buffer = (PFN_vkEndCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkEndCommandBuffer");
    PFN_vkCmdBeginRenderPass cmd_begin_render_pass = (PFN_vkCmdBeginRenderPass)p_info->library.vk_get_device_proc_addr(device, "vkCmdBeginRenderPass");
    PFN_vkCmdEndRenderPass cmd_end_render_pass = (PFN_vkCmdEndRenderPass)p_info->library.vk_get_device_proc_addr(device, "vkCmdEndRenderPass");
    PFN_vkCmdBindPipeline cmd_bind_pipeline = (PFN_vkCmdBindPipeline)p_info->library.vk_get_device_proc_addr(device, "vkCmdBindPipeline");
    PFN_vkCmdSetViewport cmd_set_viewport = (PFN_vkCmdSetViewport)p_info->library.vk_get_device_proc_addr(device, "vkCmdSetViewport");
    PFN_vkCmdSetScissor cmd_set_scissor = (PFN_vkCmdSetScissor)p_info->library.vk_get_device_proc_addr(device, "vkCmdSetScissor");
    PFN_vkCmdDraw cmd_draw = (PFN_vkCmdDraw)p_info->library.vk_get_device_proc_addr(device, "vkCmdDraw");
    PFN_vkQueueSubmit queue_submit = (PFN_vkQueueSubmit)p_info->library.vk_get_device_proc_addr(device, "vkQueueSubmit");
    PFN_vkQueuePresentKHR queue_present = (PFN_vkQueuePresentKHR)p_info->library.vk_get_device_proc_addr(device, "vkQueuePresentKHR");

    if (wait_for_fences == nullptr || reset_fences == nullptr ||
        acquire_next_image == nullptr || reset_command_buffer == nullptr ||
        begin_command_buffer == nullptr || end_command_buffer == nullptr ||
        cmd_begin_render_pass == nullptr || cmd_end_render_pass == nullptr ||
        cmd_bind_pipeline == nullptr || cmd_set_viewport == nullptr ||
        cmd_set_scissor == nullptr || cmd_draw == nullptr ||
        queue_submit == nullptr || queue_present == nullptr) {
        PRINT_ERROR("load frame functions fail!\n");
        return false;
    }

    VkResult result = wait_for_fences(device, 1, &in_flight, VK_TRUE, UINT64_MAX);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("wait frame fence fail!\n");
        return false;
    }

    uint32_t image_index = 0;
    result = acquire_next_image(device, swapchain, UINT64_MAX, image_available, VK_NULL_HANDLE, &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return true;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        PRINT_ERROR("acquire swapchain image fail!\n");
        return false;
    }

    result = reset_command_buffer(command_buffer, 0);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("reset command buffer fail!\n");
        return false;
    }
    result = begin_command_buffer(command_buffer, &(VkCommandBufferBeginInfo){
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr});
    if (result != VK_SUCCESS) {
        PRINT_ERROR("begin command buffer fail!\n");
        return false;
    }
    VkClearValue clear_value = {.color = {{0.03f, 0.05f, 0.12f, 1.0f}}};
    VkRenderPassBeginInfo render_pass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = p_info->wsi.render_pass,
        .framebuffer = p_info->wsi.framebuffer_array[image_index],
        .renderArea = {.offset = {0, 0}, .extent = p_info->wsi.enabled_image_extent_size},
        .clearValueCount = 1,
        .pClearValues = &clear_value};
    cmd_begin_render_pass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)p_info->wsi.enabled_image_extent_size.width,
        .height = (float)p_info->wsi.enabled_image_extent_size.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f};
    VkRect2D scissor = {.offset = {0, 0}, .extent = p_info->wsi.enabled_image_extent_size};
    cmd_set_viewport(command_buffer, 0, 1, &viewport);
    cmd_set_scissor(command_buffer, 0, 1, &scissor);
    cmd_bind_pipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_info->wsi.graphics_pipeline);
    cmd_draw(command_buffer, 30, 1, 0, 0);
    cmd_end_render_pass(command_buffer);

    result = end_command_buffer(command_buffer);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("end command buffer fail!\n");
        return false;
    }

    result = reset_fences(device, 1, &in_flight);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("reset frame fence fail!\n");
        return false;
    }

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &image_available,
        .pWaitDstStageMask = &wait_stage,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &render_finished};
    result = queue_submit(queue, 1, &submit_info, in_flight);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("submit frame fail!\n");
        return false;
    }

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_finished,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_index,
        .pResults = nullptr};
    result = queue_present(queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        return true;
    }
    if (result != VK_SUCCESS) {
        PRINT_ERROR("present frame fail!\n");
        return false;
    }

    return true;
}

void cg_destroy_render_resources(cg_info_t *p_info) {
    VkDevice device = p_info->logic_device.vk_logic_device;
    if (device == VK_NULL_HANDLE) {
        return;
    }

    PFN_vkDestroyFramebuffer destroy_framebuffer = (PFN_vkDestroyFramebuffer)p_info->library.vk_get_device_proc_addr(device, "vkDestroyFramebuffer");
    PFN_vkDestroyPipeline destroy_pipeline = (PFN_vkDestroyPipeline)p_info->library.vk_get_device_proc_addr(device, "vkDestroyPipeline");
    PFN_vkDestroyPipelineLayout destroy_pipeline_layout = (PFN_vkDestroyPipelineLayout)p_info->library.vk_get_device_proc_addr(device, "vkDestroyPipelineLayout");
    PFN_vkDestroyRenderPass destroy_render_pass = (PFN_vkDestroyRenderPass)p_info->library.vk_get_device_proc_addr(device, "vkDestroyRenderPass");
    PFN_vkDestroyImageView destroy_image_view = (PFN_vkDestroyImageView)p_info->library.vk_get_device_proc_addr(device, "vkDestroyImageView");

    if (destroy_framebuffer != nullptr && p_info->wsi.framebuffer_array != nullptr) {
        for (uint32_t i = 0; i < p_info->wsi.swapchain_image_count; i++) {
            if (p_info->wsi.framebuffer_array[i] != VK_NULL_HANDLE) {
                destroy_framebuffer(device, p_info->wsi.framebuffer_array[i], nullptr);
            }
        }
    }
    if (destroy_pipeline != nullptr && p_info->wsi.graphics_pipeline != VK_NULL_HANDLE) {
        destroy_pipeline(device, p_info->wsi.graphics_pipeline, nullptr);
    }
    if (destroy_pipeline_layout != nullptr && p_info->wsi.pipeline_layout != VK_NULL_HANDLE) {
        destroy_pipeline_layout(device, p_info->wsi.pipeline_layout, nullptr);
    }
    if (destroy_render_pass != nullptr && p_info->wsi.render_pass != VK_NULL_HANDLE) {
        destroy_render_pass(device, p_info->wsi.render_pass, nullptr);
    }
    if (destroy_image_view != nullptr && p_info->wsi.swapchain_image_view_array != nullptr) {
        for (uint32_t i = 0; i < p_info->wsi.swapchain_image_count; i++) {
            if (p_info->wsi.swapchain_image_view_array[i] != VK_NULL_HANDLE) {
                destroy_image_view(device, p_info->wsi.swapchain_image_view_array[i], nullptr);
            }
        }
    }

    p_info->wsi.graphics_pipeline = VK_NULL_HANDLE;
    p_info->wsi.pipeline_layout = VK_NULL_HANDLE;
    p_info->wsi.render_pass = VK_NULL_HANDLE;
}
