#include "Terrain.h"

int Terrain::TerrainSize = 1024;

SVertex* Terrain::generateWaterFace(SVertex* ori,const char* data)
{
	
	if (ori == NULL)
	{
		const int size = 1024; // 可以改成TerrianSize
		
		heightMap = Texture::loadHeightMap(".\\resources\\heightMap.png");
		ori = new SVertex[size*size];
		cache = ori;
		for (int i = 0;i < size;i++)
		{
			for (int j = 0;j < size;j++)
			{
				float add = 0;//(rand() % 100) / 200.0;
				ori[i * size + j].Position = { ((i)-(size / 2))/10.0,
					heightMap[(i*size + j) * 3] / 32.0 + 10,((j)-(size / 2)) / 10.0 };
				
				
				ori[i * size + j].Normal = { 0,1,0 };
				ori[i * size + j].TexCoords = { i / 8.0 + add / 2.0,j / 8.0 + add / 10.0 };
			}
		}
		for (int i = 1; i < size - 1;i++)
		{
			for (int j = 1;j < size - 1;j++)
			{
				glm::vec3 nTmp = { 0,0,0 };

				ori[i * size + j].Position = { ((i)-(size / 2)) / 10.0,
					(heightMap[(i*size + j+1) * 3] * 4+
						heightMap[(i*size + j-1) * 3] *4+
						heightMap[(i*size + j+size) * 3] *4+
						heightMap[(i*size + j-size) * 3] *4+
						heightMap[(i*size + j + 1 + size) * 3] +
						heightMap[(i*size + j - 1 - size) * 3] +
						heightMap[(i*size + j + size - 1) * 3]  +
						heightMap[(i*size + j - size + 1) * 3]  +
						heightMap[(i*size + j) * 3] * 8) / 32.0 / 28.0 + 6,((j)-(size / 2)) / 10.0 };


				ori[i * size + j].Normal = { 0,1,0 };
				ori[i * size + j].TexCoords = { i / 8.0 ,j / 8.0};

				nTmp += glm::cross(ori[i*size + j].Position - ori[i*size + j + size].Position,
					ori[i*size + j].Position - ori[i*size + j - 1].Position);
				nTmp += glm::cross(ori[i*size + j].Position - ori[i*size + j - 1].Position,
					ori[i*size + j].Position - ori[i*size + j - size].Position);
				nTmp += glm::cross(ori[i*size + j].Position - ori[i*size + j - size].Position,
					ori[i*size + j].Position - ori[i*size + j + 1].Position);
				nTmp += glm::cross(ori[i*size + j].Position - ori[i*size + j + 1].Position,
					ori[i*size + j].Position - ori[i*size + j + size].Position);
				nTmp = glm::normalize(nTmp);
				//printf("%f %f %f\n", nTmp.x, nTmp.y, nTmp.z);
				if (nTmp.y < 0) nTmp = -nTmp;
				ori[i * size + j].Normal = nTmp;
			}
		}
	
		waterindex = new GLuint[(size - 1)*(size - 1) * 6];

		for (int i = 0;i < size - 1;i++)
		{
			for (int j = 0;j < size - 1;j++)
			{
				waterindex[i * (size - 1) * 6 + j * 6] = i * size + j;
				waterindex[i * (size - 1) * 6 + j * 6 + 1] = i * size + j + 1;
				waterindex[i * (size - 1) * 6 + j * 6 + 2] = i * size + j + size;
				waterindex[i * (size - 1) * 6 + j * 6 + 3] = i * size + j + size;
				waterindex[i * (size - 1) * 6 + j * 6 + 4] = i * size + j + 1;
				waterindex[i * (size - 1) * 6 + j * 6 + 5] = i * size + j + size + 1;
			}
		}
		return ori;
	}

	return ori;
}

Terrain::Terrain() : waterfloor(0),waterindex(0)
{
	waterfloor = generateWaterFace(waterfloor,".\\resources\\heightMap.png");
	waterFace = new SMesh(waterfloor, 1024 * 1024, waterindex, 1023 * 1023 * 6);
	waterFace->setup();

}


Terrain::~Terrain()
{
	delete[] waterfloor;
	delete[] waterindex;
	delete waterFace;
}
