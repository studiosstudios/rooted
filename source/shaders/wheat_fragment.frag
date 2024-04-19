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

// What to output
uniform int uType;

uniform float WIND_TIME;


// Textures
uniform sampler2D grass_tex;
uniform sampler2D noise_tex;
uniform sampler2D gradient_tex;
uniform sampler2D wheat_side_tex;
uniform sampler2D wheat_top_tex;

uniform float wind_speed;
uniform vec2 wind_direction;

uniform vec4 tip_color;
uniform vec4 wind_color;

uniform vec2 SCREEN_PIXEL_SIZE;
uniform float player_transparency;
uniform float transparency_radius;

uniform float MAX_BLADE_LENGTH;
const float PI = 3.14f;
uniform float STEP_SIZE;

uniform vec2 WHEAT_SIDE_SIZE;
uniform vec2 WHEAT_TOP_SIZE;

/** Objects */
uniform vec2 player_pos;
uniform vec2 positions[500];
uniform float velocities[500];
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
//vec4 sampleColor(float dist, float bladeLen) {
//    return texture(gradient_tex, vec2(dist + 0.5f, 0.0f) / 3.0f);
//}

/**
 Retrieves the color from the wheat side texture for the side of the wheat
 
 Converts the current coordinate to coordinate in wheat side texture
 
 - x: coordinate
 - dist: distance from base
 */
vec4 wheatSideColor(float x, float dist) {
    return texture(wheat_side_tex, vec2(x*5.0f, 576.0f - (dist * 0.04f)));
}

/**
 Retrieves the color from the wheat top texture for the top of the wheat
 
 Converts the current coordinate to coordinate in wheat top texture
*/
vec4 wheatTopColor(float x, float y) {
    return texture(wheat_top_tex, vec2(x, y) * 4.0f);
}

/**
 Calculates blade length from r value of texture and scales to 255

 - uv: position to evaluate blade length at
 */
float sampleBladeLength(vec2 uv) {
    vec3 samp = texture(grass_tex, uv).rgb;
//    return samp.r > 0.0f ? clamp((samp.r + samp.g - samp.b) * 255.0f + 10.0f, 0.0, MAX_BLADE_LENGTH) : 0.0f;
    return samp.r > 0.0f ? clamp((samp.r) * 255.0f + 10.0f, 0.0, MAX_BLADE_LENGTH) : 0.0f;
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
//    return texture(noise_tex, vec2(uv.x / texture_pixel_size.x + offset, 0.0f)).r;
    return texture(noise_tex, vec2(uv.x / texture_pixel_size.x + offset, uv.y / texture_pixel_size.y)).r;
}

/**
 * Performs the main fragment shading.
 */
void main(void) {
        
    // Convert fragCoord to UV
    vec2 uv = outTexCoord;
    
    float noise = sampleNoise(uv, SCREEN_PIXEL_SIZE*50.0f, 0.1f * WIND_TIME) * (1.0f+texture(grass_tex, uv + vec2(0,0.03f)).g*700.0f);

    vec2 fragUV = uv - vec2(0.0f, SCREEN_PIXEL_SIZE.y * noise);
    
    // Color the base of the grass with the first gradient color
    vec4 baseColor = vec4(0.0f);

    float dist = distance(fragUV / SCREEN_PIXEL_SIZE, player_pos / SCREEN_PIXEL_SIZE);
    float alpha = clamp(smoothstep(0.0f, transparency_radius, dist), player_transparency, 1.0f);
    
    if (texture(grass_tex, fragUV).r > 0.0f && texture(grass_tex, fragUV).g == 0.0f) {
//        baseColor = sampleColor(0.0f, 0.0f);
//        baseColor = wheatSideColor(
    }

    // Sample the wind
    float windValue = wind(outTexCoord/SCREEN_PIXEL_SIZE, WIND_TIME);
    for (float dist = 0.0f; dist <= MAX_BLADE_LENGTH; dist += STEP_SIZE) {

        // Get the height of the blade originating at the current pixel
        // (0 means no blade)
        float bladeLength = sampleBladeLength(fragUV);

        if (bladeLength > 0.0f) {
            // Blades are pressed down by the wind
            if (windValue > 0.5f) {
                bladeLength -= 3.0f;
            }

            // Color basec on distance from root
            if (abs(dist - bladeLength) < 0.0001f) {
                // Color grass tips
                if (windValue <= 0.5f) {
//                    baseColor = tip_color;
                    baseColor = wheatTopColor(fragUV.x, fragUV.y);
                    // a little shading on top of noise
//                    if (texture(grass_tex, uv+vec2(0.0f, 0.03f)).g > 0) {
//                        baseColor -= texture(grass_tex, uv+vec2(0.0f, 0.03f)).g*vec4(2.0f, 2.01f, 2.01f, 0.0f);
//                    }
//                    baseColor = windValue > 0.49 ? sampleColor(dist, bladeLength) : tip_color;
                } else {
//                    baseColor = wind_color;
                    baseColor = wheatTopColor(fragUV.x, fragUV.y) + vec4(0.06, 0.06, 0.06, 0.0);
                }
                break;
            } else if (dist < bladeLength) {
                // Color grass stems
//                baseColor = sampleColor(dist, bladeLength);
                baseColor = wheatSideColor(fragUV.x, dist);
            }
            
//            if (texture(grass_tex, uv+vec2(0.0f, bladeLength-0.01f)).g > 0 && texture(grass_tex, uv+vec2(0.0f, bladeLength-0.01f)).r > 0) {
//    //            baseColor -= vec4(0.01, 0.01, 0.01, 0.0);
//                baseColor -= vec4(100.0, 100.0, 100.0, 0.0);
//            }
        }
        
//        vec3 samp = texture(grass_tex, uv).rgb;
//        float new_height =  samp.r > 0.0f ? clamp((samp.r + samp.g - samp.b) * 255.0f + 10.0f, 0.0, MAX_BLADE_LENGTH) : 0.0f;
        

        // Move on to the next pixel, down the blades
        fragUV += vec2(0.0, SCREEN_PIXEL_SIZE.y) * STEP_SIZE;

    }

    if (baseColor.rgb == vec3(0.0)) {
        frag_color = vec4(0.0);
    } else {
        frag_color = baseColor * outColor;
        frag_color.a = alpha;
    }

}

/////////// SHADER END //////////)"
