#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <D3DCSX.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <cmath>
#include <stdio.h>
#include <winrt/base.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "kernel32.lib")
#pragma comment (lib, "d3dcsx.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "Dxgi.lib")
#pragma comment (lib, "dxguid.lib")

#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 750

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;
ID3D11RenderTargetView *backbuffer;
ID3D11InputLayout *pLayout;
ID3D11VertexShader *pVS;
ID3D11PixelShader *pPS;
ID3D11Buffer *pConstantBuffer;
D3D11_BUFFER_DESC cbd;
D3D11_SUBRESOURCE_DATA csd = {};

using namespace std;
namespace dx = DirectX;

struct Vertex{
    float x;
    float y;
    float z;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct Index{
	unsigned int a, b, c;
};

Index indices[4309];
Vertex vertices[4727];
unsigned int check = 0;
string curline;

Vertex getFloats(string in)
{
	Vertex fOut;
	string strV[3];
	in.erase(0,2);
	int spc;
	

	spc = in.find(" ") + 1;
	strV[0] = in.substr(0, spc);
	in.erase(0, spc);
	cout << "strv0: " << strV[0] << endl;


	spc = in.find(" ") + 1;
	strV[1] = in.substr(0, spc);
	in.erase(0, spc);
	cout << "strv1: " << strV[1] << endl;

	strV[2] = in;
	cout << "strv2:" << strV[2] << endl;

	fOut.x = stof(strV[0]);
	fOut.y = stof(strV[1]);
	fOut.z = stof(strV[2]);

    unsigned char r = 0;
    unsigned char g = 255;
    unsigned char b = 0;
    unsigned char a = 0;

	fOut.r = r;
	fOut.g = g;
	fOut.b = b;
	fOut.a = a;

	return fOut;
}

Index getUint(string in)
{
	Index out;
	unsigned int uint[3];
	int spc;
	int sls;
	string strV[3];

	in.erase(0,2);

	sls = in.find("/");
	spc = in.find(" ") + 1;
	strV[0] = in.substr(0,sls);
	in.erase(0,spc);


	sls = in.find("/");
	spc = in.find(" ") + 1;
	strV[1] = in.substr(0,sls);
	in.erase(0,spc);


	sls = in.find("/");
	spc = in.find(" ") + 1;
	strV[2] = in.substr(0,sls);
	in.erase(0,spc);

	cout << "0: " << strV[0] << " 1: " << strV[1] << " 2: " << strV[2] << endl;

	out.a = stoi(strV[0]);
	out.b = stoi(strV[1]);
	out.c = stoi(strV[2]);
	return out;
}



int loadObj()
{
	cout << "start:" << endl;

	ifstream file("P-51 Mustang.obj");

	if(!file.is_open())
		return false;

	int vertex = 0;
	int indice = 0;


	curline.clear();
	while (getline(file, curline))
	{
		char first = curline[0];
		char second = curline[1];
		char vCheck = 'v';
		char fCheck = 'f';
		char space = ' ';
		char currnum[20];
		cout << first << endl;

		if(first == vCheck && second == space){
			vertices[vertex] = getFloats(curline);
			vertex += 1;
		}

		if(first == fCheck && second == space){
			indices[indice] = getUint(curline);
			indice += 1;
		}

		curline.clear();
	}
	cout << "Vertex count : " << vertex << "\n";
	cout << "Indice count : " << indice << "\n";

	return 1;
}




float angle = 0.0f;
float x = 0.0f;
float y = 0.0f;

tagPOINT mouse;


    float s = 1.0f;
    float ns = s * -1;


struct ConstantBuffer
{
    dx::XMMATRIX transform;
} cb;

void RenderFrame(HWND hWnd);
void CleanD3D(void);        
void initD3d(HWND hWnd);


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    
    std::cout << "start\n" ;

	loadObj();

    HWND hWnd;
    WNDCLASSEX wc;
    
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);
    
    RECT wr = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(NULL,
                          "WindowClass",
                          "Typhoon Simulation",
                          WS_OVERLAPPEDWINDOW,
                          300,
                          300,
                          wr.right - wr.left,
                          wr.bottom - wr.top,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);
    
    initD3d(hWnd);

    MSG msg;
    
    while(TRUE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT){
                MessageBox(hWnd, "Quitting", "Quitting", 0);
                break;
            }
        }
        RenderFrame(hWnd);
    }

    CleanD3D();

    return msg.wParam;
}



LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}



void initD3d(HWND hWnd){
    DXGI_SWAP_CHAIN_DESC scd;

    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;                                   
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    
    scd.BufferDesc.Width = SCREEN_WIDTH;                   
    scd.BufferDesc.Height = SCREEN_HEIGHT;                 
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     
    scd.OutputWindow = hWnd;                               
    scd.SampleDesc.Count = 4;                              
    scd.Windowed = TRUE;                                   
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    


    D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &scd,
                                  &swapchain,
                                  &dev,
                                  NULL,
                                  &devcon);

    winrt::com_ptr<ID3D11Texture2D> pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    dev->CreateRenderTargetView(pBackBuffer.get(), NULL, &backbuffer);
    pBackBuffer->Release();

    devcon->OMSetRenderTargets(1, &backbuffer, NULL);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    devcon->RSSetViewports(1, &viewport);

    ID3DBlob *VS, *PS;

    D3DCompileFromFile(L"vertexShader.hlsl", NULL, NULL, "main", "vs_4_0", 0, 0, &VS, &VS);
    D3DCompileFromFile(L"pixelShader.hlsl", NULL, NULL, "main", "ps_4_0", 0, 0, &PS, &PS);

    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };  
    
    dev->CreateInputLayout(
        ied, (UINT)std::size(ied), 
        VS->GetBufferPointer(), 
        VS->GetBufferSize(), 
        &pLayout);

    devcon->IASetInputLayout(pLayout);

    ID3D11Buffer *pVertexBuffer;
    D3D11_BUFFER_DESC bd;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;   
    bd.ByteWidth = sizeof(vertices);  
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.Usage = D3D11_USAGE_DEFAULT;                
    bd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    dev->CreateBuffer(&bd, &sd, &pVertexBuffer);
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    devcon->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);

    ID3D11Buffer *pIndexBuffer;
    D3D11_BUFFER_DESC ibd;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;   
    ibd.ByteWidth = sizeof(indices);  
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.Usage = D3D11_USAGE_DEFAULT;                
    ibd.StructureByteStride = sizeof(Index);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    dev->CreateBuffer(&ibd, &isd, &pIndexBuffer);
    devcon->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);

    cb = {
            {
                dx::XMMatrixTranspose(
                            //dx::XMMatrixRotationX(angle) *
                            dx::XMMatrixRotationY(angle) *
                            //dx::XMMatrixRotationZ(angle) *
                            dx::XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) *
                            dx::XMMatrixTranslation(mouse.x/400.0f - 1.0f, -mouse.y/300.0f + 1.0f, 0.5f) 
                            //dx::XMMatrixTranslation(0.0f, 0.5f, 0.1f) 
                            //dx::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
                )
            }
        };
   
    ZeroMemory(&cbd, sizeof(cbd));
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;
    csd.pSysMem = &cb;
    dev->CreateBuffer(&cbd, &csd, &pConstantBuffer);
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}



void RenderFrame(HWND hWnd)
{
    check += 1;
    FLOAT ColorRGBA[4] = {0.3f, 0.2f, 0.4f, 1.0f};
    devcon->ClearRenderTargetView(backbuffer, ColorRGBA);

    GetCursorPos(&mouse);
    ScreenToClient(hWnd, &mouse);
      
    angle += 0.001f;
    if (angle > 10.0f) 
        angle = 0.0f;
        
    cb = {
            {
                dx::XMMatrixTranspose(
                            //dx::XMMatrixRotationX(angle) *
                            dx::XMMatrixRotationY(angle) *
                            //dx::XMMatrixRotationZ(angle) *
                            dx::XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) *
                            //dx::XMMatrixTranslation(mouse.x/400.0f - 1.0f, -mouse.y/300.0f + 1.0f, 0.5f) 
                            dx::XMMatrixTranslation(0.0f, -0.5f, 0.1f) 
                            //dx::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
                )
            }
        };
    dev->CreateBuffer(&cbd, &csd, &pConstantBuffer);
    int res;
    devcon->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
    devcon->DrawIndexed((UINT)sizeof(indices), 0u, 0u);
    pConstantBuffer->Release();

