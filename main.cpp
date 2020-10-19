// include the basic windows header files and the Direct3D header files
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

using namespace std;


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


namespace dx = DirectX;



// include the Direct3D Library file

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "kernel32.lib")
#pragma comment (lib, "d3dcsx.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "Dxgi.lib")
#pragma comment (lib, "dxguid.lib")
//#pragma comment (lib, "assimp-vc140-mt.lib")

// define the screen resolution
#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 750

// global declarations
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer
ID3D11InputLayout *pLayout;            // the pointer to the input layout
ID3D11VertexShader *pVS;               // the pointer to the vertex shader
ID3D11PixelShader *pPS;                // the pointer to the pixel shader
ID3D11Buffer *pVBuffer;                // the pointer to the vertex buffer

float angle = 0.0f;
float x = 0.0f;
float y = 0.0f;

tagPOINT mouse;


    float s = 1.0f;
    float ns = s * -1;


 struct ConstantBuffer
 {
    dx::XMMATRIX transform;
};

void RenderFrame(HWND hWnd);     // renders a single frame
void CleanD3D(void);        // closes Direct3D and releases memory
void initD3d(HWND hWnd);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    
    std::cout << "start\n" ;

	loadObj();

    //objLoad("P-51 Mustang.obj");
	
    
    HWND hWnd;
    WNDCLASSEX wc;

    //MessageBoxW(hWnd, L"LOL", L"LOL", MB_OK);
	

    
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
                          "My First Direct3D Program",
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
    
    

    // set up and initialize Direct3D
    initD3d(hWnd);

         


    // enter the main loop:
    
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

    // clean up DirectX and COM
    CleanD3D();

    return msg.wParam;
}


// this is the main message handler for the program
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
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                   // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
    scd.BufferDesc.Width = SCREEN_WIDTH;                   // set the back buffer width
    scd.BufferDesc.Height = SCREEN_HEIGHT;                 // set the back buffer height
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
    scd.OutputWindow = hWnd;                               // the window to be used
    scd.SampleDesc.Count = 4;                              // how many multisamples
    scd.Windowed = TRUE;                                   // windowed/full-screen mode
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

    // create a device, device context and swap chain using the information in the scd struct
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


    // get the address of the back buffer
    winrt::com_ptr<ID3D11Texture2D> pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer.get(), NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, NULL);


    // Set the viewport
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

    // encapsulate both shaders into shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    // create the input layout object
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    //MessageBoxW(hWnd, (LPCWSTR)std::size(ied), L"LOL", MB_OK);
    
    
    dev->CreateInputLayout(
        ied, (UINT)std::size(ied), 
        VS->GetBufferPointer(), 
        VS->GetBufferSize(), 
        &pLayout);

    devcon->IASetInputLayout(pLayout);
}


// this is the function used to render a single frame
void RenderFrame(HWND hWnd)
{

    
    /*Vertex vertices[] = 
    {
        {ns,ns,ns,      255, 0, 0, 0},
        {s,ns,ns,       0, 255, 0, 0},
        {ns,s,ns,       0, 255, 0, 0},
        {s,s,ns,        255, 0, 0, 0},
        {ns,ns,s,       255, 0, 0, 0},
        {s,ns,s,        0, 0, 255, 0},
        {ns,s,s,        255, 0, 0, 0},
        {s,s,s,         0, 0, 255, 0},
    };




    unsigned short indices[] =
    {
        0,2,1,2,3,1,
        1,3,5,3,7,5,
        2,6,3,3,6,7,
        4,5,7,4,7,6,
        0,4,2,2,4,6,
        0,1,4,1,5,4,
    };*/
    

    FLOAT ColorRGBA[4] = {0.3f, 0.2f, 0.4f, 1.0f};
    // clear the back buffer to a deep blue
    devcon->ClearRenderTargetView(backbuffer, ColorRGBA);



    //Vertex vertices[9454];
    //std::copy(newVertices.begin(), newVertices.end(), vertices);
    
    //Vertex* vertices = &newVertices[0];

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

    //devcon->IASetVertexBuffers(UINT(0), sizeof(vertices), &pVertexBuffer, sizeof(Vertex), 0)

    //unsigned short indices[12927];
    //std::copy(newIndices.begin(), newIndices.end(), indices);
    //unsigned short* indices = &newIndices[0];

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



    GetCursorPos(&mouse);
    ScreenToClient(hWnd, &mouse);

      
    angle += 0.001f;
    if (angle > 10.0f) 
        angle = 0.0f;
        
    ConstantBuffer cb = 
    {
        {
            dx::XMMatrixTranspose(
            dx::XMMatrixRotationX(angle) *
            dx::XMMatrixRotationY(angle) *
            dx::XMMatrixRotationZ(angle) *
            dx::XMMatrixScaling(0.25f, 0.25f, 0.25f) *
            //dx::XMMatrixTranslation(mouse.x/400.0f - 1.0f, -mouse.y/300.0f + 1.0f, 6.0f)
            dx::XMMatrixTranslation(0.0f, 0.0f, 0.5f)
            //dx::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.0f)
            )
        }
    };
   

    ID3D11Buffer *pConstantBuffer;
    D3D11_BUFFER_DESC cbd;
    ZeroMemory(&cbd, sizeof(cbd));
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    dev->CreateBuffer(&cbd, &csd, &pConstantBuffer);
    devcon->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);

    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    devcon->DrawIndexed((UINT)sizeof(indices), 0u, 0u);
    swapchain->Present(0, 0);
   
}













































// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
    swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

    // close and release all existing COM objects
    pLayout->Release();
    pVS->Release();
    pPS->Release();
    pVBuffer->Release();
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
}




