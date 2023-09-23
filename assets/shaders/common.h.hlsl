float2 applyParallax(float2 factor, float2 pos, float2 cameraPos) {
  // Parallax factor as given by Tiled. 1 is equal to no parallax, 0 is
  // one-to-one with the camera.
  return pos + (float2(1, 1) - factor) * cameraPos;
}
