#include "Precompiled.h"
#include "GraphicsAssetsLoader.h"
#include "XMLSerialization.h"
#include <iostream>
#include "Utility.h"

namespace Framework
{
	/*void GraphicsAssetsLoader::Initialize()
	{
		LoadModels("Assets\\GraphicsAssets.xml");
		LoadModels2("Assets\\GraphicsAssets.xml");
	}

	GraphicsAssetsLoader::GraphicsAssetsLoader()
	{
		//LevelClass=this;
	}

	GraphicsAssetsLoader::~GraphicsAssetsLoader()
	{
		for(unsigned i=0; i<m_modelInfoVector.size() ;++i){
			delete m_modelInfoVector[i];
		}
		m_modelInfoVector.clear();
		for(unsigned i=0; i<m_model2InfoVector.size() ;++i){
			delete m_model2InfoVector[i];
		}
		m_model2InfoVector.clear();
	}

	
	void GraphicsAssetsLoader::LoadModels(const std::string& filename)
	{
		
		XMLReader *reader = new XMLReader();
		if(reader->open(filename.c_str()))
		{
			ISerializer* str = reader->data().clone();
			std::vector<ISerializer*> comps;
			if ( str->readProperty( "Models", *str ) )
			{
				str->readProperty("Model", comps);
				//
				for(std::vector<ISerializer*>::iterator it1 = comps.begin(), it2 = comps.end(); it1 != it2; ++it1)
				{
					ModelInfo *modelItem = new ModelInfo;
					ISerializer* stream = *it1;
					std::string mstring;
					
					if(stream->readValue("texture1", mstring))
					{
						if (!mstring.empty())
							modelItem->texture1=mstring.c_str();
					}
					if(stream->readValue("texture2", mstring))
					{
						if (!mstring.empty())
							modelItem->texture2=mstring.c_str();
					}
					if(stream->readValue("texture3", mstring))
					{
						if (!mstring.empty())
							modelItem->texture3=mstring.c_str();
					}
					if(stream->readValue("swaptexture1", mstring))
					{
						if (!mstring.empty())
							modelItem->swaptexture1=mstring.c_str();
					}
					if(stream->readValue("swaptexture2", mstring))
					{
						if (!mstring.empty())
							modelItem->swaptexture2=mstring.c_str();
					}
					if(stream->readValue("swaptexture3", mstring))
					{
						if (!mstring.empty())
							modelItem->swaptexture3=mstring.c_str();
					}
					if(stream->readValue("bin", mstring))
					{
						modelItem->bin=mstring;
					}
					if(stream->readValue("textureType",mstring))
					{
						if (mstring=="SingleTexture")
						{
							modelItem->textureType=MultiTextureType::SingleTexture ;
						}
						else if (mstring=="TextureBlend")
						{
							modelItem->textureType=MultiTextureType::TextureBlend ;
						}
						else if (mstring=="LightMap")
						{
							modelItem->textureType=MultiTextureType::LightMap ;
						}
						else if (mstring=="AlphaBlend")
						{
							modelItem->textureType=MultiTextureType::AlphaBlend ;
						}
						else if (mstring=="Fog")
						{
							modelItem->textureType=MultiTextureType::Fog ;
						}
						else if (mstring=="ClipPlane")
						{
							modelItem->textureType=MultiTextureType::ClipPlane ;
						}
						else if (mstring=="Transparent")
						{
							modelItem->textureType=MultiTextureType::Transparent ;
						}
						else if (mstring=="Glass")
						{
							modelItem->textureType=MultiTextureType::Glass ;
						}
						else if (mstring=="Invisible")
						{
							modelItem->textureType=MultiTextureType::Invisible ;
						}
					}
					m_modelInfoVector.push_back(modelItem);
				}	
						
			}
			//
			utility::deletePointersVec(comps);
			delete str;
		}
		delete reader;
	}

	void GraphicsAssetsLoader::LoadModels2(const std::string& filename)
	{
		
		XMLReader *reader = new XMLReader();
		if(reader->open(filename.c_str()))
		{
			ISerializer* str = reader->data().clone();
			std::vector<ISerializer*> comps;
			if ( str->readProperty( "Models2", *str ) )
			{
				str->readProperty("Model2", comps);
				//
				for(std::vector<ISerializer*>::iterator it1 = comps.begin(), it2 = comps.end(); it1 != it2; ++it1)
				{
					ModelInfo* modelItem= new ModelInfo;
					ISerializer* stream = *it1;
					std::string mstring;
					
					if(stream->readValue("texture1", mstring))
					{
						if (!mstring.empty())
							modelItem->texture1=mstring.c_str();
					}
					if(stream->readValue("texture2", mstring))
					{
						if (!mstring.empty())
							modelItem->texture2=mstring.c_str();
					}
					if(stream->readValue("texture3", mstring))
					{
						if (!mstring.empty())
							modelItem->texture3=mstring.c_str();
					}
					if(stream->readValue("swaptexture1", mstring))
					{
						if (!mstring.empty())
							modelItem->swaptexture1=mstring.c_str();
					}
					if(stream->readValue("swaptexture2", mstring))
					{
						if (!mstring.empty())
							modelItem->swaptexture2=mstring.c_str();
					}
					if(stream->readValue("swaptexture3", mstring))
					{
						if (!mstring.empty())
							modelItem->swaptexture3=mstring.c_str();
					}
					if(stream->readValue("bin", mstring))
					{
						modelItem->bin=mstring;
					}
					if(stream->readValue("textureType",mstring))
					{
						if (mstring=="Bump")
						{
							modelItem->textureType=MultiTextureType::Bump ;
						}
						else if (mstring=="Spec")
						{
							modelItem->textureType=MultiTextureType::Spec ;
						}
					}
					m_model2InfoVector.push_back(modelItem);
					
				}	
						
			}
			//
			utility::deletePointersVec(comps);
			delete str;
		}
		delete reader;
	}

	ModelInfo::ModelInfo()
	{
		texture1="";
		texture2="";
		texture3="";
		swaptexture1="";
		swaptexture2="";
		swaptexture3="";

	}

	ModelInfo::~ModelInfo()
	{
	}*/
}


