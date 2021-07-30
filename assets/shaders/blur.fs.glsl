#version 330 core
layout (location = 2) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D finalImage;

void main()
{             
   // FragColor = vec4(texture(finalImage, TexCoords).rgb+texture(finalImage2, TexCoords).rgb, 1.0);
   vec4 tex = texture(finalImage, TexCoords);
   vec4 result = tex;
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
   

   FragColor = result;
}