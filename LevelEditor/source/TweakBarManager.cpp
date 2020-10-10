#include "TweakBarManager.h"
#include "Physics.h"
#include "Graphics.h"
#include "GameData.h"
#include "ActivationProperties.h"
#include "ConstraintsComponent.h"
#include "AutoMovingComponent.h"
#include "RopeComponent.h"
#include "GameLogic.h"
#include "DominoComponent.h"
#include "ConnectorComponent.h"
#include "LightComponent.h"
#include "lightclass.h"
#include "LoadLevelClass.h"
#include "GlowingCylinder.h"

namespace Framework
{

TweakBarManager::TweakBarManager()
{
	m_selectedGOC=NULL; m_archEnumId=0;
	Initialized=false;
	dominosNo=10;
	dominosDistance=2.5;
	xC=yC=zC=0;
	leftToRight=false;
}

TweakBarManager::~TweakBarManager()
{
	free(s2);
}


void TweakBarManager::SetSelectedGOCs(std::vector<GOC*>  vec)
{
	m_selectedGOC=0;
	m_SelectedGOCs=vec;
	if(m_SelectedGOCs.size()==1)
		m_selectedGOC=m_SelectedGOCs[0];
	ShowObjectDetails();
}

void TweakBarManager::CreateConstraintObject(Vec3 & worldPos, Vec3 & RelPos, real err, int AttachedToId, bool insideBody,Vec3 mult)
{
	GOC *g;
	g=FACTORY->BuildFromArchetype("GOC_Type_Constraint");
	g->has(Transform)->Position = worldPos;
	g->has(Transform)->QUATERNION=D3DXQUATERNION(0,0,0,1);
	g->has(Transform)->Scale=Vec3(0.4f,0.4f,0.4f);
	static_cast<CollisionSphere*>(g->has(CollisionPrimitive))->radius = g->has(Transform)->Scale.x*0.5f;
	g->has(ConstraintComponent)->PosRelative=RelPos;
	g->has(ConstraintComponent)->AttachedToId = AttachedToId;
	g->has(ConstraintComponent)->err=err;
	g->has(ConstraintComponent)->isInsideBody = insideBody;
	g->has(ConstraintComponent)->multiplier = mult;
	g->Initialize();
}


std::string tbString;
void TweakBarManager::ShowObjectDetails ()
{
	
	TweakBarControl* c_bar = GetTweakBarControl( GetStringFromEnum(MyEnum::Object) );
	if (m_SelectedGOCs.size()==0)
	{
		c_bar->HideBar();
		return;
	}
	
	if(m_SelectedGOCs.size()==1)
	{
		TwRemoveAllVars(c_bar->GetTWBar());
		c_bar->ShowBar();
		
		tbString = m_selectedGOC->archId;

		s2 = _strdup(tbString.c_str());
 
		//TwAddVarRW(bar, "s2", TW_TYPE_CDSTRING, &s2, ""); // pass the pointer to s2


		TwAddVarRO(c_bar->GetTWBar(), "ArchId", TW_TYPE_CDSTRING, &s2, " label='Archetype: ' ");
		TwAddVarRO(c_bar->GetTWBar(), "GlobalID", TW_TYPE_INT32, &m_selectedGOC->GlobalId, " label='Global Id: ' ");
		if (m_selectedGOC->CompositionTypeId==GOC_Type_LevelBox || m_selectedGOC->CompositionTypeId==GOC_Type_TutorialBox)
		{
				TwAddVarRW(c_bar->GetTWBar(), "LevelBoxId", TW_TYPE_INT32, &m_selectedGOC->LevelBoxId, " label='LevelBoxId: ' ");
		}
		TwAddSeparator(c_bar->GetTWBar(), "sep1", NULL);
		//pos
		TwAddVarRW( c_bar->GetTWBar(), "XPos", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Position.x), " step=0.01 group=Position label='X Position' precision=4" );	
		TwAddVarRW( c_bar->GetTWBar(), "YPos", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Position.y), " step=0.01 group=Position label='Y Position' precision=4" );	
		TwAddVarRW( c_bar->GetTWBar(), "ZPos", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Position.z), " step=0.01 group=Position label='Z Position' precision=4" );	
		//scale
		if (m_selectedGOC->has(CollisionPrimitive) && m_selectedGOC->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Sphere)
		{
			TwAddVarCB(c_bar->GetTWBar(), "Radius", TW_TYPE_FLOAT, TweakBarManager::SetRadius, TweakBarManager::GetRadius, this, "min= 0.1 step=0.1 group=Scale label='Radius' ");
		}
		else
		{
			TwAddVarRW( c_bar->GetTWBar(), "XScale", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Scale.x), "min= 0.1 step=0.1 group=Scale label='X Scale' precision=4" );	
			TwAddVarRW( c_bar->GetTWBar(), "YScale", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Scale.y), "min= 0.1 step=0.1 group=Scale label='Y Scale' precision=4" );	
			TwAddVarRW( c_bar->GetTWBar(), "ZScale", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Scale.z), "min= 0.1 step=0.1 group=Scale label='Z Scale' precision=4" );	
		}//orientation
		TwAddVarRW( c_bar->GetTWBar(), "Orientation", TW_TYPE_QUAT4F, &(m_selectedGOC->has(Transform)->QUATERNION), " opened=true axisz=-z group=Rotation label='Rotation' " );	
		TwAddButton( c_bar->GetTWBar(), "Rotate90X", TweakBarManager::Rotate90X, this, " group=Rotation label='Rotate 90 on X' ");
		TwAddButton( c_bar->GetTWBar(), "Rotate90Y", TweakBarManager::Rotate90Y, this, " group=Rotation label='Rotate 90 on y' ");
		TwAddButton( c_bar->GetTWBar(), "Rotate90Z", TweakBarManager::Rotate90Z, this, " group=Rotation label='Rotate 90 on Z' ");

		if (m_selectedGOC->has(ActivationProperties))
		{
			TwAddSeparator(c_bar->GetTWBar(), "sep2", NULL);
			TwAddVarRW( c_bar->GetTWBar(), "ObjectActivationId", TW_TYPE_INT32, &(m_selectedGOC->has(ActivationProperties)->m_ActivationId), " step=1 min=0 max=100 group=ActivationProperties label='ObjectActivationId' precision=0" );	
			TwAddVarCB(c_bar->GetTWBar(), "ActivatedBy", ActivationPropertiesStructType, TweakBarManager::SetActivatedBy, TweakBarManager::GetActivatedBy, this, "group=ActivationProperties label='Activated By' ");
			TwAddVarRW(c_bar->GetTWBar(), "AttachedTo",TW_TYPE_INT32,&(m_selectedGOC->has(ActivationProperties)->m_attachedToId) , "group=ActivationProperties label='AttachedTo' ");
			TwAddVarRW(c_bar->GetTWBar(), "RestrictedBy",TW_TYPE_INT32,&(m_selectedGOC->has(ActivationProperties)->m_restrictedById) , "group=ActivationProperties label='RestrictedBy' ");
			TwAddVarRW(c_bar->GetTWBar(), "NeverActivate",TW_TYPE_BOOLCPP ,&(m_selectedGOC->has(ActivationProperties)->m_neverActivate) , "group=ActivationProperties label='Never Activate' ");
			TwAddVarRW(c_bar->GetTWBar(), "PointingTo",TW_TYPE_DIR3F ,&(m_selectedGOC->has(ActivationProperties)->m_pointing) , "group=ActivationProperties label='Pointing To' ");
			TwAddVarRW(c_bar->GetTWBar(), "TimeTostayActivated",TW_TYPE_FLOAT ,&(m_selectedGOC->has(ActivationProperties)->m_timeToStayActivated) , "group=ActivationProperties label='Time To Stay Activated' ");
			TwAddVarRW(c_bar->GetTWBar(), "OneTimeActivated",TW_TYPE_BOOLCPP ,&(m_selectedGOC->has(ActivationProperties)->m_oneTimeActivated) , "group=ActivationProperties label='One Time Activated' ");
			TwAddVarRW(c_bar->GetTWBar(), "ShootingSpeed",TW_TYPE_FLOAT ,&(m_selectedGOC->has(ActivationProperties)->m_shootingSpeed) , " min=0 max=100 group=ActivationProperties label='ShootingSpeed' ");
		}

		if (m_selectedGOC->has(AutoMovingComponent))
		{
 			TwAddSeparator(c_bar->GetTWBar(), "sep6", NULL);
			TwAddVarRW(c_bar->GetTWBar(), "MovingAxisX",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_MovingAxis.x ) ,  "step=1.0 min=-1 max=1 precision=0 group='AutoMoving Properties' label='X Moving' ");
			TwAddVarRW(c_bar->GetTWBar(), "MovingAxisy",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_MovingAxis.y ) ,  "step=1.0 min=-1.0 max=1.0 precision=0 group='AutoMoving Properties' label='Y Moving' ");
			TwAddVarRW(c_bar->GetTWBar(), "MovingAxisz",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_MovingAxis.z ) ,  "step=1.0 min=-1 max=1 precision=0 group='AutoMoving Properties' label='Z Moving' ");
			TwAddVarRW(c_bar->GetTWBar(), "MovingSpeed",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_movingSpeed) ,  "min=0.0f group='AutoMoving Properties' label='MovingSpeed' ");
			TwAddVarRW(c_bar->GetTWBar(), "RotationAxisX",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_rotationAxis.x ) ,  "step=1.0 min=-1 max=1 precision=0 group='AutoMoving Properties' label='X Rotation' ");
			TwAddVarRW(c_bar->GetTWBar(), "RotationAxisY",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_rotationAxis.y ) ,  "step=1.0 min=-1 max=1 precision=0 group='AutoMoving Properties' label='Y Rotation' ");
			TwAddVarRW(c_bar->GetTWBar(), "RotationAxisZ",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_rotationAxis.z ) ,  "step=1.0 min=-1 max=1 precision=0 group='AutoMoving Properties' label='Z Rotation' ");
			TwAddVarRW(c_bar->GetTWBar(), "RotationPointX",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_RotationPoint.x ) ,  "step=1.0 min=-1 max=1 precision=0 group='AutoMoving Properties' label='Rotation Point X' ");
			TwAddVarRW(c_bar->GetTWBar(), "RotationPointY",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_RotationPoint.y ) ,  "step=1.0 min=-1 max=1 precision=0 group='AutoMoving Properties' label='Rotation Point Y' ");
			TwAddVarRW(c_bar->GetTWBar(), "RotationPointZ",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_RotationPoint.z ) ,  "step=1.0 min=-1 max=1 precision=0 group='AutoMoving Properties' label='Rotation Point Z' ");
			TwAddVarRW(c_bar->GetTWBar(), "RotationSpeed",TW_TYPE_FLOAT,&(m_selectedGOC->has(AutoMovingComponent)->m_rotatingSpeed) ,  "min=0.0f group='AutoMoving Properties' label='RotatingSpeed' ");
			TwAddVarRW(c_bar->GetTWBar(), "AutoCanMove",TW_TYPE_BOOLCPP ,&(m_selectedGOC->has(AutoMovingComponent)->m_autoCanMove) , "group='AutoMoving Properties' label='Can Move' ");
			TwAddVarRW(c_bar->GetTWBar(), "AutoCanRotate",TW_TYPE_BOOLCPP ,&(m_selectedGOC->has(AutoMovingComponent)->m_autoCanRotate) , "group='AutoMoving Properties' label='Can Rotate' ");

		}

		if (m_selectedGOC->has(LightComponent))
		{
 			TwAddSeparator(c_bar->GetTWBar(), "sep21", NULL);
			TwAddVarRW(c_bar->GetTWBar(), "Color",TW_TYPE_COLOR3F,&(m_selectedGOC->has(LightComponent)->Color),  " group='Light Properties' label='Color' ");
			TwAddVarRW(c_bar->GetTWBar(), "Fallout",TW_TYPE_FLOAT,&(m_selectedGOC->has(LightComponent)->fallout ) ,  "min=0.0f step=0.1f group='Light Properties' label='Fallout' ");
			TwAddVarRW(c_bar->GetTWBar(), "LightCanMove",TW_TYPE_BOOLCPP ,&(m_selectedGOC->has(LightComponent)->CanMove) , "group='Light Properties' label='Light Can Move' ");
		}

		if (m_selectedGOC->has(ConnectorComponent))
		{
 			TwAddSeparator(c_bar->GetTWBar(), "sep20", NULL);
			TwAddVarRW(c_bar->GetTWBar(), "BridgeVertical",TW_TYPE_BOOLCPP ,&(m_selectedGOC->has(ConnectorComponent)->m_isVertical) , "group='Connector Properties' label='Is Vertical' ");
			TwAddVarRW(c_bar->GetTWBar(), "BridgeCanCollideWithStatic",TW_TYPE_BOOLCPP ,&(m_selectedGOC->has(ConnectorComponent)->m_canCollideWithStatic) , "group='Connector Properties' label='Can Collide Static' ");

		}

		if (m_selectedGOC->has(GlowingCylinders))
		{
 			TwAddSeparator(c_bar->GetTWBar(), "sep59", NULL);
			TwAddVarRW(c_bar->GetTWBar(), "ParentActivationId",TW_TYPE_INT32 ,&(m_selectedGOC->has(GlowingCylinders)->m_parentActivationId) , "group='Glow Cylinder' label='Parent ActivationId' ");

		}

		if (m_selectedGOC->has(RopeComponent))
		{
 			TwAddSeparator(c_bar->GetTWBar(), "sep9", NULL);
			TwAddVarRW(c_bar->GetTWBar(), "IsRopeStart",TW_TYPE_BOOLCPP ,&(m_selectedGOC->has(RopeComponent)->m_isStartPoint ) , "group='Rope Properties' label='Is Start Point' ");
			TwAddVarRW(c_bar->GetTWBar(), "RopePointID",TW_TYPE_INT32 ,&(m_selectedGOC->has(RopeComponent)->Id) , "group='Rope Properties' label='Rope Point Id' ");
			TwAddVarRW(c_bar->GetTWBar(), "RopeAttachedID",TW_TYPE_INT32 ,&(m_selectedGOC->has(RopeComponent)->AttachedId) , "group='Rope Properties' label='Attached to Id' ");

		}

		

		if (m_selectedGOC->has(RigidBody) || m_selectedGOC->CompositionTypeId==GOC_Type_Constraint)
		{
			TwAddSeparator(c_bar->GetTWBar(), "sep5", NULL);
			if(m_selectedGOC->CompositionTypeId==GOC_Type_Constraint)
			{
				TwAddVarRW(c_bar->GetTWBar(), "c_AttachedToId",TW_TYPE_INT32 ,&(m_selectedGOC->has(ConstraintComponent)->AttachedToId) , "group=Contraints/Joints label='Attached To' ");
				TwAddVarRW(c_bar->GetTWBar(), "err",TW_TYPE_DOUBLE ,&(m_selectedGOC->has(ConstraintComponent)->err) , "group=Contraints/Joints label='Err' ");
				TwAddVarRW(c_bar->GetTWBar(), "posRelX",TW_TYPE_DOUBLE ,&(m_selectedGOC->has(ConstraintComponent)->PosRelative.x ) , "  group=Contraints/Joints label='Pos Relative X' ");
				TwAddVarRW(c_bar->GetTWBar(), "posRelY",TW_TYPE_DOUBLE ,&(m_selectedGOC->has(ConstraintComponent)->PosRelative.y ) , "  group=Contraints/Joints label='Pos Relative Y' ");
				TwAddVarRW(c_bar->GetTWBar(), "posRelZ",TW_TYPE_DOUBLE ,&(m_selectedGOC->has(ConstraintComponent)->PosRelative.z ) , "  group=Contraints/Joints label='Pos Relative Z' ");
			}
			else
			{
				TwAddVarRO( c_bar->GetTWBar(), "HasContraint", TW_TYPE_BOOLCPP, &(m_selectedGOC->has(RigidBody)->hasContraint), " group=Contraints/Joints label='Has Contraints' " );	
				if(!m_selectedGOC->has(RigidBody)->hasContraint && m_selectedGOC->CompositionTypeId!=GOC_Type_Constraint )
				{
					TwAddButton(c_bar->GetTWBar(), "AddContraintC", TweakBarManager::AddContraintComponent,this, "group=Contraints/Joints label='Add Contraint Comp' ");
				}
				else if(m_selectedGOC->has(RigidBody)->hasContraint && m_selectedGOC->CompositionTypeId!=GOC_Type_Constraint)
				{
					TwAddButton(c_bar->GetTWBar(), "RemoveContraint", TweakBarManager::RemoveContraintComponent,this, "group=Contraints/Joints label='Remove Contraint Comp' ");
				}
				if(m_selectedGOC->has(RigidBody)->hasContraint)
				{
					TwAddVarRW(c_bar->GetTWBar(), "ConstraintsId",TW_TYPE_INT32 ,&(m_selectedGOC->has(ConstraintComponent)->Id) , "group=Contraints/Joints label='Id' ");
					TwAddVarRW(c_bar->GetTWBar(), "xc",TW_TYPE_INT32 ,&xC , " step=1 min=-1 max=1 group=Contraints/Joints label='X Constraint' ");
					TwAddVarRW(c_bar->GetTWBar(), "yc",TW_TYPE_INT32 ,&yC , " step=1 min=-1 max=1 group=Contraints/Joints label='Y Constraint' ");
					TwAddVarRW(c_bar->GetTWBar(), "zc",TW_TYPE_INT32 ,&zC , " step=1 min=-1 max=1 group=Contraints/Joints label='Z Constraint' ");
					TwAddButton(c_bar->GetTWBar(), "AddContraintInBody", TweakBarManager::AddConstaintToObject,this, "group=Contraints/Joints label='Add InBody Constraint' ");
				}
				
			}
			
			
		}


		TwRefreshBar(c_bar->GetTWBar());
	}
	else// >1
	{
		TwRemoveAllVars(c_bar->GetTWBar());
		c_bar->ShowBar();
		//alignment
		TwAddButton( c_bar->GetTWBar(), "AlignX", TweakBarManager::AlighX, this, " group=Alignment label='Align X' ");
		TwAddButton( c_bar->GetTWBar(), "AlighY", TweakBarManager::AlighY, this, " group=Alignment label='Align Y' ");
		TwAddButton( c_bar->GetTWBar(), "AlighZ", TweakBarManager::AlighZ, this, " group=Alignment label='Align Z' ");
		TwAddButton( c_bar->GetTWBar(), "MakeSameSize", TweakBarManager::MakeSameSize, this, " group=Alignment label='Make Same Size' ");
		
		TwAddButton( c_bar->GetTWBar(), "AlignDistanceX", TweakBarManager::AlignDistanceX, this, " group=Alignment label='Align Distance X' ");
		TwAddButton( c_bar->GetTWBar(), "AlignDistanceZ", TweakBarManager::AlignDistanceZ, this, " group=Alignment label='Align Distance Z' ");
		TwAddButton( c_bar->GetTWBar(), "AlignDistanceY", TweakBarManager::AlignDistanceY, this, " group=Alignment label='Align Distance Y' ");
		
		TwAddButton( c_bar->GetTWBar(), "Rotate90X", TweakBarManager::Rotate90X, this, " group=Rotation label='Rotate 90 on X' ");
		TwAddButton( c_bar->GetTWBar(), "Rotate90Y", TweakBarManager::Rotate90Y, this, " group=Rotation label='Rotate 90 on y' ");
		TwAddButton( c_bar->GetTWBar(), "Rotate90Z", TweakBarManager::Rotate90Z, this, " group=Rotation label='Rotate 90 on Z' ");

		if(CheckIfAllSelectedAreGlowingCylinders())
		{
			TwAddSeparator(c_bar->GetTWBar(), "sep59", NULL);
			TwAddVarCB(c_bar->GetTWBar(), "ParentActivationId", TW_TYPE_INT32, TweakBarManager::SetAllGlowingParentId, TweakBarManager::GetetAllGlowingParentId, this, " group='Glow Cylinder' label='Parent ActivationId' ");
		}
	}

}

