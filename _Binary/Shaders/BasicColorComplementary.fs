#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform bool complementary = false;

out vec4 Fragment;
vec3 tempValue = vec3(-1,-1,-1);

void main()
{
	Fragment = vec4(Color,1);
	
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
    {
        Fragment = vec4(SolidColor, 1);
    }
    
    if(complementary == true)
    {
      Fragment = vec4(1-Fragment.r,1-Fragment.g,1-Fragment.b,1);
    }
    
    

	return;
}