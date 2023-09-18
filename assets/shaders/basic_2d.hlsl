#include "scene_cb.h.hlsl"
#include "common.h.hlsl"

struct VSInput {
  float2 pos : POSITION;
  float2 uv : TEXCOORD0;
  float4x4 transform : TRANSFORM;
  float4 texRect : TEXTURE_RECT;
  float4 color : INSTANCE_COLOR;
  float2 parallax : INSTANCE_PARALLAX;
};

struct PSInput {
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
  float4 color : COLOR;
};

#if ANKER_VS

PSInput main(VSInput vin) {
  float3 pos = float3(vin.pos, 1);
  pos = mul((float3x3)vin.transform, pos);
  pos.xy = applyParallax(vin.parallax, pos.xy, SceneCameraPos);
  pos = mul((float3x3)Scene_view, pos);

  PSInput pin;
  pin.pos = float4(pos.xy, 0, 1);
  pin.uv = vin.uv;
  pin.uv *= float2(vin.texRect.x, vin.texRect.y);
  pin.uv += float2(vin.texRect.z, vin.texRect.w);
  pin.color = vin.color;
  return pin;
}

#elif ANKER_PS

Texture2D colorTex : register(t0);
SamplerState colorSampler : register(s0);

float4 main(PSInput pin) : SV_TARGET {
  return colorTex.Sample(colorSampler, pin.uv) * pin.color;
}

#endif
