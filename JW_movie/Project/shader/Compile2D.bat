set NowDir=%CD%
cd %~dp0

ShaderCompiler.exe /Tvs_4_0 VS_lens.hlsl
ShaderCompiler.exe /Tps_4_0 PS_lens.hlsl

ShaderCompiler.exe /Tvs_4_0 VS_SSR.hlsl
ShaderCompiler.exe /Tps_4_0 PS_SSR.hlsl

ShaderCompiler.exe /Tvs_4_0 VS_DoF.hlsl
ShaderCompiler.exe /Tps_4_0 PS_DoF.hlsl

ShaderCompiler.exe /Tvs_4_0 DepthVS.fx
ShaderCompiler.exe /Tps_4_0 DepthPS.fx

ShaderCompiler.exe /Tvs_4_0 NormalMesh_DirLight_PhongVS.fx
ShaderCompiler.exe /Tps_4_0 NormalMesh_DirLight_PhongPS.fx
pause

cd %NowDir%