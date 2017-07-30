// FastNoiseLibrary.h
//
// MIT License
//
// Copyright(c) 2017 Jordan Peck
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The developer's email is jorzixdan.me2@gzixmail.com (for great email, take
// off every 'zix'.)
//

// VERSION: 0.4.1

// This is a modified version of FastNoise by Jordan Peck (Auburns on Github)
// Modified by Daniel Dickson (Fluroclad)
// It has been modified to be usable in Unreal Engine 4 with Blueprints and C++.

// VERSION UE4: 4.16.2

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FastNoiseLibrary.generated.h"

UENUM(BlueprintType)
enum ENoiseType
{
	Value,
	ValueFractal,
	Perlin,
	PerlinFractal,
	Simplex,
	SimplexFractal,
	Cellular,
	WhiteNoise,
	Cubic,
	CubicFractal
};

UENUM(BlueprintType)
enum EInterpolation
{
	Linear,
	Hermite,
	Quintic
};

UENUM(BlueprintType)
enum EFractalType
{
	FBM,
	Billow,
	RigidMulti
};

UENUM(BlueprintType)
enum ECellularDistanceFunction
{
	Euclidean,
	Manhattan,
	Natural
};

UENUM(BlueprintType)
enum ECellularReturnType
{
	CellValue,
	NoiseLookup,
	Distance,
	Distance2,
	Distance2Add,
	Distance2Sub,
	Distance2Mul,
	Distance2Div
};

