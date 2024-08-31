#pragma once
#include	"Header.hpp"
//
namespace FPS_n2 {
	//
	static float GameSpeed = 1.0f;

	static const auto GetMocroSec(void) noexcept {
		return GetNowHiPerformanceCount() * (LONGLONG)(GameSpeed*1000.f) / 1000;
	}

	template <class T>
	static void easing_set_SetSpeed(T* first, const T& aim, const float& ratio) noexcept {
		if (ratio == 0.f) {
			*first = aim;
		}
		else {
			auto* DrawParts = DXDraw::Instance();
			*first = Lerp(*first, aim,(1.f - std::powf(ratio, 60.f / DrawParts->GetFps() * GameSpeed)));
		}
	};
	//
	class LoadScriptClass {
	private:
		struct VARIABLE {
			std::string Base;
			std::string After;
		};
	private:
		std::vector<VARIABLE> m_Variable;
		std::string m_Func;
		std::vector<std::string> m_Args;
	private:
		static void Sub_Func(std::string& func_t, const char& in_str) noexcept {
			size_t str_switch = 0;
			size_t str_in = std::string::npos;
			bool in = false;
			while (true) {
				if (str_switch != std::string::npos) { str_switch = func_t.find('\"', str_switch + 1); in ^= 1; }
				str_in = func_t.find(in_str, str_in + 1);
				if (str_in != std::string::npos) {
					if (str_switch != std::string::npos && str_switch < str_in && in) {
						continue;
					}
					func_t = func_t.erase(str_in, 1);
					continue;
				}
				break;
			}
		}
	public:
		//Getter
		const auto& Getfunc(void) const noexcept { return m_Func; }
		const auto& Getargs(void) const noexcept { return m_Args; }
		const auto* GetArgFromPath(std::string_view Path) const noexcept {
			const VARIABLE* sel = nullptr;
			for (const auto& a : m_Variable) {
				if (a.After == Path) {
					sel = &a;
					break;
				}
			}
			return sel;
		}
		//スクリプト読み込み処理
		void LoadScript(std::string_view func_t) noexcept {
			m_Args.clear();
			m_Func = func_t;
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
			//()と,で囲われた部分から引数を取得
			if (m_Func != "") {
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
		}
		//処理
		bool SetArgs(std::vector<std::string>* NAMES) noexcept {
			//変数登録
			if (m_Func.find("SetArg") != std::string::npos) {
				m_Variable.resize(m_Variable.size() + 1);
				m_Variable.back().Base = m_Args[0];
				if (m_Args[1].find(".pmx") != std::string::npos) {
					auto mv1s = m_Args[1].substr(0, m_Args[1].find(".pmx")) + ".mv1";
					if (std::filesystem::exists(mv1s.c_str())) {
						m_Variable.back().After = mv1s;
					}
					else {
						m_Variable.back().After = m_Args[1];
					}

					NAMES->resize(NAMES->size() + 1);
					NAMES->back() = m_Variable.back().After;
				}
				else {
					m_Variable.back().After = m_Args[1];
				}
				return true;
			}
			//変数変換処理
			else {
				for (auto& a1 : m_Args) {
					for (auto& a2 : m_Variable) {
						if (a1 == a2.Base) {
							a1 = a2.After;
							break;
						}
					}
				}
				return false;
			}
			//
		}
	};
	class TelopClass {
	private:
		class Cut_tex {
			int xpos = 0;
			int ypos = 0;
			int size = 0;
			int LMR = 1;
			std::string str;
			LONGLONG START_TIME = 0;
			LONGLONG END_TIME = 0;
		public:
			Cut_tex(void) noexcept {
				xpos = 0;
				ypos = 0;
				size = 12;
				str = "test";
				START_TIME = (LONGLONG)(1000000.f * 0.01f);
				END_TIME = (LONGLONG)(1000000.f * 1.01f);
			}
			void Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) noexcept {
				this->xpos = xp;
				this->ypos = yp;
				this->size = Fontsize;
				this->str = mag;
				this->START_TIME = StartF;
				this->END_TIME = StartF + ContiF;;
				this->LMR = m_LMR;
			}
			void Draw(LONGLONG nowTimeWait) const noexcept {
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
		};
	private:
		std::vector<Cut_tex> Texts;
		LONGLONG StartF = 0;
		LONGLONG ContiF = 0;
	public:
		void Init(void) noexcept {
			StartF = 0;
			ContiF = 0;
		}
		void LoadTelop(const std::string &func, const std::vector<std::string>& args) noexcept {
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
		void Draw(LONGLONG nowTimeWait) const noexcept {
			for (auto& t : Texts) {
				t.Draw(nowTimeWait);
			}
		}
	};
	//
	class CutInfoClass {
		class On_Off {
		public:
			int On = 0;
			int Off = 0;
			void SetSwitch(int on, int off) noexcept {
				On = on;
				Off = off;
			}
			bool IsIn(size_t value) const noexcept {
				return On <= value && value <= Off;
			}
		};
	private:
		bool a_switch = false;
	public:
		std::vector<On_Off> Switch;
		int nowcut = 0;
		bool isFirstCut = false;
	public:
		const auto& GetSwitch(void) const noexcept { return a_switch; }
		void Init(int startFrame, int ofset) noexcept {
			this->Switch.resize(this->Switch.size() + 1);
			this->Switch.back().SetSwitch(startFrame, startFrame + ofset);
		}
		void Start(size_t Counter) noexcept {
			this->a_switch = false;
			this->isFirstCut = false;

			this->nowcut = 0;
			while (true) {
				if (this->Switch.size() > this->nowcut) {
					auto& inf_b = this->Switch[this->nowcut];
					if (Counter > inf_b.On) {
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
				isFirstCut = (Counter == inf_b.On);
				if (inf_b.IsIn(Counter)) {
					this->nowcut--;
					this->a_switch = true;
				}
			}
		}
		bool Update_(size_t Counter) noexcept {
			if (this->Switch.size() > this->nowcut) {
				auto& inf_b = this->Switch[this->nowcut];
				isFirstCut = (Counter == inf_b.On);
				if (isFirstCut) {
					this->a_switch = true;
				}
				if (this->a_switch) {
					if (Counter > inf_b.Off) {
						this->a_switch = false;
						this->nowcut++;
						return true;
					}
				}
			}
			return false;
		}
		void Insert(int ID, int Start, int End) noexcept {
			Switch.insert(Switch.begin() + ID, On_Off());
			Switch[ID].SetSwitch(Start, End);
		}
	};
	//詳細
	class CutinfoDetail {
	public:
		CutInfoClass Blur;
		int animsel = -1;
		bool isloop{ false };
		float animspeed{ 0.f };
		float animspeed_Dist{ -1.f };
		float animspeed_Per{ -1.f };


		Matrix4x4DX mat_p;
		//matrix用情報
		Vector3DX pos_p;
		float Yrad1_p{ 0.f };
		float Yrad2_p{ 0.f };
		bool usemat{ false };
		float startframe{ 0.f };
		float PhysicsSpeed_{ -1.f };
		float OpacityRate{ -1.f };
		float OpacityRate_Dist{ -1.f };
		float OpacityRate_Per{ -1.f };
	};
	class CutAttachDetail {
	public:
		Vector3DX poscam;
	};
	//モデルコントロール
	class ModelControl {
	public:
		class Model {
			float b_run{ 0.f };
			float b_runrange{ 0.f };
			std::vector<std::pair<int, std::string>> FrameNum;
			moves move;
			int Anim_Sel = 0;
			int DrawCount{ 0 };
			Matrix4x4DX mat_p;			//matrix用情報
			Vector3DX pos_p;
			float Yrad1_p{ 0.f };
			float Zrad1_p{ 0.f };
			float Yrad2_p{ 0.f };
			int prevID = -1;
			float AnimChange{ 0.f };
		public:
			bool isBGModel{ false };
			bool ShadowDrawActive{ true };//影を書き込むか
			bool IsNearShadow = true;
			bool IsFarShadow{ false };
			bool isBase = true;
			std::string Path;
			size_t BaseID = 0;
			size_t numBase = 0;
			MV1 obj;
			float PhysicsSpeed{ 1.f };
			float OpacityRate{ 1.f };
			float OpacityRate_Dist{ 1.f };
			float OpacityRate_Per{ 1.f };
			CutInfoClass Cutinfo;
			std::vector<CutinfoDetail> CutDetail;//カットの詳細
			bool isDraw{ false };
			bool isEndLoad{ false };
			bool isBigModel{ false };
			bool isFarPhysics{ false };
			bool isOutFrustum{ false };
			bool canUpdate{ true };
			float animspd{ 0.f };
			float animspd_Dist{ 1.f };
			float animspd_Per{ 1.f };
		public:
			Model(void) noexcept {
				isDraw = false;
				isEndLoad = false;
				isFarPhysics = false;
				isOutFrustum = false;
				isBigModel = true;

				animspd = 1.f;
			}
			void Init(int startFrame, int ofset) noexcept {
				this->CutDetail.resize(this->CutDetail.size() + 1);
				this->Cutinfo.Switch.resize(this->Cutinfo.Switch.size() + 1);
				this->Cutinfo.Switch.back().SetSwitch(startFrame, startFrame + ofset);
			}
			void AddFrame(std::string_view FrameName) noexcept {
				int siz = obj.GetFrameNum();
				for (int i = 0; i < siz; i++) {
					if (obj.GetFrameName(i) == FrameName) {
						FrameNum.resize(FrameNum.size() + 1);
						FrameNum.back().first = (int)i;
						FrameNum.back().second = FrameName;
						return;
					}
				}
			}
			const auto GetFrame(std::string_view FrameName) const noexcept {
				for (auto& F : FrameNum) {
					if (F.second == FrameName) {
						return static_cast<Vector3DX>(obj.GetFramePosition(F.first));
					}
				}
				return Vector3DX::zero();
			}
			const auto GetFrameMat(std::string_view FrameName) const noexcept {
				for (auto& F : FrameNum) {
					if (F.second == FrameName) {
						return obj.GetFrameLocalWorldMatrix(F.first);
					}
				}
				return Matrix4x4DX::identity();
			}
			static void Sel_AnimNum(MV1&model, int sel, float pers) noexcept {
				for (int i = 0, Num = static_cast<int>(model.GetAnimNum()); i < Num; ++i) {
					model.SetAnim(i).SetPer((i == sel) ? pers : (1.f - pers));
				}
			}
			void ChangeAnim(int ID, bool isloop, float speed, bool isfastchange) noexcept {
				auto* DrawParts = DXDraw::Instance();
				if (isfastchange) {
					AnimChange = 1.f;
				}
				if (prevID != ID) {
					AnimChange = 1.f;
					//this->obj.SetAnim(ID).time = this->obj.GetAnim(prevID).time;
				}
				prevID = ID;
				Sel_AnimNum(this->obj, ID, AnimChange);
				AnimChange = std::clamp(AnimChange + 1.f / DrawParts->GetFps(), 0.f, 1.f);
				if (ID < this->obj.GetAnimNum()) {
					this->obj.SetAnim(ID).Update(isloop, speed);
				}
			}
			void Update(bool isUpdateAnim) noexcept {
				this->DrawCount = 0;
				if (this->OpacityRate_Per < 1.f) {
					easing_set_SetSpeed(&this->OpacityRate, this->OpacityRate_Dist, this->OpacityRate_Per);
				}
				if (this->animspd_Per < 1.f) {
					easing_set_SetSpeed(&this->animspd, this->animspd_Dist, this->animspd_Per);
				}

				MV1SetOpacityRate(this->obj.GetHandle(), this->OpacityRate);
				if (this->isDraw && isUpdateAnim) {
					this->obj.UpdateAnimAll();
				}
			}
			void Draw(int isfar_ = -1) noexcept {
				if (this->isDraw && this->OpacityRate > 0.f) {
					if (!this->isBigModel) {
						if (isfar_ == TRUE) {
							if (this->isOutFrustum) {
								this->DrawCount++;
								this->obj.DrawModel();
							}
						}
						else if (isfar_ == FALSE) {
							if (!this->isOutFrustum) {
								this->DrawCount++;
								this->obj.DrawModel();
							}
						}
						else {
							this->DrawCount++;
							this->obj.DrawModel();
						}
					}
					else {
						this->DrawCount++;
						this->obj.DrawModel();
					}
				}
			}
			void SetPhysics(bool isReset, float x) const noexcept {
				if (this->isDraw) {
					if (isReset) {
						this->obj.PhysicsResetState();
					}
					else {
						auto* DrawParts = DXDraw::Instance();
						this->obj.PhysicsCalculation(1000.0f / (DrawParts->GetFps() * std::max(GameSpeed, 0.1f) * (120.f / 60.f)) * this->PhysicsSpeed * x);
					}
				}
			}
		};
	private:
		std::vector<Model> model;
		size_t Max = 0;
		size_t P_cnt = 0;
	public:
		const auto& GetMax(void) const noexcept { return Max; }
		const auto& GetModel(void) const noexcept { return model; }
		ModelControl(void) noexcept {
			model.resize(64);
			Max = 0;
		}
		~ModelControl(void) noexcept {

		}
		void Load(std::string_view Path) noexcept {
			MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
			for (int i = 0; i < 256; i++) {
				MV1SetLoadCalcPhysicsWorldGravity(i, Vector3DX::vget(0, 0, 1.f).get());
			}
			MV1SetLoadModelPhysicsCalcPrecision(1);

			for (size_t i = 0; i < Max; i++) {
				if (model[i].isBase && model[i].Path == Path) {
					model[Max].Path = Path;
					model[Max].isBase = false;
					model[Max].obj = model[i].obj.Duplicate();
					model[i].numBase++;
					model[Max].BaseID = model[i].numBase;
					Max++;
					return;
				}
			}
			model[Max].Path = Path;
			model[Max].isBase = true;
			model[Max].numBase = 0;
			model[Max].BaseID = 0;
			MV1::Load(model[Max].Path, &(model[Max].obj), DX_LOADMODEL_PHYSICS_REALTIME);/*DX_LOADMODEL_PHYSICS_REALTIME*/
			Max++;
		}
		Model* Get(std::string_view Path, size_t Sel = 0) noexcept {
			for (size_t i = 0; i < Max; i++) {
				if (model[i].Path == Path && model[i].BaseID == Sel) {
					return &(model[i]);
				}
			}
			return nullptr;
		}
		void Start(size_t Counter) noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				m.Cutinfo.Start(Counter);
				m.isDraw = m.Cutinfo.GetSwitch();
			}
		}
		void FirstUpdate(size_t Counter, bool isFirstLoop) noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				while (true) {
					bool tt = m.Cutinfo.Update_(Counter);
					m.isDraw = m.Cutinfo.GetSwitch();
					if (m.isDraw && (m.Cutinfo.nowcut < m.CutDetail.size())) {
						auto& inf = m.CutDetail[m.Cutinfo.nowcut];
						if (isFirstLoop && m.Cutinfo.isFirstCut) {
							//最初のアニメーション動作
							if (inf.usemat) {
								m.obj.SetMatrix(inf.mat_p);
							}
							if (inf.animsel >= 0 && inf.startframe >= 0.f) {
								m.obj.SetAnim(inf.animsel).SetTime(inf.startframe);
							}
							if (inf.PhysicsSpeed_ >= 0.f) {
								m.PhysicsSpeed = inf.PhysicsSpeed_;
							}
							if (inf.OpacityRate >= 0.f) {
								m.OpacityRate = inf.OpacityRate;
							}
							if (inf.OpacityRate_Dist >= 0.f) {
								m.OpacityRate_Dist = inf.OpacityRate_Dist;
							}
							if (inf.OpacityRate_Per >= 0.f) {
								m.OpacityRate_Per = inf.OpacityRate_Per;
							}
							if (inf.animspeed_Dist >= 0.f) {
								m.animspd_Dist = inf.animspeed_Dist;
							}
							if (inf.animspeed_Per >= 0.f) {
								m.animspd_Per = inf.animspeed_Per;
							}
						}
						//アニメーション動作
						if (inf.animsel >= 0) {
							m.ChangeAnim(inf.animsel, inf.isloop, inf.animspeed * m.animspd * GameSpeed, isFirstLoop && m.Cutinfo.isFirstCut);
						}
					}
					if (tt) {
						continue;
					}
					break;
				}
			}

