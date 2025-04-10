#version 330

bool tex0HasAlpha = true;

uniform int tex0Blend;
uniform sampler2D tex0;
uniform bool alphaTest;
uniform int fogMode;
uniform float fogStart;
uniform float fogEnd;
uniform vec3 fogColor;

in vec4 diffuse;
in vec4 specular;
in vec2 uv0;

out vec4 color;

void main() {
  vec4 tex = texture(tex0, uv0);
  vec4 src = diffuse;
  if (tex0Blend == 2) { // D3DTBLEND_MODULATE
    color.rgb = src.rgb * tex.rgb;
    if (tex0HasAlpha) {
      color.a = tex.a;
    } else {
      color.a = src.a;
    }
  } else if (tex0Blend == 4) { // D3DTBLEND_MODULATEALPHA
    color.rgb = src.rgb * tex.rgb;
    color.a = src.a * tex.a;
  } else { // Unknown blend mode; Signals error by choosing a pink color
    color = vec4(1.0, 0.0, 1.0, 0.5);
  }
  if (alphaTest && !(int(round(color.a * 255.0)) != 0)) { discard; }
  if (fogMode == 0) { // D3DFOG_NONE
    color.rgb = color.rgb;
  } else if (fogMode == 3) { // D3DFOG_LINEAR
    float fogVisibility = (fogEnd - gl_FragCoord.z / gl_FragCoord.w) / (fogEnd - fogStart);
    fogVisibility = clamp(fogVisibility, 0.0, 1.0);
    color.rgb = mix(fogColor, color.rgb, fogVisibility);
  } else { // Unknown fog mode; Signal error by coloring primitive green
    color.rgb = vec3(0.0, 1.0, 0.0);
  }
}
