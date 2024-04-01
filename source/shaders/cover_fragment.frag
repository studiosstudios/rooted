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

uniform vec2 SCREEN_PIXEL_SIZE;
uniform vec2 SCREEN_SIZE;

uniform vec2 camera_pos;
uniform float camera_zoom;

// The output color
out vec4 frag_color;

// The inputs from the vertex shader
in vec2 outPosition;
in vec4 outColor;
in vec2 outTexCoord;
in vec2 outGradCoord;

void main()
{
	vec2 wheatCoord = vec2((gl_FragCoord.x/camera_zoom/2.0+camera_pos.x)/SCREEN_SIZE.x,
	    1.0-(gl_FragCoord.y/camera_zoom/2.0+camera_pos.y)/SCREEN_SIZE.y); //do not know why it is divided by two

    frag_color = vec4(mix(texture(uTexture, outTexCoord), texture(grass_tex, wheatCoord), 0.8).rgb, 1.0);
}
/////////// SHADER END //////////)"
