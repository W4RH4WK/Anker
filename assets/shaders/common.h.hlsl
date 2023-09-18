float2 applyParallax(float2 factor, float2 pos, float2 cameraPos) {
  return pos + (float2(1, 1) - factor) * cameraPos;
}
