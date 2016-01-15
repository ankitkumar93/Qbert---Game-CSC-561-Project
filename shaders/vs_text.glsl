#version 330 core

// input vertex data, different for all executions of this shader.
layout( location = 0 ) in vec2 vertexPosition;
layout( location = 1 ) in vec2 vertexTexture;

varying vec2 texcoord;
void main() {
    gl_Position = vec4(vertexPosition, -0.9, 1.0);
	texcoord = vec2(vertexTexture.x, 1 - vertexTexture.y);
}