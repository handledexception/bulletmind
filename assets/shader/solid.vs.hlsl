cbuffer ShaderParams : register(b0)
{
    float4x4 world : packoffset(c0);
    float4x4 view_proj : packoffset(c4);
};

struct VSInput
{
    float4 pos : SV_Position; // POSITION?
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    input.pos.w = 1.0f;

    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view_proj);
    // output.position = mul(output.position, proj);

    output.uv = input.uv;

    return output;
}
