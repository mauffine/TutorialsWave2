#pragma once
#include <glm\vec3.hpp>

class DirectionalLight
{
private:
	glm::vec3 m_colour;
	float m_ambientIntensity;
	glm::vec3 m_direction;
	float m_diffuseIntensity;
	float m_specularIntensity;

public:
	DirectionalLight(const glm::vec3& a_colour,
		const float& a_ambientIntensity,
		const glm::vec3& a_direction,
		const float& a_diffuseIntensity,
		const float& a_specularIntensity);
	DirectionalLight() {};

	// Getter functions
	inline glm::vec3 GetColour() const { return m_colour; }
	inline float GetAmbientIntensity() const { return m_ambientIntensity; }
	inline glm::vec3 GetDirection() const { return m_direction; }
	inline float GetDiffuseIntensity() const { return m_diffuseIntensity; }
	inline float GetSpecularIntensity() const { return m_specularIntensity; }

	// Setter functions
	inline void SetColour(const glm::vec3& a_colour) { m_colour = a_colour; }
	inline void SetAmbientIntensity(const float& a_intensity) { m_ambientIntensity = a_intensity; }
	inline void SetDirection(const glm::vec3& a_dir) { m_direction = a_dir; }
	inline void SetDiffuseIntensity(const float& a_intensity) { m_diffuseIntensity = a_intensity; }
	inline void SetSpecularIntensity(const float& a_intensity) { m_specularIntensity = a_intensity; }
};