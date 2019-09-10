#pragma once
#include<glm/glm.hpp>
#include<string>
class Shader
{
public:
	Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
	~Shader();
	void Bind()const;
	void Unbind() const;

	void SetUniform1i(const std::string &name,int val) const;
	void SetUniform1f(const std::string &name, float val) const;
	void SetUniformMat4f(const std::string &name, const glm::mat4 &mat) const;
	void SetUniformVec3f(const std::string & name, const glm::vec3& value) const;


private:
	uint32_t m_RendererID;


};
