// 頂点シェーダーの入力
struct VS_INPUT
{
	float3 Position        : POSITION;		// 座標( ローカル空間 )
	float3 Normal          : NORMAL0;      // 法線( ローカル空間 )
	float4 Diffuse         : COLOR0;       // ディフューズカラー
	float4 Specular        : COLOR1;       // スペキュラカラー
	float4 TexCoords0      : TEXCOORD0;	// テクスチャ座標
};

// 頂点シェーダーの出力
struct VS_OUTPUT
{
	float4 Diffuse         : COLOR0;
	float4 Specular        : COLOR1;
	float2 TexCoords0      : TEXCOORD0;
	float4 VPosition	   : TEXCOORD1;	// 座標( 射影空間 )ピクセルシェーダーで参照する為の物
	float4 Position        : SV_POSITION;	// 座標( プロジェクション空間 )
};

// 基本パラメータ
struct DX_D3D11_VS_CONST_BUFFER_BASE
{
	float4		AntiViewportMatrix[4];				// アンチビューポート行列
	float4		ProjectionMatrix[4];					// ビュー　→　プロジェクション行列
	float4		ViewMatrix[3];						// ワールド　→　ビュー行列
	float4		LocalWorldMatrix[3];					// ローカル　→　ワールド行列

	float4		ToonOutLineSize;						// トゥーンの輪郭線の大きさ
	float		DiffuseSource;							// ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
	float		SpecularSource;						// スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
	float		MulSpecularColor;						// スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
	float		Padding;
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_VS_BASE				: register(b1)
{
	DX_D3D11_VS_CONST_BUFFER_BASE				g_Base;
};

// main関数
VS_OUTPUT main(VS_INPUT VSInput)
{
	VS_OUTPUT VSOutput;
	float4 lLocalPosition;
	float4 lWorldPosition;

	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	// ローカル座標のセット
	lLocalPosition.xyz = VSInput.Position;
	lLocalPosition.w = 1.0f;

	// ローカル座標をワールド座標に変換
	lWorldPosition.x = dot(lLocalPosition, g_Base.LocalWorldMatrix[0]);
	lWorldPosition.y = dot(lLocalPosition, g_Base.LocalWorldMatrix[1]);
	lWorldPosition.z = dot(lLocalPosition, g_Base.LocalWorldMatrix[2]);
	lWorldPosition.w = 1.0f;

	// ワールド座標をビュー座標に変換
	VSOutput.VPosition.x = dot(lWorldPosition, g_Base.ViewMatrix[0]);
	VSOutput.VPosition.y = dot(lWorldPosition, g_Base.ViewMatrix[1]);
	VSOutput.VPosition.z = dot(lWorldPosition, g_Base.ViewMatrix[2]);
	VSOutput.VPosition.w = 1.0f;

	// ビュー座標を射影座標に変換
	VSOutput.Position.x = dot(VSOutput.VPosition, g_Base.ProjectionMatrix[0]);
	VSOutput.Position.y = dot(VSOutput.VPosition, g_Base.ProjectionMatrix[1]);
	VSOutput.Position.z = dot(VSOutput.VPosition, g_Base.ProjectionMatrix[2]);
	VSOutput.Position.w = dot(VSOutput.VPosition, g_Base.ProjectionMatrix[3]);

	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )

	// 出力パラメータを返す
	return VSOutput;
}

