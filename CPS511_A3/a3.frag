in vec2 uv;
in vec4 Normal;
in vec4 FragPos;

out vec4 FragColor;

struct Light {
    vec4 position;       // Light position in world space
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

uniform Light light;    // Light properties
uniform vec4 viewPos;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform Material material;

layout (binding = 0) uniform sampler2D  textureColor;
layout (binding = 1) uniform sampler2D textureOcclusion;
layout (binding = 2) uniform sampler2D textureRoughness;

void main() {


    
    vec4 N = normalize(vec4(Normal.x, Normal.y, Normal.z, Normal.w));
    vec4 L = -normalize(light.position - FragPos);
    float NdotL = max(dot(N, L), 0.0);
    vec4 Diffuse = vec4(1.0, 1.0, 1.0, 1.0) * NdotL * (texture(textureColor, uv));

    float roughness = texture(textureRoughness, uv).r;
    float shininess = 1.0 - roughness;

    // Compute the specular term.
    vec4 V = normalize(viewPos - FragPos);
    vec4 H = normalize(L + V);
    float RdotV = max(dot(reflect(-L, N), V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    vec4 Specular = pow(RdotV, shininess) * vec4(1.0, 1.0, 1.0, 1.0) * material.specular;

    // Add the ambient term separately
    vec4 Ambient = (texture(textureOcclusion, uv) + material.ambient) * light.ambient;

    // Final lighting calculation (sum of ambient, diffuse, and specular)
    vec4 Lighting = Ambient + Diffuse + Specular;
    FragColor = Lighting;
  
}