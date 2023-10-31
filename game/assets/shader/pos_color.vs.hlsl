cbuffer ShaderParams : register(b0)
{
    float4x4 world : packoffset(c0);
    float4x4 view_proj : packoffset(c4);
    // float4x4 view : packoffset(c4);
    // float4x4 proj : packoffset(c8);
};

struct VSInput
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

struct VSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    input.position.w = 1.0f;
    output.position = mul(input.position, world);
    // output.position = mul(output.position, proj);
    // output.position = mul(output.position, view);
    output.position = mul(output.position, view_proj);
    output.color = input.color;
    return output;
}
