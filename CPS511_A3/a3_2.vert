layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoord;

out vec2 uv;
out vec4 Normal;
out vec4 FragPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 projection;
uniform float tuneDisplacement;

layout (binding = 3) uniform sampler2D  textureDisplacement;
layout (binding = 4) uniform sampler2D textureNormal;

void main() {

	// found online
	//vec3 normalFromMap = texture(textureNormal, textureCoord).xyz * 2.0 - 1.0;

	// Transform the normal from the model space to the world space
 //  vec3 normalWorld = normalize(mat3(transpose(inverse(Model))) * normalFromMap);
  // vec4 normalWorld = Model * vec4(normalFromMap, 0);

   //Normal = normalize(Model * vec4(normal, 0) + normalWorld);

	//Normal = vec4(normalize(mat3(transpose(inverse(Model))) * normal + normalWorld), 0);


//	//float displacement = texture(textureDisplacement, textureCoord).r;
	//vec3 displaced = position + normalize(position) * ((displacement / tuneDisplacement));
	uv = textureCoord;
	Normal = Model * vec4(normal, 0);//normalize(modelView * vec4(normal, 0.0)).xyz;
	gl_Position = projection * View * Model * vec4(position, 1.0);
	FragPos = Model  * vec4(position, 1);
	
}