#include "scene_cb.h.hlsl"
#include "sprite_renderer_cb.h.hlsl"

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
  float3 pos = mul(SpriteRenderer_transform, float3(vin.pos));
  pos.x /= Scene_aspectRatio;

  PSInput pin;
  pin.pos = float4(pos.xy, 0, 1);
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
