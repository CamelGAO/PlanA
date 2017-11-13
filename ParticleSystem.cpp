#include "stdafx.h"
#include "ParticleSystem.h"

#define PATH(X) "E:\\newstart\\Data_Texture\\##X"

ParticleSystem::ParticleSystem()
{
	isInit = false;
	list = 0;
}

ParticleSystem::~ParticleSystem()
{

}

/****************************************************************************
* 名称：LoadBMP
* 功能：载入纹理
* 入口参数：Filename 纹理图片路径名
* 出口参数：AUX_RGBImageRec 位图数据
****************************************************************************/
AUX_RGBImageRec* ParticleSystem::loadBMP(const char *Filename)	
{
	FILE *File=NULL;	// 文件指针

	if (!Filename)
	{
		return NULL;	// 如果文件指针不存在，则返回
	}

	File=fopen(Filename,"r");// 核实文件是否存在

	if (File)				 // 如果文件存在
	{
		fclose(File);		 // 关闭文件
		return auxDIBImageLoad(Filename);// 读入位图数据，并返回
	}
	else
		return NULL;			// 如果读入失败，则返回NULL
}

/****************************************************************************
* 名称：LoadTextures
* 功能：载入纹理
* 入口参数：无
* 出口参数：int  载入是否成功的标识
****************************************************************************/
int ParticleSystem::loadTextures()
{
	int Status=FALSE;			// 状态指示器

	AUX_RGBImageRec *TextureImage[2];		// 创建保存纹理的空间

	memset(TextureImage,0,sizeof(void *)*2);  

	if (TextureImage[0] = loadBMP("E:\\newstart\\Data_Texture\\rain.bmp")) 
	{
		Status=TRUE;

		glGenTextures(1, &texture[0]);	// 创建第0号纹理

		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

		if(glIsTexture(texture[0]) == GL_FALSE)
			std::cout << texture[0] << " create rain texture error" <<std::endl;
	}
	else
		std::cout << "read rain texture error!" << std::endl;

	if (TextureImage[1] = loadBMP("E:\\newstart\\Data_Texture\\snow.bmp")) 
	{
		Status=TRUE;	

		glGenTextures(1, &texture[1]);	// 创建第1号纹理

		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[1]->sizeX, TextureImage[1]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[1]->data);

		if(glIsTexture(texture[1]) == GL_FALSE)
			std::cout << texture[1] << "create snow texture error!" << std::endl;
	}
	else
		std::cout << " read snow texture error!" << std::endl;

	for(int i=0;i<2;i++)
	{
		if (TextureImage[i])						// 如果纹理存在
		{
			if (TextureImage[i]->data)				// 如果纹理数据存在
			{
				free(TextureImage[i]->data);		// 释放纹理数据内存
			}

			free(TextureImage[i]);					// 释放图像结构
		}
	}

	return Status;								// 返回状态
}

/****************************************************************************
* 名称：draw
* 功能：绘制
* 入口参数：无
* 出口参数：无
****************************************************************************/
void ParticleSystem::setParticle(vector<particle>::iterator &_particle)
{
	_particle->p = 
		vuVec3d(
		environmentData.areaCenter[0] + rand()%(environmentData.areaRange*2) - environmentData.areaRange, 
		environmentData.areaCenter[1] + rand()%(environmentData.areaRange*2) - environmentData.areaRange, 
		environmentData.areaCenter[2]
		);
	_particle->v = particleData.v;
	_particle->a = particleData.a;
	_particle->life = particleData.life - rand()%((int)(particleData.life*0.33));
	_particle->fade = particleData.fade;
	_particle->dimension = particleData.dimension;
	_particle->scale = particleData.scale;
	_particle->rotate[2] = rand()%90 + 1;
}


/****************************************************************************
* 名称：draw
* 功能：绘制
* 入口参数：无
* 出口参数：无
****************************************************************************/
void ParticleSystem::draw()
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glDepthMask(GL_FALSE);

	glBindTexture(GL_TEXTURE_2D, texture[environmentData.tex]);
	glPolygonMode(GL_FRONT, GL_FILL);

	glNewList(list,GL_COMPILE);
		for(vector<particle>::iterator iter= particles.begin();iter != particles.end();iter++)
		{ 
			if(iter->life > 0.0f)
			{
				iter->p += iter->v;
				//iter->v += iter->a;
				iter->life -= iter->fade;

				create(iter);
			}
			else
			{
				setParticle(iter);
				//std::cout << iter->p[0] << " " << iter->p[1] << " " << iter->p[2] << std::endl;
			}
		}
	glEndList();

	glCallList(list);

	glDisable(GL_TEXTURE_2D);
	glDisable (GL_BLEND);
	glDepthMask (GL_TRUE);
}

