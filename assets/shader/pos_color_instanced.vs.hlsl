cbuffer ShaderParams : register(b0)
{
    // float4x4 world : packoffset(c0);
    float4x4 view_proj : packoffset(c0);
    // float4x4 view : packoffset(c4);
    // float4x4 proj : packoffset(c8);
};

struct VSInput
{
    float4 position : SV_Position;
    float4 color : COLOR0;
    float4x4 transform : InstMatrix;
};

struct VSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR0;
    float4x4 transform : InstMatrix;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    input.position.w = 1.0f;
    output.position = mul(input.position, input.transform);
    output.position = mul(output.position, view_proj);
    output.color = input.color;
    return output;
}