/**
*
*/
UCLASS()
class GAME_API UFastNoiseLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFastNoiseLibrary();

	~UFastNoiseLibrary();
	// General

	// Returns seed used in all noise types
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static int32 GetSeed();

	// Set seed for all noise types
	// Default: 1337
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetSeed(int32 Seed = 1337);

	// Set frequency for all noise types
	// Default: 0.01
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetFrequency(float Frequency = 0.01f);

	// Changes the interpolation method used to smooth between noise values
	// Possible interpolation methods (lowest to highest quality) :
	// - Linear
	// - Hermite
	// - Quintic
	// Used in Value, Perlin Noise and Position Warping
	// Default: Quintic
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetInterpolation(TEnumAsByte<EInterpolation> Interpolation = Quintic);

	// Set noise return type for GetNoise
	// Default: Simplex
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetNoiseType(TEnumAsByte<ENoiseType> NoiseType = Simplex);

	// Set octave count
	// Default: 3
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetFractalOctaves(int32 Octaves = 3);

	// Set fractal lacunarity
	// Default: 2.0
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetFractalLacunarity(float Lacunarity = 2.0f);

	// Set octave gain
	// Default: 0.5
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetFractalGain(float Gain = 0.5f);

	// Set fractal type
	// Default: FBM
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetFractalType(TEnumAsByte<EFractalType> FractalType = FBM);

	// Sets distance function used in cellular noise calculations
	// Default: Euclidean
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetCellularDistanceFunction(TEnumAsByte<ECellularDistanceFunction> CellularDistanceFunction = Euclidean);

	// Sets return type for cellular noise calculations
	// Default: CellValue
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetCellularReturnType(TEnumAsByte<ECellularReturnType> CellularReturnType = CellValue);

	// Noise used to calculate a cell value if cellular return type is noise
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetCellularNoiseLookup(UFastNoiseLibrary* Noise = nullptr);

	// Sets maximum warp distance from original location
	// Default: 1.0
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void SetGradientPerturbAmp(float GradientPerturbAmp = 1.0f);


	// 2D
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float GetValue2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float GetValueFractal2D(float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float GetPerlin2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float GetPerlinFractal2D(float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float GetSimplex2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float GetSimplexFractal2D(float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cellular")
		static float GetCellular2D(float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | WhiteNoise")
		static float GetWhiteNoise2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | WhiteNoise")
		static float GetWhiteNoiseInt2D(int32 x, int32 y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float GetCubic2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float GetCubicFractal2D(float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Noise")
		static float GetNoise2D(float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Gradient")
		static void GradientPerturb2D(float& x, float& y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Gradient")
		static void GradientPerturbFractal2D(float& x, float& y);


	// 3D
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float GetValue3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float GetValueFractal3D(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float GetPerlin3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float GetPerlinFractal3D(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float GetSimplex3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float GetSimplexFractal3D(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cellular")
		static float GetCellular3D(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | WhiteNoise")
		static float GetWhiteNoise3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | WhiteNoise")
		static float GetWhiteNoiseInt3D(int32 x, int32 y, int32 z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float GetCubic3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float GetCubicFractal3D(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Noise")
		static float GetNoise3D(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Gradient")
		static void GradientPerturb3D(float& x, float& y, float& z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Gradient")
		static void GradientPerturbFractal3D(float& x, float& y, float& z);

	// 4D
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float GetSimplex4D(float x, float y, float z, float w);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | WhiteNoise")
		static float GetWhiteNoise4D(float x, float y, float z, float w);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | WhiteNoise")
		static float GetWhiteNoiseInt4D(int32 x, int32 y, int32 z, int32 w);


protected:
	static uint8 mPerm[512];
	static uint8 mPerm12[512];

	// Variables
	static int32 mSeed;

	static float mFrequency;

	static TEnumAsByte<EInterpolation> mInterpolation;

	static TEnumAsByte<ENoiseType> mNoiseType;

	static int32 mOctaves;

	static float mLacunarity;

	static float mGain;

	static TEnumAsByte<EFractalType> mFractalType;

	static float mFractalBounding;

	static TEnumAsByte<ECellularDistanceFunction> mCellularDistanceFunction;

	static TEnumAsByte<ECellularReturnType> mCellularReturnType;

	static UFastNoiseLibrary* mCellularNoiseLookup;

	static float mGradientPerturbAmp;

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary")
		static void CalculateFractalBounding();

	// 2D
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float SingleValueFractalFBM2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float SingleValueFractalBillow2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float SingleValueFractalRigidMulti2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float SingleValue2D(uint8 Offset, float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float SinglePerlinFractalFBM2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float SinglePerlinFractalBillow2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float SinglePerlinFractalRigidMulti2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float SinglePerlin2D(uint8 Offset, float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplexFractalFBM2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplexFractalBillow2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplexFractalRigidMulti2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplexFractalBlend2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplex2D(uint8 Offset, float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float SingleCubicFractalFBM2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float SingleCubicFractalBillow2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float SingleCubicFractalRigidMulti2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float SingleCubic2D(uint8 Offset, float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cellular")
		static float SingleCellular2D(float x, float y);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cellular")
		static float SingleCellular2Edge2D(float x, float y);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Gradient")
		static void SingleGradientPerturb2D(uint8 Offset, float WarpAmp, float Frequency, float& x, float& y);

	// 3D
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float SingleValueFractalFBM3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float SingleValueFractalBillow3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float SingleValueFractalRigidMulti3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Value")
		static float SingleValue3D(uint8 Offset, float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float SinglePerlinFractalFBM3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float SinglePerlinFractalBillow3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float SinglePerlinFractalRigidMulti3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Perlin")
		static float SinglePerlin3D(uint8 Offset, float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplexFractalFBM3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplexFractalBillow3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplexFractalRigidMulti3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplex3D(uint8 Offset, float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float SingleCubicFractalFBM3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float SingleCubicFractalBillow3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float SingleCubicFractalRigidMulti3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cubic")
		static float SingleCubic3D(uint8 Offset, float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cellular")
		static float SingleCellular3D(float x, float y, float z);
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Cellular")
		static float SingleCellular2Edge3D(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Gradient")
		static void SingleGradientPerturb3D(uint8 Offset, float WarpAmp, float Frequency, float& x, float& y, float& z);

	// 4D
	UFUNCTION(BlueprintCallable, Category = "FastNoiseLibrary | Simplex")
		static float SingleSimplex4D(uint8 Offset, float x, float y, float z, float w);

private:
	static inline uint8 Index2D_12(uint8 Offset, int32 x, int32 y);
	static inline uint8 Index3D_12(uint8 Offset, int32 x, int32 y, int32 z);
	static inline uint8 Index4D_32(uint8 Offset, int32 x, int32 y, int32 z, int32 w);
	static inline uint8 Index2D_256(uint8 Offset, int32 x, int32 y);
	static inline uint8 Index3D_256(uint8 Offset, int32 x, int32 y, int32 z);
	static inline uint8 Index4D_256(uint8 Offset, int32 x, int32 y, int32 z, int32 w);

	static inline float ValCoord2DFast(uint8 Offset, int32 x, int32 y);
	static inline float ValCoord3DFast(uint8 Offset, int32 x, int32 y, int32 z);
	static inline float GradCoord2D(uint8 Offset, int32 x, int32 y, float xd, float yd);
	static inline float GradCoord3D(uint8 Offset, int32 x, int32 y, int32 z, float xd, float yd, float zd);
	static inline float GradCoord4D(uint8 Offset, int32 x, int32 y, int32 z, int32 w, float xd, float yd, float zd, float wd);
};
