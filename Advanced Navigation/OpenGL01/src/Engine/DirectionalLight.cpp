#include "DirectionalLight.h"
#include <glm\ext.hpp>


DirectionalLight::DirectionalLight(const glm::vec3& a_colour,
	const float& a_ambientIntensity,
	const glm::vec3& a_direction,
	const float& a_diffuseIntensity,
	const float& a_specularIntensity)
	: m_colour(a_colour),
	m_ambientIntensity(a_ambientIntensity),
	m_direction(a_direction),
	m_diffuseIntensity(a_diffuseIntensity),
	m_specularIntensity(a_specularIntensity)
{

}