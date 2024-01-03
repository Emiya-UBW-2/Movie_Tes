set NowDir=%CD%
cd %~dp0
@rem ShaderCompiler.exe /Tvs_4_0 GetDepthVS.fx
@rem ShaderCompiler.exe /Tps_4_0 GetDepthPS.fx
@rem pause

cd %NowDir%