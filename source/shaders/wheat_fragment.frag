R"(////////// SHADER BEGIN /////////
//  Shader.frag
//  Cornell University Game Library (CUGL)
//
//  This is a primitive SpriteBatch fragment shader for both OpenGL and OpenGL ES.
//  It supports textures which can be tinted per vertex. It supports gradients
//  but does not support scissors.  It is for testing purposes only.
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
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

// GRADIENT UNIFORM
// The gradient matrix
uniform mat3 gdMatrix;
// The gradient inner color
uniform vec4 gdInner;
// The gradient outer color
uniform vec4 gdOuter;
// The gradient extent
uniform vec2 gdExtent;
// The gradient radius
uniform float gdRadius;
// The gradient feather
uniform float gdFeathr;

// What to output
uniform int uType;

uniform float TIME;

// Textures
uniform sampler2D uTexture;
uniform sampler2D grass_tex;
uniform sampler2D cloud_tex;
uniform sampler2D noise_tex;
uniform sampler2D gradient_tex;
uniform sampler2D wheat_details_tex;

uniform float wind_speed;
uniform float cloud_speed;
uniform vec2 wind_direction;

uniform vec4 tip_color;
uniform vec4 wind_color;

uniform vec2 noise_tex_size;

uniform vec2 SCREEN_PIXEL_SIZE;

const float MAX_BLADE_LENGTH = 100.0f;
const float PI = 3.1415926535f;

/** Objects */
//uniform vec2 farmer_pos;
uniform vec2 cam_pos;
uniform vec2 cam_vel;
uniform vec2 positions[100];
uniform float velocities[100];
uniform int num_entities;

/**
 Calculates a sine wave
 
 - T: period
 - a: amplitude
 - phase: phase offset
 - dir: direction
 - pos: position to evaluate at
 */
float sineWave(float T, float a, float phase, vec2 dir, vec2 pos) {
    return a * sin(2.0f * PI / T * dot(dir, pos) + phase);
}

/**
 Retrieves the color from the gradient texture for the side of the wheat
 
 - dist: distance from base
 */
vec4 sampleColor(float dist, float bladeLen) {
//    if (dist/bladeLen > 0.6) {
//        return texture(gradient_tex, vec2(2.1f, 0.0f) / 3.0f);
//    }
//    else if (dist/bladeLen > 0.4) {
//        return texture(gradient_tex, vec2(1.1f, 0.0f) / 3.0f);
//    }
//    return texture(gradient_tex, vec2(0.2f, 0.0f) / 3.0f);
    return texture(gradient_tex, vec2(dist + 0.5f, 0.0f) / 3.0f);
}

/**
 Calculates blade length from r value of texture and scales to 255

 - uv: position to evaluate blade length at
 */
float sampleBladeLength(vec2 uv) {
    if (texture(grass_tex, uv).r > 0.0f) {
        return texture(grass_tex, uv).r * 255.0f + 2.0f;
    }
    return 0.0f;
}

/**
 Calculates wind
 
 - pos: position to evaluate at
 - t: time to evaluate at
 */
float wind(vec2 pos, float t) {
    return (sineWave(200.0f, 1.8f, 1.0f * wind_speed * t, normalize(wind_direction), pos)
           + sineWave(70.0f, 0.1f, 2.0f * wind_speed * t, normalize(wind_direction - vec2(0.0f, 0.4f)), pos)
           + sineWave(75.0f, 0.1f, 1.5f * wind_speed * t, normalize(wind_direction + vec2(0.4f, 0.0f)), pos))
           / 3.0f;
}

/**
 Calculates noise from using noise texture
 
 - uv: position to evaluate at
 - texture_pixel_size: size of the pixel to scale at
 - offset: offset sampling along x axis for jagged look
 */
float sampleNoise(vec2 uv, vec2 texture_pixel_size, float offset) {
    return texture(noise_tex, vec2(uv.x / texture_pixel_size.x / noise_tex_size.x + offset, 0.0f)).r;
}

/**
 * Performs the main fragment shading.
 */
void main(void) {
        
    // Convert fragCoord to UV
    vec2 uv = outTexCoord;
    
    vec2 cloud_uv = uv;
    
    cloud_uv += cloud_speed * normalize(wind_direction) * TIME;
    
    float noise = sampleNoise(uv, SCREEN_PIXEL_SIZE, 0.1f * TIME);

    vec2 fragUV = uv - vec2(0.0f, SCREEN_PIXEL_SIZE.y * noise);
    
    vec2 cloud_fragUV = cloud_uv - vec2(0.0f, SCREEN_PIXEL_SIZE.y * noise);
    
    // Color the base of the grass with the first gradient color
    vec4 baseColor;
    
    if (texture(grass_tex, fragUV).r > 0.0f) {
        baseColor = sampleColor(0.0f, 0.0f);
        baseColor -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0f);
    }
    else {
//        baseColor = vec4(0.5333, 0.7529, 0.5176, 1.0);
        //baseColor = vec4(0.819608, 0.819608, 0.219608, 1.0);
//        baseColor = vec4(0.0);
//        if (distance(fragUV, cam_pos) < 0.03) {
//            baseColor = vec4(0.6039216, 0.6039216, 0.196078, 1.0);
//        }
        baseColor -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0f);
    }

    // Sample the wind
    float windValue = wind(outTexCoord/SCREEN_PIXEL_SIZE, TIME);
    for (float dist = 0.0f; dist < MAX_BLADE_LENGTH; ++dist) {

        // Get the height of the blade originating at the current pixel
        // (0 means no blade)
        float bladeLength = sampleBladeLength(fragUV);

        if (bladeLength > 0.0f) {
            // push up entity positions
            bool empty = true;
            for (int i = 0; i < num_entities; i++ ){
                if (distance(fragUV, positions[i]) < 0.02) {
                    bladeLength += round(0.9*length(velocities[i]));
                    empty = false;
                }
            }

            // Blades are pressed down by the wind
            if (empty && windValue > 0.5f) {
                bladeLength -= 3.0f;
            }

            // Color basec on distance from root
            if (abs(dist - bladeLength) < 0.0001f) {
                // Color grass tips
                if (windValue <= 0.5f) {
                    baseColor = windValue > 0.49 ? sampleColor(dist, bladeLength) : tip_color;
                } else {
                    baseColor = vec4(1.0, 0.984314, 0.639216, 1.0);
                }
                // Add the cloud shadow
                baseColor -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0);
                break;
            } else if (dist < bladeLength) {
                // Color grass stems
                baseColor = sampleColor(dist, bladeLength);
                // Add the cloud shadow
                baseColor -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0);
            }
        }

        // Move on to the next pixel, down the blades
        fragUV += vec2(0.0, SCREEN_PIXEL_SIZE.y);

    }

    frag_color = baseColor;
    
}

/////////// SHADER END //////////)"