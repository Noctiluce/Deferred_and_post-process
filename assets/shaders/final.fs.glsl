//  Shader Toy : https://www.shadertoy.com/view/ldSyzV
#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D finalImage;
uniform int currentEffect;

uniform float weight[9] = float[] (1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
uniform float u_time;


const float PI = 3.1415926536;
const float PI2 = PI * 2.0; 
const int mSize = 9;
const int kSize = (mSize-1)/2;
const float sigma = 3.0;
float kernel[mSize];
const float levels = 10.0;

// Gaussian PDF
float normpdf(in float x, in float sigma) 
{
	return 0.39894 * exp(-0.5 * x * x / (sigma * sigma)) / sigma;
}

// 
vec3 colorDodge(in vec3 src, in vec3 dst)
{
    return step(0.0, dst) * mix(min(vec3(1.0), dst/ (1.0 - src)), vec3(1.0), step(1.0, src)); 
}

float greyScale(in vec3 col) 
{
    return dot(col, vec3(0.3, 0.59, 0.11));
    //return dot(col, vec3(0.2126, 0.7152, 0.0722)); //sRGB
}

vec2 random(vec2 p){
	p = fract(p * (vec2(314.159, 314.265)));
    p += dot(p, p.yx + 17.17);
    return fract((p.xx + p.yx) * p.xy);
}

void main()
{             
   // FragColor = vec4(texture(finalImage, TexCoords).rgb+texture(finalImage2, TexCoords).rgb, 1.0);
   vec4 tex = texture(finalImage, TexCoords);
   
   vec4 result = tex;

   /*
   result= vec4(vec3(greyScale(result.rgb)), 1.0);

   float level = floor(result.x * levels);
   result = vec4( texture(albedoImage, TexCoords).rgb * vec3(level/levels), 1.0);
   */

   
   if(currentEffect == 1){
    //https://www.shadertoy.com/view/ltGyz1
    float weights = 0.;
    vec3 bloom = vec3(0);
    const int kernel = 10;
    for ( int j=-kernel; j <= kernel; j++ )
    {
        for ( int i=-kernel; i <= kernel; i++ )
        {
            float weight = pow( smoothstep(float(kernel+1),0.,length(vec2(i,j))), 1. );
            bloom += texture( finalImage, TexCoords+ivec2(i,j), 0 ).rgb * weight;
            weights += weight;
        }
    }
    
    result.rgb += bloom*0.3 / weights;
   }
   else if (currentEffect == 2){

   // SOBEL
    vec2 texelSize = 1.0 / vec2(textureSize(finalImage, 0));
    result = vec4(0.0,0.0,0.0,1.0);
    int i = 0;
    for (int x = -1; x <= 1; ++x) 
    {
        for (int y = 1; y >= -1; --y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            vec4 tmp = texture(finalImage, TexCoords + offset).rgba;
            tmp = vec4(vec3((tmp.r + tmp.g + tmp.b) /3.0), 1.0);
            result += tmp * weight[i++];
            

        }
    }

    i = 0;
    for (int y = 1; y >= -1; --y) 
    {
        for (int x = -1; x <= 1; ++x) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            vec4 tmp = texture(finalImage, TexCoords + offset).rgba;
            tmp = vec4(vec3((tmp.r + tmp.g + tmp.b) /3.0), 1.0);
            result += tmp * weight[i++];
            

        }
    }

    result = 1.0-result ;
   
    }
   else if(currentEffect == 3){  
   vec3 col = tex.rgb;
   vec2 r = random(TexCoords+vec2(u_time/1000000.0));
   r.x *= PI2;
   vec2 cr = vec2(sin(r.x),cos(r.x))*sqrt(r.y);
   vec3 blurred = texture(finalImage, TexCoords + cr * (vec2(mSize) / vec2(1920.0,1080.0)) ).rgb;
   vec3 inv = vec3(1.0) - blurred; 
   vec3 lighten = colorDodge(col, inv);
   vec3 res = vec3(greyScale(lighten));
    res = vec3(pow(res.x, 5.0)); 
    //res = clamp(res * 0.7 + 0.3 * res * res * 1.2, 0.0, 1.0);

   result = vec4(res,1.0) ;
   
   
   }

    
   // add bloom
    FragColor = result ;


}