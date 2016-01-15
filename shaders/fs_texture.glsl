#version 330 core
precision highp float;
precision highp int;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float n;

uniform sampler2D texture;


out vec4 frag_Color;

varying vec3 position;
varying vec3 normal;
varying vec2 texcoord;
void main() {
	vec3 eye = vec3(0,0,0);
	vec3 light = vec3(0,5,1);

	vec3 vVector = normalize(eye - position);
	vec3 lVector = normalize(light - position);
	vec3 hVector = normalize(vVector + lVector);

	float diffFactor = max(dot(lVector, normal), 0.0);
	float specFactor = 0.0;
	if(diffFactor > 0.0)
		specFactor = max(dot(hVector, normal), 0.0);

	specFactor = pow(specFactor, n);

	vec3 texture_color = texture2D(texture, texcoord).rgb;

	vec3 color = (ka + kd*diffFactor + ks*specFactor)*texture_color;

	frag_Color = vec4(color, 1.0);
}