struct VSout
{
    float4 color : Color;
    float4 pos : SV_Position;
    
};

cbuffer CBuf{
    matrix transform;
};

VSout main(float3 pos : Position, float4 color : Color)
{
    VSout vso;
    vso.color = color;
    vso.pos = mul(float4(pos, 1.0f), transform);
    
    return vso;
}