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

uniform float WIND_TIME;

// Textures
uniform sampler2D uTexture;
uniform sampler2D grass_tex;
uniform sampler2D noise_tex;

uniform vec2 wind_direction;
uniform float radius;
uniform vec2 SCREEN_PIXEL_SIZE;

/** Objects */
uniform vec2 positions[400];
uniform int num_entities;

/**
 Calculates noise from using noise texture
 
 - uv: position to evaluate at
 - texture_pixel_size: size of the pixel to scale at
 - offset: offset sampling along x axis for jagged look
 */
float sampleNoise(vec2 uv, vec2 texture_pixel_size, float offset) {
    return texture(noise_tex, vec2(uv.x / texture_pixel_size.x + offset, 0.0)).r;
}

/**
 * Performs the main fragment shading.
 */
void main(void) {
        
    // Convert fragCoord to UV
    vec2 uv = outTexCoord;
    
    float noise = sampleNoise(uv, SCREEN_PIXEL_SIZE * 50.0, 0.1 * WIND_TIME);

    vec2 fragUV = uv - vec2(0.0, SCREEN_PIXEL_SIZE.y * noise);

    frag_color = vec4(0.0);

    for (int i = 0; i < num_entities; i++ ){
        if (distance(fragUV, positions[i]) < radius) {
            frag_color = vec4(0.0, 0.0, 0.0, 0.2);
            break;
        }
    }
    frag_color *= outColor;
}

/////////// SHADER END //////////)"
