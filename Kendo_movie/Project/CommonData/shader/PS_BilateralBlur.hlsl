// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 dif : COLOR0; // ディフューズカラー
    float2 texCoords0 : TEXCOORD0; // テクスチャ座標
    float4 pos : SV_POSITION; // 座標( プロジェクション空間 )
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0; // 色
};

// 定数バッファピクセルシェーダー基本パラメータ
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
    float4 FactorColor; // アルファ値等

    float MulAlphaColor; // カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
    float AlphaTestRef; // アルファテストで使用する比較値
    float2 Padding1;

    int AlphaTestCmpMode; // アルファテスト比較モード( DX_CMP_NEVER など )
    int3 Padding2;

    float4 IgnoreTextureColor; // テクスチャカラー無視処理用カラー
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1)
{
    DX_D3D11_PS_CONST_BUFFER_BASE g_Base;
};

// プログラムとのやり取りのために使うレジスタ1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

SamplerState g_DiffuseMapSampler : register(s0); // ディフューズマップサンプラ
Texture2D g_DiffuseMapTexture : register(t0); // ディフューズマップテクスチャ

//SamplerState g_albedoMapSampler : register(s1); // ディフューズマップサンプラ
//Texture2D g_albedoMapTexture : register(t1); // ディフューズマップテクスチャ


float Gaussian(float sigma, float x)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

float JoinedBilateralGaussianBlur(float2 uv)
{
    float2 g_pixelSize = float2(0.001, 0.001);
    float2 g_pixelSizeGuide = float2(0.001, 0.001);
    g_pixelSize = float2(1.0 / dispsize.x, 1.0 / dispsize.y);
    g_pixelSizeGuide = float2(1.0 / dispsize.x, 1.0 / dispsize.y);
    float g_sigmaX = 3.;
    float g_sigmaY = 3.;
    float g_sigmaV = 3.;

    
    const float c_halfSamplesX = 4.;
    const float c_halfSamplesY = 4.;

    float total = 0.0;
    float ret = 0.0;

    float2 pivot = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, uv).rg;
    
    for (float iy = -c_halfSamplesY; iy <= c_halfSamplesY; iy++)
    {
        float fy = Gaussian(g_sigmaY, iy);
        float offsety = iy * g_pixelSize.y;

        for (float ix = -c_halfSamplesX; ix <= c_halfSamplesX; ix++)
        {
            float fx = Gaussian(g_sigmaX, ix);
            float offsetx = ix * g_pixelSize.x;
            
            float2 value = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, uv + float2(offsetx, offsety)).rg;
                        
            float fv = Gaussian(g_sigmaV, abs(value.g - pivot.g));
            
            total += fx * fy * fv;
            ret += fx * fy * fv * value.r;
        }
    }
        
    return ret / total;
}
  
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    
    float filtered = JoinedBilateralGaussianBlur(PSInput.texCoords0);
    //float3 pivot = g_albedoMapTexture.Sample(g_albedoMapSampler, PSInput.texCoords0).xyz;
    //PSOutput.color0 = float4(lerp(float3(1, 1, 1), pivot * 0.25f, 1 - filtered), 1);
    PSOutput.color0 = float4(filtered, filtered, filtered, 1);
    return PSOutput;
}
