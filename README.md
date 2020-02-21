### Use GBuffer --把光照和场景渲染分开

时间复杂度减少为：O(window resolution * lights num)

#### 1.先把整个场景写入gBuffer的多个attachment(position,normal,diffuseColor,specularColor),再使用gBufer中的attachment计算光照

GBufferRenderer.cpp

```
	void GBufferRenderer::Render(std::vector<Object*> objects,Object * gBufferDebugQuad)
	{
		GE_ASSERT(m_IsInitialized,"GBufferRenderer have not been initialized! ")
		
		// 1. Geometry Pass: render scene's geometry/color data into gbuffer
		m_GBffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		m_GBufferShader->Bind();
		DrawObjects(objects,m_GBufferShader);

		m_GBffer->UnBind();

		//	DrawGBuffer(gBufferDebugQuad);

		// 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_LightingShader->Bind();
		m_LightingShader->SetUniform1i("gPosition", 0);
		m_LightingShader->SetUniform1i("gNormal", 1);
		m_LightingShader->SetUniform1i("gAlbedoSpec", 2);
		m_LightingShader->SetUniform1i("gSpecular", 3);

		glActiveTexture(GL_TEXTURE0);
		m_GBffer->GetPositionTexture()->Bind();
		glActiveTexture(GL_TEXTURE1);
		m_GBffer->GetNormalTexture()->Bind();
		glActiveTexture(GL_TEXTURE2);
		m_GBffer->GetAlbedoTexture()->Bind();
		glActiveTexture(GL_TEXTURE3);
		m_GBffer->GetSpecularTexture()->Bind();

		DrawObject(m_ScreenQuad, m_LightingShader);


	}
	
```
#### 2.Shader

gBuffer.glsl shader: write scene to gBuffer

gBufferLighting.glsl shader: draw screen Quad(calculate lights) according to gBuffer data


gBuffer.glsl 

```
#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;


void main(){

	gl_Position = u_ProjectionView*u_Model*vec4(aPos,1.0);
	
	
	v_TexCoord = aTexCoords;
	v_Normal   = mat3(u_TranInverseModel)*aNormal;
	v_FragPos  = vec3(u_Model*vec4(aPos,1.0));




}




#type fragment
#version 430 core

/* render to gBuffer */
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gSpecular;

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

uniform struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;
	
	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color

}u_Material;

void main(){
    // Store the fragment position vector in the first gbuffer texture
	gPosition       = v_FragPos;
	// Also store the per-fragment normals into the gbuffer
	gNormal         = v_Normal;

	gAlbedoSpec.rgb = (u_Material.diffuseColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.diffuse,v_TexCoord).rgb*u_Material.isTextureSample);


	gSpecular.rgb = (u_Material.specularColor *(1-u_Material.isTextureSample)
					+ texture(u_Material.specular,v_TexCoord).rgb*u_Material.isTextureSample);


	
}

```

gBufferLighting.glsl 

```
#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 v_TexCoords;


void main()
{
	v_TexCoords = aTexCoords;
	gl_Position = vec4(aPos,1.0);
}

#type fragment
#version 430 core

in vec2 v_TexCoords;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gSpecular;




//uniform sampler2D texture_diffuse1;

struct PointLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

};
struct ParallelLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};
struct SpotLight{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
	vec3 position;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;
};
struct gBufferMaterial{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;

	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color

};

uniform bool u_HasSpotLight;
uniform bool u_HasParallelLight;
uniform int u_PointLightNums;

//uniform Material u_Material;
uniform ParallelLight u_ParallelLight;
uniform PointLight u_PointLights[100];
uniform SpotLight u_SpotLight;
uniform vec3 u_CameraViewPos;

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir,gBufferMaterial material,vec3 fragPos);
//vec3 CalcParallelLight(ParallelLight light,vec3 normal,vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 viewDir);

out vec4 FragColor;
void main(){
	gBufferMaterial material;

	vec3 v_FragPos = texture(gPosition,v_TexCoords).rgb;
	vec3 v_Normal = texture(gNormal,v_TexCoords).rgb;
	material.diffuseColor = texture(gAlbedoSpec,v_TexCoords).rgb;
	
	

	material.isBlinnLight = false;
	material.specularColor = texture(gSpecular,v_TexCoords).rgb;

	vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
	vec3 outColor =vec3(0.0,0.0,0.0);
//	if(u_HasParallelLight)
//		outColor += CalcParallelLight(u_ParallelLight,v_Normal,viewDir);
//	if(u_HasSpotLight)
//		outColor += CalcSpotLight(u_SpotLight, v_Normal,viewDir);
//
	for(int i=0;i<u_PointLightNums;i++){
		outColor += CalcPointLight(u_PointLights[i], v_Normal,viewDir,material,v_FragPos);
	}

	FragColor = vec4(outColor,1.0);



}



vec3 CalcPointLight(PointLight light,vec3 normal,vec3 viewDir,gBufferMaterial material,vec3 fragPos){
	vec3 fragColor;

	float distance = length(light.position-fragPos);
	float attenuation = 1.0f/(light.constant+light.linear * distance+light.quadratic*distance*distance);
	/*ambient*/
//	vec3 ambient = light.ambient * (material.ambientColor * (1-material.isTextureSample)+ texture(material.diffuse,v_TexCoords).rgb * material.isTextureSample);
	
	/*diffuse*/
	vec3 lightDir = normalize(light.position-fragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDir,norm),0.0f);
	vec3 diffuse = light.diffuse * diff * (material.diffuseColor);

	/*specular*/
	vec3 specular;
	float spec;
	if(material.isBlinnLight){

		vec3 halfwayDir = normalize(lightDir+viewDir);
		spec = pow(max(dot(norm,halfwayDir),0.0),material.shininess);
		specular =  light.specular * spec  *  material.specularColor;
	}
	else{

		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);
		specular =  light.specular * spec  *  material.specularColor;//texture(material.specular,v_TexCoords).rgb;
	}
//	 vec3 emission = texture(material.emission, v_TexCoords).rgb;


	//ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	//emission *= attenuation;

	fragColor = diffuse + specular;//+ ambient +emission;// * mix(texture(u_Texture1, v_TexCoords), texture(u_Texture2, vec2(1.0 - v_TexCoords.x, v_TexCoords.y)), u_MixValue);
	
	return fragColor;
}


```
![gBuffer](/results/gBuffer_leftTopPos_leftBotNormal_rightTopSpecular_rightBotDiffuse.png)
![gBuffer](/results/gBufferBlending.png)



