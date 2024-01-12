#version 330 core
out vec4 FragColor;
in vec3	color;
in vec2 texCoord;

in vec3 Normal;
in vec3 crntPos;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;

uniform vec3 camPos;

uniform float diffuseFactor;
uniform float specFactor;
uniform vec3 colorMod;

vec4 pointLighting()
{
   vec3 lightvec = lightPos - crntPos;
   float dist = length(lightvec);
   float a=0.1;
   float b=0.7;
   float eq = 5.0f/(a*dist*dist+b*dist+1.0f);

   float ambient = 0.0f;

   vec3 normal = normalize(Normal);
   vec3 lightDirection = normalize(lightvec);
   float diffuse = diffuseFactor*max(dot(normal, lightDirection),0.0f)+(1.0f-diffuseFactor)*(1.0f);


   float specularLight = 0.7f;
   vec3 viewDirection = normalize(camPos - crntPos);
   vec3 reflectionDirection = reflect(-lightDirection,normal);
   float specAmount = pow(max(dot(viewDirection,reflectionDirection),0.0f),16);
   float specular = specFactor * specAmount * specularLight;

   return (texture(diffuse0, texCoord)*vec4(colorMod,1.0f)  * (diffuse*eq + ambient) + texture(specular0, texCoord).r * specular*eq)* lightColor;
}

vec4 parallelLighting()
{
// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
   float outerConeCos=0.9f;
   float innerConeCos=0.95f;

   vec3 lightvec = lightPos - crntPos;
   float dist = length(lightvec);
   float a=2;
   float b=0.7;
   float eq = 1.0f/(a*dist*dist+b*dist+1.0f);

   float ambient = 0.20f;

   vec3 normal = normalize(Normal);
   vec3 lightDirection = normalize(lightvec);
   float diffuse = max(dot(normal, lightDirection),0.0f);

   float specularLight = 0.3f;
   vec3 viewDirection = normalize(camPos - crntPos);
   vec3 reflectionDirection = reflect(-lightDirection,normal);
   float specAmount = pow(max(dot(viewDirection,reflectionDirection),0.0f),16);
   float specular = specAmount * specularLight;

   float angle = dot(vec3(0.0f,-0.0f,-1.0f),-lightDirection);
   float inten = clamp((angle-outerConeCos)/(innerConeCos-outerConeCos),0.0f,1.0f);

   return (texture(diffuse0, texCoord) * (diffuse*inten + ambient) + texture(specular0, texCoord).r * specular*inten) * lightColor;
}

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness , float offset)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}


void main()
{
   float depthFactor = logisticDepth(gl_FragCoord.z,0.5f,5.0f);
   FragColor =(1.0f-depthFactor)* pointLighting()+(vec4(vec3(depthFactor*1.0f),1.0f));
}