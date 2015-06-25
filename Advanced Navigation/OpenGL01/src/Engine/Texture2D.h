#include <string>
class Texture2D
{
public:
	Texture2D();
	~Texture2D();
	void UseTexture();
private:
	unsigned int m_ID;
	void LoadTexture(std::string a_file);

	int m_imageHeight, m_imageWidth, m_imageType;
};