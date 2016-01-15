#version 330 core
precision highp float;
precision highp int;

uniform sampler2D texture;
uniform vec3 color;

out vec4 frag_Color;

varying vec2 texcoord;
void main() {
	frag_Color = vec4(texture2D(texture, texcoord).rgb * color, 1.0);
}