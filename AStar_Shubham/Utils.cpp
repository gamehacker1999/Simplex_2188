#include "Utils.h"
#include<math.h>

//function that rounds the float to its nearest half
float RoundToNearestHalf(float num)
{
	return floor((num * 2) + 0.5f) / 2.f;
}
