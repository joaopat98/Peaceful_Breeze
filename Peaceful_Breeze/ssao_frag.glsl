#version 330 core
out float fragOcclusion;
  
in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTex;

uniform vec3 samples[512];
uniform int numSamples;
uniform float sampleRadius;


uniform mat4 ProjectionMatrix;

uniform int width, height;

uniform float bias;

void main()
{
	vec2 noiseScale = vec2(width, height)/4.0;
	// get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoord).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoord).rgb);
    vec3 randomVec = normalize(texture(noiseTex, TexCoord * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < numSamples; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * sampleRadius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = ProjectionMatrix * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
		//only consider sample if within clip space, avoids edge artifacts and saves some calculations
		if (offset.x >= 0 && offset.x <= 1 && offset.y >= 0 && offset.y <= 1) {
	        // get sample depth		
			float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
			// range check & accumulate
			float rangeCheck = smoothstep(0.0, 1.0, sampleRadius / abs(fragPos.z - sampleDepth));
			occlusion += (sampleDepth + bias <= samplePos.z ? 1.0 : 0.0) * rangeCheck;     
		}
    }
    fragOcclusion = (occlusion / numSamples);
    //fragOcclusion = vec4(randomVec,1);
}