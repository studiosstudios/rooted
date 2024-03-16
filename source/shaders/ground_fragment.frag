R"(////////// SHADER BEGIN /////////
//  wheat_fragment.frag
//  
//
//  Author: Nicole Thean
//  Version: 3/14/24.


#ifdef CUGLES
// This one line is all the difference
precision mediump float;
#endif

// The output color
out vec4 frag_color;

// Color result from vertex shader
in vec4 outColor;

// Texture result from vertex shader
in vec2 outTexCoord;

uniform float TIME;

// Textures
uniform sampler2D uTexture;
uniform sampler2D grass_tex;
uniform sampler2D cloud_tex;
uniform sampler2D noise_tex;

uniform float cloud_speed;
uniform vec2 wind_direction;

uniform vec2 noise_tex_size;

uniform vec2 SCREEN_PIXEL_SIZE;

/** Objects */
uniform vec2 cam_pos;
uniform vec2 cam_vel;

uniform vec2 positions[100];
uniform int num_entities;

/**
 Calculates noise from using noise texture
 
 - uv: position to evaluate at
 - texture_pixel_size: size of the pixel to scale at
 - offset: offset sampling along x axis for jagged look
 */
float sampleNoise(vec2 uv, vec2 texture_pixel_size, float offset) {
    return texture(noise_tex, vec2(uv.x / texture_pixel_size.x / noise_tex_size.x + offset, 0.0)).r;
}

/**
 * Performs the main fragment shading.
 */
void main(void) {
        
    // Convert fragCoord to UV
    vec2 uv = outTexCoord;
    
    vec2 cloud_uv = uv;
    
    cloud_uv += cloud_speed * normalize(wind_direction) * TIME;
    
    float noise = sampleNoise(uv, SCREEN_PIXEL_SIZE, 0.1 * TIME);

    vec2 fragUV = uv - vec2(0.0, SCREEN_PIXEL_SIZE.y * noise);
    
    vec2 cloud_fragUV = cloud_uv - vec2(0.0, SCREEN_PIXEL_SIZE.y * noise);
    
    frag_color = vec4(0.819608, 0.819608, 0.219608, 1.0);

    for (int i = 0; i < num_entities; i++ ){
        if (distance(fragUV, positions[i]) < 0.012) {
            frag_color = vec4(0.6039216, 0.6039216, 0.196078, 1.0);
        }
    }
    
    if (texture(grass_tex, fragUV).r == 0.0) {
        // TODO: see if can factor out this cloud thing
        frag_color -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0);
    }
    
}

/////////// SHADER END //////////)"
