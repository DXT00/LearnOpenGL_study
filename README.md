### IBL specular part for pbr textures

#### 1.�����в��ʵ�sphere

#### 2.�޸�shader : iblTexture.glsl

```
void main(){
	//material properties
	vec3 albedo = pow(texture(u_Material.diffuse,v_TexCoord).rgb,vec3(2.2));
	float metallic = texture(u_Material.mentallic, v_TexCoord).r;
    float roughness = texture(u_Material.roughness, v_TexCoord).r;
    float ao = texture(u_Material.ao, v_TexCoord).r;
       
	vec3 N = getNormalFromMap();
	vec3 V = normalize(u_CameraViewPos-v_FragPos);
	vec3 R = reflect(-V,N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);

	//reflection equation
	vec3 Lo = vec3(0.0);
	for(int i=0;i<u_PointLightNums;i++){
		vec3 L = normalize(u_PointLights[i].position-v_FragPos);
		vec3 H = normalize(V+L);
		float attenuation = calculateAttenuation(u_PointLights[i],v_FragPos);
		vec3 radiance = u_PointLights[i].diffuse * attenuation;

		float NDF = NoemalDistribution_TrowbridgeReitz_GGX(N,H,roughness);
		float G = GeometrySmith(N, V,L,roughness);
		vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
		
		//CookTorrance
		vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
        vec3 specular     = nominator / denominator;
		
		// kS is equal to Fresnel
		vec3 Ks = F;
		// for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 Kd = vec3(1.0)-Ks;
		 // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
		Kd *= (1.0 - metallic);

		
		float NdotL = max(dot(N,L),0.0);
		Lo+= BRDF(Kd,Ks,specular)*LightRadiance(v_FragPos,u_PointLights[i])*NdotL;
	}

	//ambient lightings (we now use IBL as the ambient term)!
	vec3 F =  FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 Ks = F;
	vec3 Kd = vec3(1.0)-Ks;
	Kd *= (1.0 - metallic);
	vec3 environmentIrradiance = texture(u_IrradianceMap,N).rgb;
	vec3 diffuse = environmentIrradiance*u_Material.diffuseColor;

	//sample both the prefilter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part
	const float MAX_REFLECTION_LOD = 4.0;
	//sample MAX_REFLECTION_LOD level mipmap everytime !
	vec3 prefileredColor = textureLod(u_PrefilterMap,R,roughness*MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(u_BrdfLUTMap,vec2(max(dot(N,V),0.0),roughness)).rg;

	vec3 specular = prefileredColor * (F*brdf.x+brdf.y);
	vec3 ambient = (Kd*diffuse+specular) * ao;
    vec3 color =ambient + Lo;

	//HDR tonemapping
    color = color / (color + vec3(1.0));
	//gamma correction
    color = pow(color, vec3(1.0/2.2));  
    FragColor = vec4(color, 1.0);
}

```


!IBLTextureSpheres(/results/IBLTextureSpheres.png)