void TW_CALL TweakBarManager::SetAllGlowingParentId(const void *value, void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);

	for(unsigned int i=0;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		int x = *( ( int* )value );
		GOC * g = mnagerP->m_SelectedGOCs[i];
		if (g->has(GlowingCylinders))
		{
			g->has(GlowingCylinders)->m_parentActivationId = x;
		}
	}
}
void TW_CALL TweakBarManager::GetetAllGlowingParentId(void *value, void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	int prev = mnagerP->m_SelectedGOCs[0]->has(GlowingCylinders)->m_parentActivationId;
	for(unsigned int i=0;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		GOC * g = mnagerP->m_SelectedGOCs[i];
		if (g->has(GlowingCylinders))
		{
			if(g->has(GlowingCylinders)->m_parentActivationId !=prev)
			{
				prev=-1;
				break;
			}
		}
	}
	*( ( int* )value )=prev;
}


void TW_CALL TweakBarManager::AddConstaintToObject(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	
	
	if(mnagerP->m_selectedGOC->has(ConstraintComponent) && mnagerP->m_selectedGOC->has(ConstraintComponent)->Id>0)
	{
		Vector3 halfsize = mnagerP->m_selectedGOC->has(Transform)->Scale*0.5f;
		Vector3 pos = Vector3(mnagerP->xC*halfsize.x,mnagerP->yC*halfsize.y,mnagerP->zC*halfsize.z);
		mnagerP->m_selectedGOC->has(RigidBody)->position=mnagerP->m_selectedGOC->has(Transform)->Position;
		Vector3 worldpos = mnagerP->m_selectedGOC->has(RigidBody)->getPointInWorldSpace(pos);
		Vec3 mul = Vec3((float)mnagerP->xC,(float)mnagerP->yC,(float)mnagerP->zC);
		mnagerP->CreateConstraintObject( Vector3ToVec3(worldpos), Vector3ToVec3(pos),0.01, mnagerP->m_selectedGOC->has(CollisionPrimitive)->Id ,true,mul);
	}
}


