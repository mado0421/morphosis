//------------------------------------------------------- ----------------------
// File: Font.h
//-----------------------------------------------------------------------------
#pragma once
#include "Object/Object.h"

struct FontChar
{
	// �����ڵ� ���̵�
	int id;

	// �ؽ��� ��ǥ�� ��ȯ�ʿ� , 0~1 ����
	// uv ��ǥ
	float u;
	float v;
	// �ؽ��Ŀ����� ��Ʈ �ʺ�� ����
	float twidth;
	float theight;

	// ��ũ�� ��ǥ��󿡼��� ��Ʈ�� ���̿� �ʺ�
	float width;
	float height;

	// ��Ʈ ����� ������� ����ȭ�Ǿ���Ѵ�.
	float xoffset; // ���� Ŀ�� ��ġ���� ���������� �󸶳� ������ �ִ°�
	float yoffset; // ������ ���κп������� �󸶳� �۾��� �������ֳ�
	float xadvance; // ���� ���ڱ��� �󸶳� ������ �ִ°�
};

struct FontKerning
{
	int firstid; // ù��° ������ ���̵�
	int secondid; // �ι�° ������ ���̵�
	float amount; // �ι�° ���ڰ� �󸶳� �߰������� ����������, �Ǵ� ���̴���
};

class CFont : public CGameObject
{
public:
	static constexpr wchar_t* DefaultFontPath{ L"../../../Assets/Textures/Font/Default32.fnt" };

	struct VertexController
	{
		CTextVertex& textvertex;
		VertexController(CTextVertex& vertex) : textvertex(vertex) {}
	};

private:
	std::wstring						name;			// ��Ʈ �̸�
	std::wstring						fontImage;
	int									size;			// ��Ʈ ������,
	float								lineHeight;		// �����ٰ� ������ ����
	float								baseHeight;		// ��� ������ ����
	int									textureWidth;	// ��Ʈ �ؽ����� ���� ����
	int									textureHeight;	// ��Ʈ �ؽ����� ���� ����
	int									numCharacters;	// ��Ʈ ���� ����
	FontChar*							CharList;		// ���� ����Ʈ
	int									numKernings;	// ���� ���� ���� ��
	FontKerning*						KerningsList;	// Ŀ�װ� ���� ���

	// �󸶳� ���ڰ� �е��� �̷���� �ִ���?
	// �ؽ��Ļ󿡼� �� ���� �ִ� �ٸ� ������ �Ϻΰ� �߸����·� �������Ǵ°��� �����ϰ��� ��
	float								leftpadding;
	float								toppadding;
	float								rightpadding;
	float								bottompadding;

	UINT								m_nControllers;
	VertexController**					m_ppControllers;

	UINT								m_currentVertexIndex;

public:
	CFont(
		  ID3D12Device*					pd3dDevice
		, ID3D12GraphicsCommandList*	pd3dCmdLst
		, eTex::Font					eTexture
		, LPCWSTR						filename
		, UINT							maxControlVertex
		, int							windowWidth
		, int							windowHeight);
	virtual ~CFont();

	void LoadFont(LPCWSTR filename, int windowWidth, int windowHeight);

	float		GetKerning(wchar_t first, wchar_t second);	// �� ���ڰ� ���� ������ ��ȯ
	FontChar*	GetChar(wchar_t c);							// wchar_t�� �����Ǵ� FontChar�� ��ȯ.

	// �� ������ ó���� ȣ���� ��.
	void ResetTextData();
	void AppendTextData(std::wstring	text
		, XMFLOAT2						pos
		, XMFLOAT2						scale = XMFLOAT2(1.0f, 1.0f)
		, XMFLOAT2						padding = XMFLOAT2(0.5f, 0.0f)
		, XMFLOAT4						color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	virtual void Render(
		  ID3D12GraphicsCommandList*	pd3dCmdLst
		, CCamera*						pCamera);
};