Texture2D<float4> gInputTexture;
SamplerState gSampleState;
float2 gViewportResolution;

struct VSOutput
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
};

float4 SampleTextureCatmullRom(in Texture2D<float4> tex, in SamplerState linearSampler, in float2 uv, in float2 texSize)
{
    // We're going to sample a a 4x4 grid of texels surrounding the target UV coordinate. We'll do this by rounding
    // down the sample location to get the exact center of our "starting" texel. The starting texel will be at
    // location [1, 1] in the grid, where [0, 0] is the top left corner.
    float2 samplePos = uv * texSize;
    float2 texPos1 = floor(samplePos - 0.125f) + 0.125f;

    // Compute the fractional offset from our starting texel to our original sample location, which we'll
    // feed into the Catmull-Rom spline function to get our filter weights.
    float2 f = samplePos - texPos1;
    //float2 f2 = f * f;

    // Compute the Catmull-Rom weights using the fractional offset that we calculated earlier.
    // These equations are pre-expanded based on our knowledge of where the texels will be located,
    // which lets us avoid having to evaluate a piece-wise function.
    float2 w0 = f * ( -0.5 + f * (1.0 - 0.5*f));
    float2 w1 = 1.0 + f * f * (1.5*f - 2.5);
    float2 w2 = f * ( 0.5 + f * (2.0 - 1.5*f) );
    float2 w3 = f * f * (-0.5 + 0.5 * f);

    // Work out weighting factors and sampling offsets that will let us use bilinear filtering to
    // simultaneously evaluate the middle 2 samples from the 4x4 grid.
    float2 w12 = w1 + w2;
    float2 offset12 = w2 / (w1 + w2);

    // Compute the final UV coordinates we'll use for sampling the texture
    float2 texPos0 = texPos1 - 1;
    float2 texPos3 = texPos1 + 2;
    float2 texPos12 = texPos1 + offset12;

    texPos0 /= texSize;
    texPos3 /= texSize;
    texPos12 /= texSize;

    float4 result = 0.0f;
    result += tex.Sample(linearSampler, float2(texPos0.x, texPos0.y), 0.0f) * w0.x * w0.y;
    result += tex.Sample(linearSampler, float2(texPos12.x, texPos0.y), 0.0f) * w12.x * w0.y;
    result += tex.Sample(linearSampler, float2(texPos3.x, texPos0.y), 0.0f) * w3.x * w0.y;

    result += tex.Sample(linearSampler, float2(texPos0.x, texPos12.y), 0.0f) * w0.x * w12.y;
    result += tex.Sample(linearSampler, float2(texPos12.x, texPos12.y), 0.0f) * w12.x * w12.y;
    result += tex.Sample(linearSampler, float2(texPos3.x, texPos12.y), 0.0f) * w3.x * w12.y;

    result += tex.Sample(linearSampler, float2(texPos0.x, texPos3.y), 0.0f) * w0.x * w3.y;
    result += tex.Sample(linearSampler, float2(texPos12.x, texPos3.y), 0.0f) * w12.x * w3.y;
    result += tex.Sample(linearSampler, float2(texPos3.x, texPos3.y), 0.0f) * w3.x * w3.y;

    return result;
}

float4 TextureFilterBilinear(Texture2D<float4> tex, SamplerState state, float2 uv, float2 tex_size)
{
    float2 texel_pos = tex_size * uv;
    float2 lerp_amt = frac(texel_pos);
    float2 texel_size = 1.f / tex_size;
    float4 top_left  = tex.Sample(state, uv);
    float4 top_right = tex.Sample(state, uv + float2(texel_size.x, 0.f));
    float4 bot_left  = tex.Sample(state, uv + float2(0.f, texel_size.y));
    float4 bot_right = tex.Sample(state, uv + texel_size);
    return lerp(
        lerp(top_left, top_right, lerp_amt.x),
        lerp(bot_left, bot_right, lerp_amt.x),
        lerp_amt.y
    );
}

// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12HeterogeneousMultiadapter/src/blurShaders.hlsl
float4 PSMain(VSOutput input) : SV_TARGET0
{
    float2 tex_size;
    gInputTexture.GetDimensions(tex_size.x, tex_size.y);
    return gInputTexture.Sample(gSampleState, input.tex + (1.f / gViewportResolution));
    // return SampleTextureCatmullRom(gInputTexture, gSampleState, input.tex, tex_size);
    // return TextureFilterBilinear(gInputTexture, gSampleState, input.tex, tex_size);
    // return float4(1.f, 0.f, 0.f, 1.f);
}
