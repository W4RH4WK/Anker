#include "screen.hlsl"

#if ANKER_PS

cbuffer post : register(b0) {
  float post_exposure;
  float post_temperature;
  float post_tint;
  float post_contrast;
  float post_brightness;
  float3 post_colorFilter;
  float post_saturation;
  float post_gamma;
  int post_toneMapping;
}

Texture2D colorTex : register(t0);
SamplerState colorSampler : register(s0);

float3 whiteBalance(float3 In, float Temperature, float Tint) {
  // https://docs.unity3d.com/Packages/com.unity.shadergraph@6.9/manual/White-Balance-Node.html

  // Range ~[-1.67;1.67] works best
  float t1 = Temperature * 10 / 6;
  float t2 = Tint * 10 / 6;

  // Get the CIE xy chromaticity of the reference white point.
  // Note: 0.31271 = x value on the D65 white point
  float x = 0.31271 - t1 * (t1 < 0 ? 0.1 : 0.05);
  float standardIlluminantY = 2.87 * x - 3 * x * x - 0.27509507;
  float y = standardIlluminantY + t2 * 0.05;

  // Calculate the coefficients in the LMS space.
  float3 w1 = float3(0.949237, 1.03542, 1.08728); // D65 white point

  // CIExyToLMS
  float Y = 1;
  float X = Y * x / y;
  float Z = Y * (1 - x - y) / y;
  float L = 0.7328 * X + 0.4296 * Y - 0.1624 * Z;
  float M = -0.7036 * X + 1.6975 * Y + 0.0061 * Z;
  float S = 0.0030 * X + 0.0136 * Y + 0.9834 * Z;
  float3 w2 = float3(L, M, S);

  float3 balance = float3(w1.x / w2.x, w1.y / w2.y, w1.z / w2.z);

  float3x3 LIN_2_LMS_MAT = {3.90405e-1, 5.49941e-1, 8.92632e-3,
                            7.08416e-2, 9.63172e-1, 1.35775e-3,
                            2.31082e-2, 1.28021e-1, 9.36245e-1};

  float3x3 LMS_2_LIN_MAT = {2.85847e+0,  -1.62879e+0, -2.48910e-2,
                            -2.10182e-1, 1.15820e+0,  3.24281e-4,
                            -4.18120e-2, -1.18169e-1, 1.06867e+0};

  float3 lms = mul(LIN_2_LMS_MAT, In);
  lms *= balance;
  return mul(LMS_2_LIN_MAT, lms);
}

float3 Uncharted2TonemapOp(float3 x) {
  float A = 0.15;
  float B = 0.50;
  float C = 0.10;
  float D = 0.20;
  float E = 0.02;
  float F = 0.30;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float3 Uncharted2Tonemap(float3 color, float whitePoint) {
  return Uncharted2TonemapOp(color) / Uncharted2TonemapOp(whitePoint);
}

float4 main(PSInput pin) : SV_TARGET {
  float3 color = colorTex.Sample(colorSampler, pin.uv).xyz;

  // exposure
  color *= post_exposure;

  // white balancing
  color = whiteBalance(color, post_temperature, post_tint);

  // contrast + brightness
  color = post_contrast * (color - 0.5) + 0.5 + post_brightness;

  // color filtering
  color *= post_colorFilter;

  // saturation
  float luminance = dot(color, float3(0.299f, 0.587f, 0.114f));
  color = lerp(luminance, color, post_saturation);

  if (post_toneMapping == 1) {
    color = Uncharted2Tonemap(2 * color, 11.2f);
  } else if (post_toneMapping == 2) {
    // Narkowicz ACES
    color = (color * (2.51f * color + 0.03f)) /
            (color * (2.43f * color + 0.59f) + 0.14f);
  }

  return float4(pow(saturate(color), 1 / post_gamma), 1);
}

#endif // ANKER_PS