void TW_CALL TweakBarManager::AddContraintComponent(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	if(!mnagerP->m_selectedGOC->has(ConstraintComponent))
	{
		ConstraintComponent * c = new ConstraintComponent();
		mnagerP->m_selectedGOC->AddComponent(CT_ConstraintComponent,c);
		mnagerP->m_selectedGOC->has(RigidBody)->hasContraint=true;
		mnagerP->m_selectedGOC->has(ConstraintComponent)->Id = mnagerP->GetNextConstraintId();

	}
	else
	{
		mnagerP->m_selectedGOC->has(RigidBody)->hasContraint=true;
	}
	mnagerP->ShowObjectDetails();
}

void TW_CALL TweakBarManager::RemoveContraintComponent(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	mnagerP->m_selectedGOC->has(RigidBody)->hasContraint=false;
//	mnagerP->ObjectsThatHaveConstraints.erase(mnagerP->m_selectedGOC->has(CollisionPrimitive)->Id);
	mnagerP->ShowObjectDetails();
}

void TW_CALL TweakBarManager::AlighX(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	for(unsigned int i=0;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		mnagerP->m_SelectedGOCs[i]->has(Transform)->Position.x = mnagerP->m_SelectedGOCs[0]->has(Transform)->Position.x;
	}
}
void TW_CALL TweakBarManager::AlighY(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	for(unsigned int i=0;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		mnagerP->m_SelectedGOCs[i]->has(Transform)->Position.y = mnagerP->m_SelectedGOCs[0]->has(Transform)->Position.y;
	}
}
void TW_CALL TweakBarManager::AlighZ(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	for(unsigned int i=0;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		mnagerP->m_SelectedGOCs[i]->has(Transform)->Position.z = mnagerP->m_SelectedGOCs[0]->has(Transform)->Position.z;
	}
}
void TW_CALL TweakBarManager::AlignDistanceX(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	float dist=abs(mnagerP->m_SelectedGOCs[0]->has(Transform)->Position.x - mnagerP->m_SelectedGOCs[1]->has(Transform)->Position.x);
	
	for(unsigned int i=1;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		mnagerP->m_SelectedGOCs[i]->has(Transform)->Position.x = mnagerP->m_SelectedGOCs[i-1]->has(Transform)->Position.x - dist;
	}
}
void TW_CALL TweakBarManager::AlignDistanceZ(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	float dist=abs(mnagerP->m_SelectedGOCs[0]->has(Transform)->Position.z - mnagerP->m_SelectedGOCs[1]->has(Transform)->Position.z);
	
	for(unsigned int i=1;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		mnagerP->m_SelectedGOCs[i]->has(Transform)->Position.z = mnagerP->m_SelectedGOCs[i-1]->has(Transform)->Position.z - dist;
	}
}

