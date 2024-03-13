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
const float PI = 3.1415926535;

/** Objects */
//uniform vec2 farmer_pos;
uniform vec2 cam_pos;
uniform vec2 cam_vel;

/**
 Calculates a sine wave
 
 - T: period
 - a: amplitude
 - phase: phase offset
 - dir: direction
 - pos: position to evaluate at
 */
float sineWave(float T, float a, float phase, vec2 dir, vec2 pos) {
    return a * sin(2.0 * PI / T * dot(dir, pos) + phase);
}

/**
 Retrieves the color from the gradient texture for the side of the wheat
 
 - dist: distance from base
 */
vec4 sampleColor(float dist) {
    return texture(gradient_tex, vec2(dist + 0.5f, 0.0f) / 3.0f);
}

/**
 Calculates blade length from r value of texture and scales to 255

 - uv: position to evaluate blade length at
 */
float sampleBladeLength(vec2 uv) {
    if (texture(grass_tex, uv).r > 0) {
        return texture(grass_tex, uv).r * 255.0 + 2.0;
    }
    return 0.0;
}

/**
 Calculates wind
 
 - pos: position to evaluate at
 - t: time to evaluate at
 */
float wind(vec2 pos, float t) {
    return (sineWave(200.0, 1.8, 1.0 * wind_speed * t, normalize(wind_direction), pos)
           + sineWave(70.0, 0.1, 2.0 * wind_speed * t, normalize(wind_direction - vec2(0.0, 0.4)), pos)
           + sineWave(75.0, 0.1, 1.5 * wind_speed * t, normalize(wind_direction + vec2(0.4, 0.0)), pos))
           / 3.0;
}

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
    
    // Color the base of the grass with the first gradient color
    vec4 baseColor;
    
    if (texture(grass_tex, fragUV).r > 0.0) {
        baseColor = sampleColor(0.0);
        baseColor -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0);
    }
    else {
        baseColor = vec4(0.5333, 0.7529, 0.5176, 1.0);
        baseColor -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0);
    }

    for (float dist = 0.0; dist < MAX_BLADE_LENGTH; ++dist) {
        // Sample the wind
        float windValue = wind(outTexCoord/SCREEN_PIXEL_SIZE, TIME);
        // Get the height of the blade originating at the current pixel
        // (0 means no blade)
        float bladeLength = sampleBladeLength(fragUV);

        if (bladeLength > 0.0) {
            if (distance(fragUV, cam_pos) < 0.03) {
//                float rustle_noise = sampleNoise(vec2(round(100*distance(fragUV, cam_pos))/300.0, 0.0), SCREEN_PIXEL_SIZE, 0.4 * TIME);
                bladeLength += round(1.0*length(cam_vel));
//                bladeLength -= round((rustle_noise - 2.0) * 1.0);
            }

            // Blades are pressed down by the wind
            if (windValue > 0.5) {
                bladeLength -= 3.0;
            }

            // Color basec on distance from root
            if (abs(dist - bladeLength) < 0.0000001) {
                // Color grass tips
                if (windValue <= 0.5) {
                    baseColor = tip_color;
                } else {
                    baseColor = wind_color;
                }
                // Add the cloud shadow
                baseColor -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0);
                break;
            } else if (dist < bladeLength) {
                // Color grass stems
                baseColor = sampleColor(dist);
                // Add the cloud shadow
                baseColor -= vec4(texture(cloud_tex, cloud_fragUV).rgb, 0.0);
            }
        }

        // Move on to the next pixel, down the blades
        fragUV += vec2(0.0, SCREEN_PIXEL_SIZE.y);

    }
    
    // Shade player positions


    frag_color = baseColor;
    
}

/////////// SHADER END //////////)"
