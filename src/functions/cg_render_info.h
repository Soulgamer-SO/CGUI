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

#ifndef CG_RENDER_INFO_H
#define CG_RENDER_INFO_H 1
#include <vulkan/vulkan.h>

typedef struct render_info {
    uint32_t image_view_count;
    VkImageView *image_view_array;
    VkRenderPass render_pass;
    uint32_t framebuffer_count;
    VkFramebuffer *framebuffer_array;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
} render_info_t;

#endif // CG_RENDER_INFO_H 1
