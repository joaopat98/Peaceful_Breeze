#version 330 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPos;

in vec2 TexCoords;

uniform vec3 Color;
uniform vec3 LightColor;
uniform vec3 LightPos;
uniform vec3 LightDir;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform mat4 LightSpaceMatrix;

out vec4 out_Color;


const float shineDamper = 25.0; // Water
const float reflectivity = 0.6; // Water
const float levels = 4.0;

const float ambientStrength = 0.2f;

const float rimAmount = 0.75f;

uniform bool useTexture;

uniform sampler2D Texture;
uniform sampler2D shadowMap;
uniform sampler2D occlusion;

uniform float minBias;
uniform float maxBias;

uniform bool useOcclusion;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir )
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow


	// add bias to shadow;
	float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias);  
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

	shadow = 0.0;
	// basic pcf with 25 samples;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 25.0;

	if(projCoords.z > 1.0) shadow = 0.0;

    return shadow;
}  

void main(void)
{
	vec4 FragViewPos = ProjectionMatrix * ViewMatrix * vec4(FragPos, 1);
	vec4 fragPosLightSpace = LightSpaceMatrix * vec4(FragPos, 1.0);
	vec3 toCameraVector = vec3(cameraPos) - FragPos;

	vec3 rimColor = LightColor;
	vec3 objColor;
	float alpha = 1;
	if (useTexture) {
		vec4 col = texture(Texture, TexCoords);
		objColor = col.rgb;
		alpha = col.a;
		if (alpha < 0.5)
			discard;
	}
	else {
		objColor = Color;
	}

	vec3 viewDir = normalize(toCameraVector);

	vec3 ambient = ambientStrength * objColor * LightColor;

	//Calculate Diffuse lighting with cel shading
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	lightDir = LightDir;
	float diff = max(dot(norm, lightDir), 0.0);
	float level = floor(diff * levels);
	float cel_diff = level / levels;

	cel_diff = dot(norm, lightDir) > 0 ? 1 : 0;

	cel_diff = smoothstep(0, 0.01, dot(norm, lightDir));

	vec3 diffuse = mix(diff,cel_diff, 0.4f )  * objColor  * LightColor;

	//Calculate Specular lighting with cel shading
	vec3 hVector = normalize(viewDir + normalize(lightDir));

	//vec3 reflectedLight = reflect(hVector, norm);
	//float specular = max(dot(reflectedLight, normalize(toCameraVector)), 0.0);

	float specular = max(dot(hVector, norm), 0.0);
	specular = pow(specular * cel_diff, shineDamper * shineDamper);

	//level = floor(specular * levels);
	//specular = level / levels;

	float specularIntensitySmooth = smoothstep(0.005, 0.01, specular);
	vec3 specularHighlight = objColor * LightColor * specularIntensitySmooth * reflectivity;

	//Calculate outer rim
	float rimDot = 1 - dot(viewDir, norm);
	float rimIntensity = max(rimDot * pow(dot(norm, lightDir), 0.3), 0);
	rimIntensity = smoothstep(rimAmount - 0.01, rimAmount + 0.01, rimIntensity);
	vec3 rim = rimIntensity * mix(objColor*LightColor, rimColor, 0.2f);

	float shadow = ShadowCalculation(fragPosLightSpace, norm, lightDir ); 
	//shadow = 0;
	vec2 fragCoord = FragViewPos.xy / FragViewPos.w * 0.5 + 0.5;
	float occlusionVal = useOcclusion ? texture(occlusion, fragCoord).r : 0;
    vec3 lighting = ((1-occlusionVal) * ambient + (1.0 - shadow) * (diffuse + specularHighlight + rim)) ;
	out_Color = vec4(lighting, alpha);
	//out_Color *= 0.0001;
	//out_Color += vec4(vec3(occlusionVal), 1);
	//out_Color += vec4(TexCoords.x,TexCoords.y, 0,1) * 0.0000001;
	//out_Color += vec4(vec3(fragPosLightSpace), 1);
}