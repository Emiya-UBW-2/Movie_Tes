#pragma once

namespace FPS_n2 {
	namespace Sceneclass {
		std::array<std::string, 4> ModelType{ "SKY_TRUE","NEAR_FALSE","FAR_TRUE","SHADOW_DISACTIVE" };

		std::vector<std::string> NAMES;
		ModelControl models;
		GraphControl graphs;
		LoadScriptClass LSClass;		//スクリプト読み込み
		TelopClass TLClass;				//テロップ
		std::vector<Cut_Info_First> m_CutInfo;
		std::vector<Cut_Info_Update> m_CutInfoUpdate;
		CutInfoClass attached;
		std::vector<CutAttachDetail> attachedDetail;
		//Grass grassmodel;
	}
}