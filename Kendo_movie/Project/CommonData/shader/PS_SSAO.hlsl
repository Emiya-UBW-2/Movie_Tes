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

// �v���O�����Ƃ̂����̂��߂Ɏg�����W�X�^2
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3)
{
    float4 caminfo;
}
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b4)
{
    float4 caminfo2;
}

SamplerState g_DiffuseMapSampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_DiffuseMapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

SamplerState g_NormalMapSampler : register(s1); // �@���}�b�v�T���v��
Texture2D g_NormalMapTexture : register(t1); // �@���}�b�v�e�N�X�`��

SamplerState g_DepthMapSampler : register(s2); // �[�x�}�b�v�T���v��
Texture2D g_DepthMapTexture : register(t2); // �[�x�}�b�v�e�N�X�`��


#define SAMPLES 16
#define INTENSITY 1.0
#define SCALE 2.5
#define BIAS 0.05
#define SAMPLE_RAD 0.02
#define MAX_DISTANCE 0.07

#define MOD3 float3(0.1031,0.11369,0.13787)

float hash12(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * MOD3);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float3 DisptoProj(float2 screenUV)
{
    float2 pos;

    pos.x = screenUV.x * 2.0f - 1.0f;
    pos.y = 1.0f - screenUV.y * 2.0f;

    float3 V_to_Eye;
    V_to_Eye.x = pos.x * caminfo.z * dispsize.x / dispsize.y;
    V_to_Eye.y = pos.y * caminfo.z;
    V_to_Eye.z = 1.0f;

    return V_to_Eye * (g_DepthMapTexture.Sample(g_DepthMapSampler, screenUV).r / (caminfo.y * 0.005f)); //����
}

float AmbientOcclusion(in float3 diff, in float3 normal)
{
    float len = length(diff);
    float dotnrm = dot(normal, diff / len); //�P�ʃx�N�g��
    if (dotnrm < 0)
    {
        return 0;
    }
    float d = (1.0 / (1.0 + len * caminfo2.x));//�߂��قǍ����W����������
    float ao = max(0.0, dotnrm - caminfo2.y) * d;
    ao *= smoothstep(caminfo2.w, caminfo2.w * 0.5, len);
    return ao;

}

float SSAO(float2 tcoord, float3 pos, float3 normal, float rad)
{
    float goldenAngle = 2.4;
    float inv = 1.0 / float(SAMPLES);

    float radian = hash12(tcoord * 100.0) * 6.28;
    float rStep = inv * rad;
    float2 spiralUV;

    float radius = 0.0;
    float ao = 0.0;
    for (int i = 0; i < SAMPLES; i++)
    {
        spiralUV.x = sin(radian);
        spiralUV.y = cos(radian);
        radius += rStep;
        ao += AmbientOcclusion(DisptoProj(tcoord + spiralUV * radius) - pos, normal);
        radian += goldenAngle;
    }
    ao *= inv;
    ao = 1 - ao * caminfo.w;
    return ao;
}

PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    float3 normal = g_NormalMapTexture.Sample(g_NormalMapSampler, PSInput.texCoords0).xyz;
    normal.x = normal.x * 2.0f - 1.0f;
    normal.y = normal.y * 2.0f - 1.0f;
    normal.z = normal.z * 2.0f - 1.0f;

    float3 pos = DisptoProj(PSInput.texCoords0);

    float ao = 1.0;
    float rad = caminfo2.z / pos.z;
    ao = SSAO(PSInput.texCoords0, pos, normal, rad);
    

    //ao = lerp(ao, 1.0f, abs(normal.x)/1.5f);
    //ao = lerp(ao, 1.0f, saturate(pos.z / 1000.f));
    
    PSOutput.color0 = float4(ao, ao, ao, 1.0f);
    return PSOutput;
}