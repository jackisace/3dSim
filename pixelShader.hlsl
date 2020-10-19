//float4 main(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
float4 main(float4 color : Color) : SV_Target
{
    return color;
}