void TW_CALL TweakBarManager::AlignDistanceY(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	float dist=abs(mnagerP->m_SelectedGOCs[0]->has(Transform)->Position.y - mnagerP->m_SelectedGOCs[1]->has(Transform)->Position.y);
	
	for(unsigned int i=1;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		mnagerP->m_SelectedGOCs[i]->has(Transform)->Position.y = mnagerP->m_SelectedGOCs[i-1]->has(Transform)->Position.y - dist;
	}
}

void TW_CALL TweakBarManager::MakeSameSize(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	for(unsigned int i=0;i<mnagerP->m_SelectedGOCs.size();++i)
	{
		mnagerP->m_SelectedGOCs[i]->has(Transform)->Scale = mnagerP->m_SelectedGOCs[0]->has(Transform)->Scale;
	}
}


void TW_CALL TweakBarManager::SetRadius (const void *value, void *clientData)
{
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(Transform)->Scale.x  = (*static_cast<const float *>(value)) ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(Transform)->Scale.y = (*static_cast<const float *>(value)) ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(Transform)->Scale.z = (*static_cast<const float *>(value)) ;
}
void TW_CALL TweakBarManager::GetRadius(void *value, void *clientData)
{
	(*static_cast<float *>(value)) = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(Transform)->Scale.x;
}

void TW_CALL TweakBarManager::SetActivatedBy (const void *value, void *clientData)
{
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.x = (*static_cast<const ActivationPropertiesStruct *>(value)).Object1  ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.y = (*static_cast<const ActivationPropertiesStruct *>(value)).Object2 ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.z = (*static_cast<const ActivationPropertiesStruct *>(value)).Object3 ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.w = (*static_cast<const ActivationPropertiesStruct *>(value)).Object4 ;
}
void TW_CALL TweakBarManager::GetActivatedBy (void *value, void *clientData)
{
	(*static_cast<ActivationPropertiesStruct *>(value)).Object1 = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.x;
	(*static_cast<ActivationPropertiesStruct *>(value)).Object2 = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.y;
	(*static_cast<ActivationPropertiesStruct *>(value)).Object3 = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.z;
	(*static_cast<ActivationPropertiesStruct *>(value)).Object4 = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.w;
}