cb = {
            {
                dx::XMMatrixTranspose(
                            //dx::XMMatrixRotationX(angle) *
                            dx::XMMatrixRotationY(angle) *
                            //dx::XMMatrixRotationZ(angle) *
                            dx::XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) *
                            //dx::XMMatrixTranslation(mouse.x/400.0f - 1.0f, -mouse.y/300.0f + 1.0f, 0.5f) 
                            dx::XMMatrixTranslation(0.0f, 0.5f, 0.1f) 
                            //dx::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
                )
            }
        };
    dev->CreateBuffer(&cbd, &csd, &pConstantBuffer);
    devcon->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
    devcon->DrawIndexed((UINT)sizeof(indices), 0u, 0u);
    pConstantBuffer->Release();

    cb = {
            {
                dx::XMMatrixTranspose(
                            //dx::XMMatrixRotationX(angle) *
                            dx::XMMatrixRotationY(angle) *
                            //dx::XMMatrixRotationZ(angle) *
                            dx::XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) *
                            //dx::XMMatrixTranslation(mouse.x/400.0f - 1.0f, -mouse.y/300.0f + 1.0f, 0.5f) 
                            dx::XMMatrixTranslation(0.0f, 0.0f, 0.1f) 
                            //dx::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
                )
            }
        };
    dev->CreateBuffer(&cbd, &csd, &pConstantBuffer);
    devcon->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
    devcon->DrawIndexed((UINT)sizeof(indices), 0u, 0u);
    pConstantBuffer->Release();

        cb = {
            {
                dx::XMMatrixTranspose(
                            //dx::XMMatrixRotationX(angle) *
                            dx::XMMatrixRotationY(angle) *
                            //dx::XMMatrixRotationZ(angle) *
                            dx::XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) *
                            //dx::XMMatrixTranslation(mouse.x/400.0f - 1.0f, -mouse.y/300.0f + 1.0f, 0.5f) 
                            dx::XMMatrixTranslation(-0.5f, 0.0f, 0.1f) 
                            //dx::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
                )
            }
        };
    dev->CreateBuffer(&cbd, &csd, &pConstantBuffer);
    devcon->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
    devcon->DrawIndexed((UINT)sizeof(indices), 0u, 0u);
    pConstantBuffer->Release();

        cb = {
            {
                dx::XMMatrixTranspose(
                            //dx::XMMatrixRotationX(angle) *
                            dx::XMMatrixRotationY(angle) *
                            //dx::XMMatrixRotationZ(angle) *
                            dx::XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) *
                            //dx::XMMatrixTranslation(mouse.x/400.0f - 1.0f, -mouse.y/300.0f + 1.0f, 0.5f) 
                            dx::XMMatrixTranslation(0.5f, 0.0f, 0.1f) 
                            //dx::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
                )
            }
        };
    dev->CreateBuffer(&cbd, &csd, &pConstantBuffer);
    devcon->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
    //Exception thrown at 0x76CF4662 in main.exe: Microsoft C++ exception: _com_error at memory location 0x010FDE44.
    //check = 134791
    devcon->DrawIndexed((UINT)sizeof(indices), 0u, 0u);
    pConstantBuffer->Release();

    swapchain->Present(0, 0);

    if(check == 100000)
        cout << check << endl;
    if(check == 200000)
        cout << check << endl;
   if(check == 300000)
        cout << check << endl;
   if(check == 400000)
        cout << check << endl;
   if(check == 500000)
        cout << check << endl;
   if(check == 600000)
        cout << check << endl;
   if(check == 700000)
        cout << check << endl;
    if(check == 800000)
        cout << check << endl;
   if(check == 900000)
        cout << check << endl;
   if(check == 1000000)
        cout << check << endl;
   if(check == 1100000)
        cout << check << endl;
   
   
   
       
   
   
}


void CleanD3D(void)
{
    swapchain->SetFullscreenState(FALSE, NULL);
    pLayout->Release();
    pVS->Release();
    pPS->Release();
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
    pConstantBuffer->Release();
}




