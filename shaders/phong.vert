#version 410

subroutine void shadeModelType(out vec3 position, out vec3 normal, out vec4 glVert);
subroutine uniform shadeModelType shadeModel;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

layout (location = 3) in vec4 VertexWeightVal;
layout (location = 4) in vec4 VertexWeightIds;

out vec3 Position;
out vec3 Normal;

uniform mat4 joints[31];

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

subroutine( shadeModelType )
void character(out vec3 position, out vec3 normal, out vec4 glVert) 
{
    vec3 pos=vec3(0);
    pos+=(VertexWeightVal.x*joints[int(VertexWeightIds.x)]*vec4(VertexPosition,1)).xyz;
    pos+=(VertexWeightVal.y*joints[int(VertexWeightIds.y)]*vec4(VertexPosition,1)).xyz;
    pos+=(VertexWeightVal.z*joints[int(VertexWeightIds.z)]*vec4(VertexPosition,1)).xyz;
    pos+=(VertexWeightVal.w*joints[int(VertexWeightIds.w)]*vec4(VertexPosition,1)).xyz;
    
    position = pos;
    
    vec3 norm=vec3(0);
    norm+=VertexWeightVal.x*mat3(joints[int(VertexWeightIds.x)])*VertexNormal;
    norm+=VertexWeightVal.y*mat3(joints[int(VertexWeightIds.y)])*VertexNormal;
    norm+=VertexWeightVal.z*mat3(joints[int(VertexWeightIds.z)])*VertexNormal;
    norm+=VertexWeightVal.w*mat3(joints[int(VertexWeightIds.w)])*VertexNormal;

    normal = normalize( NormalMatrix * norm);

    glVert = MVP * vec4(pos, 1.0);
}

subroutine( shadeModelType )
void mesh(out vec3 position, out vec3 normal, out vec4 glVert) {

    position = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
    normal = normalize( NormalMatrix * VertexNormal);

    glVert = MVP * vec4(VertexPosition, 1.0);
}

void main()
{
    shadeModel(Position, Normal, gl_Position);
}