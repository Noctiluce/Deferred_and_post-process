#version 430 core

layout (local_size_x = 10, local_size_y = 10) in;

layout (rgba32f, binding = 0) uniform image2D img_output;
layout (binding = 1) uniform sampler2D img_input;

vec4 getSample (in sampler2D img, in vec2 uv, in vec2 resolution){
    ivec2       position = ivec2(uv.xy);
    vec2        screenNormalized = vec2(position) / vec2(resolution);
    return      texture2D(img, screenNormalized);
}

void main() {
  vec4 BrightColor = vec4(0,0,0,1);     // final image pixel
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID);  // pixel coordinate
  ivec2 img_resolution = imageSize(img_output);       // image resolution

  vec4 realColor = getSample(img_input, pixel_coords, img_resolution);

    if((realColor.r + realColor.g + realColor.b)/3.0 > 0.5)
        BrightColor = vec4(realColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

  imageStore(img_output, pixel_coords, realColor);
}