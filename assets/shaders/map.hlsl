#include "common.h.hlsl"
#include "scene_cb.h.hlsl"

cbuffer MapLayer : register(b1) {
  float4x4 MapLayerTransform;
  float4 MapLayerColor;
  float2 MapLayerParallax;
  float2 MapLayer_pad;
}

struct VSInput {
  float2 pos : POSITION;
  float2 uv : TEXCOORD0;
};

struct PSInput {
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
};

#if ANKER_VS

PSInput main(VSInput vin) {
  float3 pos = float3(vin.pos, 1);
  pos = mul((float3x3)MapLayerTransform, pos);
  pos.xy = applyParallax(MapLayerParallax, pos.xy, SceneCameraPos);
  pos = mul((float3x3)SceneView, pos);

  PSInput pin;
  pin.pos = float4(pos.xy, 0, 1);
  pin.uv = vin.uv;
  return pin;
}

#elif ANKER_PS

Texture2D colorTex : register(t0);
SamplerState colorSampler : register(s0);

float4 main(PSInput pin) : SV_TARGET {
  return colorTex.Sample(colorSampler, pin.uv) * MapLayerColor;
}

#endif
