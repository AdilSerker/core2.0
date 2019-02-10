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
    vec4 pos=vec4(0);
    pos+=VertexWeightVal.x*joints[int(VertexWeightIds.x)]*vec4(VertexPosition,1);
    pos+=VertexWeightVal.y*joints[int(VertexWeightIds.y)]*vec4(VertexPosition,1);
    pos+=VertexWeightVal.z*joints[int(VertexWeightIds.z)]*vec4(VertexPosition,1);
    pos+=VertexWeightVal.w*joints[int(VertexWeightIds.w)]*vec4(VertexPosition,1);
    position=pos.xyz/pos.w;
    // position = vec3( ModelViewMatrix * vec4(pos.xyz/pos.w,1.0) );
    
    vec3 norm=vec3(0);
    norm+=VertexWeightVal.x*mat3(joints[int(VertexWeightIds.x)])*VertexNormal;
    norm+=VertexWeightVal.y*mat3(joints[int(VertexWeightIds.y)])*VertexNormal;
    norm+=VertexWeightVal.z*mat3(joints[int(VertexWeightIds.z)])*VertexNormal;
    norm+=VertexWeightVal.w*mat3(joints[int(VertexWeightIds.w)])*VertexNormal;
    // normal = norm;
    normal = normalize( NormalMatrix * norm);

    glVert = MVP * vec4(position, 1.0);
}

subroutine( shadeModelType )
void mesh(out vec3 position, out vec3 normal, out vec4 glVert) {
    position = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
    normal = normalize( NormalMatrix * VertexNormal);

    glVert = MVP * vec4(VertexPosition, 1.0);
}

void main()
{
    vec3 pos;
    vec3 norm;
    vec4 vert;
    shadeModel(pos, norm, vert);

    Position = pos;
    Normal = norm;

    gl_Position = vert;
}