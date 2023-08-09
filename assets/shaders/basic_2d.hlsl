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
  float3 pos = float3(vin.pos, 1);
  pos = mul((float3x3)SpriteRenderer_transform, pos);
  pos = mul((float3x3)Scene_view, pos);

  // float2 pos = mul((float3x3)SpriteRenderer_transform, float3(vin.pos, 1)).xy;
  // pos.x /= Scene_aspectRatio;

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
