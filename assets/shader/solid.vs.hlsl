cbuffer ShaderParams : register(b0)
{
    float4x4 world : packoffset(c0);
    float4x4 view_proj : packoffset(c4);
};

struct VSInput
{
    float4 position : SV_Position; // POSITION?
    float2 tex : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    input.position.w = 1.0f;

    output.position = mul(input.position, world);
    output.position = mul(output.position, view_proj);
    // output.position = mul(output.position, proj);

    output.tex = input.tex;

    return output;
}
