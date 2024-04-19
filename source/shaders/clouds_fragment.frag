R"(////////// SHADER BEGIN /////////
//  Shader.frag
//  Nicole Thean
//  Version: 2/10/20


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

// Gradient result from vertex shader
in vec2 outGradCoord;

// Textures
uniform sampler2D clouds_tex;
uniform sampler2D noise_tex;

uniform float cloud_alpha_scale;
uniform float WIND_TIME;
uniform float CLOUD_TIME;
uniform float cloud_speed;
uniform vec2 wind_direction;
uniform vec2 SCREEN_PIXEL_SIZE;
uniform float scale;

/**
 Calculates noise from using noise texture

 - uv: position to evaluate at
 - texture_pixel_size: size of the pixel to scale at
 - offset: offset sampling along x axis for jagged look
 */
float sampleNoise(vec2 uv, vec2 texture_pixel_size, float offset) {
    return texture(noise_tex, vec2(uv.x / texture_pixel_size.x + offset, 0.0f)).r;
}

/**
 * Performs the main fragment shading.
 */
void main(void) {
    float noise = sampleNoise(outTexCoord, SCREEN_PIXEL_SIZE*50.0, 0.2f * WIND_TIME);
    vec2 cloud_uv = outTexCoord/scale + cloud_speed * normalize(wind_direction) * CLOUD_TIME  - vec2(0.0f, SCREEN_PIXEL_SIZE.y * noise)/scale;
    frag_color.a = texture(clouds_tex, cloud_uv).r*cloud_alpha_scale;
    frag_color *= outColor;
}

///////// SHADER END //////////)"
