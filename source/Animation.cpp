///////////////////////////////////////////////////////////////////////////////////////
//
//	Animation.cpp
//	Authors: 
//	Copyright 2012, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Animation.h"
#include "FileElements.hpp"
#include "ChunkReader.h"
namespace Framework{

inline D3DXQUATERNION Slerp(D3DXQUATERNION& q1,D3DXQUATERNION& q2, float param)
{
	//
	// Quaternion Interpolation With Extra Spins, pp. 96f, 461f
	// Jack Morrison, Graphics Gems III, AP Professional
	//

	D3DXQUATERNION qt;

	float alpha, beta;
	float cosom = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w; 
	float slerp_epsilon = 0.00001f;

	bool flip;

	if (flip = (cosom < 0))
		cosom = -cosom;

	if ((1.0 - cosom) > slerp_epsilon)
	{
		float omega = acos (cosom);
		float sinom = sin (omega);
		alpha = (float)(sin((1.0 - param) * omega) / sinom);
		beta = (float)(sin(param * omega) / sinom);
	}
	else
	{
		alpha = (float)(1.0 - param);
		beta = (float)param;
	}

	if (flip) beta = -beta;

	qt.x = (float) (alpha*q1.x + beta*q2.x);
	qt.y = (float) (alpha*q1.y + beta*q2.y);
	qt.z = (float) (alpha*q1.z + beta*q2.z);
	qt.w = (float) (alpha*q1.w + beta*q2.w);

	return qt;
}

void Skel::ReadFrom( ChunkReader& file )
{
	GChunk skeletonChunk = file.ReadChunkHeader();

	unsigned int NumberOfBones = 0;
	file.Read( NumberOfBones );

	bones.resize( NumberOfBones );
	for(unsigned int i=0;i<NumberOfBones;++i)
	{
		file.Read( bones[i].Name );
		file.Read( bones[i].ParentBoneIndex );
		file.Read( bones[i].BindT );
		file.Read( bones[i].BindR );
		file.Read( bones[i].ModelToBoneSpaceT );
		file.Read( bones[i].ModelToBoneSpaceR );
	}
}

void Skel::Initialize()
{
	for(unsigned int i=0;i<bones.size();++i)
	{
		Bone& bone = bones[i];
		bone.BoneIndex = i;

		if( bone.ParentBoneIndex != -1 ){ 
			bones[ bone.ParentBoneIndex ].Children.push_back( &bone );
		}else{
			RootBones.push_back( &bone );
		}

		bone.ModelToBoneSpace = BuildTransform(bone.ModelToBoneSpaceT , bone.ModelToBoneSpaceR);
		//bone.BindTransform = BuildTransform(bone.BindT , bone.BindR);

	}
}

D3DXMATRIX& Skel::GetModelToBoneSpaceTransform(int boneIndex)
{
	return bones[boneIndex].ModelToBoneSpace;
}

void Skel::ProcessBindPose(MatrixBuffer& buffer) const
{
	for(unsigned int i=0;i<bones.size();++i)
		buffer[i] = BuildTransform( bones[i].BindT , bones[i].BindR );
}

void Skel::ProcessAnimationGraph(float time,MatrixBuffer& buffer, Animation& anim,TrackBuffer& trackData) const
{
	//Linear Form This only works because the bone parent is always guaranteed to be in front of its children (breath first order)
	D3DXMATRIX identity;
	D3DXMatrixIdentity( &identity );
	for( unsigned int boneIndex=0;boneIndex<bones.size();++boneIndex)
	{
		const Bone& bone = bones[boneIndex];
		AnimTransform atx;
		anim.CalculateTransform( time , boneIndex , atx , trackData[boneIndex] );
		D3DXMATRIX parentTransform = bone.ParentBoneIndex != -1 ?  buffer[ bone.ParentBoneIndex ] :  identity;
		/*
		if( bone.ParentBoneIndex != -1 ) parentTransform = buffer[ bone.ParentBoneIndex ];
		else parentTransform = identity;
		*/
		D3DXMATRIX localTransform = atx.GetMatrix();
		D3DXMATRIX modelTransform ;
		D3DXMatrixMultiply( &modelTransform, &localTransform  , &parentTransform);
		buffer[boneIndex] = modelTransform;
	}
}

D3DXMATRIX AnimTransform::GetMatrix()
{
	return BuildTransform( T , R );
}

void Animation::CalculateTransform(float animTime,unsigned int trackIndex,AnimTransform& atx,TrackData& data)
{
	//Since keys are not spaced at regular intervals we need to search
	//for the keyframes that will be interpolated between.  The track data is
	//used to store what the last keyframe was to prevent searching the entire
	//track.
	int CurKey = data.LastKey;
	Track& CurPath = Tracks[trackIndex];

	//Search Forward in the keyframes for the interval
	while( CurKey!= CurPath.KeyFrames.size() - 1 && 
		CurPath.KeyFrames[ CurKey + 1 ].time < animTime  )
		++CurKey;

	//Search Backward in the keyframes for the interval
	while( CurKey!= 0 && CurPath.KeyFrames[ CurKey ].time > animTime  )
		--CurKey;

	if( CurKey == CurPath.KeyFrames.size()  - 1 )
	{
		//Past the last keyframe for this path so use the last frame and the transform data
		//so the animation is clamped to the last frame
		atx.R = CurPath.KeyFrames[CurKey].R;
		atx.T = CurPath.KeyFrames[CurKey].T;
	}
	else
	{
		//Generate transform data by interpolating between the two keyframes
		KeyFrame& KeyOne = CurPath.KeyFrames[ CurKey ];
		KeyFrame& KeyTwo = CurPath.KeyFrames[ CurKey + 1 ];

		float t1 = KeyOne.time;
		float t2 = KeyTwo.time;

		//Normalize the distance between the two keyframes
		float segLen = t2 - t1;
		float segStart = animTime - t1;
		float segNormalizedT = segStart/segLen;


		//Slerp Example Using D3DX
		//D3DXQuaternionSlerp( &atx.R , &KeyOne.R , &KeyTwo.R , segNormalizedT  );

		//Using provided slerp function as an example
		atx.R = Slerp( KeyOne.R , KeyTwo.R , segNormalizedT );

		//standard linear interpolation
		atx.T = ( 1.0f - segNormalizedT ) * KeyOne.T  + segNormalizedT *  KeyTwo.T;
	}

	//Remember the last keyframe
	data.LastKey = CurKey;
}

void Animation::ReadFrom(ChunkReader& file)
{
	GChunk animation = file.ReadChunkHeader();

	unsigned int NumberOfTracks = 0;
	file.Read( Duration );
	file.Read( NumberOfTracks );
	Tracks.resize( NumberOfTracks );
	for( unsigned int  t=0 ; t < NumberOfTracks ; ++t)
	{
		unsigned int NumberOfKeyframes = 0;
		file.Read( NumberOfKeyframes );
		Tracks[t].KeyFrames.resize( NumberOfKeyframes );
		file.ReadArray( &Tracks[t].KeyFrames[0] , NumberOfKeyframes );
	}
}

void AnimationController::Process( const Skel* const _skel  )
{	
	_skel->ProcessAnimationGraph( animTime , BoneMatrixBuffer , *ActiveAnim , AnimTrackData );
}

void AnimationController::ProcessBindPose( const Skel* const _skel )
{
	_skel->ProcessBindPose( BoneMatrixBuffer );
}

AnimationController::AnimationController() : 
animTime( 0.0f ), 
animSpeed( 1.0f ), 
ActiveAnim( NULL )
{
}

AnimationController::AnimationController( const AnimationContent* const _ac ) : 
animTime( 0.0f ), 
animSpeed( 1.0f ), 
animPause(true)
{
	BoneMatrixBuffer.resize( _ac->skel->bones.size() );
	AnimTrackData.resize( _ac->skel->bones.size() );
	for(unsigned int i=0;i<AnimTrackData.size();++i)
			AnimTrackData[i].LastKey = 0;
	ActiveAnim = _ac->Animations[0];
}

AnimationController::~AnimationController()
{
	
}

void AnimationController::Update(float dt)
{
	animTime+=dt*animSpeed;
	//Just loop forever
	if( animTime > ActiveAnim->Duration )
	{
		animTime =  0.0f;
		//ClearTrackData();
		//Reset all the keys back to zero
		for(unsigned int i=0;i<AnimTrackData.size();++i)
			AnimTrackData[i].LastKey = 0;
	}
}

AnimationContent::AnimationContent() : 
skel(NULL)
{
}

AnimationContent::~AnimationContent()
{
	SafeRelease( skel );
	for( unsigned int i=0;i<Animations.size();++i)
		Animations[i]->Release();
}

void AnimationContent::AddAnimation(Animation * newAnim)
{
	Animations.push_back( newAnim );
	newAnim->AddRef();
}

void AnimationContent::SetSkel(Skel * pSkel)
{
	pSkel->AddRef();
	skel = pSkel;
}

}