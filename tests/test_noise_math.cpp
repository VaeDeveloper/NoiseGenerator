#include <gtest/gtest.h>
#include "Noise/NoiseMath.h"


// Distance test
TEST(NoiseMathTest, ZeroDistance)
{
    EXPECT_FLOAT_EQ(NG::Distance(0.0f, 0.0f, 0.0f, 0.0f), 0.0f);
}

TEST(NoiseMathTest, HorizontalDistance)
{
    EXPECT_FLOAT_EQ(NG::Distance(0.0f, 0.0f, 3.0f, 0.0f), 3.0f);
}

TEST(NoiseMathTest, VerticalDistance)
{
    EXPECT_FLOAT_EQ(NG::Distance(0.0f, 0.0f, 0.0f, 4.0f), 4.0f);
}

TEST(NoiseMathTest, DiagonalDistance)
{
    EXPECT_FLOAT_EQ(NG::Distance(0.0f, 0.0f, 3.0f, 4.0f), 5.0f); // 3-4-5 треугольник
}

TEST(NoiseMathTest, NegativeCoordinates)
{
    EXPECT_FLOAT_EQ(NG::Distance(-1.0f, -1.0f, 2.0f, 3.0f), 5.0f);
}


// CalcIndex1D_Test
TEST(NoiseMathTest, CalcIndex1D_Basic) 
{
    EXPECT_EQ(NG::CalcIndex1D(2, 5), 2);
    EXPECT_EQ(NG::CalcIndex1D(5, 5), 0);
    EXPECT_EQ(NG::CalcIndex1D(-1, 5), 4);
    EXPECT_EQ(NG::CalcIndex1D(-6, 5), 4);
}

TEST(NoiseMathTest, CalcIndex2D_Basic) 
{
    EXPECT_EQ(NG::CalcIndex2D(1, 2, 4), 1 + 2 * 4);
    EXPECT_EQ(NG::CalcIndex2D(-1, 2, 4), 3 + 2 * 4);  
    EXPECT_EQ(NG::CalcIndex2D(1, -1, 4), 1 + 3 * 4);
}

TEST(NoiseMathTest, CalcIndex3D_Basic) 
{
    EXPECT_EQ(NG::CalcIndex3D(1, 2, 3, 4), 1 + 2 * 4 + 3 * 4 * 4);
    EXPECT_EQ(NG::CalcIndex3D(-1, 0, 0, 4), 3);
    EXPECT_EQ(NG::CalcIndex3D(0, -1, 0, 4), 3 * 4);
    EXPECT_EQ(NG::CalcIndex3D(0, 0, -1, 4), 3 * 16);
}