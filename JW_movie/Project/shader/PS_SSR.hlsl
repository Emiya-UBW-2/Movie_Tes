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

// プログラムとのやり取りのために使うレジスタ2
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3)
{
	float4 caminfo;
}

SamplerState g_DiffuseMapSampler : register(s0); // ディフューズマップサンプラ
Texture2D g_DiffuseMapTexture : register(t0); // ディフューズマップテクスチャ

SamplerState g_NormalMapSampler : register(s1); // 法線マップサンプラ
Texture2D g_NormalMapTexture : register(t1); // 法線マップテクスチャ

SamplerState g_DepthMapSampler : register(s2); // 深度マップサンプラ
Texture2D g_DepthMapTexture : register(t2); // 深度マップテクスチャ


float3 DisptoProj(float2 screenUV)
{
	float2 pos;

	pos.x = screenUV.x * 2.f - 1.f;
	pos.y = 1.f - screenUV.y * 2.f;

	float3 V_to_Eye;
	V_to_Eye.x = pos.x * caminfo.z * dispsize.x / dispsize.y;
	V_to_Eye.y = pos.y * caminfo.z;
	V_to_Eye.z = 1.f;

	return V_to_Eye * (g_DepthMapTexture.Sample(g_DepthMapSampler, screenUV).r / (caminfo.y*0.005f)); //距離
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

	float3 reflectVec = normalize(reflect(normalize(position), normal)); // 反射ベクトル
	int iteration = (int)(caminfo.x); // 繰り返し数
	float maxLength = 500.f; // 反射最大距離
	int BinarySearchIterations = 6; //2分探索最大数

	float pixelStride = maxLength / (float)iteration;
	float3 delta = reflectVec * pixelStride; // １回で進む距離
	int isend = 0;
	for (int i = 0; i < iteration; i++)
	{
		if (isend == 0)
		{
			position += delta;
			if (Hitcheck(position))
			{
				//交差したので二分探索
				position -= delta; //元に戻し
				delta /= BinarySearchIterations; //進む量を下げる
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
				// 交差したので色をブレンドする
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