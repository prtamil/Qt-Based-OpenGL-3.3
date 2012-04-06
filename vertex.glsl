#version 330 core

#define POSITION 0
precision highp float;
precision highp int;

uniform mat4 MVP;

layout(location = POSITION) in vec2 Position;

void main()
{
    gl_Position = MVP * vec4(Position,0.25,1.0);


}