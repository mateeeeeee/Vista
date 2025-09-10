
Texture2D<float4> gTex : register(t0);
SamplerState gSampler  : register(s0);

cbuffer Params : register(b0)
{
    float4 ChannelMask;   // (1,0,0,0) to view R, etc
    float RangeMin;
    float RangeMax;
    float SliceZ;         // for 3D textures, else ignored
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 c = gTex.Sample(gSampler, input.uv);

    // Channel mask
    c *= ChannelMask;

    // Normalize values into [0,1]
    c = saturate((c - RangeMin) / (RangeMax - RangeMin));

    return c;
}