R"(////////// SHADER BEGIN /////////
//  ground_vertex.vert
//
//  Author: Nicole Thean
//  Version: 3/14/24.

// Positions
in vec4 aPosition;

// Colors
in  vec4 aColor;
out vec4 outColor;

// Texture Coordinates
in  vec2 aTexCoord;
out vec2 outTexCoord;

// Gradient Coordinates
in  vec2 aGradCoord;
out vec2 outGradCoord;

// Matrices
uniform mat4 uPerspective;

// Transform and pass through
void main(void) {
    gl_Position = uPerspective*aPosition;
    outColor = aColor;
    outTexCoord = aTexCoord;
    outGradCoord = aGradCoord;
}

/////////// SHADER END //////////)"
