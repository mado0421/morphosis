//-----------------------------------------------------------------------------
// File: Font.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Font.h"

CFont::CFont(
	  ID3D12Device*					pd3dDevice
	, ID3D12GraphicsCommandList*	pd3dCmdLst
	, eTex::Font					eTexture
	, LPCWSTR						filename
	, UINT							maxControlVertex
	, int							windowWidth
	, int							windowHeight)
	: m_currentVertexIndex{ 0 }
{
	LoadFont(filename, windowWidth, windowHeight);

	// ���ؽ� �ִ� ������ ���⼭ ����
	shared_ptr<CFontMesh> fontmesh = make_shared<CFontMesh>(pd3dCmdLst, maxControlVertex);

	m_nControllers = fontmesh->GetnVertices();
	m_ppControllers = new VertexController*[m_nControllers];
	for (int i = 0; i < m_nControllers; ++i)
	{
		m_ppControllers[i] = new VertexController(fontmesh->GetVertices()[i]);
	}

	SetMesh(0, fontmesh);
	CreateShaderVariables();

	CResMgr* pRes = CResMgr::Instance();
	shared_ptr<CTexture> pTexture = make_shared<CTexture>(1, pd3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, eTex::Type::RESOURCE_TEXTURE2D_ARRAY);
	pTexture->CreateTextureSet(0, 1, ROOT_PARAMETER_TEXTURE1);
	pTexture->SetTexture(pd3dDevice, 0, 0, pRes->GetTexture(eTexture));

	shared_ptr<CMaterial> pMaterial = make_shared<CMaterial>();
	pMaterial->SetTexture(pTexture);
	pMaterial->SetGlobalMaterialIdx(0);
	pMaterial->SetShader(pRes->GetShader(ePSO::Graphic::Font));
	SetMaterial(pMaterial);
}
CFont::~CFont()
{
	for (int i = 0; i < m_nControllers; ++i)
		delete m_ppControllers[i];
	delete[] m_ppControllers;
}

void CFont::LoadFont(LPCWSTR filename, int windowWidth, int windowHeight)
{
	std::wifstream fs;
	fs.open(filename);

	std::wstring tmp;
	int startpos;

	// ��Ʈ �̸�
	// ��Ʈ �̸��� ���Ⱑ ������ �ȵ��ư�, 
	// ���� �� �κп��� �̸��бⰡ ���������� ����
	fs >> tmp >> tmp; // info face=""
	startpos = tmp.find(L'"') + 1;
	name = tmp.substr(startpos, tmp.size() - startpos - 1);

	// ��Ʈ ������ �޾Ƴ���
	fs >> tmp; // size=""
	startpos = tmp.find(L"=") + 1;
	size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
	fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp;
	// bold= italic= charset="" unicode= stretchH= smooth= aa=

	// get padding
	fs >> tmp; // padding=
	startpos = tmp.find(L"=") + 1;
	tmp = tmp.substr(startpos, tmp.size() - startpos);

	// up padding
	startpos = tmp.find(L",") + 1;
	toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// right padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	// down padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

	//  left padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	leftpadding = std::stoi(tmp) / (float)windowWidth;

	fs >> tmp; // spacing=

			   // lineheight
	fs >> tmp >> tmp; // common lineHeight=
	startpos = tmp.find(L"=") + 1;
	lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// �⺻ ����, ����ȭ
	fs >> tmp; // base=
	startpos = tmp.find(L"=") + 1;
	baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

	// texture width
	fs >> tmp; // scaleW=
	startpos = tmp.find(L"=") + 1;
	textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// texture height
	fs >> tmp; // scaleH=
	startpos = tmp.find(L"=") + 1;
	textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// pages, packed, page id
	fs >> tmp >> tmp; // pages= packed=
	fs >> tmp >> tmp; // page id=

					  // �ؽ��� ���� �̸�
	std::wstring wtmp;
	fs >> wtmp; // file=
	startpos = wtmp.find(L'"') + 1;
	fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

	// ���� �� �ޱ�
	fs >> tmp >> tmp; // chars count=
	startpos = tmp.find(L"=") + 1;
	numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// �ʱ�ȭ
	CharList = new FontChar[numCharacters];

	for (int c = 0; c < numCharacters; ++c)
	{
		// �����ڵ� id
		fs >> tmp >> tmp; // char id=
		startpos = tmp.find(L"=") + 1;
		CharList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		//  x
		fs >> tmp; // x=
		startpos = tmp.find(L"=") + 1;
		CharList[c].u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)textureWidth;

		//y
		fs >> tmp; // y=
		startpos = tmp.find(L"=") + 1;
		CharList[c].v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)textureHeight;

		// get width
		fs >> tmp; // width=
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		CharList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
		CharList[c].twidth = (float)std::stoi(tmp) / (float)textureWidth;

		//height
		fs >> tmp; // height=
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		CharList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
		CharList[c].theight = (float)std::stoi(tmp) / (float)textureHeight;

		//xoffset
		fs >> tmp; // xoffset=
		startpos = tmp.find(L"=") + 1;
		CharList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// yoffset
		fs >> tmp; // yoffset=
		startpos = tmp.find(L"=") + 1;
		CharList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

		// xadvance
		fs >> tmp; // xadvance=
		startpos = tmp.find(L"=") + 1;
		CharList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

		// page
		// channel
		fs >> tmp >> tmp; // page= chnl=
	}

	// ���ڰ� ���� <Ŀ��> ����
	fs >> tmp >> tmp; // kernings count=
	startpos = tmp.find(L"=") + 1;
	numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// Ŀ�� ����Ʈ �ʱ�ȭ
	KerningsList = new FontKerning[numKernings];

	for (int k = 0; k < numKernings; ++k)
	{
		// ù����
		fs >> tmp >> tmp; // kerning first=
		startpos = tmp.find(L"=") + 1;
		KerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// �ι�° ����
		fs >> tmp; // second=
		startpos = tmp.find(L"=") + 1;
		KerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// ���� , ���̴� �� �ޱ�
		fs >> tmp; // amount=
		startpos = tmp.find(L"=") + 1;
		int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
		KerningsList[k].amount = (float)t / (float)windowWidth;
	}
}

