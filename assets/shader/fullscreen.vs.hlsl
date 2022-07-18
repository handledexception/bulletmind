cbuffer ShaderParams : register(b0)
{
    float4x4 world : packoffset(c0);
    float4x4 view_proj : packoffset(c4);
};

struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(uint id : SV_VertexID)
{
    VSOutput v;
    v.uv = float2((id << 1) & 2, id & 2) * 2;
    float2 xy = v.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    v.pos = float4(
        xy.x,
        xy.y,
        0.0f,
        1.0f);
    return v;
}
