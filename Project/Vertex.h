#pragma once

class Vertex {
public:
	XMFLOAT3 m_xmf3Position;
};

class ModelVertex : public Vertex {
public:
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT2 m_xmf2TexCoord;
};

class AnimatedModelVertex : public Vertex {
public:
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT3 m_xmf3Tangent;
	XMFLOAT2 m_xmf2TexCoord;
	XMFLOAT4 m_xmf4BoneWeight;
	XMINT4	 m_xmi4BoneIdx;
};

class UIVertex : public Vertex {
public:
	XMFLOAT2 m_xmf2TexCoord;
};