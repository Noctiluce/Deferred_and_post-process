#version 430 core

layout (local_size_x = 10, local_size_y = 10) in;

layout (rgba32f, binding = 0) uniform image2D img_output;
layout (binding = 1) uniform sampler2D img_input1;
layout (binding = 2) uniform sampler2D img_input2;

vec4 getSample (in sampler2D img, in vec2 uv, in vec2 resolution){
    ivec2       position = ivec2(uv.xy);
    vec2        screenNormalized = vec2(position) / vec2(resolution);
    return      texture2D(img, screenNormalized);
}

void main()
{             

  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID);  // pixel coordinate
  ivec2 img_resolution = imageSize(img_output);       // image resolution

  vec4 result = getSample(img_input1, pixel_coords, img_resolution)+getSample(img_input2, pixel_coords, img_resolution);

    
  imageStore(img_output, pixel_coords, result);
}