float CFont::GetKerning(wchar_t first, wchar_t second)
{
	for (int i = 0; i < numKernings; ++i) {
		if ((wchar_t)KerningsList[i].firstid == first
			&& (wchar_t)KerningsList[i].secondid == second) {
			return KerningsList[i].amount;
		}
	}
	return 0.0f;
}

FontChar * CFont::GetChar(wchar_t c)
{
	for (int i = 0; i < numCharacters; ++i) {
		if (c == (wchar_t)CharList[i].id) return &(CharList[i]);
	}
	return nullptr;
}

void CFont::ResetTextData()
{
	// ������ ���° ���ؽ��� ������ �ٲܰ������� ���� ������ �ʱ�ȭ�Ѵ�.
	m_currentVertexIndex = 0;

	// ���ؽ� �ٽ� ȭ�� ������ ġ���
	for (int i = 0; i < m_nControllers; ++i) {
		m_ppControllers[i]->textvertex.m_xmf4Pos = XMFLOAT4(-10000, -10000, 0, 0);
	}
}

void CFont::AppendTextData(std::wstring text, XMFLOAT2 pos, XMFLOAT2 scale, XMFLOAT2 padding, XMFLOAT4 color)
{
	float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;

	float x = topLeftScreenX;
	float y = topLeftScreenY;

	float horrizontalPadding = (leftpadding + rightpadding) * padding.x;
	float verticalPadding = (toppadding + bottompadding) * padding.y;

	wchar_t lastChar = -1;
	for (int i = 0; i < text.size(); ++i)
	{
		// ��� ���ؽ��� �����ϸ� ��������
		if (m_currentVertexIndex >= m_nControllers) break;

		wchar_t c = text[i];

		FontChar* fc = GetChar(c);

		if (fc == nullptr) continue;
		if (c == L'\n')
		{
			x = topLeftScreenX;
			y -= (lineHeight + verticalPadding) * scale.y;
			continue;
		}

		float kerning = 0.0f;
		if (i > 0) kerning = GetKerning(lastChar, c);

		m_ppControllers[m_currentVertexIndex]->textvertex.SetVertexData(
			{ x + ((fc->xoffset + kerning) * scale.x)
			, y - (fc->yoffset * scale.y)
			, fc->width * scale.x
			, fc->height * scale.y }
			, { fc->u, fc->v, fc->twidth, fc->theight }
			, color
		);

		x += (fc->xadvance - horrizontalPadding) * scale.x;

		lastChar = c;

		m_currentVertexIndex++;
	}

}

void CFont::Render(ID3D12GraphicsCommandList * pd3dCmdLst, CCamera * pCamera)
{
	//if (!m_bActive) return;

	OnPrepareRender(pd3dCmdLst);
	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->Render(pd3dCmdLst, pCamera);
		}
		if (m_pMaterial->m_pTexture) {
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCmdLst);
		}
	}

	if (!m_vecMeshes.empty()) {
		UpdateShaderVariables(pd3dCmdLst);

		for (int i = 0; i < m_vecMeshes.size(); ++i) {
			m_vecMeshes[i]->Render(pd3dCmdLst);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
