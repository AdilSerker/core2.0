#version 410

// subroutine void shadeModelType(out vec3 position, out vec3 normal);
// subroutine uniform shadeModelType shadeModel;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// layout (location = 3) in vec4 VertexWeightVal;
// layout (location = 4) in vec4 VertexWeightIds;

out vec3 Position;
out vec3 Normal;

// uniform mat4 joints[31];

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

// subroutine( shadeModelType )
// void character(out vec3 position, out vec3 normal) 
// {
//     vec4 pos=vec4(0);
//     pos+=VertexWeightVal.x*joints[int(VertexWeightIds.x)]*vec4(VertexPosition,1);
//     pos+=VertexWeightVal.y*joints[int(VertexWeightIds.y)]*vec4(VertexPosition,1);
//     pos+=VertexWeightVal.z*joints[int(VertexWeightIds.z)]*vec4(VertexPosition,1);
//     pos+=VertexWeightVal.w*joints[int(VertexWeightIds.w)]*vec4(VertexPosition,1);
//     position=pos.xyz/pos.w;
    
//     vec3 norm=vec3(0);
//     norm+=VertexWeightVal.x*mat3(joints[int(VertexWeightIds.x)])*VertexNormal;
//     norm+=VertexWeightVal.y*mat3(joints[int(VertexWeightIds.y)])*VertexNormal;
//     norm+=VertexWeightVal.z*mat3(joints[int(VertexWeightIds.z)])*VertexNormal;
//     norm+=VertexWeightVal.w*mat3(joints[int(VertexWeightIds.w)])*VertexNormal;
//     normal = norm;
// }

// subroutine( shadeModelType )
void mesh(out vec3 position, out vec3 normal) {
    position = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
    normal = normalize( NormalMatrix * VertexNormal);
}

void main()
{
    // mesh(Normal, Position);
    Position = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
    Normal = normalize( NormalMatrix * VertexNormal);

    gl_Position = MVP * vec4(Position, 1.0);
}