/****************************************************************************
* 名称：create
* 功能：给定坐标值和颜色值贴纹理
* 入口参数：	
                x：坐标值x
				y：坐标值y
				z：坐标值z
				dimensione：图片维度
				r：RGB色的r值
				g：RGB色的g值
				b：RGB色的b值
				tex：纹理号

* 出口参数：无
****************************************************************************/
void ParticleSystem::create(vector<particle>::iterator &_particle)
{
// 	glColor3f(1.0f, 1.0f, 1.0f);
// 
// 	glEnable(GL_TEXTURE_2D);
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_ONE,GL_ONE);
// 	glDepthMask(GL_FALSE);

	glPushMatrix();
	glTranslated(_particle->p[0], _particle->p[1], _particle->p[2]);
	glScalef(1.0f,1.0f,1.0f);
	glRotatef(environmentData.rotate[0] + _particle->rotate[2], 0, 0, 1);

// 	glBindTexture(GL_TEXTURE_2D, texture[environmentData.tex]);
// 	glPolygonMode(GL_FRONT, GL_FILL);

	glBegin(GL_QUADS);

	//front    
	glTexCoord2d( 0.0, 0.0 );
	glVertex3d(_particle->scale[0]*_particle->dimension[0], 0, _particle->scale[2]*_particle->dimension[2]);
	glTexCoord2d( 1.0, 0.0 );
	glVertex3d(-_particle->scale[0]*_particle->dimension[0], 0, _particle->scale[2]*_particle->dimension[2]);
	glTexCoord2d( 1.0, 1.0 );
	glVertex3d(-_particle->scale[0]*_particle->dimension[0], 0, -_particle->scale[2]*_particle->dimension[2]);
	glTexCoord2d( 0.0, 1.0 );
	glVertex3d(_particle->scale[0]*_particle->dimension[0], 0, -_particle->scale[2]*_particle->dimension[2]);

	//back
// 	glTexCoord2d( 0.0, 0.0 );
// 	glVertex3d(_particle->scale[0]*_particle->dimension[0], 0, -_particle->scale[2]*_particle->dimension[2]);
// 	glTexCoord2d( 1.0, 0.0 );
// 	glVertex3d(-_particle->scale[0]*_particle->dimension[0], 0, -_particle->scale[2]*_particle->dimension[2]);
// 	glTexCoord2d( 1.0, 1.0 );
// 	glVertex3d(-_particle->scale[0]*_particle->dimension[0], 0, _particle->scale[2]*_particle->dimension[2]);
// 	glTexCoord2d( 0.0, 1.0 );
// 	glVertex3d(_particle->scale[0]*_particle->dimension[0], 0, _particle->scale[2]*_particle->dimension[2]);

	glEnd();

	glPopMatrix();

// 	glDisable(GL_TEXTURE_2D);
// 	glDisable (GL_BLEND);
// 	glDepthMask (GL_TRUE);

}

/****************************************************************************
* 名称：initData
* 功能：数据初始化
* 入口参数：无
* 出口参数：无
****************************************************************************/
void ParticleSystem::initData()
{
	loadTextures();
	int i;
	srand(unsigned(time(0)));

	particle _particle;
	for(i=0; i < environmentData.total; i++)
	{
		_particle.p = vuVec3d(0.0, 0.0, -1000.0);
		_particle.v = vuVec3d(0.0, 0.0, 0.0);
		_particle.a = vuVec3d(0.0, 0.0, 0.0);
		_particle.life = rand()%particleData.life;
		_particle.fade = particleData.fade;
		_particle.dimension = particleData.dimension;
		_particle.scale = particleData.scale;
		_particle.rotate = particleData.rotate;
		particles.push_back(_particle);
	}
	isInit = true;
	list = glGenLists(1);
	if(list == 0) 
		std::cout << "ShowList Error : glGenLists() returned a zero" << std::endl;
}

