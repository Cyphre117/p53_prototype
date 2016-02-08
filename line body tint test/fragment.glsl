#version 410

uniform float brightness;
uniform sampler2D tex;

in vec3 fColour;
in vec2 fTexCoord;

out vec4 outColour;

void main()
{
    vec4 col = texture(tex, fTexCoord);
    if( col.r < 0.2f )
    {
        outColour = vec4(1.0, 1.0, 0.0, 1.0);
    }
    else
    {
        outColour = col * vec4(1.0, brightness, 1.0, 1.0);
    }
}
