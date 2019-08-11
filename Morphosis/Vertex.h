#pragma once

class CVertex {
public:
	XMFLOAT3	m_xmf3Position;
};

class CModelVertex : public CVertex {
public:
	XMFLOAT3	m_xmf3Normal;
	XMFLOAT2	m_xmf2TexCoord;
};

class CAnimatedVertex : public CVertex {
public:
	XMFLOAT3	m_xmf3Normal;
	XMFLOAT3	m_xmf3Tangent;
	XMFLOAT2	m_xmf2TexCoord;
	XMFLOAT4	m_xmf4BoneWeight;
	XMINT4		m_xmi4BoneIdx;
};

class CUIVertex : public CVertex {
public:
	XMFLOAT2	m_xmf2TexCoord;
};