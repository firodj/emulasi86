#version 330

uniform mat4 projectionMatrix;

uniform vec3 clipScale;
uniform vec3 clipOffset;

in vec4 positionIn;
in vec4 diffuseIn;
in vec4 specularIn;
in vec2 uv0In;

out vec4 diffuse;
out vec4 specular;
out vec2 uv0;

void main() {
  gl_Position = vec4(positionIn.xyz * clipScale + clipOffset, 1.0);
  gl_Position /= positionIn.w;
  gl_Position.y = -gl_Position.y;
  diffuse = diffuseIn.bgra;
  specular = specularIn.bgra;
  uv0 = uv0In;
}
