R"(////////// SHADER BEGIN /////////
//  FSQShader.frag
//  Cornell University Game Library (CUGL)
//
//  This shader renders a full screen quad. The vertex shader has hardcoded
//  positions (so that no buffer is needed), and the fragment shader has a
//  single uniform for the texture.
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
//  Author: Zachary Schecter
//  Version: 4/12/23

#ifdef CUGLES
// This one line is all the difference
precision highp float;  // highp required for gradient precision
#endif

// The texture for sampling
uniform sampler2D uTexture;
uniform sampler2D grass_tex;
uniform sampler2D noise_tex;

uniform vec2 TEXTURE_PIXEL_SIZE;
uniform vec2 SCENE_SIZE;
uniform float WIND_TIME;

uniform float wind_speed;
uniform vec2 wind_direction;

// The output color
out vec4 frag_color;

// The inputs from the vertex shader
in vec2 outPosition;
in vec4 outColor;
in vec2 outTexCoord;
in vec2 outGradCoord;

uniform float MAX_WHEAT_HEIGHT;
const float PI = 3.14f;

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

float sampleHeight(vec2 uv) {
    vec3 samp = texture(grass_tex, uv).rgb;
    return samp.r > 0.0f ? clamp((samp.r + samp.g - samp.b) * 255.0f + 10.0f, 0.0, MAX_WHEAT_HEIGHT) : 0.0f;
}

/**
 * gets coordinate in wheat texture given coordinate in scene space
 */
vec2 getWheatCoord(vec2 uv) {
    return vec2((uv.x)/SCENE_SIZE.x,
           	    1.0-(uv.y)/SCENE_SIZE.y);
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
    return texture(noise_tex, vec2(uv.x / texture_pixel_size.x + offset, 0.0f)).r;
}

void main()
{

	vec2 wheatCoord = getWheatCoord(outPosition);

    frag_color = texture(uTexture, outTexCoord);
    float windValue = wind(wheatCoord/TEXTURE_PIXEL_SIZE, WIND_TIME);
    float noise = sampleNoise(wheatCoord, TEXTURE_PIXEL_SIZE*50.0, 0.1f * WIND_TIME);

    //note that this assume that all textures are 32px tall (not accounting for camera zoom)
    //we cant pass this in as a uniform per texture since spritebatch draws in one call
    //to fix this we will have to modify spritebatch
    float height = (1.0 - outTexCoord.y) * 32.0 / SCENE_SIZE.y / TEXTURE_PIXEL_SIZE.y;

    vec2 wheatUV = wheatCoord + vec2(0, 1.0 - outTexCoord.y) * 32.0 / SCENE_SIZE.y;

    for (float dist = 0.0f; dist <= MAX_WHEAT_HEIGHT; ++dist) {
        //sample wheat height
        float wheat_height = sampleHeight(wheatUV);

        wheat_height -= noise;

        if (windValue > 0.5) {
            wheat_height -= 3.0f + outGradCoord.x * 0.00001; //this stupid little thing is to suppress warnings
        }

        if (height + dist <= wheat_height) {
            frag_color = vec4(0.0);
            break;
        }

        wheatUV += vec2(0.0, TEXTURE_PIXEL_SIZE.y);
    }

    frag_color *= outColor;

}
/////////// SHADER END //////////)"
