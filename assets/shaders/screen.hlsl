struct PSInput {
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
};

#if ANKER_VS

PSInput main(uint id : SV_VertexID) {
  PSInput pin;
  pin.uv = float2(id & 2, (id << 1) & 2);
  pin.pos = float4(pin.uv * float2(2, -2) + float2(-1, 1), 0, 1);
  return pin;
}

#endif // ANKER_VS