void TweakBarManager::ShowCreateObjectDetails()
{

	TweakBarControl* c_bar = GetTweakBarControl( GetStringFromEnum(MyEnum::CreateObject ) );
	
	if(c_bar->GetIsVisible())
	{
		c_bar->HideBar();
	}
	else
	{
		TwRemoveAllVars(c_bar->GetTWBar());
		//if(m_archetypes.size()0)
		//{

		std::map<std::string, const Archetype*>  tempmap = GAMEDATA->archetypes()->getArchetypeMap();
	
		int i=0;
		std::string str("");
			
		std::map<std::string,const Archetype*>::iterator it;
		for(it = tempmap.begin(); it != tempmap.end(); ++it){
			m_archetypes.insert( std::pair<int, std::string>(i, it->first) );
			str.append(it->first);
			str.append(",");
			++i;
		}
		str = str.substr(0, str.size()-1);
		TwType archetypeType;
		archetypeType = TwDefineEnumFromString("Archetype", str.c_str());
		TwAddVarRW(c_bar->GetTWBar(), "Archetype", archetypeType, &m_archEnumId , NULL);
		TwAddButton( c_bar->GetTWBar(), "Create", TweakBarManager::BuildFromArchetype, this, NULL );
		TwAddSeparator(c_bar->GetTWBar(), "sep4", NULL);
		TwAddVarRW( c_bar->GetTWBar(), "NoOfDominos", TW_TYPE_INT32, &this->dominosNo, "min= 1 step=1 group=Dominos label='Dominos No' " );	
		TwAddVarRW( c_bar->GetTWBar(), "DistanceBetweenDom", TW_TYPE_FLOAT, &this->dominosDistance, "min= 1 step=0.1 group=Dominos label='Distance Between' " );	
		TwAddButton( c_bar->GetTWBar(), "CreateDominos", TweakBarManager::CreateDominos, this, " group = Dominos label='Create Dominos'" );
		TwAddVarRW(c_bar->GetTWBar(), "LeftToRightD",TW_TYPE_BOOLCPP ,&leftToRight , "group=Dominos label='Left to Right' ");
		if(m_selectedGOC)
			TwAddButton( c_bar->GetTWBar(), "CreateDominosOnObj", TweakBarManager::CreateDominosOnObject, this, " group = Dominos label='Create Dominos on Obj'" );
		//}
		c_bar->ShowBar();

	}



	
}

void TweakBarManager::ShowGenericProperties()
{
	
	TweakBarControl* c_bar = GetTweakBarControl( GetStringFromEnum(MyEnum::GenericProperties ) );
	
	if(c_bar->GetIsVisible())
	{
		c_bar->HideBar();
	}
	else
	{
		//TwRemoveAllVars(c_bar->GetTWBar());
		c_bar->ShowBar();

	}

}

void TweakBarManager::ShowLightProperties()
{
	
	TweakBarControl* c_bar = GetTweakBarControl( GetStringFromEnum(MyEnum::LightProperties ) );
	
	if(c_bar->GetIsVisible())
	{
		c_bar->HideBar();
	}
	else
	{
		c_bar->ShowBar();

	}

}




	
void TW_CALL TweakBarManager::BuildFromArchetype(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	if(mnagerP->m_archetypes.size()>0)
	{
		GOC *g;
		std::map<int,std::string>::iterator it;
		it=mnagerP->m_archetypes.find(mnagerP->m_archEnumId);
		if ( it != mnagerP->m_archetypes.end( ) )
		{
			g=FACTORY->BuildFromArchetype(it->second.c_str());
			g->has(Transform)->Position=Vector3ToVec3( EDITOR->CenterPos + EDITOR->CenterDir*10);
			g->has(Transform)->QUATERNION=D3DXQUATERNION(0,0,0,1);
			g->Initialize();
			if (g->has(ActivationProperties))
			{
				g->has(ActivationProperties)->m_ActivationId = mnagerP->GetNextFreeActivationId();
				LOGIC->ActivatableObjectsList[g->has(ActivationProperties)->m_ActivationId]=g;
			}
			if (g->has(RopeComponent))
			{
				g->has(RopeComponent)->Id = mnagerP->GetNextRopeId ();
			}
			if (g->has(LightComponent))
			{
				EDITOR->AddLightToGame(g);			
			}
		}
	}
}



void TW_CALL TweakBarManager::CreateDominos(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	GOC *g;
	int domNo = int(static_cast<TweakBarManager*>(clientData)->dominosNo);
	int domDist = int(static_cast<TweakBarManager*>(clientData)->dominosDistance);
	float prevX=0.0f;
	for (int i=0;i<domNo;i++)
	{
		g=FACTORY->BuildFromArchetype("GOC_Type_Domino");
		Vec3 pos=Vector3ToVec3( EDITOR->CenterPos + EDITOR->CenterDir*10);
		pos.x+=prevX;
		g->has(Transform)->Position= pos;
		g->has(Transform)->QUATERNION=D3DXQUATERNION(0,0,0,1);
		ConstraintComponent * c = new ConstraintComponent();
		g->AddComponent(CT_ConstraintComponent,c);
		g->has(ConstraintComponent)->Id=mnagerP->GetNextConstraintId();
		g->has(RigidBody)->hasContraint=true;
		g->Initialize();
		prevX+=domDist;
	}
	
}

