#include "scene_cb.h.hlsl"

struct VSInput {
  float2 pos : POSITION;
  float2 uv : TEXCOORD0;
};

struct PSInput {
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
};

#if IKAROS_VS

PSInput main(VSInput vin) {
  PSInput pin;
  pin.pos = float4(vin.pos, 0.0f, 1.0f);
  pin.pos.x /= scene_aspectRatio;
  pin.uv = vin.uv;
  return pin;
}

#elif IKAROS_PS

Texture2D colorTex : register(t0);
SamplerState colorSampler : register(s0);

float4 main(PSInput pin) : SV_TARGET {
  return colorTex.Sample(colorSampler, pin.uv);
}

#endif
