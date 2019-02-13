#version 410

subroutine void shadeModelType(out float fog, in float dist);
subroutine uniform shadeModelType fogFactor;

in vec3 Position;
in vec3 Normal;

struct LightInfo {
    vec4 position;
    vec3 intensity;
};
uniform LightInfo Light;

struct FogInfo {
  float maxDist;
  float minDist;
  vec3 color;
};
uniform FogInfo Fog;

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor

layout( location = 0 ) out vec4 FragColor;

vec3 ads( )
{
    vec3 s = normalize( Light.position.xyz - Position.xyz );
    vec3 v = normalize(vec3(-Position));
    vec3 h = normalize( v + s );

    vec3 ambient = Ka;
    vec3 diffuse = Kd * max(0.0, dot(s, Normal) );
    vec3 spec = Ks * pow( max( 0.0, dot( h, Normal) ), Shininess );

    return Light.intensity * (ambient + diffuse + spec);
}
subroutine( shadeModelType )
void character(out float fog, in float dist) {
    fog = (Fog.maxDist - dist) /
                      (Fog.maxDist - Fog.minDist);
}
subroutine( shadeModelType )
void mesh(out float fog, in float dist) {
    fog = (Fog.maxDist - dist) /
                      (Fog.maxDist - Fog.minDist);
}

void main() {
    float dist = abs( Position.z );
    float fog;
    fogFactor(fog, dist);
    
    fog = clamp( fog, 0.0, 1.0 );
    vec3 shadeColor = ads();
    vec3 color = mix( Fog.color, shadeColor, fog );

    FragColor = vec4(color, 1.0);
}