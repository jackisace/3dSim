
    // INIT PIPELINE

        // load and compile the two shaders
    ID3DBlob *VS, *PS;

    D3DCompileFromFile(L"shaders.hlsl", NULL, NULL, "VShader", "vs_4_0", 0, 0, &VS, &VS);
    D3DCompileFromFile(L"shaders.hlsl", NULL, NULL, "PShader", "ps_4_0", 0, 0, &PS, &PS);

    // encapsulate both shaders into shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);
    

    
    // INIT GRAPHICS

    VERTEX OurVertices[] =
    {
        {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.45f, -0.5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
        {-0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}
    };

    
    
    
    

    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
//
    bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
//
    dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer

    
    // copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    MessageBox(hWnd, "LOL", "LOL", MB_OK);
    devcon->Map(pConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
    MessageBox(hWnd, "LOL", "LOL", MB_OK);
    memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
    
    devcon->Unmap(pConstantBuffer, NULL);                                      // unmap the buffer

    
    


    



    
    // RENDER FRAME
    
	FLOAT ColorRGBA[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    // clear the back buffer to a deep blue
    devcon->ClearRenderTargetView(backbuffer, ColorRGBA);

    // select which vertex buffer to display
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, &pConstantBuffer, &stride, &offset);
    // select which primtive type we are using
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    

    // draw the vertex buffer to the back buffer
    devcon->Draw(3, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
    