#pragma once
#include"Header.hpp"

//Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {

	float GameSpeed = 1.0f;

	static const auto GetMocroSec() noexcept {
		return GetNowHiPerformanceCount() * (int)(GameSpeed*1000.f) / 1000;
	}
	template <class T>
	static void easing_set_SetSpeed(T* first, const T& aim, const float& ratio) {
		if (ratio == 0.f) {
			*first = aim;
		}
		else {
			*first = *first + (aim - *first) * (1.f - std::powf(ratio, 60.f / FPS * GameSpeed));
		}
	};

	//ファイル走査
	std::vector<WIN32_FIND_DATA> data_t;
	void GetFileNames(std::string path_t) {
		data_t.clear();
		WIN32_FIND_DATA win32fdt;
		HANDLE hFind = FindFirstFile((path_t + "*").c_str(), &win32fdt);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (win32fdt.cFileName[0] != '.') {
					data_t.resize(data_t.size() + 1);
					data_t.back() = win32fdt;
				}

			} while (FindNextFile(hFind, &win32fdt));
		} //else{ return false; }
		FindClose(hFind);
	}
	//フォントプール
	class FontPool {
	public:
		class Fonthave {
			int size = 0;
			FontHandle handle;
		public:
			const auto& Get_size(void)const noexcept { return size; }
			const auto& Get_handle(void)const noexcept { return handle; }
			void Set(int siz_t) {
				this->size = siz_t;
				this->handle = FontHandle::Create(siz_t, DX_FONTTYPE_EDGE, -1, 2);
			}
		};
	private:
		std::vector<Fonthave> havehandle;
		size_t Add(int siz_t) {
			for (auto& h : this->havehandle) {
				if (h.Get_size() == siz_t) {
					return &h - &this->havehandle.front();
				}
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(siz_t);
			return this->havehandle.size() - 1;
		}
	public:
		Fonthave& Get(int siz_t) { return this->havehandle[Add(siz_t)]; }
	};
	FontPool Fonts;
	//エフェクトリソース
	class EffectControl {
		LONGLONG Update_effect_was = 0;					//エフェクトのアップデートタイミングタイマー
	public:
		bool Update_effect_f{ true };					//エフェクトのアップデートタイミングフラグ
		std::vector<EffekseerEffectHandle> effsorce;	/*エフェクトリソース*/

		void Init(void) noexcept {
			GetFileNames("data/effect/");
			for (auto& d : data_t) {
				std::string p = d.cFileName;
				if (p.find(".efk") != std::string::npos) {
					effsorce.resize(effsorce.size() + 1);
					effsorce.back() = EffekseerEffectHandle::load("data/effect/" + p);
				}
			}
			effsorce.resize(effsorce.size() + 1);
			effsorce.back() = EffekseerEffectHandle::load("data/effect/gndsmk.efk");								//戦車用エフェクト
			Update_effect_was = GetMocroSec();
		}

		void Calc(void) noexcept {
			Update_effect_f = ((GetMocroSec() - Update_effect_was) >= 1000000 / 60);
			if (Update_effect_f) {
				Update_effect_was = GetMocroSec();
			}
		}

		void Dispose(void) noexcept {
			for (auto& e : effsorce) {
				e.Dispose();
			}
		}
	};
	EffectControl effectControl;
	//ライト
	class LightPool {
		class Lights {
		public:
			LightHandle handle;
			LONGLONG time{ 0 };
		};
		std::array<Lights, 2> handles;
		int now = 0;
		VECTOR_ref campos;
	public:
		void Put(const VECTOR_ref& pos) {
			if ((pos - campos).size() >= 10.f) { return; }
			if (handles[now].handle.get() != -1) {
				handles[now].handle.Dispose();
			}
			handles[now].time = GetMocroSec();
			handles[now].handle = LightHandle::Create(pos, 2.5f, 0.5f, 1.5f, 0.5f);
			SetLightDifColorHandle(handles[now].handle.get(), GetColorF(1.f, 1.f, 0.f, 1.f));
			++now %= handles.size();
		}

		void Update(const VECTOR_ref& campos_t) {
			campos = campos_t;
			for (auto& h : handles) {
				if (h.handle.get() != -1) {
					if ((GetMocroSec() - h.time) >= 1000000 / 30) {
						h.handle.Dispose();
					}
				}
			}
		}
	};
	LightPool Light_pool;
	//エフェクト利用コントロール
	class Effect_UseControl {
		std::array<EffectS, int(Effect::effects)> effcs;	/*エフェクト*/
		std::array<EffectS, 256> effcs_G;					/*エフェクト*/
		int G_cnt = 0;
	public:
		//
		void Set_FootEffect(const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept {
			this->effcs_G[this->G_cnt].Stop();
			this->effcs_G[this->G_cnt].Set(pos_t, nomal_t, scale);
			++this->G_cnt %= 256;
		}
		const auto Check_FootEffectCnt() noexcept {
			int cnt = 0;
			for (auto& t : this->effcs_G) {
				if (t.GetStart()) { cnt++; }
			}
			return cnt;
		}
		//
		void Set_Effect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept { this->effcs[(int)ef_].Set(pos_t, nomal_t, scale); }
		void Stop_Effect(Effect ef_) noexcept { this->effcs[(int)ef_].Stop(); }
		void SetSpeed_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Speed(value); }
		void SetScale_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Scale(value); }
		//エフェクトの更新
		void Update_Effect(void) noexcept {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs.front();
				if (index != (int)Effect::ef_smoke) {
					t.put(effectControl.effsorce[index]);
				}
			}
			for (auto& t : this->effcs_G) {
				t.put(effectControl.effsorce[(int)Effect::ef_gndsmoke]);
			}
		}
		/*おわり*/
		void Dispose_Effect(void) noexcept {
			for (auto& t : this->effcs) { t.handle.Dispose(); }
		}
	};
	//シェーダー
	class shaders {
	public:
		class shader_Vertex {
		public:
			VERTEX3DSHADER Screen_vertex[6] = { 0.0f };

			// 頂点データの準備
			void Set(std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
				int xp1 = 0;
				int yp1 = 0;
				int xp2 = DrawPts_t->disp_x;
				int yp2 = DrawPts_t->disp_y;
				Screen_vertex[0].pos = VGet(float(xp1), float(DrawPts_t->disp_y - yp1), 0.0f);
				Screen_vertex[1].pos = VGet(float(xp2), float(DrawPts_t->disp_y - yp1), 0.0f);
				Screen_vertex[2].pos = VGet(float(xp1), float(DrawPts_t->disp_y - yp2), 0.0f);
				Screen_vertex[3].pos = VGet(float(xp2), float(DrawPts_t->disp_y - yp2), 0.0f);
				Screen_vertex[0].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[1].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[2].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[3].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
				Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
				Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
				Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;
				Screen_vertex[4] = Screen_vertex[2];
				Screen_vertex[5] = Screen_vertex[1];
			}
		};
	private:
		int pshandle{ -1 }, vshandle{ -1 };
		int pscbhandle{ -1 };
		int pscbhandle2{ -1 };
	public:
		void Init(std::string vs, std::string ps) {
			this->vshandle = LoadVertexShader(("shader/" + vs).c_str());	// 頂点シェーダーバイナリコードの読み込み
			this->pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->pscbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->pshandle = LoadPixelShader(("shader/" + ps).c_str());		// ピクセルシェーダーバイナリコードの読み込み
		}
		void Set_dispsize(int dispx, int dispy) {
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->pscbhandle);			// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = float(dispx);
			dispsize->v = float(dispy);
			UpdateShaderConstantBuffer(this->pscbhandle);								// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->pscbhandle, DX_SHADERTYPE_PIXEL, 2);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
		}
		void Set_param(float param1, float param2, float param3, float param4) {
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->pscbhandle2);			// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->pscbhandle2);							// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->pscbhandle2, DX_SHADERTYPE_PIXEL, 3);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}
		void Draw_lamda(std::function<void()> doing) {
			SetUseVertexShader(this->vshandle);		// 使用する頂点シェーダーをセット
			SetUsePixelShader(this->pshandle);		// 使用するピクセルシェーダーをセット
			MV1SetUseOrigShader(TRUE);
			doing();
			MV1SetUseOrigShader(FALSE);
			SetUseVertexShader(-1);					// 使用する頂点シェーダーをセット
			SetUsePixelShader(-1);					// 使用するピクセルシェーダーをセット
		}
		void Draw(shader_Vertex& Screen_vertex) {
			Draw_lamda([&] {DrawPolygon3DToShader(Screen_vertex.Screen_vertex, 2); });
		}
	};

	static float GetRandf(float m_arg) noexcept { return -m_arg + (float)(GetRand((int)(m_arg * 2.f * 10000.f))) / 10000.f; }
	//
	class LoadScriptClass {
	private:
		class VARIABLE {
		public:
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
		const auto& Getfunc() const noexcept { return m_Func; }
		const auto& Getargs() const noexcept { return m_Args; }
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
					switch (this->LMR)
					{
					case 0:
						Fonts.Get(this->size).Get_handle().DrawString(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
						break;
					case 1:
						Fonts.Get(this->size).Get_handle().DrawString_MID(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
						break;
					case 2:
						Fonts.Get(this->size).Get_handle().DrawString_RIGHT(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
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
				Texts.back().Set(y_r(std::stoi(args[0])), y_r(std::stoi(args[1])), y_r(std::stoi(args[2])), args[3], StartF, ContiF, t);
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
		const auto& GetSwitch() const noexcept { return a_switch; }

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
			if (0 <= this->nowcut - 1 && this->nowcut - 1 < this->Switch.size()) {
				auto& inf_b = this->Switch[this->nowcut - 1];
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
		bool isloop = false;
		float animspeed = 0.f;
		MATRIX_ref mat_p;
		//matrix用情報
		VECTOR_ref pos_p;
		float Yrad1_p;
		float Yrad2_p;

		bool usemat = false;
		float startframe = 0.f;
		float PhysicsSpeed_ = -1.f;

		float OpacityRate = -1.f;
		float OpacityRate_Dist = -1.f;
		float OpacityRate_Per = -1.f;
	};
	class CutAttachDetail {
	public:
		VECTOR_ref poscam;
	};
	//モデルコントロール
	class ModelControl {
	public:
		class Model {
		public:
			bool isBGModel = false;
			bool IsNearShadow = true;
			bool IsFarShadow = false;
			bool isBase = true;
			std::string Path;
			size_t BaseID = 0;

			size_t numBase = 0;
			float PhysicsSpeed = 1.f;

			float b_run = 0;
			float b_runrange = 0;

			std::vector<std::pair<int, std::string>> FrameNum;
			MV1 obj;
			moves move;
			//private:
			bool isDraw = false;
			bool isEndLoad = false;
		public:
			CutInfoClass Cutinfo;
			std::vector<CutinfoDetail> CutDetail;//カットの詳細
			float OpacityRate = 1.f;
			float OpacityRate_Dist = 1.f;
			float OpacityRate_Per = 1.f;
			int Anim_Sel = 0;

			bool isFarPhysics = false;
			bool isOutFrustum = false;
			bool isBigModel = false;
			int DrawCount;

			MATRIX_ref mat_p;
			//matrix用情報
			VECTOR_ref pos_p;

			float Yrad1_p;
			float Zrad1_p;
			float Yrad2_p;

			VECTOR_ref Aim_p;
			VECTOR_ref OLDPos_p;
			VECTOR_ref OLDVec_p;
			float dist;
			float OLDDist;
			float pos_z_p;
			bool firststart = true;
			float Yradaddbuf_p;
			float Yradadd_p;
			float Zrad1buf_p;
			float animspd;
			float xposition = 0.f;

			VECTOR_ref footEffPos;
			bool footflag = true;
			bool footok = false;
			float foottime = 0.f;
			int footLR{ 0 };

			std::array<int, 2> GuideNum;

			bool canUpdate = true;

			bool Lookok = false;
			bool LookLR = false;
			float Looktime = 0.f;

			bool Turbook = false;
			float Turbotime = 0.f;
			float TurboSpeed = 0.f;

			std::string NearID = "";
			VECTOR_ref Nearest;

			std::string FrontID = "";
			float FrontLength;
			int Rank = -1;

			int prevID = -1;
			float AnimChange = 0.f;

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
				size_t siz = obj.frame_num();
				for (size_t i = 0; i < siz; i++) {
					if (obj.frame_name(i) == FrameName) {
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
						return obj.frame(F.first);
					}
				}
				return VECTOR_ref::zero();
			}

			static void Sel_AnimNum(MV1&model, int sel, float pers) noexcept {
				for (auto& anim_t : model.get_anime()) {
					model.get_anime(&anim_t - &model.get_anime().front()).per = (&anim_t - &model.get_anime().front() == sel) ? pers : (1.f - pers);
				}
			}
			void ChangeAnim(int ID, bool isloop, float speed, bool isfastchange) noexcept {
				if (isfastchange) {
					AnimChange = 1.f;
				}
				if (prevID != ID) {
					AnimChange = 1.f;
					this->obj.get_anime(ID).time = this->obj.get_anime(prevID).time;
				}
				prevID = ID;
				Sel_AnimNum(this->obj, ID, AnimChange);
				AnimChange = std::clamp(AnimChange + 1.f / FPS, 0.f, 1.f);
				if (ID < this->obj.get_anime().size()) {
					this->obj.get_anime(ID).Update(isloop, speed);
				}
			}
			void Update(bool isUpdateAnim) noexcept {
				this->DrawCount = 0;
				if (this->OpacityRate_Per < 1.f) {
					easing_set_SetSpeed(&this->OpacityRate, this->OpacityRate_Dist, this->OpacityRate_Per);
				}
				MV1SetOpacityRate(this->obj.get(), this->OpacityRate);
				if (this->isDraw && isUpdateAnim) {
					this->obj.work_anime();
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
			void SetPhysics(bool isReset, float x) noexcept {
				if (this->isDraw) {
					if (isReset) {
						this->obj.PhysicsResetState();
					}
					else {
						this->obj.PhysicsCalculation(1000.0f / (FPS * std::max(GameSpeed, 0.1f) * (120.f / 60.f)) * this->PhysicsSpeed * x);
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
				MV1SetLoadCalcPhysicsWorldGravity(i, VECTOR_ref::vget(0, 0, 1.f).get());
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
			MV1::Load(model[Max].Path, &(model[Max].obj), DX_LOADMODEL_PHYSICS_LOADCALC);/*DX_LOADMODEL_PHYSICS_REALTIME*/
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
		void FirstUpdate(int Counter, bool isFirstLoop) noexcept {
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
								m.obj.get_anime(inf.animsel).time = inf.startframe;
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
					m.AddFrame("右つま先");
					m.AddFrame("左つま先");
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
		void Draw(bool innearshadow, bool infarshadow, int isCheckFar = -1) noexcept {
			if (infarshadow) {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (!m.isBGModel && m.IsFarShadow) {
						m.Draw(isCheckFar);
					}
				}
			}
			else if (innearshadow) {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (!m.isBGModel && m.IsNearShadow) {
						m.Draw(isCheckFar);
					}
				}
			}
			else {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (!m.isBGModel) {
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
					if (center != VECTOR_ref::zero()) {
						//center += (center - GetCameraPosition()).Norm()*-7.f;

						if (CheckCameraViewClip(center.get()) == TRUE) {
							m.isOutFrustum = true;
						}
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
				/*
				void Set(float m_p, float m_base, float m_per, float m_rand, float m_randbase, float m_randper) noexcept {
					Ans = m_p;
					Base = m_base;
					Per = m_per;
					Rand_.Set(m_rand, m_randbase, m_randper);
				}
				*/
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
		public:
			CutInfoClass Cutinfo;
			std::vector<CutinfoDetail> CutDetail;//オンにするカット
			std::string Path;

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
				this->blur_t.resize(15);
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
						this->blur_t[i].X.Ans = this->blur_t[i + 1].X.Ans;
						this->blur_t[i].Y.Ans = this->blur_t[i + 1].Y.Ans;
						this->blur_t[i].Rad.Ans = this->blur_t[i + 1].Rad.Ans;
						this->blur_t[i].Scale.Ans = this->blur_t[i + 1].Scale.Ans;
						this->blur_t[i].Alpha.Ans = (float)(i + 1) / (float)(this->blur_t.size()) * this->Alpha.Ans;
						this->blur_t[i].isDraw = this->blur_t[i + 1].isDraw;
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
		void Load(float xp, float yp, float rd, float al, float sc, std::string_view Path) noexcept {
			model[Max].Path = Path;
			model[Max].Set(xp, yp, rd, al, sc, Path);
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
		void FirstUpdate(int Counter, bool isFirstLoop) noexcept {
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
		VECTOR_ref Add;
	public:
		float Per = 1.f;

		void SetUse(bool value) noexcept { Use = value; }
		const auto& GetIsUse() const noexcept { return Use; }
		void Init(void) noexcept {
			Use = false;
			Path = "";
			ID = 0;
			Frame = "";
			Add.clear();
		}
		void Set(std::string_view path, size_t id, std::string_view frame, const VECTOR_ref& add) noexcept {
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
			else if (frame == "FAR_TRUE") {
				this->Frame = "右ひざ";
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
		cam_info Aim_camera;
		float cam_per{ 0.f };
		bool isResetRandCampos{ false };
		bool isResetRandCamvec{ false };
		bool isResetRandCamup{ false };

		int bright[3]{ -1,-1,-1 };

		int fog[3]{ -1,-1,-1 };
		float fog_range[2]{ -1.f,-1.f };
	public:
		std::vector<ForcusControl> Forcus;
	public:
		//Getter
		const auto& GetTimeLimit() const noexcept { return TimeLimit; }
		void SetTimeLimit(LONGLONG value) noexcept { TimeLimit = value; }
	public:
		Cut_Info_First(void) noexcept {
			Aim_camera.campos = VECTOR_ref::vget(0, 10, -30);
			Aim_camera.camvec = VECTOR_ref::vget(0, 10, 0);
			Aim_camera.camup = VECTOR_ref::up();
			Aim_camera.set_cam_info(deg2rad(15), 1.f, 300.f);
			cam_per = 0.95f;
			IsResetPhysics = false;
		}
		~Cut_Info_First(void) noexcept {
		}
		//
		void SetPrev(const Cut_Info_First& tgt) noexcept {
			if (this->UsePrevAim) {
				this->Aim_camera = tgt.Aim_camera;
				this->cam_per = tgt.cam_per;
			}
		}
		//
		bool LoadScript(const std::string &func, const std::vector<std::string>& args) noexcept {
			//Campos
			if (func.find("SetCampos_NoneRad") != std::string::npos) {
				this->Aim_camera.campos = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
				return true;
			}
			//Camvec
			else if (func.find("SetCamvec") != std::string::npos) {
				this->Aim_camera.camvec = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
				return true;
			}
			else if (func.find("SetCamForcus") != std::string::npos) {
				this->Forcus.resize(this->Forcus.size() + 1);
				this->Forcus.back().Set(args[0], std::stol(args[1]), args[2], VECTOR_ref::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5])));
			}
			//CamUp
			else if (func.find("SetCamup") != std::string::npos) {
				this->Aim_camera.camup = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
				return true;
			}
			//Else
			else if (func.find("SetCamInfo") != std::string::npos) {
				this->Aim_camera.fov = deg2rad(std::stof(args[0]));
				this->Aim_camera.near_ = std::stof(args[1]);
				this->Aim_camera.far_ = std::stof(args[2]);
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
			else if (func.find("SetBright") != std::string::npos) {
				this->bright[0] = std::stoi(args[0]);
				this->bright[1] = std::stoi(args[1]);
				this->bright[2] = std::stoi(args[2]);
			}
			else if (func.find("SetFog") != std::string::npos) {
				if (args.size() == 5) {
					this->fog[0] = std::stoi(args[0]);
					this->fog[1] = std::stoi(args[1]);
					this->fog[2] = std::stoi(args[2]);
					this->fog_range[1] = std::stof(args[3]);
					this->fog_range[2] = std::stof(args[4]);
				}
				else {
					this->fog[0] = -2;
				}
			}
			return false;
		}
		//
	};
	class Cut_Info_Update {
	private:
		bool isUseNotFirst{ false };
		float NotFirst_per = -1.f;
		float fov_per{ 0.f };
	public:
		float m_RandcamupPer;
		VECTOR_ref m_RandcamupSet;
		float m_RandcamvecPer;
		VECTOR_ref m_RandcamvecSet;
		float m_RandcamposPer;
		VECTOR_ref m_RandcamposSet;
	public:
		std::vector<ForcusControl> Forcus;
		bool IsUsePrevBuf{ false };

		size_t CutSel = 0;
		size_t OLDCutSel = SIZE_MAX;

		float campos_per{ 0.f };
		float camvec_per{ 0.f };
		float camup_per{ 0.f };

		cam_info CameraNotFirst;
		cam_info CameraNotFirst_Vec;

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
			m_RandcamupSet.clear();
			m_RandcamvecPer = 0.f;
			m_RandcamvecSet.clear();
			m_RandcamposPer = 0.f;
			m_RandcamposSet.clear();
			for (auto&f : Forcus) {
				f.Init();
			}
		}
		~Cut_Info_Update(void) noexcept {
		}

		void SetForce(float camvecPer, std::string_view ModelPath, int ModelID, std::string_view Frame, const VECTOR_ref& Add) noexcept {
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
					this->CameraNotFirst.camvec.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
				}
			}
			else if (func.find("SetUpdateCamForcus") != std::string::npos) {
				this->SetForce(std::stof(args[0]), args[1], std::stoi(args[2]), args[3], VECTOR_ref::vget(std::stof(args[4]), std::stof(args[5]), std::stof(args[6])));
			}
			else if (func.find("SetVectorUpdateCamvec") != std::string::npos) {
				this->CameraNotFirst_Vec.camvec.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
			}
			//campos
			else if (func.find("SetUpdateCampos") != std::string::npos) {
				this->campos_per = std::stof(args[0]);
				if (args.size() > 1) {
					this->CameraNotFirst.campos.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
				}
			}
			else if (func.find("SetVectorUpdateCampos") != std::string::npos) {
				this->CameraNotFirst_Vec.campos.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
			}
			//camup
			else if (func.find("SetUpdateCamup") != std::string::npos) {
				this->camup_per = std::stof(args[0]);
				if (args.size() > 1) {
					this->CameraNotFirst.camup.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
				}
			}
			else if (func.find("SetVectorUpdateCamup") != std::string::npos) {
				this->CameraNotFirst_Vec.camup.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
			}
			//fov
			else if (func.find("SetUpdateCamfov") != std::string::npos) {
				this->fov_per = std::stof(args[0]);
				this->CameraNotFirst.fov = deg2rad(std::stof(args[1]));
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
			VECTOR_ref& m_RandcamupBuf,
			VECTOR_ref& m_RandcamvecBuf,
			VECTOR_ref& m_RandcamposBuf
		) noexcept {
			if (this->NotFirst_per >= 0.f) {
				Camera.cam_per = this->NotFirst_per;
			}
			easing_set_SetSpeed(&m_RandcamposBuf, VECTOR_ref::vget(GetRandf(this->m_RandcamposSet.x()), GetRandf(this->m_RandcamposSet.y()), GetRandf(this->m_RandcamposSet.z())), this->m_RandcamposPer);
			easing_set_SetSpeed(&m_RandcamvecBuf, VECTOR_ref::vget(GetRandf(this->m_RandcamvecSet.x()), GetRandf(this->m_RandcamvecSet.y()), GetRandf(this->m_RandcamvecSet.z())), this->m_RandcamvecPer);
			easing_set_SetSpeed(&m_RandcamupBuf, VECTOR_ref::vget(GetRandf(this->m_RandcamupSet.x()), GetRandf(this->m_RandcamupSet.y()), GetRandf(this->m_RandcamupSet.z())), this->m_RandcamupPer);
			if (this->isUseNotFirst) {
				VECTOR_ref vec;
				bool isforcus = false;
#if 0
				for (auto&f : Forcus) {
					if (f.GetIsUse()) {
						vec += f.GetForce(models);
						isforcus = true;
					}
				}
				if (isforcus) {
					this->CameraNotFirst.camvec = vec / (float)(Forcus.size());
				}
#else
				for (auto&f : Forcus) {
					if (f.GetIsUse()) {
						vec += f.GetForce(models) * f.Per;
						isforcus = true;
					}
				}
				if (isforcus) {
					this->CameraNotFirst.camvec = vec;
				}
#endif
				easing_set_SetSpeed(&Camera.Aim_camera.campos, this->CameraNotFirst.campos + m_RandcamposBuf, this->campos_per);
				easing_set_SetSpeed(&Camera.Aim_camera.camvec, this->CameraNotFirst.camvec + m_RandcamvecBuf, this->camvec_per);
				easing_set_SetSpeed(&Camera.Aim_camera.camup, this->CameraNotFirst.camup + m_RandcamupBuf, this->camup_per);
				easing_set_SetSpeed(&Camera.Aim_camera.fov, this->CameraNotFirst.fov, this->fov_per);
			}
		}
	};
	//kusa
	class Grass {
		//インスタシング
		class Model_Instance {
		public:
			int hitss = 0;					/*hitsの数*/
			std::vector<VERTEX3D> hitsver;	/*hits*/
			std::vector<DWORD> hitsind;	    /*hits*/
			int VerBuf = -1, IndexBuf = -1;	/*hits*/
			MV1 hits;						/*hitsモデル*/
			GraphHandle hits_pic;			/*画像ハンドル*/
			int IndexNum = -1, VerNum = -1;	/*hits*/
			int vnum = -1, pnum = -1;		/*hits*/
			MV1_REF_POLYGONLIST RefMesh{};	/*hits*/
			//初期化
			void Init(std::string pngpath, std::string mv1path) noexcept {
				SetUseASyncLoadFlag(FALSE);
				this->hits_pic = GraphHandle::Load(pngpath);		 /*grass*/
				MV1::Load(mv1path, &this->hits, false);	//弾痕
				Init_one();
			}
			void Init_one(void) noexcept {
				MV1RefreshReferenceMesh(this->hits.get(), -1, TRUE);			/*参照用メッシュの更新*/
				this->RefMesh = MV1GetReferenceMesh(this->hits.get(), -1, TRUE);	/*参照用メッシュの取得*/
			}
			//毎回のリセット
			void Clear(void) noexcept {
				this->hitss = 0;
				this->vnum = 0;
				this->pnum = 0;
				this->hitsver.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				this->hitsind.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				this->hitsver.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				this->hitsind.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			}

			void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
				this->hitss++;
				Set_start();
				{
					float asize = 200.f * caliber;
					const auto& y_vec = Normal;
					auto z_vec = y_vec.cross(Zvec).Norm();
					auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
					auto pos = Position + y_vec * 0.02f;
					MATRIX_ref mat = MATRIX_ref::GetScale(scale) * MATRIX_ref::Axis1_YZ(y_vec, z_vec);

					this->hits.SetMatrix(mat * MATRIX_ref::Mtrans(pos));
				}
				Set_one();
			}
			void Set_start(void) noexcept {
				this->IndexNum = this->RefMesh.PolygonNum * 3 * this->hitss;				/*インデックスの数を取得*/
				this->VerNum = this->RefMesh.VertexNum * this->hitss;						/*頂点の数を取得*/
				this->hitsver.resize(this->VerNum);									/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				this->hitsind.resize(this->IndexNum);								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
			}
			void Set_one(void) noexcept {
				Init_one();
				for (size_t j = 0; j < size_t(this->RefMesh.VertexNum); ++j) {
					auto& g = this->hitsver[j + this->vnum];
					g.pos = this->RefMesh.Vertexs[j].Position;
					g.norm = this->RefMesh.Vertexs[j].Normal;
					g.dif = this->RefMesh.Vertexs[j].DiffuseColor;
					g.spc = this->RefMesh.Vertexs[j].SpecularColor;
					g.u = this->RefMesh.Vertexs[j].TexCoord[0].u;
					g.v = this->RefMesh.Vertexs[j].TexCoord[0].v;
					g.su = this->RefMesh.Vertexs[j].TexCoord[1].u;
					g.sv = this->RefMesh.Vertexs[j].TexCoord[1].v;
				}
				for (size_t j = 0; j < size_t(this->RefMesh.PolygonNum); ++j) {
					for (size_t k = 0; k < std::size(this->RefMesh.Polygons[j].VIndex); ++k)
						this->hitsind[j * 3 + k + this->pnum] = WORD(this->RefMesh.Polygons[j].VIndex[k] + this->vnum);
				}
				this->vnum += this->RefMesh.VertexNum;
				this->pnum += this->RefMesh.PolygonNum * 3;
			}

			void Update(void) noexcept {
				this->VerBuf = CreateVertexBuffer(this->VerNum, DX_VERTEX_TYPE_NORMAL_3D);
				this->IndexBuf = CreateIndexBuffer(this->IndexNum, DX_INDEX_TYPE_32BIT);
				SetVertexBufferData(0, this->hitsver.data(), this->VerNum, this->VerBuf);
				SetIndexBufferData(0, this->hitsind.data(), this->IndexNum, this->IndexBuf);
			}
			void Draw(void) noexcept {
				DrawPolygonIndexed3D_UseVertexBuffer(this->VerBuf, this->IndexBuf, this->hits_pic.get(), TRUE);
			}
		};
		class grass_t {
		private:
			//std::shared_ptr<Map> MAPPTs{ nullptr };
		public:
			bool canlook = true;
			Model_Instance inst;
		public:

			void Init(int total, int sel) {
				switch (sel) {
				case 0:
					this->inst.Init("data/model/grass/grass.png", "data/model/grass/model.mv1");
					break;
				case 1:
					this->inst.Init("data/model/grass/grass2.png", "data/model/grass/model.mv1");
					break;
				case 2:
					this->inst.Init("data/model/grass/grass3.png", "data/model/grass/model.mv1");
					break;
				}
				this->inst.Clear();
				this->inst.hitss = total;

				this->inst.Set_start();
			}

			void Set_one(void) noexcept {
				this->inst.Set_one();
			}
			void put(void) noexcept {
				canlook = true;
				this->inst.Update();
			}

			void Dispose(void) noexcept {
				this->inst.hitsver.clear();
				this->inst.hitsind.clear();

				this->inst.hits.Dispose();
				this->inst.hits_pic.Dispose();
			}

			/*視界外か否かを判断*/
			void Check_CameraViewClip(const VECTOR_ref& min, const VECTOR_ref& max) noexcept {
				this->canlook = true;
				if (CheckCameraViewClip_Box(min.get(), max.get())) {
					this->canlook = false;
					return;
				}
			}

			void Draw(void) noexcept {
				if (this->canlook) {
					this->inst.Draw();
				}
			}
		};
		struct GrassPos {
			int X_PosMin = 0;
			int Y_PosMin = 0;
			int X_PosMax = 0;
			int Y_PosMax = 0;
		};
	public:
		static const int grassDiv{ 12 };//6;
		const float size{ 20.f };
	private:
		const int grasss = 50 * 50;						/*grassの数*/
		std::array<grass_t, grassDiv>grass__;
		std::array<VECTOR_ref, grassDiv>grassPosMin;
		std::array<VECTOR_ref, grassDiv>grassPosMax;
		int grasss2 = 30 * 30;							/*grassの数*/
		std::array<grass_t, grassDiv>grass2__;
		int grasss3 = 12 * 12;							/*grassの数*/
		std::array<grass_t, grassDiv>grass3__;
		int Flag = 0;
		std::array<GrassPos, grassDiv> grassPos;
	private:
		int GetColorSoftImage(int softimage, int x_, int y_) {
			int _r_, _g_, _b_;
			int CCC = 0;
			GetPixelSoftImage(softimage, x_, y_, &_r_, &_g_, &_b_, nullptr);
			if (_r_ <= 64) {}
			else if (_r_ <= 192) { CCC |= (1 << 1); }
			else if (_r_ <= 256) { CCC |= (1 << 2); }
			if (_g_ <= 64) {}
			else if (_g_ <= 192) { CCC |= (1 << 4); }
			else if (_g_ <= 256) { CCC |= (1 << 5); }
			if (_b_ <= 64) {}
			else if (_b_ <= 192) { CCC |= (1 << 7); }
			else if (_b_ <= 256) { CCC |= (1 << 8); }
			return CCC;
		}
		//y方向に操作する前提
		void SetMinMax(int CCC, int ID, int softimage, int x_t, int y_t, int sizex, int sizey) {
			int BufC = -1;
			if ((Flag & (1 << ID)) == 0) {
				Flag |= (1 << ID);
				//xmin
				grassPos[ID].X_PosMin = x_t;
				y_t;
				//ymin
				BufC = -1;
				for (int y_ = 0; y_ < sizey; y_++) {
					for (int x_ = grassPos[ID].X_PosMin; x_ < sizex; x_++) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].Y_PosMin = y_;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//xmax
				BufC = -1;
				for (int x_ = sizex - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
					for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].X_PosMax = x_ + 1;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//ymax
				BufC = -1;
				for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
					for (int x_ = grassPos[ID].X_PosMax - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].Y_PosMax = y_ + 1;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//ok
			}
		}
	public:
		void Init(void) noexcept {
			float MAPX = 8367.5f;
			float MAPZ = 5063.76f;
			float PosX = 0.f;
			float PosZ = -2300.0f;

			float MINX = -MAPX / 2.f + PosX;
			float MINZ = -MAPZ / 2.f + PosZ;

			float MAXX = MAPX / 2.f + PosX;
			float MAXZ = MAPZ / 2.f + PosZ;

			auto softimage = LoadSoftImage("data/model/map/grass.png");

			int sizex = 0, sizey = 0;
			GetSoftImageSize(softimage, &sizex, &sizey);

			Flag = 0;
			for (int x_ = 0; x_ < sizex; x_++) {
				for (int y_ = 0; y_ < sizey; y_++) {
					int CCC = GetColorSoftImage(softimage, x_, y_);
					//255,0,0
					if (CCC == (1 << 2)) {
						SetMinMax(CCC, 0, softimage, x_, y_, sizex, sizey);
					}
					//255,128,0
					else if (CCC == ((1 << 2) | (1 << 4))) {
						SetMinMax(CCC, 1, softimage, x_, y_, sizex, sizey);
					}
					//255,255,0
					else if (CCC == ((1 << 2) | (1 << 5))) {
						SetMinMax(CCC, 2, softimage, x_, y_, sizex, sizey);
					}
					//128,255,0
					else if (CCC == ((1 << 1) | (1 << 5))) {
						SetMinMax(CCC, 3, softimage, x_, y_, sizex, sizey);
					}
					//0,255,0
					else if (CCC == (1 << 5)) {
						SetMinMax(CCC, 4, softimage, x_, y_, sizex, sizey);
					}
					//0,255,128
					else if (CCC == ((1 << 5) | (1 << 7))) {
						SetMinMax(CCC, 5, softimage, x_, y_, sizex, sizey);
					}
					//0,255,255
					else if (CCC == ((1 << 5) | (1 << 8))) {
						SetMinMax(CCC, 6, softimage, x_, y_, sizex, sizey);
					}
					//0,128,255
					else if (CCC == ((1 << 4) | (1 << 8))) {
						SetMinMax(CCC, 7, softimage, x_, y_, sizex, sizey);
					}
					//0,0,255
					else if (CCC == (1 << 8)) {
						SetMinMax(CCC, 8, softimage, x_, y_, sizex, sizey);
					}
					//128,0,255
					else if (CCC == ((1 << 1) | (1 << 8))) {
						SetMinMax(CCC, 9, softimage, x_, y_, sizex, sizey);
					}
					//255,0,255
					else if (CCC == ((1 << 2) | (1 << 8))) {
						SetMinMax(CCC, 10, softimage, x_, y_, sizex, sizey);
					}
					//255,0,128
					else if (CCC == ((1 << 2) | (1 << 7))) {
						SetMinMax(CCC, 11, softimage, x_, y_, sizex, sizey);
					}

					//MINX + (MAXX - MINX) * x_ / sizex = x_t 
					//MINZ + (MAXZ - MINZ) * y_ / sizey = z_t 
				}
			}
			for (int ID = 0; ID < grassDiv; ID++) {
				//ポジション決定
				float xp = MINX + (MAXX - MINX) * grassPos[ID].X_PosMin / sizex;
				float zp = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMin / sizey;
				float xp2 = MINX + (MAXX - MINX) * grassPos[ID].X_PosMax / sizex;
				float zp2 = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMax / sizey;
				float xsize = xp2 - xp;
				float zsize = zp2 - zp;
				//
				{
					grassPosMin[ID] = VECTOR_ref::vget(xp, 0.2f, zp);
					grassPosMax[ID] = grassPosMin[ID] + VECTOR_ref::vget(xsize, 1.f, zsize);
					float xmid = xsize / 2.f;
					float zmid = zsize / 2.f;
					if (grasss != 0) {
						auto& tgt_g = grass__[ID];
						tgt_g.Init(grasss, 0);
						for (int i = 0; i < grasss; ++i) {
							float x1 = xmid + GetRandf(xmid);
							float z1 = zmid + GetRandf(zmid);
							while (true) {
								int CCC = GetColorSoftImage(softimage,
									(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
									(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
								);
								if (CCC != 0) {
									break;
								}
								x1 = xmid + GetRandf(xmid);
								z1 = zmid + GetRandf(zmid);
							}

							auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.2f, z1);
							auto tmpscale = VECTOR_ref::vget(size*1.5f, 1.0f + GetRandf(0.5f), size*1.5f);
							tgt_g.inst.hits.SetMatrix(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::Mtrans(tmpvect));
							tgt_g.Set_one();
						}
						tgt_g.put();
					}
					if (grasss2 != 0) {
						auto& tgt_g = grass2__[ID];
						tgt_g.Init(grasss2, 1);
						for (int i = 0; i < grasss2; ++i) {
							float x1 = xmid + GetRandf(xmid);
							float z1 = zmid + GetRandf(zmid);
							while (true) {
								int CCC = GetColorSoftImage(softimage,
									(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
									(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
								);
								if (CCC != 0) {
									break;
								}
								x1 = xmid + GetRandf(xmid);
								z1 = zmid + GetRandf(zmid);
							}
							auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.f, z1);
							auto tmpscale = VECTOR_ref::vget(size*1.f, 1.2f + GetRandf(0.5f), size*1.f);
							tgt_g.inst.hits.SetMatrix(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::Mtrans(tmpvect));
							tgt_g.Set_one();
						}
						tgt_g.put();
					}
					if (grasss3 != 0) {
						auto& tgt_g = grass3__[ID];
						tgt_g.Init(grasss3, 2);
						for (int i = 0; i < grasss3; ++i) {
							float x1 = xmid + GetRandf(xmid);
							float z1 = zmid + GetRandf(zmid);
							while (true) {
								int CCC = GetColorSoftImage(softimage,
									(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
									(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
								);
								if (CCC != 0) {
									break;
								}
								x1 = xmid + GetRandf(xmid);
								z1 = zmid + GetRandf(zmid);
							}
							auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.f, z1);
							auto tmpscale = VECTOR_ref::vget(size*1.f, 1.2f + GetRandf(0.5f), size*1.f);
							tgt_g.inst.hits.SetMatrix(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::Mtrans(tmpvect));
							tgt_g.Set_one();
						}
						tgt_g.put();
					}
				}
				//
			}
			DeleteSoftImage(softimage);
		}
		void Dispose(void) noexcept {
			for (int ID = 0; ID < grassDiv; ID++) {
				if (grasss != 0) {
					grass__[ID].Dispose();
				}
				if (grasss2 != 0) {
					grass2__[ID].Dispose();
				}
				if (grasss3 != 0) {
					grass3__[ID].Dispose();
				}
			}
		}
		void Draw(cam_info camera_buf) noexcept {
			SetFogEnable(TRUE);
			SetFogStartEnd(camera_buf.near_*3.f, camera_buf.far_*3.f);
			SetFogColor(184, 187, 118);
			SetDrawAlphaTest(DX_CMP_GREATER, 128);
			//SetUseLighting(FALSE);
			SetUseLightAngleAttenuation(FALSE);
			//auto dir=GetLightDirection();
			//VECTOR_ref vec = (VECTOR_ref)GetCameraPosition() - GetCameraTarget();
			//SetLightDirection(vec.Norm().get());

			for (int ID = 0; ID < grassDiv; ID++) {
#ifdef DEBUG
				DrawCube3D(grassPosMin[ID].get(), grassPosMax[ID].get(), GetColor(0, 0, 0), GetColor(0, 0, 0), FALSE);
#endif
				if (grasss != 0) {
					this->grass__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
					grass__[ID].Draw();
				}
				if (grasss2 != 0) {
					this->grass2__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
					grass2__[ID].Draw();
				}
				if (grasss3 != 0) {
					this->grass3__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
					grass3__[ID].Draw();
				}
			}
			//SetLightDirection(dir);

			SetUseLightAngleAttenuation(TRUE);
			//SetUseLighting(TRUE);
			SetDrawAlphaTest(-1, 0);
			SetFogEnable(FALSE);
		}
	};
};