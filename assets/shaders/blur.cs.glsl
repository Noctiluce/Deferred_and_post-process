#version 430 core

layout (local_size_x = 10, local_size_y = 10) in;

layout (rgba32f, binding = 0) uniform image2D img_output;
layout (binding = 1) uniform sampler2D img_input;

vec4 getSample (in sampler2D img, in vec2 uv, in vec2 resolution){
    ivec2       position = ivec2(uv.xy);
    vec2        screenNormalized = vec2(position) / vec2(resolution);
    return      texture2D(img, screenNormalized);
}

#define BLOOM_LEVELS   1
#define BLOOM_STRENGTH 1.0
#define BLOOM_FALLOFF  0.667

vec3 readBloomTile(in sampler2D tex, in vec2 coord, in float lod) {
    // Calculate those values to compute both tile transform and sampling bounds
    float offset = 1.0 - exp2(1.0 - lod);
    float width = exp2(-lod);
    
    // Inverse atlas transform
    coord *= width; // /= exp2(lod)
    coord += offset;
    
    // The single-texel margin is needed to account for linear atlas filtering issues
    // Can be removed if set to nearest, but the bloom will look blocky and awful
    // The bounding without margin is not needed at all, so both shall be removed together
    vec2 bounds = vec2(offset, offset + width);
    vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    float margin = max(texelSize.x, texelSize.y);
    bounds.x += margin;
    bounds.y -= margin;
    coord = clamp(coord, bounds.x, bounds.y);
    
    return texture(tex, coord).xyz;
}

vec3 getBloom(in sampler2D tex, in vec2 coord) {
    float weight = 1.0;
        
    vec4 color = vec4(0.0);
    for(int i = 1; i <= BLOOM_LEVELS; i++) {
    	color.xyz += readBloomTile(tex, coord, float(i)) * weight;
        color.w   += weight;
        
        weight *= BLOOM_FALLOFF;
    }
    return color.xyz / color.w;
}
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             

  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID);  // pixel coordinate
  ivec2 img_resolution = imageSize(img_output);       // image resolution

  
    ivec2       position = ivec2(pixel_coords.xy);
    vec2        screenNormalized = vec2(position) / vec2(img_resolution);

  vec4 result = vec4(getBloom(img_input, screenNormalized),1.0);
  imageStore(img_output, pixel_coords, result);
}