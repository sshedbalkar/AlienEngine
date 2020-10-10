#include "Precompiled.h"
#include "LevelEditor.h"
#include "Physics.h"
#include "Graphics.h"
#include "WindowsSystem.h"
#include "Factory.h"
#include "Core.h"
#include "Utility.h"
#include <ctime>
#include "particleEmitter.h"
#include "LoadLevelClass.h"
#include "Level_1.h"
#include "debugGeometry.h"
#include "GSM.h"
#include "TweakBarManager.h"
#include "VMath.h"
#include "SaveLevelClass.h"
#include "ModelComponent.h"
#include "cameraclass.h"
#include "GameLogic.h"
#include "Hero.h"
#include "DebugComponent.h"
#include "ComponentCreator.h"
#include "LightComponent.h"


namespace Framework
{
	

	LevelEditor* EDITOR=NULL;

	LevelEditor::LevelEditor()
	{
		EDITOR=this;
		state=select;
		inEditor=false;
		leftclicked=false;
		twbmanager = new TweakBarManager;
		maxSelectDistance = 100;
		axisObject= new Axis;
		editorFunc = new EditorFunctionality;
		hoveredGOC=0;
		m_showLevelBoxes=false;
		m_showBoxExcluded=true;
		
	}

	LevelEditor::~LevelEditor()
	{
	}

	
	void LevelEditor::Initialize()
	{
		m_gocsToCopy.clear();
		RegisterComponent(DebugComponent);
		selectedObjects.clear();
		leftclicked=false;
		inEditor=false;
		state=select;
		if(!twbmanager->Initialized )
			twbmanager->InitializeTweakBar();
		axisObject->Initialize();
		hoveredGOC=0;
		

	}
	
	void LevelEditor::Free()
	{
	}

	void LevelEditor::AddLightToGame(GOC* g)
	{
		LevelClass->CreateLight(g);
		GRAPHICS->CreateLightBuffer();
		LOGIC->BulbList.push_back(g);
	}

	void LevelEditor::RemoveLightFromGame(GOC* g)
	{
		bool found=false;
		int index=-1;
		for(unsigned int i=0;i<LOGIC->BulbList.size();++i) 
		{
			GOC * gg=LOGIC->BulbList[i];
			if(found)
			{
				gg->has(LightComponent)->SetIndex(i-1);
			}
			if(gg==g)
			{
				index=i;
				break;
				found=true;
			}
		}
		if(index>-1)
		{
			LOGIC->BulbList.erase(LOGIC->BulbList.begin()+index);
			int max=GRAPHICS->GetMAXNoOfLights();
			while(index<max-1)
			{
				GRAPHICS->DeferredLights[index].Color =GRAPHICS->DeferredLights[index+1].Color ;
				GRAPHICS->DeferredLights[index].Falloff =GRAPHICS->DeferredLights[index+1].Falloff ;
				GRAPHICS->DeferredLights[index].Position =GRAPHICS->DeferredLights[index+1].Position ;
				++index;
			}
			GRAPHICS->SetNumOfLights(GRAPHICS->GetNoOfLights()-1);
			GRAPHICS->CreateLightBuffer();

		}

	}
	