void TW_CALL TweakBarManager::CreateDominosOnObject(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	Vec3 pos;
	if(!mnagerP->m_selectedGOC ) return;
	GOC *g;
	int domNo = int(static_cast<TweakBarManager*>(clientData)->dominosNo);
	int domDist = int(static_cast<TweakBarManager*>(clientData)->dominosDistance);
	bool onX=false;
	bool onZ=false;
	int reverse=1;
	if(mnagerP->leftToRight) reverse=-1;

	if(mnagerP->m_selectedGOC->has(Transform)->Scale.x > mnagerP->m_selectedGOC->has(Transform)->Scale.z)
	{
		onX=true; 
		pos = mnagerP->m_selectedGOC->has(Transform)->Position;
		pos.x=mnagerP->m_selectedGOC->has(Transform)->Position.x - reverse * mnagerP->m_selectedGOC->has(Transform)->Scale.x*0.5f;
	}
	else
	{
		onZ=true;
		pos = mnagerP->m_selectedGOC->has(Transform)->Position;
		pos.z=mnagerP->m_selectedGOC->has(Transform)->Position.z - reverse * mnagerP->m_selectedGOC->has(Transform)->Scale.z*0.5f;
	}
	pos.y=mnagerP->m_selectedGOC->has(Transform)->Position.y + mnagerP->m_selectedGOC->has(Transform)->Scale.y*0.5f;
	
	float prev=0.0f;
	if(onX==true )
	{
		bool check=true;
		

		while (check)
		{
			g=FACTORY->BuildFromArchetype("GOC_Type_Domino");
			pos.x+=reverse*domDist;
			g->has(Transform)->Position= pos;
			g->has(Transform)->Position.y+=g->has(Transform)->Scale.y*0.5f;
			g->has(Transform)->QUATERNION=D3DXQUATERNION(0,0,0,1);
			ConstraintComponent * c = new ConstraintComponent();
			g->AddComponent(CT_ConstraintComponent,c);
			g->has(ConstraintComponent)->Id=mnagerP->GetNextConstraintId();
			g->has(RigidBody)->hasContraint=true;
			g->Initialize();

			//add constraints to the ground
			Vector3 halfsize=g->has(Transform)->Scale*0.5; 
			Vec3 relPos1=Vec3(float(halfsize.x*reverse),float(-halfsize.y),float(halfsize.z));
			Vec3 mul1 = Vec3(float(1*reverse),-1.0f,1.0f);
			Vec3 relPos2=Vec3(float(halfsize.x*reverse),float(-halfsize.y),float(-halfsize.z));
			Vec3 mul2 = Vec3(float(1*reverse),-1.0f,-1.0f);
			Vec3 worldPos1=Vector3ToVec3 ( g->has(RigidBody)->getPointInWorldSpace(relPos1));
			Vec3 worldPos2=Vector3ToVec3(g->has(RigidBody)->getPointInWorldSpace(relPos2));
			mnagerP->CreateConstraintObject(worldPos1,relPos1,0.001,g->has(ConstraintComponent)->Id,true,mul1);
			mnagerP->CreateConstraintObject(worldPos2,relPos2,0.001,g->has(ConstraintComponent)->Id,true,mul2);


			if(!mnagerP->leftToRight)
			{
				check=(pos.x < mnagerP->m_selectedGOC->has(Transform)->Position.x + mnagerP->m_selectedGOC->has(Transform)->Scale.x*0.5);
			}
			else
			{
				check=(pos.x > mnagerP->m_selectedGOC->has(Transform)->Position.x - mnagerP->m_selectedGOC->has(Transform)->Scale.x*0.5);
			}
		}
	}
	if(onZ==true )
	{
		bool check=true;
		while (check)
		{
			g=FACTORY->BuildFromArchetype("GOC_Type_Domino");
			pos.z+=reverse*domDist;
			g->has(Transform)->Position= pos;
			g->has(Transform)->Position.y+=g->has(Transform)->Scale.y*0.5f;
			D3DXQUATERNION q;
			q=calculateQuaterion(float(R_PI/2.0f),Vec3(0,1,0));
			g->has(Transform)->QUATERNION*=q;
			ConstraintComponent * c = new ConstraintComponent();
			g->AddComponent(CT_ConstraintComponent,c);
			g->has(ConstraintComponent)->Id=mnagerP->GetNextConstraintId();
			g->has(RigidBody)->hasContraint=true;
			g->Initialize();

			//add constraints to the ground
			Vector3 halfsize=g->has(Transform)->Scale*0.5f; 
			Vec3 relPos1=Vec3(float(-halfsize.x*reverse),float(-halfsize.y),float(halfsize.z));
			Vec3 mul1 = Vec3(float(-1*reverse),-1.0f,1.0f);
			Vec3 relPos2=Vec3(float(-halfsize.x*reverse),float(-halfsize.y),float(-halfsize.z));
			Vec3 mul2 = Vec3(float(-1*reverse),-1,-1);
			Vec3 worldPos1=Vector3ToVec3 ( g->has(RigidBody)->getPointInWorldSpace(relPos1));
			Vec3 worldPos2=Vector3ToVec3(g->has(RigidBody)->getPointInWorldSpace(relPos2));
			mnagerP->CreateConstraintObject(worldPos1,relPos1,0.001,g->has(ConstraintComponent)->Id,true,mul1);
			mnagerP->CreateConstraintObject(worldPos2,relPos2,0.001,g->has(ConstraintComponent)->Id,true,mul2);

			if(!mnagerP->leftToRight)
			{
				check=(pos.z < mnagerP->m_selectedGOC->has(Transform)->Position.z + mnagerP->m_selectedGOC->has(Transform)->Scale.z*0.5);
			}
			else
			{
				check=(pos.z > mnagerP->m_selectedGOC->has(Transform)->Position.z - mnagerP->m_selectedGOC->has(Transform)->Scale.z*0.5);
			}
		}
	}
	
}

void TW_CALL TweakBarManager::Rotate90X(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	if(mnagerP->m_SelectedGOCs.size()==1)
	{
		for(unsigned int i=0;i<mnagerP->m_SelectedGOCs.size();++i)
		{
			GOC * g=mnagerP->m_SelectedGOCs[i];
			D3DXQUATERNION q;
			q=calculateQuaterion(float(R_PI/2.0f),Vec3(1.0f,0.0f,0.0f));
			g->has(Transform)->QUATERNION*=q;
		}
	}
	else
	{
		real dist=1.57079633;
		D3DXQUATERNION q;
		q=calculateQuaterion((float)dist,Vec3(1,0,0));
		for(unsigned int i=0;i<EDITOR->selectedObjects.size();++i)
		{
			EDITOR->selectedObjects[i]->has(Transform)->QUATERNION*=q;
			Quaternion localQ=Quaternion(q.w,q.x,q.y,q.z);
			Vec3 temp= EDITOR->selectedObjects[i]->has(Transform)->Position-EDITOR->center;
			Vector3 dir=temp;
			Vector3 newPos=((localQ*dir)*dir.magnitude())+EDITOR->center;
			EDITOR->selectedObjects[i]->has(Transform)->Position=Vector3ToVec3(newPos);
		}
	}
}
void TW_CALL TweakBarManager::Rotate90Y(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	if(mnagerP->m_SelectedGOCs.size()==1)
	{
		for(unsigned int i=0;i<mnagerP->m_SelectedGOCs.size();++i)
		{
			GOC * g=mnagerP->m_SelectedGOCs[i];
			D3DXQUATERNION q;
			q=calculateQuaterion(float(R_PI/2.0f),Vec3(0,1,0));
			g->has(Transform)->QUATERNION*=q;
		}
	}
	else
	{
		real dist=1.57079633;
		D3DXQUATERNION q;
		q=calculateQuaterion((float)dist,Vec3(0,1,0));
		for(unsigned int i=0;i<EDITOR->selectedObjects.size();++i)
		{
			EDITOR->selectedObjects[i]->has(Transform)->QUATERNION*=q;
			Quaternion localQ=Quaternion(q.w,q.x,q.y,q.z);
			Vec3 temp= EDITOR->selectedObjects[i]->has(Transform)->Position-EDITOR->center;
			Vector3 dir=temp;
			Vector3 newPos=((localQ*dir)*dir.magnitude())+EDITOR->center;
			EDITOR->selectedObjects[i]->has(Transform)->Position=Vector3ToVec3(newPos);
		}
	}
	

}
void TW_CALL TweakBarManager::Rotate90Z(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	if(mnagerP->m_SelectedGOCs.size()==1)
	{
		D3DXQUATERNION q;
		q=calculateQuaterion(float(R_PI/2.0),Vec3(0,0,1));
		mnagerP->m_selectedGOC->has(Transform)->QUATERNION*=q;
	}
	else
	{
		real dist=1.57079633;
		D3DXQUATERNION q;
		q=calculateQuaterion((float)dist,Vec3(0,0,1));
		for(unsigned int i=0;i<EDITOR->selectedObjects.size();++i)
		{
			EDITOR->selectedObjects[i]->has(Transform)->QUATERNION*=q;
			Quaternion localQ=Quaternion(q.w,q.x,q.y,q.z);
			Vec3 temp= EDITOR->selectedObjects[i]->has(Transform)->Position-EDITOR->center;
			Vector3 dir=temp;
			Vector3 newPos=((localQ*dir)*dir.magnitude())+EDITOR->center;
			EDITOR->selectedObjects[i]->has(Transform)->Position=Vector3ToVec3(newPos);
		}
	}

}

