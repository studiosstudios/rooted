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

in vec2 outUV;
uniform sampler2D uTexture;
uniform float alpha;
uniform float scale;

out vec4 frag_color;

void main() 
{
	frag_color = texture(uTexture, vec2(outUV.x, 1.0-outUV.y));
	frag_color.a = frag_color.a * alpha;
	frag_color.rgb *= scale;
}
/////////// SHADER END //////////)"
