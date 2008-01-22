#ifndef __PSK_H__
#define __PSK_H__


/******************************************************************************
 *	PSK file format structures
 *****************************************************************************/

// common (psk and psa) structure: chunk header
struct VChunkHeader
{
	char			ChunkID[20];			// text identifier
	int				TypeFlag;				// version: 1999801 or 2003321
	int				DataSize;				// sizeof(type)
	int				DataCount;				// number of array elements

	friend FArchive& operator<<(FArchive &Ar, VChunkHeader &H)
	{
		SerializeChars(Ar, ARRAY_ARG(H.ChunkID));
		return Ar << H.TypeFlag << H.DataSize << H.DataCount;
	}
};


struct VVertex
{
	int				PointIndex;				// short, padded to int
	float			U, V;
	byte			MatIndex;
	byte			Reserved;
	short			Pad;					// not used

	friend FArchive& operator<<(FArchive &Ar, VVertex &V)
	{
		Ar << V.PointIndex << V.U << V.V << V.MatIndex << V.Reserved << V.Pad;
		if (Ar.IsLoading)
			V.PointIndex &= 0xFFFF;			// clamp padding bytes
		return Ar;
	}
};


struct VMaterial
{
	char			MaterialName[64];
	int				TextureIndex;
	unsigned		PolyFlags;
	int				AuxMaterial;
	unsigned		AuxFlags;
	int				LodBias;
	int				LodStyle;

	friend FArchive& operator<<(FArchive &Ar, VMaterial &M)
	{
		SerializeChars(Ar, ARRAY_ARG(M.MaterialName));
		return Ar << M.TextureIndex << M.PolyFlags << M.AuxMaterial <<
					 M.AuxFlags << M.LodBias << M.LodStyle;
	}
};


struct VBone
{
	char			Name[64];
	unsigned		Flags;
	int				NumChildren;
	int				ParentIndex;			// 0 if this is the root bone.
	VJointPos		BonePos;

	friend FArchive& operator<<(FArchive &Ar, VBone &B)
	{
		SerializeChars(Ar, ARRAY_ARG(B.Name));
		return Ar << B.Flags << B.NumChildren << B.ParentIndex << B.BonePos;
	}
};


struct VRawBoneInfluence
{
	float			Weight;
	int				PointIndex;
	int				BoneIndex;

	friend FArchive& operator<<(FArchive &Ar, VRawBoneInfluence &I)
	{
		return Ar << I.Weight << I.PointIndex << I.BoneIndex;
	}
};


/******************************************************************************
 *	PSA file format structures
 *****************************************************************************/

// Binary bone format to deal with raw animations as generated by various exporters.
struct FNamedBoneBinary
{
	char			Name[64];				// Bone's name
	unsigned		Flags;					// reserved
	int				NumChildren;
	int				ParentIndex;			// 0/NULL if this is the root bone.
	VJointPos		BonePos;

	friend FArchive& operator<<(FArchive &Ar, FNamedBoneBinary &B)
	{
		SerializeChars(Ar, ARRAY_ARG(B.Name));
		return Ar << B.Flags << B.NumChildren << B.ParentIndex << B.BonePos;
	}
};


// Binary animation info format - used to organize raw animation keys into FAnimSeqs on rebuild
// Similar to MotionChunkDigestInfo.
struct AnimInfoBinary
{
	char			Name[64];				// Animation's name
	char			Group[64];				// Animation's group name

	int				TotalBones;				// TotalBones * NumRawFrames is number of animation keys to digest.

	int				RootInclude;			// 0 none 1 included (unused)
	int				KeyCompressionStyle;	// Reserved: variants in tradeoffs for compression.
	int				KeyQuotum;				// Max key quotum for compression
	float			KeyReduction;			// desired
	float			TrackTime;				// explicit - can be overridden by the animation rate
	float			AnimRate;				// frames per second.
	int				StartBone;				// Reserved: for partial animations
	int				FirstRawFrame;
	int				NumRawFrames;			// NumRawFrames and AnimRate dictate tracktime...

	friend FArchive& operator<<(FArchive &Ar, AnimInfoBinary &A)
	{
		SerializeChars(Ar, ARRAY_ARG(A.Name));
		SerializeChars(Ar, ARRAY_ARG(A.Group));
		return Ar << A.TotalBones << A.RootInclude << A.KeyCompressionStyle <<
					 A.KeyQuotum << A.KeyReduction << A.TrackTime << A.AnimRate <<
					 A.StartBone << A.FirstRawFrame << A.NumRawFrames;
	}
};


struct VQuatAnimKey
{
	FVector			Position;				// Relative to parent
	FQuat			Orientation;			// Relative to parent
	float			Time;					// The duration until the next key (end key wraps to first ...)

	friend FArchive& operator<<(FArchive &Ar, VQuatAnimKey &K)
	{
		return Ar << K.Position << K.Orientation << K.Time;
	}
};


#define SAVE_CHUNK(var, name)	\
	strcpy(var.ChunkID, name);	\
	var.TypeFlag = 1999801;		\
	Ar << var;


#endif // __PSK_H__