void TW_CALL TweakBarManager::CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
  destinationClientString = sourceLibraryString;
}

void TW_CALL TweakBarManager::CopyCDStringToClient(char **destPtr, const char *src)
{
  size_t srcLen = (src!=NULL) ? strlen(src) : 0;
  size_t destLen = (*destPtr!=NULL) ? strlen(*destPtr) : 0;
 
  // alloc or realloc dest memory block if needed
  if( *destPtr==NULL )
    *destPtr = (char *)malloc(srcLen+1);
  else if( srcLen>destLen )
    *destPtr = (char *)realloc(*destPtr, srcLen+1);
 
  // copy src
  if( srcLen>0 )
    strncpy(*destPtr, src, srcLen);
  (*destPtr)[srcLen] = '\0'; // null-terminated string
}
 


void TweakBarManager::InitializeTweakBar( )
{	
	GRAPHICS->initializeAntTweakBar();
	TwCopyCDStringToClientFunc(CopyCDStringToClient); //
	TwCopyStdStringToClientFunc(CopyStdStringToClient); // must be called once (just after TwInit for instance)
	Initialized=true;
	TwDefine( " GLOBAL fontsize=2 " );
	TwStructMember myStructMembers[] = {
	 { "Object1", TW_TYPE_FLOAT, offsetof(ActivationPropertiesStruct, Object1), "label='Object 1' step=1 min=0 max=100" },
	 { "Object2", TW_TYPE_FLOAT, offsetof(ActivationPropertiesStruct, Object2), "label='Object 2' step=1 min=0 max=100" },
	 { "Object3", TW_TYPE_FLOAT, offsetof(ActivationPropertiesStruct, Object3), "label='Object 3' step=1 min=0 max=100" },
	 { "Object4", TW_TYPE_FLOAT, offsetof(ActivationPropertiesStruct, Object4), "label='Object 4' step=1 min=0 max=100" }

	 };
	ActivationPropertiesStructType = TwDefineStruct("ActivationPropertiesStructType", myStructMembers, 4, sizeof(ActivationPropertiesStruct), NULL, NULL);


	InitializeBars();
	//HideAllBars();
}

void TweakBarManager::InitializeBars()
{
	//OBJECT
	TweakBarControl* bar;
	bar  =  new TweakBarControl(GetStringFromEnum(MyEnum::Object),50,50,300,500,false);
	m_twbarlist.insert( std::pair<const char*, TweakBarControl*>( bar->GetName(),bar ) );


	//ARCHETYPE
	bar  =  new TweakBarControl(GetStringFromEnum(MyEnum::CreateObject),GRAPHICS->screen_width-300,50,250,300,false);
	m_twbarlist.insert( std::pair<const char*, TweakBarControl*>( bar->GetName(),bar ) );

	//GENERIC
	bar  =  new TweakBarControl(GetStringFromEnum(MyEnum::GenericProperties),GRAPHICS->screen_width-330,GRAPHICS->screen_height-300,320,250,false);
	m_twbarlist.insert( std::pair<const char*, TweakBarControl*>( bar->GetName(),bar ) );
	TwAddVarRW(bar->GetTWBar(), "ShowLevelBoxes",TW_TYPE_BOOLCPP ,&EDITOR->m_showLevelBoxes , "group=Generic label='Show Level Boxes' ");
	TwAddVarRW(bar->GetTWBar(), "ShowExcludedFromboxes",TW_TYPE_BOOLCPP ,&EDITOR->m_showBoxExcluded , "group=Generic label='Show Excluded from Boxes' ");
	
	//LIGHTPROPERTIES
	bar  =  new TweakBarControl(GetStringFromEnum(MyEnum::LightProperties),GRAPHICS->screen_width-330,GRAPHICS->screen_height-300,320,350,false);
	m_twbarlist.insert( std::pair<const char*, TweakBarControl*>( bar->GetName(),bar ) );
	TwAddVarRW(bar->GetTWBar(), "BloomThreshold",TW_TYPE_FLOAT ,&GRAPHICS->BloomThreshold , "step=0.1 min=0 precision=4 group=Generic label='BloomThreshold' ");
	TwAddVarRW(bar->GetTWBar(), "BloomMagnitude",TW_TYPE_FLOAT ,&GRAPHICS->BloomMagnitude , "step=0.1 min=0 precision=4 group=Generic label='BloomMagnitude' ");
	TwAddVarRW(bar->GetTWBar(), "BloomBlurSigma",TW_TYPE_FLOAT ,&GRAPHICS->BloomBlurSigma , "step=0.1 min=0 precision=4 group=Generic label='BloomBlurSigma' ");
	TwAddVarRW(bar->GetTWBar(), "AdaptationRate",TW_TYPE_FLOAT ,&GRAPHICS->AdaptationRate , "step=0.1 min=0 precision=4 group=Generic label='AdaptationRate' ");
	TwAddVarRW(bar->GetTWBar(), "ExposureKeyValue",TW_TYPE_FLOAT ,&GRAPHICS->ExposureKeyValue , "step=0.1 min=0 precision=4.01 group=Generic label='ExposureKeyValue' ");
	TwAddVarRW(bar->GetTWBar(), "SpecularIntensity",TW_TYPE_FLOAT ,&GRAPHICS->SpecularIntensity , "step=0.1 min=0 precision=4 group=Generic label='SpecularIntensity' ");
	TwAddVarRW(bar->GetTWBar(), "SpecularR",TW_TYPE_FLOAT ,&GRAPHICS->SpecularR , "step=0.1 min=0 precision=4 max=1 group=Generic label='SpecularR' ");
	TwAddVarRW(bar->GetTWBar(), "SpecularG",TW_TYPE_FLOAT ,&GRAPHICS->SpecularG , "step=0.1 min=0 precision=4 max=1 group=Generic label='SpecularG' ");
	TwAddVarRW(bar->GetTWBar(), "SpecularB",TW_TYPE_FLOAT ,&GRAPHICS->SpecularB , "step=0.1 min=0 precision=4 max=1 group=Generic label='SpecularB' ");
	TwAddVarRW(bar->GetTWBar(), "Balance",TW_TYPE_FLOAT ,&GRAPHICS->Balance , "step=0.1 min=0 precision=4 group=Generic label='Balance' ");
	TwAddVarRW(bar->GetTWBar(), "Roughness",TW_TYPE_FLOAT ,&GRAPHICS->Roughness , "step=0.1 min=0 precision=4 group=Generic label='Roughness' ");
	TwAddVarRW(bar->GetTWBar(), "ConstAmbient",TW_TYPE_COLOR4F ,&GRAPHICS->ConstAmbient , " group=Generic label='ConstAmbient' ");
	TwAddVarRW(bar->GetTWBar(), "SkyColor",TW_TYPE_COLOR4F ,&GRAPHICS->SkyColor , " group=Generic label='SkyColor' ");
	TwAddVarRW(bar->GetTWBar(), "GroundColor",TW_TYPE_COLOR4F ,&GRAPHICS->GroundColor , " group=Generic label='GroundColor' ");
	TwAddVarRW(bar->GetTWBar(), "HemiIntensity",TW_TYPE_FLOAT ,&GRAPHICS->HemiIntensity , "step=0.1 min=0 precision=4 group=Generic label='HemiIntensity' ");



	
}

