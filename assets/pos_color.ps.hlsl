float gZNearPlane;
float gZFarPlane;

struct VSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

float4 PSMain(VSOutput input) : SV_TARGET0
{
    return float4(input.color.xyz, 1.f);
}
