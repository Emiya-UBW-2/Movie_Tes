// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
    float4 dif : COLOR0; // �f�B�t���[�Y�J���[
    float2 texCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 pos : SV_POSITION; // ���W( �v���W�F�N�V������� )
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0; // �F
};

// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
    float4 FactorColor; // �A���t�@�l��

    float MulAlphaColor; // �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
    float AlphaTestRef; // �A���t�@�e�X�g�Ŏg�p�����r�l
    float2 Padding1;

    int AlphaTestCmpMode; // �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
    int3 Padding2;

    float4 IgnoreTextureColor; // �e�N�X�`���J���[���������p�J���[
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1)
{
    DX_D3D11_PS_CONST_BUFFER_BASE g_Base;
};

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

SamplerState g_DiffuseMapSampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_DiffuseMapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

//SamplerState g_albedoMapSampler : register(s1); // �f�B�t���[�Y�}�b�v�T���v��
//Texture2D g_albedoMapTexture : register(t1); // �f�B�t���[�Y�}�b�v�e�N�X�`��


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
