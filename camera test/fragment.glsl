#version 410
uniform vec4 tint;
uniform vec4 line;
uniform sampler2D tex;
in vec3 fColour;
in vec2 fTexCoord;
out vec4 outColour;
void main()
{
   vec4 texCol = texture(tex, fTexCoord);
   if( texCol.r < 0.5 ) {
       outColour = line;
   } else {
       outColour = texture(tex, fTexCoord) * tint;
   }
}