			int sp = 0;
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				m.canUpdate = true;
				if (!m.isOutFrustum && m.isDraw) {
					m.canUpdate = (sp % 2 == P_cnt);
					//sp++;
				}
			}
			//++P_cnt %= 2;
		}
		void SetPhysics(bool reset_p) noexcept {
			if (reset_p) {
				for (size_t i = 0; i < Max; i++) {
					model[i].SetPhysics(true, 1.f);
				}
			}
			else {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (m.isOutFrustum) {
						if (!m.isFarPhysics) {
							m.SetPhysics(true, 1.f);
							m.isFarPhysics = true;
						}
					}
					else {
						m.isFarPhysics = false;
						if (m.isDraw && m.canUpdate) {
							m.SetPhysics(false, 2.f);
						}
					}
				}
				//
			}
		}
		void Set(void) noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				//
				if (!m.isEndLoad && CheckHandleASyncLoad(m.obj.GetHandle()) == FALSE) {
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
		}
		void Update(void) noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				if (m.isDraw) {
					m.Update(m.canUpdate);/**/
				}
			}
		}
		void Draw_Far(void) noexcept {
			auto fog_e = GetFogEnable();
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			{
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (m.isBGModel) {
						m.Draw();
					}
				}
			}
			SetUseLighting(TRUE);
			SetFogEnable(fog_e);
		}
		void Draw(bool innearshadow, bool infarshadow, bool ShadowActive, int isCheckFar = -1) noexcept {
			if (infarshadow) {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (!m.isBGModel && m.IsFarShadow) {
						if (ShadowActive && !m.ShadowDrawActive) { continue; }
						m.Draw(isCheckFar);
					}
				}
			}
			else if (innearshadow) {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (!m.isBGModel && m.IsNearShadow) {
						if (ShadowActive && !m.ShadowDrawActive) { continue; }
						m.Draw(isCheckFar);
					}
				}
			}
			else {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (!m.isBGModel) {
						if (ShadowActive && !m.ShadowDrawActive) { continue; }
						m.Draw(isCheckFar);
					}
				}
			}
		}
		void CheckInCamera(float far_) noexcept {
			auto camfar = GetCameraFar();
			//カメラのfarが適正範囲の設定の場合
			if (far_ - 1.f < camfar&& camfar < far_ + 1.f) {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					m.isOutFrustum = false;
					m.isBigModel = false;
					auto center = m.GetFrame("上半身");
					if (center != Vector3DX::zero()) {
						//center += (center - GetCameraPosition()).normalized()*-7.f;


						if (CheckCameraViewClip_Box(
							(center - Vector3DX::vget(-10.f, -10.f, -10.f)).get(),
							(center - Vector3DX::vget(10.f, 10.f, 10.f)).get()
						) == TRUE) {
							m.isOutFrustum = true;
						}
						//m.isOutFrustum = false;
						/*
						if (CheckCameraViewClip(center.get()) == TRUE) {
							m.isOutFrustum = true;
						}
						//*/
					}
					else {
						m.isBigModel = true;
					}
				}
			}
		}
	};
	class GraphControl {
		class Graph {
			class infos {
				class infos_rand {
				public:
					float Ans = 0;
					float Base = 0;
					float Per = 1.f;
					void Set(float m_p, float m_base, float m_per) noexcept {
						Ans = m_p;
						Base = m_base;
						Per = m_per;
					}
					void Set(float m_base, float m_per) noexcept {
						Base = m_base;
						Per = m_per;
					}
					void UpdateRand(void) noexcept {
						easing_set_SetSpeed(&this->Ans, GetRandf(this->Base), this->Per);
					}
				};
			public:
				float Ans = 0;
				float Base = 0;
				float Per = 1.f;
				infos_rand Rand_;
				void Set(float m_p, float m_base, float m_per) noexcept {
					Ans = m_p;
					Base = m_base;
					Per = m_per;
					Rand_.Set(0, 0, 1.f);
				}
				void Set(float m_base, float m_per) noexcept {
					Base = m_base;
					Per = m_per;
				}
				void Update(void) noexcept {
					Rand_.UpdateRand();
					easing_set_SetSpeed(&this->Ans, this->Base + this->Rand_.Ans, this->Per);
				}
			};
		private:
			std::vector<Graph> blur_t;
			GraphHandle handle;
			int Bright_R = 255;
			int Bright_G = 255;
			int Bright_B = 255;
			bool isblurstart = false;
		public:
			infos X;
			infos Y;
			infos Rad;
			infos Scale;
			infos Alpha;
			bool isDraw = false;
			bool isblur = false;
			int xsize = -1;
			int ysize = -1;
			CutInfoClass Cutinfo;
			std::vector<CutinfoDetail> CutDetail;//オンにするカット
			std::string Path;
		public:
			void Init(int startFrame, int ofset) noexcept {
				this->CutDetail.resize(this->CutDetail.size() + 1);
				this->Cutinfo.Switch.resize(this->Cutinfo.Switch.size() + 1);
				this->Cutinfo.Switch.back().SetSwitch(startFrame, startFrame + ofset);
			}
			void SetBright(int b_r, int b_g, int b_b) noexcept {
				Bright_R = b_r;
				Bright_G = b_g;
				Bright_B = b_b;
			}
			void Set(float xp, float yp, float rd, float al, float sc, std::string_view Path_t) noexcept {
				this->X.Set(xp, 0.f, 1.f);
				this->Y.Set(yp, 0.f, 1.f);
				this->Rad.Set(rd, 0.f, 1.f);
				this->Alpha.Set(al, 1.f, 1.f);
				this->Scale.Set(sc, 0.f, 1.f);
				this->isDraw = false;
				this->isblur = false;
				this->handle = GraphHandle::Load(Path_t);
				this->handle.GetSize(&xsize, &ysize);
				this->blur_t.resize(20);
			}
			void Set_Base(float xp, float xper, float yp, float yper, float rd, float scl, float rdper, float sclper, float alp, float alpper) noexcept {
				this->X.Base = xp;
				this->X.Per = xper;

				this->Y.Base = yp;
				this->Y.Per = yper;

				this->Rad.Base = rd;
				this->Rad.Per = rdper;

				this->Scale.Base = scl;
				this->Scale.Per = sclper;

				this->Alpha.Base = alp;
				this->Alpha.Per = alpper;
			}
			void Set_Rand(float xp, float xper, float yp, float yper, float rd, float rdper, float scl, float sclper) noexcept {
				this->X.Rand_.Set(xp, xper);
				this->Y.Rand_.Set(yp, yper);
				this->Rad.Rand_.Set(deg2rad(rd), rdper);
				this->Scale.Rand_.Set(scl, sclper);
			}
			void Update(void) noexcept {
				this->X.Update();
				this->Y.Update();
				this->Rad.Update();
				this->Scale.Update();
				this->Alpha.Update();
				if (this->isblur) {
					if (!this->isblurstart) {
						this->isblurstart = true;
						for (int i = 0; i < this->blur_t.size(); i++) {
							this->blur_t[i].X.Ans = this->X.Ans;
							this->blur_t[i].Y.Ans = this->Y.Ans;
							this->blur_t[i].Rad.Ans = this->Rad.Ans;
							this->blur_t[i].isDraw = false;
						}
					}
					for (int i = 0; i < (int)(this->blur_t.size()) - 1; i++) {
						this->blur_t[i].X.Ans = this->blur_t[static_cast<size_t>(i + 1)].X.Ans;
						this->blur_t[i].Y.Ans = this->blur_t[static_cast<size_t>(i + 1)].Y.Ans;
						this->blur_t[i].Rad.Ans = this->blur_t[static_cast<size_t>(i + 1)].Rad.Ans;
						this->blur_t[i].Scale.Ans = this->blur_t[static_cast<size_t>(i + 1)].Scale.Ans;
						this->blur_t[i].Alpha.Ans = (float)(static_cast<size_t>(i + 1)) / (float)(this->blur_t.size()) * this->Alpha.Ans;
						this->blur_t[i].isDraw = this->blur_t[static_cast<size_t>(i + 1)].isDraw;
					}
					this->blur_t.back().X.Ans = this->X.Ans;
					this->blur_t.back().Y.Ans = this->Y.Ans;
					this->blur_t.back().Rad.Ans = this->Rad.Ans;
					this->blur_t.back().Scale.Ans = this->Scale.Ans;
					this->blur_t.back().isDraw = this->isDraw;
				}
				else {
					this->isblurstart = false;
				}
			}
			void Draw_Common(const GraphHandle& m_handle, float scale_) const noexcept {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*this->Alpha.Ans));
				m_handle.DrawRotaGraph((int)(this->X.Ans), (int)(this->Y.Ans), scale_*this->Scale.Ans, this->Rad.Ans, true);
			}
			void Draw(float scale_) const noexcept {
				if (this->isDraw && this->Alpha.Ans > 0.f) {
					SetDrawBright(Bright_R, Bright_G, Bright_B);
					if (this->isblur) {
						for (int i = 0; i < this->blur_t.size(); i++) {
							if (this->blur_t[i].isDraw) {
								this->blur_t[i].Draw_Common(this->handle, scale_);
							}
						}
					}
					Draw_Common(this->handle, scale_);
					SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
		};
		std::vector<Graph> model;
		size_t Max = 0;
	public:
		GraphControl(void) noexcept {
			model.resize(64);
			Max = 0;
		}
		void Load(float xp, float yp, float rad, float alpha, float scale, std::string_view Path) noexcept {
			model[Max].Path = Path;
			model[Max].Set(xp, yp, rad, alpha, scale, Path);
			Max++;
		}
		Graph* Get(std::string_view Path, size_t Sel = 0) noexcept {
			int Cnt = 0;
			for (size_t i = 0; i < Max; i++) {
				if (model[i].Path == Path) {
					if (Cnt >= Sel) {
						return &(model[i]);
					}
					Cnt++;
				}
			}
			return nullptr;
		}
		void Start(size_t Counter) noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				m.Cutinfo.Start(Counter);
				m.isDraw = false;
				for (auto& B : m.CutDetail) {
					B.Blur.Start(Counter);
				}
			}
		}
		void FirstUpdate(size_t Counter, bool isFirstLoop) noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				while (true) {
					bool tt = m.Cutinfo.Update_(Counter);
					m.isDraw = m.Cutinfo.GetSwitch();
					if (m.isDraw && (m.Cutinfo.nowcut < m.CutDetail.size())) {
						auto& inf = m.CutDetail[m.Cutinfo.nowcut];
						//ブラー操作
						inf.Blur.Update_(Counter);
						m.isblur = inf.Blur.GetSwitch();

						if (inf.OpacityRate_Dist >= 0.f) {
							if (inf.OpacityRate_Per == 0) {
								if (isFirstLoop && m.Cutinfo.isFirstCut) {
									//最初のアニメーション動作
									m.Alpha.Set(inf.OpacityRate_Dist, inf.OpacityRate_Per);
								}
							}
							//0以外はいーじんぐ
							else  if (inf.OpacityRate_Per >= 0.f) {
								m.Alpha.Set(inf.OpacityRate_Dist, inf.OpacityRate_Per);
							}
						}
						//アニメーション動作
					}
					if (tt) {
						m.isblur = false;
						continue;
					}
					break;
				}
			}
		}
		void Set(void) noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				m.isDraw = false;
			}
		}
		void Update(void) noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				m.Update();
			}
		}
		void Draw(int Disp_y) const noexcept {
			for (size_t i = 0; i < Max; i++) {
				auto& m = model[i];
				m.Draw((float)Disp_y / m.ysize);
			}
		}
	};
	//
	class ForcusControl {
		bool Use{ false };
		std::string Path;
		size_t ID;
		std::string Frame;
		Vector3DX Add;
	public:
		float Per = 1.f;
	public:
		void SetUse(bool value) noexcept { Use = value; }
		const auto& GetIsUse(void) const noexcept { return Use; }
		void Init(void) noexcept {
			Use = false;
			Path = "";
			ID = 0;
			Frame = "";
			Add = Vector3DX::zero();
		}
		void Set(std::string_view path, size_t id, std::string_view frame, const Vector3DX& add) noexcept {
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
		const auto GetForce(ModelControl& models) const noexcept {
			return models.Get(this->Path, this->ID)->GetFrame(this->Frame) + this->Add;
		}
	};
	class Cut_Info_First {
	public:
		LONGLONG TimeLimit{ 0 };
		bool UsePrevAim{ false };
		bool IsResetPhysics{ false };
		Camera3DInfo Aim_camera;
		float cam_per{ 0.f };
		bool isResetRandCampos{ false };
		bool isResetRandCamvec{ false };
		bool isResetRandCamup{ false };
		int fog[3]{ -1,-1,-1 };
		float fog_range[2]{ -1.f,-1.f };
		std::vector<ForcusControl> Forcus;
	public:
		//Getter
		const auto& GetTimeLimit(void) const noexcept { return TimeLimit; }
		void SetTimeLimit(LONGLONG value) noexcept { TimeLimit = value; }
		//
		Cut_Info_First(void) noexcept {
			Aim_camera.SetCamPos(Vector3DX::vget(0, 10, -30), Vector3DX::vget(0, 10, 0), Vector3DX::up());
			Aim_camera.SetCamInfo(deg2rad(15), 1.f, 300.f);
			cam_per = 0.95f;
			IsResetPhysics = false;
		}
		~Cut_Info_First(void) noexcept {
		}
		void SetPrev(const Cut_Info_First& tgt) noexcept {
			if (this->UsePrevAim) {
				this->Aim_camera = tgt.Aim_camera;
				this->cam_per = tgt.cam_per;
			}
		}
		bool LoadScript(const std::string &func, const std::vector<std::string>& args) noexcept {
			//Campos
			if (func.find("SetCampos_NoneRad") != std::string::npos) {
				this->Aim_camera.SetCamPos(
					Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
					this->Aim_camera.GetCamVec(),
					this->Aim_camera.GetCamUp()
				);
				return true;
			}
			//Camvec
			else if (func.find("SetCamvec") != std::string::npos) {
				this->Aim_camera.SetCamPos(
					this->Aim_camera.GetCamPos(),
					Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
					this->Aim_camera.GetCamUp()
				);
				return true;
			}
			else if (func.find("SetCamForcus") != std::string::npos) {
				this->Forcus.resize(this->Forcus.size() + 1);
				this->Forcus.back().Set(args[0], std::stol(args[1]), args[2], Vector3DX::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5])));
			}
			//CamUp
			else if (func.find("SetCamup") != std::string::npos) {
				this->Aim_camera.SetCamPos(
					this->Aim_camera.GetCamPos(),
					this->Aim_camera.GetCamVec(),
					Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]))
				);
				return true;
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
					this->fog[0] = std::stoi(args[0]);
					this->fog[1] = std::stoi(args[1]);
					this->fog[2] = std::stoi(args[2]);
					this->fog_range[0] = std::stof(args[3]);
					this->fog_range[1] = std::stof(args[4]);
				}
				else {
					this->fog[0] = -2;
				}
			}
			return false;
		}
	};
	class Cut_Info_Update {
	private:
		bool isUseNotFirst{ false };
		float NotFirst_per = -1.f;
		float fov_per{ 0.f };
	public:
		float m_RandcamupPer;
		Vector3DX m_RandcamupSet;
		float m_RandcamvecPer;
		Vector3DX m_RandcamvecSet;
		float m_RandcamposPer;
		Vector3DX m_RandcamposSet;
		std::vector<ForcusControl> Forcus;
		bool IsUsePrevBuf{ false };
		size_t CutSel = 0;
		size_t OLDCutSel = SIZE_MAX;
		float campos_per{ 0.f };
		float camvec_per{ 0.f };
		float camup_per{ 0.f };
		Camera3DInfo CameraNotFirst;
		Camera3DInfo CameraNotFirst_Vec;
		bool IsSetBlack{ false };
		float Black_Per = 1.f;
		float Black = 0.f;
		bool IsSetWhite{ false };
		float White_Per = 1.f;
		float White = 0.f;;
	public:
		Cut_Info_Update(void) noexcept {
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
			for (auto&f : Forcus) {
				f.Init();
			}
		}
		~Cut_Info_Update(void) noexcept {
		}
		void SetForce(float camvecPer, std::string_view ModelPath, int ModelID, std::string_view Frame, const Vector3DX& Add) noexcept {
			this->camvec_per = camvecPer;
			this->Forcus.resize(this->Forcus.size() + 1);
			this->Forcus.back().Set(ModelPath, ModelID, Frame, Add);
		}
		void LoadScript(const std::string &func, const std::vector<std::string>& args) noexcept {
			//カメラのアップデート
			if (func.find("SetUpdateEnable") != std::string::npos) {
				this->isUseNotFirst = true;
			}
			//camvec
			else if (func.find("SetUpdateCamvec") != std::string::npos) {
				this->camvec_per = std::stof(args[0]);
				if (args.size() > 1) {
					Vector3DX pos_t = this->CameraNotFirst.GetCamPos();
					Vector3DX vec_t = this->CameraNotFirst.GetCamVec();
					Vector3DX up_t = this->CameraNotFirst.GetCamUp();
					this->CameraNotFirst.SetCamPos(
						pos_t,
						Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])),
						up_t);
				}
			}
			else if (func.find("SetUpdateCamForcus") != std::string::npos) {
				this->SetForce(std::stof(args[0]), args[1], std::stoi(args[2]), args[3], Vector3DX::vget(std::stof(args[4]), std::stof(args[5]), std::stof(args[6])));
			}
			else if (func.find("SetVectorUpdateCamvec") != std::string::npos) {
				Vector3DX pos_t = this->CameraNotFirst_Vec.GetCamPos();
				Vector3DX vec_t = this->CameraNotFirst_Vec.GetCamVec();
				Vector3DX up_t = this->CameraNotFirst_Vec.GetCamUp();
				this->CameraNotFirst_Vec.SetCamPos(
					pos_t,
					Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
					up_t);
			}
			//campos
			else if (func.find("SetUpdateCampos") != std::string::npos) {
				this->campos_per = std::stof(args[0]);
				if (args.size() > 1) {
					Vector3DX pos_t = this->CameraNotFirst.GetCamPos();
					Vector3DX vec_t = this->CameraNotFirst.GetCamVec();
					Vector3DX up_t = this->CameraNotFirst.GetCamUp();
					this->CameraNotFirst.SetCamPos(
						Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])),
						vec_t,
						up_t);
				}
			}
			else if (func.find("SetVectorUpdateCampos") != std::string::npos) {
				Vector3DX pos_t = this->CameraNotFirst_Vec.GetCamPos();
				Vector3DX vec_t = this->CameraNotFirst_Vec.GetCamVec();
				Vector3DX up_t = this->CameraNotFirst_Vec.GetCamUp();
				this->CameraNotFirst_Vec.SetCamPos(
					Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
					vec_t,
					up_t);
			}
			//camup
			else if (func.find("SetUpdateCamup") != std::string::npos) {
				this->camup_per = std::stof(args[0]);
				if (args.size() > 1) {
					Vector3DX pos_t = this->CameraNotFirst.GetCamPos();
					Vector3DX vec_t = this->CameraNotFirst.GetCamVec();
					Vector3DX up_t = this->CameraNotFirst.GetCamUp();
					this->CameraNotFirst.SetCamPos(
						pos_t,
						vec_t,
						Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])));
				}
			}
			else if (func.find("SetVectorUpdateCamup") != std::string::npos) {
				Vector3DX pos_t = this->CameraNotFirst_Vec.GetCamPos();
				Vector3DX vec_t = this->CameraNotFirst_Vec.GetCamVec();
				Vector3DX up_t = this->CameraNotFirst_Vec.GetCamUp();
				this->CameraNotFirst_Vec.SetCamPos(
					pos_t,
					vec_t,
					Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])));
			}
			//fov
			else if (func.find("SetUpdateCamfov") != std::string::npos) {
				this->fov_per = std::stof(args[0]);
				float near_t = this->CameraNotFirst.GetCamNear();
				float far_t = this->CameraNotFirst.GetCamFar();
				this->CameraNotFirst.SetCamInfo(deg2rad(std::stof(args[1])), near_t, far_t);
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
		}
		void Update(Cut_Info_First& Camera, ModelControl& models,
			Vector3DX& m_RandcamupBuf,
			Vector3DX& m_RandcamvecBuf,
			Vector3DX& m_RandcamposBuf
		) noexcept {
			if (this->NotFirst_per >= 0.f) {
				Camera.cam_per = this->NotFirst_per;
			}
			easing_set_SetSpeed(&m_RandcamposBuf, Vector3DX::vget(GetRandf(this->m_RandcamposSet.x), GetRandf(this->m_RandcamposSet.y), GetRandf(this->m_RandcamposSet.z)), this->m_RandcamposPer);
			easing_set_SetSpeed(&m_RandcamvecBuf, Vector3DX::vget(GetRandf(this->m_RandcamvecSet.x), GetRandf(this->m_RandcamvecSet.y), GetRandf(this->m_RandcamvecSet.z)), this->m_RandcamvecPer);
			easing_set_SetSpeed(&m_RandcamupBuf, Vector3DX::vget(GetRandf(this->m_RandcamupSet.x), GetRandf(this->m_RandcamupSet.y), GetRandf(this->m_RandcamupSet.z)), this->m_RandcamupPer);
			if (this->isUseNotFirst) {
				Vector3DX vec;
				bool isforcus = false;
#if 1
				for (auto&f : Forcus) {
					if (f.GetIsUse()) {
						vec += f.GetForce(models);
						isforcus = true;
					}
				}
				if (isforcus) {
					Vector3DX pos_t = this->CameraNotFirst.GetCamPos();
					Vector3DX vec_t = this->CameraNotFirst.GetCamVec();
					Vector3DX up_t = this->CameraNotFirst.GetCamUp();
					this->CameraNotFirst.SetCamPos(
						pos_t,
						vec / (float)(Forcus.size()),
						up_t);
				}
#else
				for (auto&f : Forcus) {
					if (f.GetIsUse()) {
						vec += f.GetForce(models) * f.Per;
						isforcus = true;
					}
				}
				if (isforcus) {
					this->CameraNotFirst.GetCamVec() = vec;
				}
#endif
				{
					Vector3DX pos_t = Camera.Aim_camera.GetCamPos();
					Vector3DX vec_t = Camera.Aim_camera.GetCamVec();
					Vector3DX up_t = Camera.Aim_camera.GetCamUp();
					float fov_t = Camera.Aim_camera.GetCamFov();
					float near_t = Camera.Aim_camera.GetCamNear();
					float far_t = Camera.Aim_camera.GetCamFar();

					easing_set_SetSpeed(&pos_t, this->CameraNotFirst.GetCamPos() + m_RandcamposBuf, this->campos_per);
					easing_set_SetSpeed(&vec_t, this->CameraNotFirst.GetCamVec() + m_RandcamvecBuf, this->camvec_per);
					easing_set_SetSpeed(&up_t, this->CameraNotFirst.GetCamUp() + m_RandcamupBuf, this->camup_per);
					easing_set_SetSpeed(&fov_t, this->CameraNotFirst.GetCamFov(), this->fov_per);
					Camera.Aim_camera.SetCamPos(pos_t, vec_t, up_t);
					Camera.Aim_camera.SetCamInfo(fov_t, near_t, far_t);
				}
			}
		}
	};
};
