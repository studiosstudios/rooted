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

// What to output
uniform int uType;

// Textures
uniform sampler2D uTexture;
uniform sampler2D grass_tex;
uniform sampler2D sprite_tex;

uniform vec2 SCREEN_PIXEL_SIZE;

const float MAX_BLADE_LENGTH = 8.0f;
const float PI = 3.14f;



/**
 Calculates blade length from r value of texture and scales to 255

 - uv: position to evaluate blade length at
 */
float sampleBladeLength(vec2 uv) {
    float r = texture(grass_tex, uv).r;
    return r > 0.0f ? r * 255.0f + 0.0f : 0.0f;
}


/**
 * Performs the main fragment shading.
 */
void main(void) {

    // Convert fragCoord to UV
    vec2 uv = outTexCoord;

//    float noise = sampleNoise(uv, SCREEN_PIXEL_SIZE, 0.1f * WIND_TIME);

    vec2 fragUV = uv - vec2(0.0f, SCREEN_PIXEL_SIZE.y);
    
//    float bladeLength = sampleBladeLength(fragUV);
    
//    if (bladeLength > 0.0f) {
//        frag_color = vec4(0.0, 0.0, 0.0, 0.0);
//    }
//    
    if (texture(grass_tex, uv).r > 0.0) {
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
//    else {
//        frag_color = texture(sprite_tex, uv);
//    }

//    for (float dist = 0.0f; dist < MAX_BLADE_LENGTH; ++dist) {
//        // Get the height of the blade originating at the current pixel
//        // (0 means no blade)
//        float bladeLength = sampleBladeLength(fragUV);
//
//        if (bladeLength > 0.0f) {
//            // push up entity positions
//            bool empty = true;
//            for (int i = 0; i < num_entities; i++ ){
//                if (distance(fragUV, positions[i]) < 0.008) {
//                    bladeLength = 0;
//                    empty = false;
//                }
//            }
//
//
//            // Color basec on distance from root
//            if (abs(dist - bladeLength) < 0.001f) {
//                baseColor = tip_color;
//                break;
//            } else if (dist < bladeLength) {
//                // Color grass stems
//                baseColor = sampleColor(dist, bladeLength);
//            }
//        }
//
//        // Move on to the next pixel, down the blades
//        fragUV += vec2(0.0, SCREEN_PIXEL_SIZE.y);
//    }

//    frag_color = baseColor;

}

///////// SHADER END //////////)"