	void LevelEditor::SendMessage( Message* m )
	{
		if(!inEditor) return;
		switch(m->MessageId){
		case Mid::CharacterKey:
			{
				MessageCharacterKey * key = (MessageCharacterKey*)m;
				if(key->character==VK_ESCAPE)
				{
					SAVECLASS->SaveLevel("harsha.xml"); 
					inEditor=false;
					state=select;
					twbmanager->HideAllBars();
					ChangeTexturesBackToNormal();
					GRAPHICS->TurnOffDebugCamera();
					LOGIC->m_Hero.RemoveShowPositionInEditor();
					for(ObjectLinkList<RigidBody>::iterator it=PHYSICS->RigidBodies.begin();it!=PHYSICS->RigidBodies.end();++it)
					{
						if(it->GetOwner()->has(ComplexObject))
						{
							it->GetOwner()->has(ComplexObject)->calculateInternals();
							continue;
						}
						it->editorInitialize();
						//it->setAwake(true);
					}
					for(ObjectLinkList<RigidBody>::iterator it=PHYSICS->LevelData.begin();it!=PHYSICS->LevelData.end();++it)
					{
						it->editorInitialize();
					}
				}
				else if(key->character=='e')
				{
					if(state==CreateObjectetype)
					{
						twbmanager->BuildFromArchetype(twbmanager);
					}
				}
				else if(key->character=='o')
				{
					GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
					GameObjectIdMapType::iterator p;
					selectedObjects.clear(); 
					for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++){
						if(p->second->CompositionTypeId==GOC_Type_Domino )
							selectedObjects.push_back(p->second);
					}
					twbmanager->SetSelectedGOCs(selectedObjects);
					state=select;
				}
				else if(key->character=='h')
				{
					HERO->TeleportHero(CAMERA->GetPosition());
				}
				else if(key->character=='1')
				{
					state=select;
					ChangeTexturesBackToNormal();
					selectedObjects.clear();
				}else if(key->character=='2')
				{
					state=move;
				}else if(key->character=='3')
				{
					state=rotate;
				}
				else if(key->character=='4')
				{
					state=genericProperties;
					twbmanager->ShowGenericProperties();
				}
				else if(key->character=='5')
				{
					state=CreateObjectetype;
					twbmanager->ShowCreateObjectDetails();
				}
				else if(key->character=='6')
				{
					state=CreateObjectetype;
					twbmanager->ShowLightProperties();
				}

			
				break;
			}

		case Mid::VirtualKey:
			{
				// Cast to the derived message type
				MessageVirtualKey * key = (MessageVirtualKey*)m;
				if(key->character == VK_F5)	// save game
					SAVECLASS->SaveLevel(LOGIC->GetLevelId());
				else if(key->character==0x43) //CTR+C
				{
					m_gocsToCopy.clear();
					if (selectedObjects.size()>0)
					{
						for(unsigned int i=0;i<selectedObjects.size();++i) 
						{
							m_gocsToCopy.push_back(selectedObjects[i]);
						}

					}


				}
				else if(key->character==0x56 ) //CTR+V
				{
					if (m_gocsToCopy.size()>0)
						editorFunc->CopySelectedObjects();
				}
				else if(key->character==0x44 ) //CTR+D
				{
					GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
					GameObjectIdMapType::iterator p;
					for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++){
						if(p->second->CompositionTypeId==GOC_Type_Domino )
							p->second->Destroy();
					}
				}
				else if(key->character==0x41 ) //CTR+A
				{
					GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
					GameObjectIdMapType::iterator p;
					ChangeTexturesBackToNormal();
					selectedObjects.clear();
					for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++){
						selectedObjects.push_back(p->second);
					}
					ChangeTexturesToDebug();
					state=move;
				}
				else if(key->character==VK_DELETE )
				{
					if (selectedObjects.size()!=0)
						editorFunc->DeleteSelectedObjects();
				}
				break;
			}
		case Mid::LMouseButton:
			{
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed)
				{
					clickedPosition=Vector3(lmouse->MousePosition.x,lmouse->MousePosition.y,0.0);
					clickedpoint = clickedPosition;
					GRAPHICS->screenPosition(clickedpoint);
					LeftClick();
				}
				else
				{
					unClickedPosition=Vector3(lmouse->MousePosition.x,lmouse->MousePosition.y,0.0);
					LeftUnClick();
					leftclicked=false;
				}
				break;
			}
		case Mid::MouseMove:
			{

				//The mouse has moved, update the mouse's world position
				MouseMove * mouseMove = (MouseMove*)m;
				MousePosition=Vector3(mouseMove->MousePosition.x,mouseMove->MousePosition.y,0.0);
				position.x=mouseMove->MousePosition.x;
				position.y=mouseMove->MousePosition.y;
				position.z=0;
				screenMousePosition=MousePosition;
				currentpoint = MousePosition;
				GRAPHICS->screenPosition(currentpoint);
				topPoint=Vector3(MousePosition.x,clickedPosition.y,0);
				downPoint=Vector3(clickedPosition.x,MousePosition.y,0);
				if(leftclicked)
				{
					if(abs(topPoint.x-clickedPosition.x)>10 || abs(downPoint.y-clickedPosition.y)>10)
						drawbox=true;
					else
						drawbox=false;
				}else 
					drawbox=false;
				GRAPHICS->screenPosition(topPoint);
				GRAPHICS->screenPosition(downPoint);
				direction=Vector3(0,0,0);
				CenterDir=direction;
				CenterPos=Vector3(GRAPHICS->screen_width/2,GRAPHICS->screen_height/2,0);
				GRAPHICS->ScreenToWorldSpaceRay(CenterPos,CenterDir);
				GRAPHICS->ScreenToWorldSpaceRay(position,direction);
				GRAPHICS->ScreenToWorldSpace(MousePosition,center.z);
				break;
			}
		}
	}
	
	void LevelEditor::Unload()
	{
	}

	void LevelEditor::LeftClick()
	{
		leftclicked=true;
		switch(state)
		{
		case select:{break;}
		case move:{break;}
		case rotate:{break;}
		case scale:{break;}
		}

	}

	void LevelEditor::LeftUnClick()
	{
		switch(state)
		{
		case select:
			{
				
				
				//select code
				if(abs(unClickedPosition.x-clickedPosition.x)<10 && abs(unClickedPosition.y-clickedPosition.y)<10 )
				{					
					if(hoveredGOC)
					{
						selectedObjects.push_back(hoveredGOC);
					}
				}
				else
				{
					//multiple selection code
					ChangeTexturesBackToNormal();
					selectedObjects.clear();
					editorFunc->SelectObjects();
				}
				
				break;
			}
		case move:
			{
				//movement code
				break;
			}
		case rotate:
			{
				//rotate code
				break;
			}
		case scale:
			{
				//scale code
				break;
			}
		}
	}


	void LevelEditor::Draw()
	{
		if(!inEditor) return;
		center=Vec3(0,0,0);
		if(selectedObjects.size()!=0)
		{
			for(unsigned int i=0;i<selectedObjects.size();++i)
				center+=selectedObjects[i]->has(Transform)->Position;
			center/=float(selectedObjects.size());
		}
		switch(state)
		{
		case select:
			{
				//select code
				//code for edge detection i guess
				if(leftclicked && drawbox)
				{
					Vec4 color(1,0,0,1);
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3((float)clickedpoint.x,(float)clickedpoint.y,(float)clickedpoint.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3((float)downPoint.x, (float)downPoint.y, (float)downPoint.z), color) );

					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)downPoint.x, (float)downPoint.y, (float)downPoint.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3( (float)currentpoint.x, (float)currentpoint.y, (float)currentpoint.z), color) );

					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)clickedpoint.x, (float)clickedpoint.y, (float)clickedpoint.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3( (float)topPoint.x, (float)topPoint.y, (float)topPoint.z), color) );

					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)topPoint.x, (float)topPoint.y, (float)topPoint.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3( (float)currentpoint.x, (float)currentpoint.y, (float)currentpoint.z), color) );

				}
				break;
			}
		case move:
			{
				
				//movement code

				//xaxis
				Vec4 color(1,0,0,1);
				if(axisObject->selectedAxis==axisObject->AxisSelection::x) 
					color=Vec4(1,1,1,1);
				Vector3 b= CAMERA->GetPosition() - center;
				axisObject->magicNumber = (float)b.magnitude()/6.0f;
				axisObject->magicNumber1 = (float)b.magnitude()/69.0f;
				axisObject->magicNumber2 = axisObject->magicNumber+(float)b.magnitude()/30.0f;
				
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(center, color) );
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3(center.x + axisObject->magicNumber,center.y,center.z), color) );
				float a=(float)R_PI/4.0f,angle=0.0f;
				for(int i=0;i<8;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)center.x + axisObject->magicNumber, (float)center.y+axisObject->magicNumber1*cos(angle),center.z+axisObject->magicNumber1*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)center.x + axisObject->magicNumber, (float)center.y+axisObject->magicNumber1*cos(angle+a),center.z+axisObject->magicNumber1*sin(angle+a)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)center.x + axisObject->magicNumber, (float)center.y+axisObject->magicNumber1*cos(angle),center.z+axisObject->magicNumber1*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)center.x + axisObject->magicNumber2, (float)center.y, (float)center.z), color));
					angle+=a;
				}

				//yaxis
				color=Vec4(0,1,0,1);
				if(axisObject->selectedAxis == axisObject->AxisSelection::y) 
					color=Vec4(1,1,1,1);
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(center, color));
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3(center.x,center.y+axisObject->magicNumber,center.z), color));
				a=(float)R_PI/4.0f;
				angle=0.0f;
				for(int i=0;i<8;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+axisObject->magicNumber1*cos(angle),center.y+axisObject->magicNumber,center.z+axisObject->magicNumber1*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+axisObject->magicNumber1*cos(angle+a),center.y+axisObject->magicNumber,center.z+axisObject->magicNumber1*sin(angle+a)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+axisObject->magicNumber1*cos(angle),center.y+axisObject->magicNumber,center.z+axisObject->magicNumber1*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x,center.y + axisObject->magicNumber2,center.z), color));
					angle+=a;
				}

				//zaxis
				color=Vec4(0,0,1,1);
				if(axisObject->selectedAxis==axisObject->AxisSelection::z) 
					color=Vec4(1,1,1,1);
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(center, color) );
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3(center.x,center.y,center.z+axisObject->magicNumber), color));
				a=(float)R_PI/4.0f;
				angle=0.0f;
				for(int i=0;i<8;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+axisObject->magicNumber1*cos(angle),center.y+axisObject->magicNumber1*sin(angle),center.z+axisObject->magicNumber), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+axisObject->magicNumber1*cos(angle+a),center.y+axisObject->magicNumber1*sin(angle+a),center.z+axisObject->magicNumber), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+axisObject->magicNumber1*cos(angle),center.y+axisObject->magicNumber1*sin(angle),center.z+axisObject->magicNumber), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x,center.y,center.z+axisObject->magicNumber2), color));
					angle+=a;
				}
				break;
			}
		case rotate:
			{
				//rotate code
				//xaxis
				Vec4 color(0,0,1,1);
				if(axisObject->selectedRotationAxis==axisObject->AxisSelection::x) 
					color=Vec4(1,1,1,1);
				float a=(float)R_PI/16.0f,angle=0.0f;
				for(int i=0;i<32;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x,center.y+2*cos(angle),center.z+2*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x,center.y+2*cos(angle+a),center.z+2*sin(angle+a)), color) );
					angle+=a;
				}

				//yaxis
				color=Vec4(1,0,0,1);
				if(axisObject->selectedRotationAxis==axisObject->AxisSelection::y) 
					color=Vec4(1,1,1,1);
				a=(float)R_PI/16.0f;
				angle=0.0f;
				for(int i=0;i<32;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+2*cos(angle),center.y,center.z+2*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+2*cos(angle+a),center.y,center.z+2*sin(angle+a)), color) );
					angle+=a;
				}

				//zaxis
				color=Vec4(0,1,0,1);
				if(axisObject->selectedRotationAxis==axisObject->AxisSelection::z) 
					color=Vec4(1,1,1,1);
				a=(float)R_PI/16.0f;
				angle=0.0f;
				for(int i=0;i<32;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+2.0f*cos(angle),center.y+2.0f*sin(angle),center.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+2.0f*cos(angle+a),center.y+2.0f*sin(angle+a),center.z), color) );
					angle+=a;
				}
				break;
			}
		case scale:
			{
				//scale code
				break;
			}
		}
	}

	void LevelEditor::ChangeTexturesBackToNormal()
	{
		for(unsigned i = 0 ; i<selectedObjects.size() ; ++i){
			ModelComponent* pmc = selectedObjects[i]->has(ModelComponent);
			if(pmc) pmc->ChangeTextureBack();
		}
		if(hoveredGOC) //clear previous goc hovered goc texture
		{
 			ModelComponent* pmc = hoveredGOC->has(ModelComponent);
			if(pmc) pmc->ChangeTextureBack();
		}
	}

	void LevelEditor::ChangeTexturesToDebug()
	{
		for(unsigned i = 0 ; i<selectedObjects.size() ; ++i){
			ModelComponent* pmc = selectedObjects[i]->has(ModelComponent);
			if(pmc) pmc->ChangeTextureDebug();
		}
		if(hoveredGOC) //clear previous goc hovered goc texture
		{
			ModelComponent* pmc = hoveredGOC->has(ModelComponent);
			if(pmc) pmc->ChangeTextureDebug();
		}
	}


	void LevelEditor::Update(float dt)
	{
		if(!inEditor) return;//NOTHING should be above this code
		for(unsigned int i=0;i<LOGIC->BulbList.size();++i)//update lights
		{
			GRAPHICS->DeferredLights[i].Color=LOGIC->BulbList[i]->has(LightComponent)->Color;
			GRAPHICS->DeferredLights[i].Falloff=LOGIC->BulbList[i]->has(LightComponent)->fallout;
			GRAPHICS->DeferredLights[i].Position=LOGIC->BulbList[i]->has(Transform)->Position;
		}
		GRAPHICS->CreateLightBuffer();


		if (IsCtrlHeld()){isContolHeld=true;}
		else{isContolHeld=false;}

		if(selectedObjects.size()>0 )
		{
			if(selectedObjects[0]->has(ComplexObject))
			{
				//selectedObjects[0]->has(RigidBody)->setPosition(selectedObjects[0]->has(Transform)->Position);
				//selectedObjects[0]->has(ComplexObject)->updateGraphicsObjects();
				
				//selectedObjects[0]->has(ComplexObject)->calculateInternals();
				//selectedObjects[0]->has(ComplexObject)->UpdateTheShape();
				//selectedObjects[0]->has(ComplexObject)->();
			}
		}

		switch(state)
		{
		case select:
			{
				if (selectedObjects.size()==0)
					twbmanager->SetSelectedGOCs(selectedObjects);
				if(selectedObjects.size()>0 && !isContolHeld && !leftclicked)
				{
					
					twbmanager->SetSelectedGOCs(selectedObjects);
					ChangeTexturesToDebug();
					state=move;
					break;
				}
				ChangeTexturesBackToNormal();

				//select code
				if(!isContolHeld)//dont clear if control held
					selectedObjects.clear();
				axisObject->Update(dt);
				
				hoveredGOC=editorFunc-> GetGOCFromIntersection(position,direction);//assign hover again
				ChangeTexturesToDebug();
				break;
			}
		case move:
			{
				
				ChangeTexturesBackToNormal();
				if(selectedObjects.size()==0) 
				{
					twbmanager->SetSelectedGOCs(selectedObjects);
					state=select;
					break;
				}
				axisObject->Update(dt);
				if(leftclicked)
					editorFunc->UpdatePosition();
				else
					axisObject->CheckAxes();
				break;
			}
		case rotate:
			{
				//rotate code
				if(selectedObjects.size()==0)
				{
					
					state=select;
					break;
				}
				if(leftclicked)
					editorFunc->UpdateRotation();
				else
					axisObject->CheckRotationaxes();
				break;
			}
		case scale:
			{
				//scale code
				if(selectedObjects.size()==0)
				{
					state=select;
					break;
				}
				break;
			}
		}
		center=Vec3(0,0,0);
		if(selectedObjects.size()!=0)
		{
			for(unsigned int i=0;i<selectedObjects.size();++i)
				center+=selectedObjects[i]->has(Transform)->Position;
			center/=(float)selectedObjects.size();
		}
		
		twbmanager->Update();
		prevMousePosition=MousePosition;
		prevScreenMousePosition=screenMousePosition;
	}


	

}
