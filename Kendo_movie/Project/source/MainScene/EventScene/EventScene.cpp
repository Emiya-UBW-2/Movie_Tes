#include	"EventScene.hpp"

namespace FPS_n2 {
	//-------------------------------------------------------------
	void			EventScene::CutInfoClass::Start(size_t Counter) noexcept {
		this->isCutChange = false;
		this->isActive = false;
		this->isFirstCut = false;

		this->nowcut = 0;
		while (true) {
			if (this->Switch.size() > this->nowcut) {
				auto& inf_b = this->Switch[this->nowcut];
				if (inf_b.IsOnCut(Counter)) {
					this->nowcut++;
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
		if (0 <= this->nowcut - 1 && static_cast<size_t>(this->nowcut - 1) < this->Switch.size()) {
			auto& inf_b = this->Switch[static_cast<size_t>(this->nowcut - 1)];
			this->isFirstCut = inf_b.isFirstCut(Counter);
			if (inf_b.IsIn(Counter)) {
				this->nowcut--;
				this->isActive = true;
			}
		}
	}
	void			EventScene::CutInfoClass::Update(size_t Counter) noexcept {
		this->isCutChange = false;
		if (this->Switch.size() > this->nowcut) {
			auto& inf_b = this->Switch[this->nowcut];
			this->isFirstCut = inf_b.isFirstCut(Counter);
			if (this->isFirstCut) {
				this->isActive = true;
			}
			if (this->isActive) {
				if (inf_b.IsEndCut(Counter)) {
					this->isActive = false;
					this->nowcut++;
					this->isCutChange = true;
				}
			}
		}
	}
	void			EventScene::CutInfoClass::Dispose(void) noexcept {
		Switch.clear();
	}
	//-------------------------------------------------------------
	void			EventScene::ModelControl::Load(std::string_view Path) noexcept {
		MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
		for (int i = 0; i < 256; i++) {
			MV1SetLoadCalcPhysicsWorldGravity(i, Vector3DX::vget(0, 0, 1.f).get());
		}
		MV1SetLoadModelPhysicsCalcPrecision(1);

		if (model.size() == 0) {
			model.resize(model.size() + 1);
		}
		for (auto& m : model) {
			if (&m == &model.back()) { continue; }
			if (m.isBase && m.Path == Path) {
				model.back().Path = Path;
				model.back().isBase = false;
				model.back().obj.Duplicate(m.obj);
				m.numBase++;
				model.back().BaseID = m.numBase;
				model.resize(model.size() + 1);
				return;
			}
		}
		model.back().Path = Path;
		model.back().isBase = true;
		model.back().numBase = 0;
		model.back().BaseID = 0;
		MV1::Load(model.back().Path, &(model.back().obj), DX_LOADMODEL_PHYSICS_REALTIME);/*DX_LOADMODEL_PHYSICS_REALTIME*/
		model.resize(model.size() + 1);
	}
	void			EventScene::ModelControl::Start(size_t Counter) noexcept {
		for (auto& m : model) {
			m.Cutinfo.Start(Counter);
		}
	}
	void			EventScene::ModelControl::FirstUpdate(size_t Counter, bool isFirstLoop, bool reset_p) noexcept {
		for (auto& m : model) {
			while (true) {
				m.Cutinfo.Update(Counter);
				if (m.Cutinfo.GetisActive() && (m.Cutinfo.GetNowCut() < m.CutDetail.size())) {
					m.CutDetail[m.Cutinfo.GetNowCut()].UpdataModel(isFirstLoop, &m);
				}
				if (m.Cutinfo.GetisCutChange()) {
					continue;
				}
				break;
			}
		}

		for (auto& m : model) {
			m.canUpdate = true;
		}
		//++P_cnt %= 2;

		for (auto& m : model) {
			if (m.Cutinfo.GetisActive()) {
				m.Update(m.canUpdate);/**/
			}
		}
		SetPhysics(reset_p);
	}
	void			EventScene::ModelControl::SetPhysics(bool reset_p) noexcept {
		if (reset_p) {
			for (auto& m : model) {
				m.SetPhysics(true, 1.f);
			}
		}
		else {
			for (auto& m : model) {
				if (!m.isFarPhysics) {
					m.SetPhysics(true, 1.f);
					m.isFarPhysics = true;
				}
			}
			//
		}
	}
	void			EventScene::ModelControl::SetAfterLoad(void) noexcept {
		for (auto& m : model) {
			//
			if (!m.isEndLoad && CheckHandleASyncLoad(m.obj.get()) == FALSE) {
				m.isEndLoad = true;
				m.AddFrame("上半身");
				m.AddFrame("上半身2");
				m.AddFrame("首");
				m.AddFrame("頭");
				m.AddFrame("右目");
				m.AddFrame("左目");
				m.AddFrame("右人指２");
				m.AddFrame("右ひざ");
				m.AddFrame("左人指２");
				m.AddFrame("右つま先");
				m.AddFrame("左つま先");

				m.AddFrame("ｷｬﾉﾋﾟｰ");
				m.AddFrame("前回転");
				m.AddFrame("銃口先");

				MV1::SetAnime(&(m.obj), m.obj);
			}
		}
		//モデルのMV1保存
		for (auto& m : model) {
			if ((m.Path.find(".pmx") != std::string::npos) && (m.BaseID == 0)) {
				m.obj.SaveModelToMV1File((m.Path.substr(0, m.Path.find(".pmx")) + ".mv1").c_str(), MV1_SAVETYPE_NORMAL, -1, 1, 1, 1, 0, 0);
				//m.obj.SaveModelToMV1File((m.Path.substr(0, m.Path.find(".pmx")) + ".mv1").c_str());
			}
		}
	}
	void			EventScene::ModelControl::Draw_Far(void) const noexcept {
		auto fog_e = GetFogEnable();
		SetFogEnable(FALSE);
		SetUseLighting(FALSE);
		{
			for (auto& m : model) {
				if (m.isBGModel) {
					m.Draw();
				}
			}
		}
		SetUseLighting(TRUE);
		SetFogEnable(fog_e);
	}
	void			EventScene::ModelControl::Draw(bool innearshadow, bool infarshadow, bool ShadowActive, int isCheckFar) const noexcept {
		if (infarshadow) {
			for (auto& m : model) {
				if (!m.isBGModel && m.IsFarShadow) {
					if (ShadowActive && !m.ShadowDrawActive) { continue; }
					m.Draw(isCheckFar);
				}
			}
		}
		else if (innearshadow) {
			for (auto& m : model) {
				if (!m.isBGModel && m.IsNearShadow) {
					if (ShadowActive && !m.ShadowDrawActive) { continue; }
					m.Draw(isCheckFar);
				}
			}
		}
		else {
			for (auto& m : model) {
				if (!m.isBGModel) {
					if (ShadowActive && !m.ShadowDrawActive) { continue; }
					m.Draw(isCheckFar);
				}
			}
		}
	}
	void			EventScene::ModelControl::Dispose(void) noexcept {
		for (auto& m : model) {
			m.Dispose();
		}
		model.clear();
	}
	//-------------------------------------------------------------
	EventScene::GraphControl::GraphControl(void) noexcept {
		Dispose();
	}
	void			EventScene::GraphControl::Load(float xp, float yp, float rad, float alpha, float scale, std::string_view Path) noexcept {
		if (model.size() == 0) {
			model.resize(model.size() + 1);
		}
		model.back().Path = Path;
		model.back().Set(xp, yp, rad, alpha, scale, Path);
		model.resize(model.size() + 1);
	}
	void			EventScene::GraphControl::Start(size_t Counter) noexcept {
		for (auto& m : model) {
			m.Cutinfo.Start(Counter);
		}
	}
	void			EventScene::GraphControl::FirstUpdate(size_t Counter, bool isFirstLoop) noexcept {
		for (auto& m : model) {
			while (true) {
				m.Cutinfo.Update(Counter);
				if (m.Cutinfo.GetisActive() && (m.Cutinfo.GetNowCut() < m.CutDetail.size())) {
					m.CutDetail[m.Cutinfo.GetNowCut()].UpdataGraph(isFirstLoop, &m);
				}
				if (m.Cutinfo.GetisCutChange()) {
					continue;
				}
				break;
			}
		}
		for (auto& m : model) {
			m.Update();
		}
	}
	void			EventScene::GraphControl::Draw(void) const noexcept {
		for (auto& m : model) {
			m.Draw();
		}
	}
	void			EventScene::GraphControl::Dispose()  noexcept {
		for (auto& m : model) {
			m.Dispose();
		}
		model.clear();
	}
	//-------------------------------------------------------------
	EventScene::TelopClass::Cut_tex::Cut_tex(void) noexcept {
		xpos = 0;
		ypos = 0;
		size = 12;
		str = "test";
		START_TIME = (LONGLONG)(1000000.f * 0.01f);
		END_TIME = (LONGLONG)(1000000.f * 1.01f);
	}
	void			EventScene::TelopClass::Cut_tex::Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) noexcept {
		this->xpos = xp;
		this->ypos = yp;
		this->size = Fontsize;
		this->str = mag;
		this->START_TIME = StartF;
		this->END_TIME = StartF + ContiF;;
		this->LMR = m_LMR;
	}
	void			EventScene::TelopClass::Cut_tex::Draw(LONGLONG nowTimeWait) const noexcept {
		if (this->START_TIME < nowTimeWait && nowTimeWait < this->END_TIME) {
			auto* Fonts = FontPool::Instance();
			switch (this->LMR) {
			case 0:
				Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
				break;
			case 1:
				Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
				break;
			case 2:
				Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
				break;
			default:
				break;
			}
		}
	}
	//-------------------------------------------------------------
	void			EventScene::TelopClass::Init(void) noexcept {
		StartF = 0;
		ContiF = 0;
	}
	void			EventScene::TelopClass::LoadTelop(const std::string& func, const std::vector<std::string>& args) noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (func.find("SetTelopTime") != std::string::npos) {
			StartF = (LONGLONG)(1000000.f * std::stof(args[0]));
			ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
		}
		else if (func.find("AddTelopTime") != std::string::npos) {
			StartF += (LONGLONG)(1000000.f * std::stof(args[0]));
			ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
		}
		else if (func.find("SetTelop") != std::string::npos) {
			int t = 0;
			if (args[4].find("LEFT") != std::string::npos) { t = 0; }
			else if (args[4].find("MIDDLE") != std::string::npos) { t = 1; }
			else if (args[4].find("RIGHT") != std::string::npos) { t = 2; }
			Texts.resize(Texts.size() + 1);
			Texts.back().Set(DrawParts->GetScreenY(std::stoi(args[0])), DrawParts->GetScreenY(std::stoi(args[1])), DrawParts->GetScreenY(std::stoi(args[2])), args[3], StartF, ContiF, t);
		}
	}
	void			EventScene::TelopClass::Draw(LONGLONG nowTimeWait) const noexcept {
		for (auto& t : Texts) {
			t.Draw(nowTimeWait);
		}
	}
	void			EventScene::TelopClass::Dispose(void) noexcept {
		Texts.clear();
	}
	//-------------------------------------------------------------
	EventScene::ForcusControl::ForcusControl(std::string_view path, size_t id, std::string_view frame, const Vector3DX& add) noexcept {
		this->Use = true;
		this->Path = path;
		this->ID = id;
		if (frame == "UPPER") {
			this->Frame = "上半身";
		}
		else if (frame == "UPPER_2") {
			this->Frame = "上半身2";
		}
		else if (frame == "NECK") {
			this->Frame = "首";
		}
		else if (frame == "HEAD") {
			this->Frame = "頭";
		}

		else if (frame == "LEFT_EYE") {
			this->Frame = "右目";
		}
		else if (frame == "RIGHT_EYE") {
			this->Frame = "左目";
		}
		else if (frame == "RIGHT_FINGER") {
			this->Frame = "右人指２";
		}
		else if (frame == "LEFT_FINGER") {
			this->Frame = "左人指２";
		}
		else if (frame == "RIGHTLEG") {
			this->Frame = "右ひざ";
		}
		else if (frame == "RIGHTFOOT") {
			this->Frame = "右つま先";
		}
		else if (frame == "LEFTFOOT") {
			this->Frame = "左つま先";
		}

		else if (frame == "CANOPY") {
			this->Frame = "ｷｬﾉﾋﾟｰ";
		}
		else if (frame == "FRONTWHEEL") {
			this->Frame = "前回転";
		}
		else if (frame == "MOVE") {
			this->Frame = "移動";
		}
		else if (frame == "MUZZLE") {
			this->Frame = "銃口先";
		}
		this->Add = add;
	}
	//-------------------------------------------------------------
	EventScene::Cut_Info_First::Cut_Info_First(LONGLONG value) noexcept {
		TimeLimit = value;
		Aim_camera.SetCamPos(Vector3DX::vget(0, 10, -30), Vector3DX::vget(0, 10, 0), Vector3DX::up());
		Aim_camera.SetCamInfo(deg2rad(15), 1.f, 300.f);
		cam_per = 0.95f;
		IsResetPhysics = false;
	}
	EventScene::Cut_Info_First::~Cut_Info_First(void) noexcept {
		Forcus.clear();
	}
	bool			EventScene::Cut_Info_First::GetForcusCenter(Vector3DX* vec, const ModelControl& model) const noexcept {
		bool isforcus = false;
		for (auto& f : Forcus) {
			if (f.GetIsUse()) {
				if (!isforcus) {
					*vec = Vector3DX::zero();
				}
				*vec += f.GetForce(model);
				isforcus = true;
			}
		}
		if (isforcus) {
			*vec = *vec / (float)(Forcus.size());
		}
		return isforcus;
	}
	void			EventScene::Cut_Info_First::UpdateCam(Camera3DInfo* cam) const noexcept {
		Vector3DX pos_t = cam->GetCamPos();
		Vector3DX vec_t = cam->GetCamVec();
		Vector3DX up_t = cam->GetCamUp();
		float fov_t = cam->GetCamFov();
		float near_t = cam->GetCamNear();
		float far_t = cam->GetCamFar();
		Easing(&pos_t, Aim_camera.GetCamPos(), cam_per, EasingType::OutExpo);
		Easing(&vec_t, Aim_camera.GetCamVec(), cam_per, EasingType::OutExpo);
		Easing(&up_t, Aim_camera.GetCamUp(), cam_per, EasingType::OutExpo);
		Easing(&fov_t, Aim_camera.GetCamFov(), cam_per, EasingType::OutExpo);
		Easing(&far_t, Aim_camera.GetCamFar(), cam_per, EasingType::OutExpo);
		Easing(&near_t, Aim_camera.GetCamNear(), cam_per, EasingType::OutExpo);
		cam->SetCamPos(pos_t, vec_t, up_t);
		cam->SetCamInfo(fov_t, near_t, far_t);
	}
	void			EventScene::Cut_Info_First::SetUpFog(void) noexcept {
		if (Fog.fog[0] >= 0) {
			SetFogEnable(TRUE);
			Fog.SetFog();
		}
		else if (Fog.fog[0] == -2) {
			FogParam Tmp{}; Tmp.Reset();
			SetFogEnable(TRUE);
			Tmp.SetFog();
		}
	}
	void			EventScene::Cut_Info_First::SetPrev(const Cut_Info_First& tgt) noexcept {
		if (this->UsePrevAim) {
			this->Aim_camera = tgt.Aim_camera;
			this->cam_per = tgt.cam_per;
		}
	}
	void			EventScene::Cut_Info_First::LoadScript(const std::string& func, const std::vector<std::string>& args) noexcept {
		//Campos
		if (func.find("SetCampos_NoneRad") != std::string::npos) {
			this->Aim_camera.SetCamPos(
				Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
				this->Aim_camera.GetCamVec(),
				this->Aim_camera.GetCamUp()
			);
		}
		//Camvec
		else if (func.find("SetCamvec") != std::string::npos) {
			this->Aim_camera.SetCamPos(
				this->Aim_camera.GetCamPos(),
				Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
				this->Aim_camera.GetCamUp()
			);
		}
		else if (func.find("SetCamForcus") != std::string::npos) {
			this->Forcus.emplace_back(ForcusControl(args[0], std::stol(args[1]), args[2], Vector3DX::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5]))));
		}
		//CamUp
		else if (func.find("SetCamup") != std::string::npos) {
			this->Aim_camera.SetCamPos(
				this->Aim_camera.GetCamPos(),
				this->Aim_camera.GetCamVec(),
				Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]))
			);
		}
		//Else
		else if (func.find("SetCamInfo") != std::string::npos) {
			this->Aim_camera.SetCamInfo(deg2rad(std::stof(args[0])), std::stof(args[1]), std::stof(args[2]));
			this->cam_per = std::stof(args[3]);
		}
		//物理を次でリセットする
		else if (func.find("SetNextResetPhysics") != std::string::npos) {
			this->IsResetPhysics = true;
		}
		//前のカットカメラを使用
		else if (func.find("SetPrevCamInfo") != std::string::npos) {
			this->UsePrevAim = true;
		}
		//どの距離で描画するかをセット
		else if (func.find("ResetCamPosRand") != std::string::npos) {
			this->isResetRandCampos = true;
		}
		else if (func.find("ResetCamVecRand") != std::string::npos) {
			this->isResetRandCamvec = true;
		}
		else if (func.find("ResetCamUpRand") != std::string::npos) {
			this->isResetRandCamup = true;
		}
		else if (func.find("SetFog") != std::string::npos) {
			if (args.size() == 5) {
				this->Fog.fog[0] = std::stoi(args[0]);
				this->Fog.fog[1] = std::stoi(args[1]);
				this->Fog.fog[2] = std::stoi(args[2]);
				this->Fog.fog_range[0] = std::stof(args[3]);
				this->Fog.fog_range[1] = std::stof(args[4]);
			}
			else {
				this->Fog.fog[0] = -2;
			}
		}
	}
	//-------------------------------------------------------------
	EventScene::Cut_Info_Update::Cut_Info_Update(void) noexcept {
		isUseNotFirst = false;
		IsUsePrevBuf = false;

		campos_per = 1.f;
		camvec_per = 1.f;
		camup_per = 1.f;
		fov_per = 1.f;

		NotFirst_per = -1.f;

		m_RandcamupPer = 0.f;
		m_RandcamupSet = Vector3DX::zero();
		m_RandcamvecPer = 0.f;
		m_RandcamvecSet = Vector3DX::zero();
		m_RandcamposPer = 0.f;
		m_RandcamposSet = Vector3DX::zero();
		Forcus.clear();
	}
	EventScene::Cut_Info_Update::~Cut_Info_Update(void) noexcept {
		Forcus.clear();
	}
	void			EventScene::Cut_Info_Update::SetForce(float camvecPer, std::string_view ModelPath, int ModelID, std::string_view Frame, const Vector3DX& Add) noexcept {
		this->camvec_per = camvecPer;
		this->Forcus.emplace_back(ForcusControl(ModelPath, ModelID, Frame, Add));
	}
	bool			EventScene::Cut_Info_Update::GetForcusCenter(Vector3DX* vec, const ModelControl& model) const noexcept {
		bool isforcus = false;
		for (auto& f : Forcus) {
			if (f.GetIsUse()) {
				if (!isforcus) {
					*vec = Vector3DX::zero();
				}
				*vec += f.GetForce(model);
				isforcus = true;
			}
		}
		if (isforcus) {
			*vec = *vec / (float)(Forcus.size());
		}
		return isforcus;
	}
	void			EventScene::Cut_Info_Update::LoadScript(const std::string& func, const std::vector<std::string>& args) noexcept {
		//カメラのアップデート
		if (func.find("SetUpdateEnable") != std::string::npos) {
			this->isUseNotFirst = true;
		}
		//camvec
		else if (func.find("SetUpdateCamvec") != std::string::npos) {
			this->camvec_per = std::stof(args[0]);
			if (args.size() > 1) {
				this->CameraNotFirst.SetCamPos(
					this->CameraNotFirst.GetCamPos(),
					Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])),
					this->CameraNotFirst.GetCamUp());
			}
		}
		else if (func.find("SetUpdateCamForcus") != std::string::npos) {
			this->SetForce(std::stof(args[0]), args[1], std::stoi(args[2]), args[3], Vector3DX::vget(std::stof(args[4]), std::stof(args[5]), std::stof(args[6])));
		}
		else if (func.find("SetVectorUpdateCamvec") != std::string::npos) {
			this->CameraNotFirst_Vec.SetCamPos(
				this->CameraNotFirst_Vec.GetCamPos(),
				Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
				this->CameraNotFirst_Vec.GetCamUp());
		}
		//campos
		else if (func.find("SetUpdateCampos") != std::string::npos) {
			this->campos_per = std::stof(args[0]);
			if (args.size() > 1) {
				this->CameraNotFirst.SetCamPos(
					Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])),
					this->CameraNotFirst.GetCamVec(),
					this->CameraNotFirst.GetCamUp());
			}
		}
		else if (func.find("SetVectorUpdateCampos") != std::string::npos) {
			this->CameraNotFirst_Vec.SetCamPos(
				Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
				this->CameraNotFirst_Vec.GetCamVec(),
				this->CameraNotFirst_Vec.GetCamUp());
		}
		//camup
		else if (func.find("SetUpdateCamup") != std::string::npos) {
			this->camup_per = std::stof(args[0]);
			if (args.size() > 1) {
				this->CameraNotFirst.SetCamPos(
					this->CameraNotFirst.GetCamPos(),
					this->CameraNotFirst.GetCamVec(),
					Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])));
			}
		}
		else if (func.find("SetVectorUpdateCamup") != std::string::npos) {
			this->CameraNotFirst_Vec.SetCamPos(
				this->CameraNotFirst_Vec.GetCamPos(),
				this->CameraNotFirst_Vec.GetCamVec(),
				Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])));
		}
		//fov
		else if (func.find("SetUpdateCamfov") != std::string::npos) {
			this->fov_per = std::stof(args[0]);
			this->CameraNotFirst.SetCamInfo(deg2rad(std::stof(args[1])), this->CameraNotFirst.GetCamNear(), this->CameraNotFirst.GetCamFar());
		}
		//easing
		else if (func.find("SetUpdatePer") != std::string::npos) {
			this->NotFirst_per = std::stof(args[0]);
		}
		//前のカメラアップデートを使用
		else if (func.find("SetPrevCamUpdate") != std::string::npos) {
			this->IsUsePrevBuf = true;
		}
		//アップデートカメラにランダムを指定
		else if (func.find("SetCamPosRand") != std::string::npos) {
			this->m_RandcamposSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
			this->m_RandcamposPer = std::stof(args[3]);
		}
		else if (func.find("SetCamVecRand") != std::string::npos) {
			this->m_RandcamvecSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
			this->m_RandcamvecPer = std::stof(args[3]);
		}
		else if (func.find("SetCamUpRand") != std::string::npos) {
			this->m_RandcamupSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
			this->m_RandcamupPer = std::stof(args[3]);
		}
		//
		else if (func.find("WhiteOut") != std::string::npos) {
			this->IsSetWhite = true;
			this->White_Per = std::stof(args[0]);
			this->White = 1.f;
		}
		else if (func.find("WhiteIn") != std::string::npos) {
			this->IsSetWhite = true;
			this->White_Per = std::stof(args[0]);
			this->White = 0.f;
		}
		//
		else if (func.find("BlackOut") != std::string::npos) {
			this->IsSetBlack = true;
			this->Black_Per = std::stof(args[0]);
			this->Black = 1.f;
		}
		else if (func.find("BlackIn") != std::string::npos) {
			this->IsSetBlack = true;
			this->Black_Per = std::stof(args[0]);
			this->Black = 0.f;
		}
		//
		CameraNotFirst_After = CameraNotFirst;
	}
	void			EventScene::Cut_Info_Update::ResetCam(const Camera3DInfo& cam) noexcept {
		CameraNotFirst_After.SetCamPos(cam.GetCamPos(), cam.GetCamVec(), CameraNotFirst_After.GetCamUp());
	}
	void			EventScene::Cut_Info_Update::Update(Cut_Info_First& Camera,
		Vector3DX* m_RandcamupBuf, Vector3DX* m_RandcamvecBuf, Vector3DX* m_RandcamposBuf,
		float* pBlack, float* pWhite,
		const ModelControl& model
	) noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (this->NotFirst_per >= 0.f) {
			Camera.cam_per = this->NotFirst_per;
		}
		Easing(m_RandcamposBuf, Vector3DX::vget(GetRandf(this->m_RandcamposSet.x), GetRandf(this->m_RandcamposSet.y), GetRandf(this->m_RandcamposSet.z)), this->m_RandcamposPer, EasingType::OutExpo);
		Easing(m_RandcamvecBuf, Vector3DX::vget(GetRandf(this->m_RandcamvecSet.x), GetRandf(this->m_RandcamvecSet.y), GetRandf(this->m_RandcamvecSet.z)), this->m_RandcamvecPer, EasingType::OutExpo);
		Easing(m_RandcamupBuf, Vector3DX::vget(GetRandf(this->m_RandcamupSet.x), GetRandf(this->m_RandcamupSet.y), GetRandf(this->m_RandcamupSet.z)), this->m_RandcamupPer, EasingType::OutExpo);
		if (this->isUseNotFirst) {
			Vector3DX vec;
			if (GetForcusCenter(&vec, model)) {
				this->CameraNotFirst_After.SetCamPos(this->CameraNotFirst_After.GetCamPos(), vec, this->CameraNotFirst_After.GetCamUp());
			}
			{
				Vector3DX pos_t = Camera.Aim_camera.GetCamPos();
				Vector3DX vec_t = Camera.Aim_camera.GetCamVec();
				Vector3DX up_t = Camera.Aim_camera.GetCamUp();
				float fov_t = Camera.Aim_camera.GetCamFov();
				Easing(&pos_t, this->CameraNotFirst_After.GetCamPos() + *m_RandcamposBuf, this->campos_per, EasingType::OutExpo);
				Easing(&vec_t, this->CameraNotFirst_After.GetCamVec() + *m_RandcamvecBuf, this->camvec_per, EasingType::OutExpo);
				Easing(&up_t, this->CameraNotFirst_After.GetCamUp() + *m_RandcamupBuf, this->camup_per, EasingType::OutExpo);
				Easing(&fov_t, this->CameraNotFirst_After.GetCamFov(), this->fov_per, EasingType::OutExpo);
				Camera.Aim_camera.SetCamPos(pos_t, vec_t, up_t);
				Camera.Aim_camera.SetCamInfo(fov_t, Camera.Aim_camera.GetCamNear(), Camera.Aim_camera.GetCamFar());
			}
		}

		CameraNotFirst_After.SetCamPos(
			CameraNotFirst_After.GetCamPos() + CameraNotFirst_Vec.GetCamPos() * (1.f / DrawParts->GetFps()),
			CameraNotFirst_After.GetCamVec() + CameraNotFirst_Vec.GetCamVec() * (1.f / DrawParts->GetFps()),
			CameraNotFirst_After.GetCamUp() + CameraNotFirst_Vec.GetCamUp() * (1.f / DrawParts->GetFps())
		);
		Easing(pBlack, Black, Black_Per, EasingType::OutExpo);
		Easing(pWhite, White, White_Per, EasingType::OutExpo);
	}
	void			EventScene::Cut_Info_Update::SetupByPrev(const Cut_Info_Update& Prev) noexcept {
		if (!IsUsePrevBuf) {
			return;
		}
		//
		auto PrevWhite_Set = IsSetWhite;
		auto PrevWhite_Per = White_Per;
		auto PrevWhitePrev = White;

		auto PrevBlack_Set = IsSetBlack;
		auto PrevBlack_Per = Black_Per;
		auto PrevBlackPrev = Black;
		//
		*this = Prev;
		//
		if (PrevWhite_Set) {
			IsSetWhite = PrevWhite_Set;
			White_Per = PrevWhite_Per;
			White = PrevWhitePrev;
		}
		if (PrevBlack_Set) {
			IsSetBlack = PrevBlack_Set;
			Black_Per = PrevBlack_Per;
			Black = PrevBlackPrev;
		}
	}
	//-------------------------------------------------------------
	bool			EventScene::LoadOnce(std::string_view func_t) noexcept {
		std::string					m_Func; m_Func = func_t;
		std::vector<std::string>	m_Args;
		{
			// //を削除
			size_t sls = m_Func.find("//");
			if (sls != std::string::npos) { m_Func = m_Func.substr(0, sls); }
			//いらない要素を排除
			Sub_Func(m_Func, '{');
			Sub_Func(m_Func, '}');
			Sub_Func(m_Func, ' ');
			Sub_Func(m_Func, '\t');
			Sub_Func(m_Func, ';');
			Sub_Func(m_Func, '\"');
		}
		if (m_Func == "") { return false; }
		//()と,で囲われた部分から引数を取得
		{
			std::string tmp_func = m_Func;
			size_t left = tmp_func.find("(");
			size_t right = tmp_func.rfind(")");
			if (left != std::string::npos && right != std::string::npos) {
				tmp_func = tmp_func.substr(left + 1, right - 1 - left);
			}
			while (true) {
				size_t in_str = tmp_func.find(",");
				if (in_str == std::string::npos) {
					m_Args.emplace_back(tmp_func);
					break;
				}
				else {
					std::string arg = tmp_func.substr(0, in_str);
					tmp_func = tmp_func.substr(in_str + 1);
					m_Args.emplace_back(arg);
				}
			}
		}
		//変数登録
		if (m_Func.find("SetArg") != std::string::npos) {
			std::string After = m_Args[1];
			if (After.find(".pmx") != std::string::npos) {
				auto mv1s = After.substr(0, After.find(".pmx")) + ".mv1";
				if (std::filesystem::exists(mv1s.c_str())) { After = mv1s; }
			}
			m_Variable.Add(m_Args[0], After);
		}
		//変数変換処理
		else {
			for (auto& a1 : m_Args) {
				m_Variable.ChangeStr(&a1);
			}
		}
		//モデル描画
		if (m_Func.find("LoadModel") != std::string::npos) {
			for (int i = 0, count = std::stoi(m_Args[1]); i < count; i++) {
				m_ModelControl.Load(m_Args[0]);
			}
		}
		else if (m_Args.size() >= 2 && (std::all_of(m_Args[1].cbegin(), m_Args[1].cend(), isdigit))) {
			auto* t = m_ModelControl.Get(m_Args[0], std::stoi(m_Args[1]));
			if (t) {
				//どの距離で描画するかをセット
				if (m_Func.find("SetDrawModel") != std::string::npos) {
					t->Init(NowCut, std::stoi(m_Args[2]) - 1);
				}
				else if (m_Func.find("SetModelMode") != std::string::npos) {
					static const char* Model_Type[4] = { "SKY_TRUE","NEAR_FALSE","FAR_TRUE","SHADOW_DISACTIVE" };
					if (m_Args[2] == Model_Type[0]) {
						t->isBGModel = true;
					}
					if (m_Args[2] == Model_Type[1]) {
						t->IsNearShadow = false;
					}
					if (m_Args[2] == Model_Type[2]) {
						t->IsFarShadow = true;
					}
					if (m_Args[2] == Model_Type[3]) {
						t->ShadowDrawActive = false;
					}
				}
				else {
					t->CutDetail.back().Load(m_Func, m_Args);
				}
			}
		}
		//画像描画
		if (m_Func.find("LoadGraph") != std::string::npos) {
			for (int i = 0, count = std::stoi(m_Args[1]); i < count; i++) {
				m_GraphControl.Load(
					(float)(std::stoi(m_Args[2])), (float)(std::stoi(m_Args[3])),
					std::stof(m_Args[4]), std::stof(m_Args[5]), std::stof(m_Args[6]),
					m_Args[0]);
			}
		}
		else if (m_Args.size() >= 2 && (std::all_of(m_Args[1].cbegin(), m_Args[1].cend(), isdigit))) {
			auto* t = m_GraphControl.Get(m_Args[0], std::stoi(m_Args[1]));
			if (t) {
				if (m_Func.find("SetDrawGraph") != std::string::npos) {
					t->Init(NowCut, std::stoi(m_Args[2]) - 1);
				}
				else {
					t->CutDetail.back().Load(m_Func, m_Args);
				}
			}
		}
		//SE
		if (m_Func.find("SetDrawSE") != std::string::npos) {
			m_SEControl.GetLast()->Init(m_Args[0], NowCut);
		}
		//新規カット
		if (m_Func.find("SetCut") != std::string::npos) {
			m_CutInfo.emplace_back(Cut_Info_First((LONGLONG)(1000000.f * std::stof(m_Args[0]))));
			m_CutInfoUpdate.resize(m_CutInfoUpdate.size() + 1);
			NowCut = (int)(m_CutInfo.size()) - 1;
		}
		//Camposの指定
		else if (m_Func.find("SetCampos_Attach") != std::string::npos) {
			m_PosCamCut.AddCutSwitch(NowCut, std::stoi(m_Args[0]) - 1);
			m_PosCam.emplace_back(Vector3DX::vget(std::stof(m_Args[1]), std::stof(m_Args[2]), std::stof(m_Args[3])));
		}
		else {
			//テロップ
			m_TelopClass.LoadTelop(m_Func, m_Args);
		}
		if (m_CutInfo.size() > 0) {
			m_CutInfo.back().LoadScript(m_Func, m_Args);
			m_CutInfoUpdate.back().SetupCam(m_CutInfo.back().Aim_camera);
			m_CutInfoUpdate.back().LoadScript(m_Func, m_Args);
		}
		return true;
	}
	void			EventScene::Load(void) noexcept {
		m_TelopClass.Init();
		//
		int mdata = FileRead_open("data/Cut.txt", FALSE);
		SetUseASyncLoadFlag(TRUE);
		//
		while (FileRead_eof(mdata) == 0) {
			if (ProcessMessage() != 0) {}
			if (!LoadOnce(getparams::Getstr(mdata))) { continue; }
		}
		FileRead_close(mdata);
		//
		SetUseASyncLoadFlag(FALSE);
		while (ProcessMessage() == 0 && GetASyncLoadNum() != 0) {}
		m_ModelControl.SetAfterLoad();
	}
	void			EventScene::Start(size_t StartCut) noexcept {
		m_Counter = StartCut;
		m_ModelControl.Start(m_Counter);
		m_GraphControl.Start(m_Counter);
		m_PosCamCut.Start(m_Counter);
		m_RandcamupBuf = Vector3DX::zero();
		m_RandcamvecBuf = Vector3DX::zero();
		m_RandcamposBuf = Vector3DX::zero();

		m_BaseTime = GetNowHiPerformanceCount();
		m_NowTime = (m_Counter != 0) ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0;
		m_count = 0;
	}
	void			EventScene::GetDeltaTime(void) noexcept {
		deltatime = (GetNowHiPerformanceCount() - m_BaseTime);
		m_BaseTime = GetNowHiPerformanceCount();
	}
	void			EventScene::Update(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (IsEnd()) { return; }

		if (m_NowTime >= 0) {
			if (m_count == 0) {
				m_count = 1;
				DrawParts->Update_Shadow([&] { ShadowFarDraw(); }, Vector3DX::zero(), 4.f, false);
			}
			bool isFirstLoop = (m_NowTime == 0);
			bool ResetPhysics = false;
			if (m_NowTime > m_CutInfo[m_Counter].GetTimeLimit()) {
				ResetPhysics = m_CutInfo[m_Counter].GetIsResetPhysics();
				++m_Counter;
				isFirstLoop = true;
			}

			if (IsEnd()) { return; }
			//カットの処理
			{
				m_PosCamCut.Update(m_Counter);
				m_ModelControl.FirstUpdate(m_Counter, isFirstLoop, ResetPhysics);
				m_GraphControl.FirstUpdate(m_Counter, isFirstLoop);
				m_SEControl.Update(m_Counter, isFirstLoop);
				auto& CutInfo = m_CutInfo[m_Counter];
				auto& CutInfoUpdate = m_CutInfoUpdate[m_Counter];
				if (isFirstLoop) {
					if (m_Counter > 0) {
						CutInfo.SetPrev(m_CutInfo[m_Counter - 1]);
					}
					CutInfoUpdate.SetupByPrev(m_CutInfoUpdate[m_Counter - 1]);
					CutInfo.SetUpFog();
					//
					Vector3DX vec;
					if (CutInfo.GetForcusCenter(&vec, m_ModelControl)) {
						CutInfo.Aim_camera.SetCamPos(CutInfo.Aim_camera.GetCamPos(), vec, CutInfo.Aim_camera.GetCamUp());
					}
					if (CutInfo.isResetRandCampos) { m_RandcamposBuf = Vector3DX::zero(); }
					if (CutInfo.isResetRandCamvec) { m_RandcamvecBuf = Vector3DX::zero(); }
					if (CutInfo.isResetRandCamup) { m_RandcamupBuf = Vector3DX::zero(); }

					if (m_PosCamCut.GetisActive()) {
						CutInfo.Aim_camera.SetCamPos(CutInfo.Aim_camera.GetCamVec() + m_PosCam[m_PosCamCut.GetNowCut()], CutInfo.Aim_camera.GetCamVec(), CutInfo.Aim_camera.GetCamUp());
					}
					CutInfoUpdate.ResetCam(CutInfo.Aim_camera);
				}
				else {
					CutInfoUpdate.Update(CutInfo, &m_RandcamupBuf, &m_RandcamvecBuf, &m_RandcamposBuf, &Black_Buf, &White_Buf, m_ModelControl);
					//
					if (m_PosCamCut.GetisActive()) {
						CutInfo.Aim_camera.SetCamPos(CutInfo.Aim_camera.GetCamVec() + m_PosCam[m_PosCamCut.GetNowCut()], CutInfo.Aim_camera.GetCamVec(), CutInfo.Aim_camera.GetCamUp());
					}
				}
				CutInfo.UpdateCam(&DrawParts->SetMainCamera());
			}
			//
			auto far_t = DrawParts->SetMainCamera().GetCamFar();
			PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale3DRate, far_t / 2, 0.5f * Scale3DRate, far_t);
		}
		//経過時間測定
		m_NowTime += (LONGLONG)((float)deltatime);
	}
	void			EventScene::Dispose_Load(void) noexcept {
		m_Variable.Dispose();
		m_ModelControl.Dispose();
		m_GraphControl.Dispose();
		m_SEControl.Dispose();
		m_TelopClass.Dispose();
		m_PosCamCut.Dispose();

		m_CutInfo.clear();
		m_CutInfoUpdate.clear();
		m_PosCam.clear();
	}

	void			EventScene::BGDraw(void) const noexcept {
		if (IsEnd()) { return; }
		auto* DrawParts = DXDraw::Instance();
		DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
		m_ModelControl.Draw_Far();
	}
	void			EventScene::ShadowFarDraw(void) const noexcept {
		if (IsEnd()) { return; }
		SetDrawAlphaTest(DX_CMP_GREATER, 128);
		//m_ModelControl.Draw(false, true, true);
		SetDrawAlphaTest(-1, 0);
	}
	void			EventScene::ShadowDraw(void) const noexcept {
		if (IsEnd()) { return; }
		m_ModelControl.Draw(false, false, true, TRUE);
		m_ModelControl.Draw(true, false, true, FALSE);
	}
	void			EventScene::SetShadowDraw(void) const noexcept {
		if (IsEnd()) { return; }
		m_ModelControl.Draw(false, false, true, TRUE);
		m_ModelControl.Draw(true, false, true, FALSE);
	}
	void			EventScene::MainDraw(void) const noexcept {
		if (IsEnd()) { return; }
		auto* DrawParts = DXDraw::Instance();
		auto camfar = GetCameraFar();
		if (DrawParts->GetMainCamera().GetCamFar() - 1.f < camfar && camfar < DrawParts->GetMainCamera().GetCamFar() + 1.f) {
			m_ModelControl.Draw(false, false, false, FALSE);
		}
		else {
			m_ModelControl.Draw(false, false, false, TRUE);
		}
	}
	void			EventScene::UIDraw() const noexcept {
		if (IsEnd()) { return; }
		auto* DrawParts = DXDraw::Instance();

		if (m_NowTime > 0) {
			m_TelopClass.Draw(m_NowTime);
		}
		if (Black_Buf != 0.f) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f * Black_Buf));
			DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		if (White_Buf != 0.f) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f * White_Buf));
			DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(255, 255, 255), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}

		SetDrawMode(DX_DRAWMODE_BILINEAR);
		m_GraphControl.Draw();
		SetDrawMode(DX_DRAWMODE_NEAREST);

	}
	//-------------------------------------------------------------
};