TweakBarControl* TweakBarManager::GetTweakBarControl( const char* s )
{
	std::map<const char*,TweakBarControl*>::iterator it;
	it=m_twbarlist.find(s);
	m_twbarlist.end();
	return it->second;

}

void TweakBarManager::HideAllBars()
{
	std::map<const char*,TweakBarControl*>::iterator it;
	for(it = m_twbarlist.begin(); it != m_twbarlist.end(); ++it){
		it->second->HideBar();
	}
}
void TweakBarManager::ShowAllBars()
{
	std::map<const char*,TweakBarControl*>::iterator it;
	for(it = m_twbarlist.begin(); it != m_twbarlist.end(); ++it){
		it->second->ShowBar();
	}

}

void TweakBarManager::Update()
{
	GOC * tempGOC;
	if (m_SelectedGOCs.size()>0)
	{
		for (unsigned i=0;i<m_SelectedGOCs.size();++i)
		{
			tempGOC=m_SelectedGOCs[i];
			if(tempGOC)
			{
				if(tempGOC->has(ConstraintComponent) && tempGOC->CompositionTypeId != GOC_Type_Constraint)//move all contraints when u move the object
				{
					tempGOC->has(RigidBody)->position=tempGOC->has(Transform)->Position;
					tempGOC->has(RigidBody)->orientation=tempGOC->has(Transform)->QUATERNION;
					tempGOC->has(RigidBody)->orientation.normalise();
					tempGOC->has(RigidBody)->calculateDerivedData();

					GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
					GameObjectIdMapType::iterator p;
					for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++){
						if(p->second->CompositionTypeId==GOC_Type_Constraint && p->second->has(ConstraintComponent)->AttachedToId == tempGOC->has(ConstraintComponent)->Id  && p->second->has(ConstraintComponent)->isInsideBody )
						{
							float x,y,z;
							x=p->second->has(ConstraintComponent)->multiplier.x;
							y=p->second->has(ConstraintComponent)->multiplier.y;
							z=p->second->has(ConstraintComponent)->multiplier.z;
							Vector3 halfsize = tempGOC->has(Transform)->Scale*0.5f;
							Vector3 pos = Vector3(x*halfsize.x,y*halfsize.y,z*halfsize.z);
							Vector3 worldpos = tempGOC->has(RigidBody)->getPointInWorldSpace(pos);
						
							p->second->has(Transform)->Position = Vector3ToVec3(worldpos);
							p->second->has(ConstraintComponent)->PosRelative = pos;
						}
					}
				}

				if(tempGOC->has(CollisionPrimitive))
				{
					if(tempGOC->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
					{
						static_cast<CollisionBox*>(tempGOC->has(CollisionPrimitive))->halfSize.x=tempGOC->has(Transform)->Scale.x/2.0;
						static_cast<CollisionBox*>(tempGOC->has(CollisionPrimitive))->halfSize.y=tempGOC->has(Transform)->Scale.y/2.0;
						static_cast<CollisionBox*>(tempGOC->has(CollisionPrimitive))->halfSize.z=tempGOC->has(Transform)->Scale .z/2.0;
					}else if(tempGOC->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
					{
						static_cast<CollisionSphere*>(tempGOC->has(CollisionPrimitive))->radius=tempGOC->has(Transform)->Scale .x/2.0;
					}
				}
			}
		}
	}

	
}

const char* TweakBarManager::GetStringFromEnum(MyEnum e)
{
  switch (e)
  {
	  case Object: return "ObjectDetails";
	  case CreateObject: return "CreateObject";
	  case MultipleObjects: return "MultipleObjects";
	  case GenericProperties: return "GenericProperties";
	  case LightProperties: return "LightProperties";
	  default: return "nothing";
  }
}

TweakBarManager::MyEnum TweakBarManager::GetEnumFromString(const char* s)
{
	if (std::strcmp(s,"ObjectDetails")==0)
		return MyEnum::Object ;
	else if (std::strcmp(s,"CreateObject")==0)
		return MyEnum::CreateObject ;
	else if (std::strcmp(s,"MultipleObjects")==0)
		return MyEnum::MultipleObjects  ;
	else if (std::strcmp(s,"GenericProperties")==0)
		return MyEnum::GenericProperties  ;
	else if (std::strcmp(s,"LightProperties")==0)
		return MyEnum::LightProperties;

	return MyEnum::Nothing;
}
bool TweakBarManager::CheckIfAllSelectedAreGlowingCylinders()
{
	for(unsigned int i=0;i<m_SelectedGOCs.size();++i)
	{
		if(!m_SelectedGOCs[i]->has(GlowingCylinders))
			return false;
	}
	return true;
}

int TweakBarManager::GetNextFreeActivationId()
{
	std::map<int,GOC*>::iterator  it;
	int j=0;
	bool found = true;
	while(found)
	{
		found=false;
		j++;
		for(it=LOGIC->ActivatableObjectsList.begin();it!=LOGIC->ActivatableObjectsList.end();++it)
		{
			if(it->second->has(ActivationProperties)->GetActivationId()==j)
			{
				found=true;
				break;
			}
		}
		
	}
	return j;
}

int TweakBarManager::GetNextConstraintId()
{
	std::map<int,bool>::iterator  it;
	int j=0;
	bool found = true;
	while(found)
	{
		found=false;
		j++;
		for(it=m_constraintIdsTaken.begin();it!=m_constraintIdsTaken.end();++it)
		{
			if(it->first==j)
			{
				found=true;
				break;
			}
		}
		
	}
	PushContraintId(j);
	return j;
}


int TweakBarManager::GetNextRopeId()
{
	std::map<int,bool>::iterator  it;
	int j=0;
	bool found = true;
	while(found)
	{
		found=false;
		j++;
		for(it=m_ropeIdsTaken.begin();it!=m_ropeIdsTaken.end();++it)
		{
			if(it->first==j)
			{
				found=true;
				break;
			}
		}
		
	}
	PushRopeId(j);
	return j;
}








//==================================================================================== TweakBarManager





}

