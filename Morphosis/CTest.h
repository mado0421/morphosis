#pragma once

//this class is made for test object class
class Object {
public:
	//functions
	void Render();
	void Update();

public:
	//constructor and destructer
	Object();
	~Object();

private:
	//variables

};

//object + texture + material?
class ObjectManager {
public:
	void Render();
	void Update();

	void CreateCbvDescriptorHeaps();
	void CreateObjectResource();
	void CreateConstantBufferViews();

	void LoadLevelData();
	
public:
	ObjectManager();
	ObjectManager(ID3D12GraphicsCommandList *CommandList);
	~ObjectManager();

private:
	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorStartHandle;
	ID3D12Resource				*pCBObjectResource = NULL;

	ID3D12GraphicsCommandList	*pCommandList		= NULL;
	ID3D12Device				*pDevice			= NULL;
	ID3D12DescriptorHeap		*pCbvDescriptorHeap = NULL;

	int nObjects;
};