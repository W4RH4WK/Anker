#include "scene_cb.h.hlsl"

struct VSInput {
  float2 pos : POSITION;
  float4 color : COLOR;
};

struct PSInput {
  float4 pos : SV_POSITION;
  float4 color : COLOR;
};

#if ANKER_VS

PSInput main(VSInput vin) {
  float3 pos = float3(vin.pos, 1.0f);
  pos = mul((float3x3)SceneView, pos);

  PSInput pin;
  pin.pos = float4(pos.xy, 0, 1);
  pin.color = vin.color;
  return pin;
}

#elif ANKER_PS

float4 main(PSInput pin) : SV_TARGET { return pin.color; }

#endif
