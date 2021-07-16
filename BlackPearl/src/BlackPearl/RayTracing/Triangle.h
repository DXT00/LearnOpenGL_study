#pragma once
#include "Hitable.h"
#include "glm/glm.hpp"
#include "BlackPearl/RayTracing/Vertex.h"
namespace BlackPearl {
	class Triangle : public Hitable
	{
	public:
		Triangle(const std::vector<Vertex>& points);
		Triangle(const Vertex& p0, const Vertex& p1, const Vertex& p2);

		~Triangle();
		void BuildBox() override;
		std::vector<Vertex> GetPoints() const;
	private:
		std::vector<Vertex> m_Points;
	};


}
