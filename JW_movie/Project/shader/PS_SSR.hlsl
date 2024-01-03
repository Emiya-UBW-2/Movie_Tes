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

SamplerState g_DiffuseMapSampler : register(s0); // �f�B�t���[�Y�}�b�v�T���v��
Texture2D g_DiffuseMapTexture : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��

SamplerState g_NormalMapSampler : register(s1); // �@���}�b�v�T���v��
Texture2D g_NormalMapTexture : register(t1); // �@���}�b�v�e�N�X�`��

SamplerState g_DepthMapSampler : register(s2); // �[�x�}�b�v�T���v��
Texture2D g_DepthMapTexture : register(t2); // �[�x�}�b�v�e�N�X�`��


float3 DisptoProj(float2 screenUV)
{
	float2 pos;

	pos.x = screenUV.x * 2.f - 1.f;
	pos.y = 1.f - screenUV.y * 2.f;

	float3 V_to_Eye;
	V_to_Eye.x = pos.x * caminfo.z * dispsize.x / dispsize.y;
	V_to_Eye.y = pos.y * caminfo.z;
	V_to_Eye.z = 1.f;

	return V_to_Eye * (g_DepthMapTexture.Sample(g_DepthMapSampler, screenUV).r / (caminfo.y*0.005f)); //����
}

float2 ProjtoDisp(float3 position)
{
	position = position / position.z;

	float2 pos;
	pos.x = position.x / caminfo.z * dispsize.y / dispsize.x;
	pos.y = position.y / caminfo.z;

	pos.x = pos.x / 2.f + 0.5f;
	pos.y = 0.5f - pos.y / 2.f;
	return pos;

	/*
	float4 projectPosition = mul(gProjectionMatrix, float4(position, 1.0));
	float2 pos = projectPosition.xy / projectPosition.w * 0.5f + 0.5f;
	pos.y = 1.0f - pos.y;
	return pos;
	//*/
}

bool Hitcheck(float3 position)
{
	float2 screenUV = ProjtoDisp(position);

	if (
		(-1.f <= screenUV.x && screenUV.x <= 1.f) &&
		(-1.f <= screenUV.y && screenUV.y <= 1.f)
		) {
		float z = g_DepthMapTexture.Sample(g_DepthMapSampler, screenUV).r / (caminfo.y*0.005f);
		return (position.z < z && z < position.z + caminfo.w);
	}
	else {
		return false;
	}
}




float4 applySSR(float3 normal, float2 screenUV)
{
	float3 position = DisptoProj(screenUV);

	float4 color;
	color.r = 0.f;
	color.g = 0.f;
	color.b = 0.f;
	color.a = 0.f;

	float3 reflectVec = normalize(reflect(normalize(position), normal)); // ���˃x�N�g��
	int iteration = (int)(caminfo.x); // �J��Ԃ���
	float maxLength = 500.f; // ���ˍő勗��
	int BinarySearchIterations = 6; //2���T���ő吔

	float pixelStride = maxLength / (float)iteration;
	float3 delta = reflectVec * pixelStride; // �P��Ői�ދ���
	int isend = 0;
	for (int i = 0; i < iteration; i++)
	{
		if (isend == 0)
		{
			position += delta;
			if (Hitcheck(position))
			{
				//���������̂œ񕪒T��
				position -= delta; //���ɖ߂�
				delta /= BinarySearchIterations; //�i�ޗʂ�������
				for (int j = 0; j < BinarySearchIterations; j++)
				{
					if (isend == 0)
					{
						pixelStride *= 0.5f;
						position += delta * pixelStride;
						if (Hitcheck(position))
						{
							pixelStride = -pixelStride;
						}
						if (length(pixelStride)<10.f) {
							isend = 1;
						}
					}
				}
				// ���������̂ŐF���u�����h����
				color = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, ProjtoDisp(position));
				isend = 1;
			}
		}
	}
	return color;
}

PS_OUTPUT main(PS_INPUT PSInput)
{
	PS_OUTPUT PSOutput;
	float3 normal = g_NormalMapTexture.Sample(g_NormalMapSampler, PSInput.texCoords0).xyz;
	normal.x = normal.x * 2.f - 1.f;
	normal.y = normal.y * 2.f - 1.f;
	normal.z = normal.z * 2.f - 1.f;

	if (g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.texCoords0).g == 0.f) {
		PSOutput.color0 = float4(0.f, 0.f, 0.f, 0.f);
	}
	else {
		PSOutput.color0 = applySSR(normal, PSInput.texCoords0);

		if (PSOutput.color0.a == 0.f)
		{
			PSOutput.color0.r = 0.f;
			PSOutput.color0.g = 0.f;
			PSOutput.color0.b = 0.f;
			PSOutput.color0.a = 0.f;
		}
		else {
			float4 color = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, PSInput.texCoords0);
			if (
				abs(PSOutput.color0.r - color.r) < 2.f / 255.f &&
				abs(PSOutput.color0.g - color.g) < 2.f / 255.f &&
				abs(PSOutput.color0.b - color.b) < 2.f / 255.f) {
				PSOutput.color0.r = 0.f;
				PSOutput.color0.g = 0.f;
				PSOutput.color0.b = 0.f;
				PSOutput.color0.a = 0.f;
			}
			else {
				PSOutput.color0 = lerp(float4(0.f, 0.f, 0.f, 0.f), PSOutput.color0, g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.texCoords0).g);
			}
		}
	}
	return PSOutput;
}