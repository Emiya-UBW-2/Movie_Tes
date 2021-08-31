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
				SetUseASyncLoadFlag(TRUE);
				this->handle = FontHandle::Create(siz_t, DX_FONTTYPE_EDGE);
				SetUseASyncLoadFlag(FALSE);
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
	//サウンドプール
	class SoundPool {
	private:
		class Soundhave {
			class handles {
			public:
				size_t ID_2{ 0 };
				std::string path;
				std::vector<SoundHandle> handle;

			};

			EnumSound ID{ EnumSound::Shot };
			std::vector<handles> shandle;
			size_t now = 0;
			int Set_vol = 255;
			float vol_rate = 1.f;
		public:
			const auto& Get_ID(void)const noexcept { return ID; }
			void Set(EnumSound ID_t, size_t buffersize, std::string path_t, bool is3Dsound = true) {
				if (path_t == "") { return; }
				for (auto& h : this->shandle) {
					if (h.path == path_t) { return; }
				}
				this->ID = ID_t;
				this->shandle.resize(this->shandle.size() + 1);
				this->shandle.back().path = path_t;
				this->shandle.back().handle.resize(buffersize);
				//SetUseASyncLoadFlag(TRUE);
				SetUseASyncLoadFlag(FALSE);
				if (is3Dsound) {
					SetCreate3DSoundFlag(TRUE);
				}
				this->shandle.back().handle[0] = SoundHandle::Load(this->shandle.back().path);
				SetCreate3DSoundFlag(FALSE);
				SetUseASyncLoadFlag(FALSE);

				SetCreate3DSoundFlag(TRUE);
				for (size_t i = 1; i < this->shandle.back().handle.size(); i++) {
					this->shandle.back().handle[i] = this->shandle.back().handle[0].Duplicate();
				}
				SetCreate3DSoundFlag(FALSE);
			}
			void Play(int Sel_t, const int& type_t, const int& Flag_t = 1, int vol_t = -1) {
				shandle[Sel_t].handle[now].play(type_t, Flag_t);
				if (vol_t != -1) {
					Set_vol = vol_t;
					shandle[Sel_t].handle[now].vol((int)(vol_rate * Set_vol));
				}
				++now %= shandle[Sel_t].handle.size();
			}
			void Play_3D(int Sel_t, const VECTOR_ref& pos_t, const float& radius, int vol_t = -1) noexcept {
				bool isplay = true;
				{
					//距離内にいない場合鳴らさない
					//float dist = (pos_t - GetCameraPosition()).size();
					//isplay = (dist < radius);
				}
				if (isplay) {
					shandle[Sel_t].handle[now].play_3D(pos_t, radius);
					if (vol_t != -1) {
						Set_vol = vol_t;
						shandle[Sel_t].handle[now].vol((int)(vol_rate * Set_vol));
					}
					++now %= shandle[Sel_t].handle.size();
				}
			}
			void SetVol(float vol) {
				vol = std::clamp(vol, 0.f, 1.f);
				vol_rate = vol;
				for (auto& sh : this->shandle) {
					for (auto& h : sh.handle) {
						h.vol((int)(vol_rate * Set_vol));
					}
				}
			}
		};
	private:
		std::vector<Soundhave> havehandle;
	public:
		size_t Add(EnumSound ID_t, size_t buffersize = 1, std::string path_t = "", bool is3Dsound = true) {
			for (auto& h : this->havehandle) {
				if (h.Get_ID() == ID_t) {
					h.Set(ID_t, buffersize, path_t, is3Dsound);
					return &h - &this->havehandle.front();
				}
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(ID_t, buffersize, path_t, is3Dsound);
			return this->havehandle.size() - 1;
		}
		Soundhave& Get(EnumSound ID_t) { return this->havehandle[Add(ID_t)]; }

		void SetVol(float vol) {
			for (auto& h : this->havehandle) {
				h.SetVol(vol);
			}
		}
	};
	SoundPool SE;
	SoundPool VOICE;
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
			Update_effect_was = GetNowHiPerformanceCount();
		}

		void Calc(void) noexcept {
			Update_effect_f = ((GetNowHiPerformanceCount() - Update_effect_was) >= 1000000 / 60);
			if (Update_effect_f) {
				Update_effect_was = GetNowHiPerformanceCount();
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
			handles[now].time = GetNowHiPerformanceCount();
			handles[now].handle = LightHandle::Create(pos, 2.5f, 0.5f, 1.5f, 0.5f);
			SetLightDifColorHandle(handles[now].handle.get(), GetColorF(1.f, 1.f, 0.f, 1.f));
			++now %= handles.size();
		}

		void Update(const VECTOR_ref& campos_t) {
			campos = campos_t;
			for (auto& h : handles) {
				if (h.handle.get() != -1) {
					if ((GetNowHiPerformanceCount() - h.time) >= 1000000 / 30) {
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
	public:
		void Set_Effect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept { this->effcs[(int)ef_].Set(pos_t, nomal_t, scale); }
		//エフェクトの更新
		void Update_Effect(void) noexcept {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs.front();
				if (index != (int)Effect::ef_smoke) {
					t.put(effectControl.effsorce[index]);
				}
			}
		}
		/*おわり*/
		void Dispose_Effect(void) noexcept {
			for (auto& t : this->effcs) { t.handle.Dispose(); }
		}
	};
	//パーツデータのセーブ指定
	class PresetSaveControl {
	public:
		class save_c {
		public:
			size_t cang_ = 0;									//パーツ選択
			EnumGunParts type_ = EnumGunParts::NONE;			//パーツの種類
			EnumAttachPoint pt_cat_ = EnumAttachPoint::NONE;	//ベースパーツの場所
			EnumGunParts pt_type_ = EnumGunParts::NONE;			//ベースパーツの種類
			size_t pt_sel_ = 0;									//ベースパーツの番号(マウントなど)
		};
	protected:
		std::vector<save_c> save_parts;
		int P_select = 0;
		int P_select_max = 0;
	protected:
		void ControlSel() {
			if (P_select < 0) { P_select = P_select_max - 1; }
			if (P_select > P_select_max - 1) { P_select = 0; }
		}

		/*パーツデータをロード*/
		void Load(const std::string& PreSet_Path) noexcept {
			std::fstream file;
			save_parts.clear();
			file.open(("data/save/" + PreSet_Path).c_str(), std::ios::binary | std::ios::in);
			save_c savetmp;
			while (true) {
				file.read((char*)&savetmp, sizeof(savetmp));
				if (file.eof()) {
					break;
				}
				this->save_parts.emplace_back(savetmp);
			}
			file.close();
		}
		/*ベースデータを作成*/
		void CreateData() {
			save_parts.clear();
			//magazine
			save_parts.resize(save_parts.size() + 1);
			save_parts.back().cang_ = 0;
			save_parts.back().type_ = EnumGunParts::MAGAZINE;
			save_parts.back().pt_cat_ = EnumAttachPoint::MAGAZINE_BASE;
			save_parts.back().pt_type_ = EnumGunParts::BASE;
			save_parts.back().pt_sel_ = 0;
			//grip
			save_parts.resize(save_parts.size() + 1);
			save_parts.back().cang_ = 0;
			save_parts.back().type_ = EnumGunParts::GRIP;
			save_parts.back().pt_cat_ = EnumAttachPoint::GRIP_BASE;
			save_parts.back().pt_type_ = EnumGunParts::BASE;
			save_parts.back().pt_sel_ = 0;
			//uperhandguard
			save_parts.resize(save_parts.size() + 1);
			save_parts.back().cang_ = 0;
			save_parts.back().type_ = EnumGunParts::UPER_HANDGUARD;
			save_parts.back().pt_cat_ = EnumAttachPoint::UPER_HANDGUARD;
			save_parts.back().pt_type_ = EnumGunParts::BASE;
			save_parts.back().pt_sel_ = 0;
			//underhandguard
			save_parts.resize(save_parts.size() + 1);
			save_parts.back().cang_ = 0;
			save_parts.back().type_ = EnumGunParts::UNDER_HANDGUARD;
			save_parts.back().pt_cat_ = EnumAttachPoint::UNDER_HANDGUARD;
			save_parts.back().pt_type_ = EnumGunParts::BASE;
			save_parts.back().pt_sel_ = 0;
			//マズル
			save_parts.resize(save_parts.size() + 1);
			save_parts.back().cang_ = 0;
			save_parts.back().type_ = EnumGunParts::MAZZULE;
			save_parts.back().pt_cat_ = EnumAttachPoint::MAZZULE_BASE;
			save_parts.back().pt_type_ = EnumGunParts::BASE;
			save_parts.back().pt_sel_ = 0;
			//ダストカバー
			save_parts.resize(save_parts.size() + 1);
			save_parts.back().cang_ = 0;
			save_parts.back().type_ = EnumGunParts::DUSTCOVER;
			save_parts.back().pt_cat_ = EnumAttachPoint::DUSTCOVER_BASE;
			save_parts.back().pt_type_ = EnumGunParts::BASE;
			save_parts.back().pt_sel_ = 0;
			//ストック
			save_parts.resize(save_parts.size() + 1);
			save_parts.back().cang_ = 0;
			save_parts.back().type_ = EnumGunParts::STOCK;
			save_parts.back().pt_cat_ = EnumAttachPoint::STOCK_BASE;
			save_parts.back().pt_type_ = EnumGunParts::BASE;
			save_parts.back().pt_sel_ = 0;
		}
		/*パーツのセーブ設定*/
		void Set_PartsSave(size_t chang_t, EnumGunParts parts_cat_t, EnumAttachPoint port_cat_t, EnumGunParts port_type_t) {
			if (save_parts.size() < size_t(P_select_max) + 1) {
				save_parts.resize(size_t(P_select_max) + 1);
			}
			save_parts[P_select_max].cang_ = chang_t;
			save_parts[P_select_max].type_ = parts_cat_t;
			save_parts[P_select_max].pt_cat_ = port_cat_t;
			save_parts[P_select_max].pt_type_ = port_type_t;
			save_parts[P_select_max].pt_sel_ = 0;
		}
		/*パーツデータをセーブ*/
		void Save(const std::string& PreSet_Path) {
			std::fstream file;
			file.open(("data/save/" + PreSet_Path).c_str(), std::ios::binary | std::ios::out);
			for (auto& tmp_save : save_parts) {
				file.write((char*)&tmp_save, sizeof(tmp_save));
			}
			file.close();
		}
		/*セーブデータのクリーンアップ*/
		void CleanUP() {
			/*
			//セーブデータのクリーンアップ
			bool sav = true;
			int cnt = 0;
			do {
				sav = false;
				for (auto&s : save_parts) {
					if (s.type_ != EnumGunParts::SIGHT) {
						if (erase_savedata(mine_Gun->Get_parts(s.type_))) {
							save_parts.erase(save_parts.begin() + (&s - &save_parts.front()));
							sav = true;
							break;
						}
					}
					else if (cnt < mine_Gun->Get_sight_size()) {
						auto* stt = mine_Gun->Get_parts(s.type_, cnt);
						cnt++;
						if (erase_savedata(stt) || s.cang_ > 10) {
							save_parts.erase(save_parts.begin() + (&s - &save_parts.front()));
							sav = true;
							break;
						}
					}
				}
			} while (sav);
			*/
		}
	};
	//キーバインド
	class key_bind {
	private:
		class keyhandle;
		class keys {
		public:
			int mac = 0, px = 0, py = 0;
			char onhandle[256] = "", offhandle[256] = "";
		};
		class key_pair {
			int first = 0;
			int use_mode = 0;
			switchs on_off;
			std::array<std::string, 3> second;
		public:


			bool isalways{ false };
			keyhandle* use_handle{ nullptr };

			const auto& Get_first()const noexcept { return first; }
			const auto& Get_second(size_t sel)const noexcept { return second[sel]; }

			void Set(int first_t, std::string_view second_t, int mode, std::string_view second2_t = "", std::string_view second3_t = "") {
				first = first_t;
				second[0] = second_t;
				second[1] = second2_t;
				second[2] = second3_t;
				use_mode = mode;
			}
			//private:
			bool Get_key(int id, bool checkUpdate) {
				switch (id) {
					//キー
				case 0:
					return CheckHitKey(this->first) != 0;
				case 1:
					if (checkUpdate) {
						on_off.GetInput(CheckHitKey(this->first) != 0);
					}
					return on_off.on();
				case 2:
					if (checkUpdate) {
						on_off.GetInput(CheckHitKey(this->first) != 0);
					}
					return on_off.trigger();
					//マウス
				case 3:
					return (GetMouseInput() & this->first) != 0;
				case 4:
					if (checkUpdate) {
						on_off.GetInput((GetMouseInput() & this->first) != 0);
					}
					return on_off.on();
				case 5:
					if (checkUpdate) {
						on_off.GetInput((GetMouseInput() & this->first) != 0);
					}
					return on_off.trigger();
				default:
					return CheckHitKey(this->first) != 0;
				}
			}
			//public:
			void Set_key(bool t) {
				on_off.first = t;
			}
			bool Get_key_Auto(bool checkUpdate) {
				this->isalways = true;
				return this->Get_key(this->use_mode, checkUpdate);

			}
		};
		class keyhandle {
		public:
			keys key;
			GraphHandle onhandle, offhandle;
			key_pair* use_part{ nullptr };
		};

		std::vector<keyhandle> keyg;
		std::vector<keyhandle> keyg2;

		float F1_f = 0.0f;
		float noF1_f = 0.0f;
		GraphHandle keyboad;
		GraphHandle mousehandle;

		std::shared_ptr<DXDraw> DrawPts{ nullptr };			//引き継ぐ

		std::vector<key_pair> key_use_ID;
		std::vector<key_pair> mouse_use_ID;
		size_t info = 0;
	public:
		void Set_Mode(size_t now) { info = now; }
		//
		auto& Get_key_use_ID(EnumKeyBind id_t) {
			return key_use_ID[(int)id_t];
		}
		bool Get_key_use(EnumKeyBind id_t) {
			return key_use_ID[(int)id_t].Get_key_Auto(true);
		}
		bool Get_mouse_use(EnumMouseBind id_t) {
			return mouse_use_ID[(int)id_t].Get_key_Auto(true);
		}
		//
		key_bind(std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
			SetUseASyncLoadFlag(FALSE);
			mousehandle = GraphHandle::Load("data/key/mouse.png");
			SetTransColor(0, 255, 0);
			keyboad = GraphHandle::Load("data/key/keyboad.png");
			SetTransColor(0, 0, 0);
			//
			{
				key_pair tmp_k;
				tmp_k.Set(KEY_INPUT_W, "前進", 0, "選択");
				this->key_use_ID.emplace_back(tmp_k);//0
				tmp_k.Set(KEY_INPUT_S, "後退", 0, "選択");
				this->key_use_ID.emplace_back(tmp_k);//1
				tmp_k.Set(KEY_INPUT_D, "右歩き", 0, "選択");
				this->key_use_ID.emplace_back(tmp_k);//2
				tmp_k.Set(KEY_INPUT_A, "左歩き", 0, "選択");
				this->key_use_ID.emplace_back(tmp_k);//3
				tmp_k.Set(KEY_INPUT_Q, "左リーン", 0);
				this->key_use_ID.emplace_back(tmp_k);//4
				tmp_k.Set(KEY_INPUT_E, "右リーン", 0);
				this->key_use_ID.emplace_back(tmp_k);//5
				tmp_k.Set(KEY_INPUT_R, "リロード", 0);
				this->key_use_ID.emplace_back(tmp_k);//6
				tmp_k.Set(KEY_INPUT_F, "アイテム取得", 2);
				this->key_use_ID.emplace_back(tmp_k);//7
				tmp_k.Set(KEY_INPUT_G, "グレネード投擲", 2);
				this->key_use_ID.emplace_back(tmp_k);//8
				tmp_k.Set(KEY_INPUT_C, "しゃがみ", 1);
				this->key_use_ID.emplace_back(tmp_k);//9
				tmp_k.Set(KEY_INPUT_ESCAPE, "強制終了", 1, "強制終了");
				this->key_use_ID.emplace_back(tmp_k);//10
				tmp_k.Set(KEY_INPUT_Z, "マガジン整理", 2);
				this->key_use_ID.emplace_back(tmp_k);//11
				tmp_k.Set(KEY_INPUT_LSHIFT, "走る", 0);
				this->key_use_ID.emplace_back(tmp_k);//12
				tmp_k.Set(KEY_INPUT_SPACE, "ジャンプ", 2, "決定");
				this->key_use_ID.emplace_back(tmp_k);//13
				tmp_k.Set(KEY_INPUT_LCONTROL, "視点切替", 0);
				this->key_use_ID.emplace_back(tmp_k);//14
				tmp_k.Set(KEY_INPUT_F1, "キー案内", 1, "キー案内");
				this->key_use_ID.emplace_back(tmp_k);//15
				tmp_k.Set(KEY_INPUT_V, "眺める", 2);
				this->key_use_ID.emplace_back(tmp_k);//16
				tmp_k.Set(KEY_INPUT_P, "ポーズ", 1);
				this->key_use_ID.emplace_back(tmp_k);//17
				tmp_k.Set(KEY_INPUT_H, "治療キット排出", 2);
				this->key_use_ID.emplace_back(tmp_k);//18
				tmp_k.Set(KEY_INPUT_B, "乗車", 2);
				this->key_use_ID.emplace_back(tmp_k);//19
				//
				tmp_k.Set(MOUSE_INPUT_LEFT, "射撃", 3, "射撃");
				this->mouse_use_ID.emplace_back(tmp_k);//0
				tmp_k.Set(MOUSE_INPUT_MIDDLE, "セレクター切替", 5, "ビュー切り替え");
				this->mouse_use_ID.emplace_back(tmp_k);//1
				tmp_k.Set(MOUSE_INPUT_RIGHT, "エイム", 3);
				this->mouse_use_ID.emplace_back(tmp_k);//2
			}
			{
				std::fstream file;
				/*
				std::vector<keys> key;
				key.resize(key.size() + 1);
				key.back().mac = MOUSE_INPUT_LEFT;
				key.back().px=0;
				key.back().py=0;
				strcpy_s(key.back().offhandle, 256, "data/key/mouse_gray/LEFT.bmp");
				strcpy_s(key.back().onhandle, 256, "data/key/mouse_black/LEFT.bmp");
				key.resize(key.size() + 1);
				key.back().mac = MOUSE_INPUT_MIDDLE;
				key.back().px = 0;
				key.back().py = 0;
				strcpy_s(key.back().offhandle, 256, "data/key/mouse_gray/MIDDLE.bmp");
				strcpy_s(key.back().onhandle, 256, "data/key/mouse_black/MIDDLE.bmp");
				key.resize(key.size() + 1);
				key.back().mac = MOUSE_INPUT_RIGHT;
				key.back().px = 0;
				key.back().py = 0;
				strcpy_s(key.back().offhandle, 256, "data/key/mouse_gray/RIGHT.bmp");
				strcpy_s(key.back().onhandle, 256, "data/key/mouse_black/RIGHT.bmp");


				file.open("data/2.dat", std::ios::binary | std::ios::out);
				for (auto& m : key) {
					file.write((char*)&m, sizeof(m));
				}
				file.close();
				//*/
				//*
				{
					file.open("data/1.dat", std::ios::binary | std::ios::in);
					keys keytmp;
					while (true) {
						file.read((char*)&keytmp, sizeof(keytmp));
						if (file.eof()) {
							break;
						}
						int y_size = 48;//36
						{
							this->keyg.resize(this->keyg.size() + 1);
							this->keyg.back().key = keytmp;

							keyg.back().key.px = keyg.back().key.px * y_size + ((keyg.back().key.px <= 14 && keyg.back().key.py % 2 == 1) ? y_size / 2 : 0) + ((keyg.back().key.px > 14) ? y_size : 0);
							keyg.back().key.py = keyg.back().key.py * y_size;
							this->keyg.back().onhandle = GraphHandle::Load(this->keyg.back().key.onhandle);
							this->keyg.back().offhandle = GraphHandle::Load(this->keyg.back().key.offhandle);
						}
					}
					for (auto& m : this->keyg) {
						for (auto& i : this->key_use_ID) {
							if (i.Get_first() == m.key.mac) {
								m.use_part = &i;
								i.use_handle = &m;
								break;
							}
						}
					}
					file.close();
				}
				{
					file.open("data/2.dat", std::ios::binary | std::ios::in);
					int cnt = 0;
					keys keytmp;
					while (true) {
						file.read((char*)&keytmp, sizeof(keytmp));
						if (file.eof()) {
							break;
						}
						int y_size = 48;//36
						{
							this->keyg2.resize(this->keyg2.size() + 1);
							this->keyg2.back().key = keytmp;
							keyg2.back().key.px = cnt * y_size;
							keyg2.back().key.py = 500;
							cnt++;

							this->keyg2.back().onhandle = GraphHandle::Load(this->keyg2.back().key.onhandle);
							this->keyg2.back().offhandle = GraphHandle::Load(this->keyg2.back().key.offhandle);
						}
					}
					for (auto& m : this->keyg2) {
						for (auto& i : this->mouse_use_ID) {
							if (i.Get_first() == m.key.mac) {
								m.use_part = &i;
								i.use_handle = &m;
								break;
							}
						}
					}
					file.close();
				}
				//*/
			}

			DrawPts = DrawPts_t;
		}
		//
		const auto Esc_key(void) noexcept { return this->key_use_ID[(int)EnumKeyBind::ESCAPE].Get_key_Auto(true); }
		//
		void ReSet_isalways(void) noexcept {
			for (auto& i : this->key_use_ID) {
				i.isalways = false;
			}
			for (auto& i : this->mouse_use_ID) {
				i.isalways = false;
			}
		}
		//
		void Draw(void) noexcept {
			auto tmp_f1 = this->key_use_ID[(int)EnumKeyBind::INFO].Get_key_Auto(true);
			easing_set(&F1_f, float(tmp_f1), 0.9f);
			noF1_f = std::max(noF1_f - 1.f / FPS, 0.f);
			//インフォ
			if (F1_f > 0.1f) {
				int xp_t = y_r(100), yp_t = y_r(300);
				int xp_sk = xp_t, yp_sk = yp_t, y_size_k = y_r(48);
				int xp_s = y_r(1500), yp_s = y_r(200), y_size = y_r(32);
				//背景
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f * F1_f));
				DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(0, 0, 0), TRUE);
				if (F1_f > 0.9f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					keyboad.DrawExtendGraph(0, 0, DrawPts->disp_x, DrawPts->disp_y, true);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				//前面
				if (F1_f > 0.9f) {
					//キーボード＋マウス全部
					{
						for (auto& m : this->keyg) {
							if (m.use_part != nullptr) {
								xp_sk = xp_t + y_r(m.key.px);
								yp_sk = yp_t + y_r(m.key.py);
								if (m.use_part->Get_key(0, false)) {//keyboad
									m.onhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - y_r(4)) / 26.f, 0.f, false);
								}
								else {
									m.offhandle.DrawRotaGraph(xp_sk + y_size_k / 2, yp_sk + y_size_k / 2, float(y_size_k - y_r(4)) / 26.f, 0.f, false);
								}
							}
						}
						int yss = 0;
						xp_sk = y_r(1100);
						yp_sk = y_r(800);
						for (auto& m : this->keyg2) {
							if (m.use_part != nullptr) {
								if (m.use_part->Get_key(3, false)) {
									m.onhandle.GetSize(nullptr, &yss);
									m.onhandle.DrawRotaGraph(xp_sk, yp_sk, float(y_r(float(256) / yss * 100)) / 100.f, 0.f, true);
								}
								else {
									m.offhandle.GetSize(nullptr, &yss);
									m.offhandle.DrawRotaGraph(xp_sk, yp_sk, float(y_r(float(256) / yss * 100)) / 100.f, 0.f, true);
								}
							}
						}
					}
					//詳細
					{
						int xss = 0, yss = 0;
						float siz_t = float(y_size - 4) / 25.f;
						for (auto& i : this->key_use_ID) {
							if (i.isalways && i.use_handle != nullptr) {
								if (i.Get_key(0, false)) {
									i.use_handle->onhandle.GetSize(&xss, &yss);
									xss = int(float(xss) * siz_t);
									yss = int(float(yss) * siz_t);
									i.use_handle->onhandle.DrawRotaGraph(xp_s - xss / 2, yp_s + yss / 2, siz_t, 0.f, false);
								}
								else {
									i.use_handle->offhandle.GetSize(&xss, &yss);
									xss = int(float(xss) * siz_t);
									yss = int(float(yss) * siz_t);
									i.use_handle->offhandle.DrawRotaGraph(xp_s - xss / 2, yp_s + yss / 2, siz_t, 0.f, false);
								}
								Fonts.Get(y_r(24)).Get_handle().DrawString(xp_s, yp_s + (y_size - Fonts.Get(y_r(24)).Get_size()) / 2, i.Get_second(info), GetColor(255, 255, 255));
								yp_s += y_size;
							}
						}
						for (auto& i : this->mouse_use_ID) {
							if (i.isalways && i.use_handle != nullptr) {
								{
									mousehandle.GetSize(nullptr, &yss);
									mousehandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
								}
								if (i.Get_key(3, false)) {
									i.use_handle->onhandle.GetSize(nullptr, &yss);
									i.use_handle->onhandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
								}
								else {
									i.use_handle->offhandle.GetSize(nullptr, &yss);
									i.use_handle->offhandle.DrawRotaGraph(xp_s - y_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
								}
								Fonts.Get(y_r(24)).Get_handle().DrawString(xp_s, yp_s + (y_size - Fonts.Get(y_r(24)).Get_size()) / 2, i.Get_second(info), GetColor(255, 255, 255)); yp_s += y_size;
							}
						}
					}
				}
			}
			//常時表示
			if (!tmp_f1) {
				if (noF1_f >= 0.1f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f * std::clamp(noF1_f, 0.f, 1.f)));
					int xp_s = DrawPts->disp_x - y_r(700), yp_s = DrawPts->disp_y - y_r(28), x_size = y_r(26), y_size = y_r(24);
					int xss = 0, yss = 0;
					float siz_t = float(y_size - 4) / 25.f;
					for (auto& i : this->key_use_ID) {
						if (i.isalways) {
							for (auto& m : this->keyg) {
								if (m.key.mac == i.Get_first()) {
									if (i.Get_key(0, false)) {
										noF1_f = 3.f;
										m.onhandle.GetSize(&xss, &yss);
										xss = int(float(xss) * siz_t);
										yss = int(float(yss) * siz_t);
										m.onhandle.DrawRotaGraph(xp_s + (x_size - y_size + xss) / 2, yp_s + yss / 2, siz_t, 0.f, false);
									}
									else {
										m.offhandle.GetSize(&xss, &yss);
										xss = int(float(xss) * siz_t);
										yss = int(float(yss) * siz_t);
										m.offhandle.DrawRotaGraph(xp_s + (x_size - y_size + xss) / 2, yp_s + yss / 2, siz_t, 0.f, false);
									}
									xp_s += (x_size - y_size + xss);
								}
							}
						}
					}
					for (auto& i : this->mouse_use_ID) {
						if (i.isalways) {
							for (auto& m : this->keyg2) {
								if (m.key.mac == i.Get_first()) {
									{
										mousehandle.GetSize(&xss, &yss);
										mousehandle.DrawRotaGraph(xp_s + x_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
									}
									if (i.Get_key(3, false)) {
										noF1_f = 3.f;
										m.onhandle.GetSize(&xss, &yss);
										m.onhandle.DrawRotaGraph(xp_s + x_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
									}
									else {
										m.offhandle.GetSize(&xss, &yss);
										m.offhandle.DrawRotaGraph(xp_s + x_size / 2, yp_s + y_size / 2, float(y_size) / yss, 0.f, true);
									}
									xp_s += x_size;
								}
							}
						}
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				else {
					for (auto& i : this->key_use_ID) {
						if (i.isalways) {
							for (auto& m : this->keyg) {
								if (m.key.mac == i.Get_first()) {
									if (i.Get_key(0, false)) {
										noF1_f = 3.f;
									}
								}
							}
						}
					}
					for (auto& i : this->mouse_use_ID) {
						if (i.isalways) {
							for (auto& m : this->keyg2) {
								if (m.key.mac == i.Get_first()) {
									if (i.Get_key(3, false)) {
										noF1_f = 3.f;
									}
								}
							}
						}
					}
				}
			}
			else {
				noF1_f = 3.f;
			}
			//
		}
		//
	};
	//ポーズメニュー
	class option_menu {
	private:
		std::shared_ptr<key_bind> KeyBind{ nullptr };
		std::shared_ptr<DXDraw> DrawPts{ nullptr };			//引き継ぐ
		float P_f = 0.0f;
		bool old = false;
		bool On_Option = false;

		int select = 0;
		int selmax = 3;
		//キー
		switchs up;
		switchs down;
		switchs left;
		switchs right;
		switchs shot;

		std::shared_ptr<OPTION> OPTPTs;
		float SE_vol_per = 1.f;
		float Voice_vol_per = 1.f;

		float SE_left_timer_border = 0.f;
		float SE_right_timer_border = 0.f;
		float SE_left_timer = 0.f;
		float SE_right_timer = 0.f;
		float Voice_left_timer = 0.f;
		float Voice_right_timer = 0.f;
		float Voice_left_timer_border = 0.f;
		float Voice_right_timer_border = 0.f;
	private:
		std::vector<float> sel_x;
		auto& Sel_X(size_t size) {
			if (sel_x.size() < size + 1) {
				sel_x.resize(size + 1);
				sel_x[size] = 0.f;
			}
			return sel_x[size];
		}

		void Draw_Guage(int xp, int yp, int xs, int ys, float per) {
			int edge = y_r(2);
			for (int x = 0; x < 256; x++) {
				int x1 = xp + ((xs - xp)*x / 255);
				int y1 = ys - int(float(ys - yp) *0.2) - int((float(ys - yp) *0.8)*std::powf(float(x) / 255.f, 1.25f));
				int x2 = xp + ((xs - xp)*(x + 1) / 255);
				if (x == 0) {
					DrawBox(x1 - edge, y1 - edge, x1, ys + edge, GetColor(128, 128, 128), TRUE);
				}
				DrawBox(x1, y1 - edge, x2, ys + edge, GetColor(128, 128, 128), TRUE);
				DrawBox(x1, y1, x2, ys, GetColor(64, 64, 64), TRUE);
				if (x == 255) {
					DrawBox(x2, y1 - edge, x2 + edge, ys + edge, GetColor(128, 128, 128), TRUE);
				}
				if (x < (int)(255.f*per)) {
					int r_ = (x >= 0 && x <= 85) ? 255 : (255 * 2 - 3 * x);
					int g_ = (x >= 85 && x <= 170) ? 255 : ((x < 85) ? (3 * x) : (255 * 3 - 3 * x));
					int b_ = (x >= 170 && x <= 255) ? 255 : (-255 + 3 * x);
					DrawBox(x1, y1, x2, ys, GetColor(std::max(r_, 0), std::max(g_, 0), std::max(b_, 0)), TRUE);
				}
			}
		}
	public:
		//
		option_menu(std::shared_ptr<OPTION>& OPTPTs_t, std::shared_ptr<key_bind>& KeyBind_t, std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
			OPTPTs = OPTPTs_t;
			KeyBind = KeyBind_t;
			DrawPts = DrawPts_t;
			SE_vol_per = OPTPTs->Get_SE();
			Voice_vol_per = OPTPTs->Get_VOICE();
		}
		//
		const auto& Pause_key(void) const noexcept { return On_Option; }
		void Pause_key(bool value) noexcept { On_Option = value; }

		void Set() {
			SE.SetVol(SE_vol_per);
			VOICE.SetVol(Voice_vol_per);
		}
		//
		bool Update(void) noexcept {
			if (On_Option) {
				SetMouseDispFlag(TRUE);

				KeyBind->Set_Mode(1);
				up.GetInput(KeyBind->Get_key_use(EnumKeyBind::FRONT));
				down.GetInput(KeyBind->Get_key_use(EnumKeyBind::BACK));
				left.GetInput(KeyBind->Get_key_use(EnumKeyBind::LEFT));
				right.GetInput(KeyBind->Get_key_use(EnumKeyBind::RIGHT));
				shot.GetInput(KeyBind->Get_key_use(EnumKeyBind::JUMP));
				if (up.trigger()) {
					SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
					select--;
				}
				if (down.trigger()) {
					SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
					select++;
				}

				if (select < 0) { select = selmax - 1; }
				if (select > selmax - 1) { select = 0; }

				//サウンド
				if (select == 0) {
					//
					if (left.press()) { SE_left_timer += 1.f / FPS; }
					else {
						SE_left_timer = 0.f;
						SE_left_timer_border = 1.f;
					}
					if (left.trigger() || (SE_left_timer > SE_left_timer_border)) {
						if (SE_vol_per > 0.f) {
							SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						SE_vol_per = std::clamp(SE_vol_per - 0.01f, 0.f, 1.f);
						SE.SetVol(SE_vol_per);
						OPTPTs->Set_SE(SE_vol_per);
						SE_left_timer_border += 0.1f;
					}
					//
					if (right.press()) { SE_right_timer += 1.f / FPS; }
					else {
						SE_right_timer = 0.f;
						SE_right_timer_border = 1.f;
					}
					if (right.trigger() || (SE_right_timer > SE_right_timer_border)) {
						if (SE_vol_per < 1.f) {
							SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						SE_vol_per = std::clamp(SE_vol_per + 0.01f, 0.f, 1.f);
						SE.SetVol(SE_vol_per);
						OPTPTs->Set_SE(SE_vol_per);
						SE_right_timer_border += 0.1f;
					}
				}
				//サウンド
				if (select == 1) {
					//
					if (left.press()) { Voice_left_timer += 1.f / FPS; }
					else {
						Voice_left_timer = 0.f;
						Voice_left_timer_border = 1.f;
					}
					if (left.trigger() || (Voice_left_timer > Voice_left_timer_border)) {
						if (Voice_vol_per > 0 && Voice_vol_per < 1.f) {
							SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						Voice_vol_per = std::clamp(Voice_vol_per - 0.01f, 0.f, 1.f);
						VOICE.SetVol(Voice_vol_per);
						OPTPTs->Set_VOICE(Voice_vol_per);
						Voice_left_timer_border += 0.1f;
					}
					//
					if (right.press()) { Voice_right_timer += 1.f / FPS; }
					else {
						Voice_right_timer = 0.f;
						Voice_right_timer_border = 1.f;
					}
					if (right.trigger() || (Voice_right_timer > Voice_right_timer_border)) {
						if (Voice_vol_per > 0 && Voice_vol_per < 1.f) {
							SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						Voice_vol_per = std::clamp(Voice_vol_per + 0.01f, 0.f, 1.f);
						VOICE.SetVol(Voice_vol_per);
						OPTPTs->Set_VOICE(Voice_vol_per);
						Voice_right_timer_border += 0.1f;
					}
				}

				if (select == 2) {
					//戻る
					if (shot.trigger()) {
						SE.Get(EnumSound::CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
						On_Option = false;
						OPTPTs->Save();
					}
				}
			}
			return true;
		}
		//
		void Draw(void) noexcept {
			auto tmp_P = On_Option;
			easing_set(&P_f, float(tmp_P), 0.9f);
			//インフォ
			if (P_f > 0.1f) {
				//背景
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f * P_f));
				DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(0, 0, 0), TRUE);
				if (P_f > 0.9f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					//背景画像
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				//前面
				if (P_f > 0.9f) {
					int xp_t = DrawPts->disp_x - (y_r(100) + y_r(140));
					int yp_t = y_r(100);
					int now = 0;
					//
					Fonts.Get(y_r(24)).Get_handle().DrawStringFormat_RIGHT(xp_t - int(Sel_X(now) * (float)y_r(32)), yp_t, (select == now) ? GetColor(0, 255, 0) : GetColor(0, 164, 0), "SE:%03.0f%%", SE_vol_per * 100.f);
					if (select == now) {
						Draw_Guage(xp_t - y_r(240 + 300), yp_t + y_r(4), xp_t - y_r(240), yp_t + y_r(4) + Fonts.Get(y_r(24)).Get_size(), SE_vol_per);
					}
					easing_set(&Sel_X(now), (select == now) ? 1.f : 0.f, 0.9f);
					yp_t += Fonts.Get(y_r(24)).Get_size() + y_r(30);
					now++;
					//
					Fonts.Get(y_r(24)).Get_handle().DrawStringFormat_RIGHT(xp_t - int(Sel_X(now) * (float)y_r(32)), yp_t, (select == now) ? GetColor(0, 255, 0) : GetColor(0, 164, 0), "VOICE:%03.0f%%", Voice_vol_per * 100.f);
					if (select == now) {
						Draw_Guage(xp_t - y_r(240 + 300), yp_t + y_r(4), xp_t - y_r(240), yp_t + y_r(4) + Fonts.Get(y_r(24)).Get_size(), Voice_vol_per);
					}
					easing_set(&Sel_X(now), (select == now) ? 1.f : 0.f, 0.9f);
					yp_t += Fonts.Get(y_r(24)).Get_size() + y_r(30);
					now++;
					//
					Fonts.Get(y_r(24)).Get_handle().DrawString_RIGHT(xp_t - int(Sel_X(now) * (float)y_r(32)), yp_t, "戻る", (select == now) ? GetColor(0, 255, 0) : GetColor(0, 164, 0));
					easing_set(&Sel_X(now), (select == now) ? 1.f : 0.f, 0.9f);
					yp_t += Fonts.Get(y_r(24)).Get_size() + y_r(30);
					now++;
					//
				}
			}
			else {
				for (auto& x : sel_x) {
					x = -1.f*((float)y_r(100) + (float)y_r(140)) / (float)y_r(32);
				}
			}
			//
		}
		//
	};
	//ポーズメニュー
	class pause_menu {
	private:
		std::shared_ptr<option_menu> OptionMenu{ nullptr };
		std::shared_ptr<key_bind> KeyBind{ nullptr };
		std::shared_ptr<DXDraw> DrawPts{ nullptr };			//引き継ぐ
		float P_f = 0.0f;
		bool old = false;

		int select = 0;
		int selmax = 3;
		//キー
		switchs up;
		switchs down;
		switchs left;
		switchs right;
		switchs shot;
	private:
		std::vector<float> sel_x;
		auto& Sel_X(size_t size) {
			if (sel_x.size() < size + 1) {
				sel_x.resize(size + 1);
				sel_x[size] = 0.f;
			}
			return sel_x[size];
		}
	public:
		//
		pause_menu(std::shared_ptr<option_menu>& OptionMenu_t, std::shared_ptr<key_bind>& KeyBind_t, std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
			OptionMenu = OptionMenu_t;
			KeyBind = KeyBind_t;
			DrawPts = DrawPts_t;
			SetUseASyncLoadFlag(FALSE);
		}
		//
		const auto Pause_key(void) noexcept {
			auto key_p = old;
			if (!OptionMenu->Pause_key()) {
				key_p = KeyBind->Get_key_use_ID(EnumKeyBind::PAUSE).Get_key_Auto(true);
			}

			if (key_p != old) {
				SE.Get(EnumSound::CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			old = key_p;
			return key_p;
		}
		//
		bool Update(void) noexcept {
			SetMouseDispFlag(TRUE);
			bool selend = true;

			KeyBind->Set_Mode(1);
			up.GetInput(KeyBind->Get_key_use(EnumKeyBind::FRONT) && !OptionMenu->Pause_key());
			down.GetInput(KeyBind->Get_key_use(EnumKeyBind::BACK) && !OptionMenu->Pause_key());
			left.GetInput(KeyBind->Get_key_use(EnumKeyBind::LEFT) && !OptionMenu->Pause_key());
			right.GetInput(KeyBind->Get_key_use(EnumKeyBind::RIGHT) && !OptionMenu->Pause_key());
			shot.GetInput(KeyBind->Get_key_use(EnumKeyBind::JUMP) && !OptionMenu->Pause_key());
			if (up.trigger()) {
				SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
				select--;
			}
			if (down.trigger()) {
				SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
				select++;
			}

			if (select < 0) { select = selmax - 1; }
			if (select > selmax - 1) { select = 0; }

			if (shot.trigger()) {
				//オプション
				if (select == 0) {
					SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
					OptionMenu->Pause_key(true);
				}
				//戦闘に戻る
				if (select == 1) {
					SE.Get(EnumSound::CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
					KeyBind->Get_key_use_ID(EnumKeyBind::PAUSE).Set_key(false);
				}
				//強制帰還
				if (select == 2) {
					SE.Get(EnumSound::CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
					KeyBind->Get_key_use_ID(EnumKeyBind::PAUSE).Set_key(false);
					selend = false;
				}
			}

			return selend;
		}
		//
		void Draw(void) noexcept {
			auto tmp_P = KeyBind->Get_key_use_ID(EnumKeyBind::PAUSE).Get_key_Auto(false);
			easing_set(&P_f, float(tmp_P), 0.9f);
			//インフォ
			if (P_f > 0.1f) {
				//背景
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(192.f * P_f));
				DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(0, 0, 0), TRUE);
				if (P_f > 0.9f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					//背景画像
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				//前面
				if (P_f > 0.9f) {
					int xp_t = DrawPts->disp_x - y_r(100);
					int yp_t = y_r(100);
					int now = 0;
					//
					Fonts.Get(y_r(24)).Get_handle().DrawString_RIGHT(xp_t - int(Sel_X(now) * (float)y_r(32)), yp_t, "オプション", (select == now) ? GetColor(0, 255, 0) : GetColor(0, 164, 0));
					easing_set(&Sel_X(now), (select == now) ? 1.f : 0.f, 0.9f);
					yp_t += Fonts.Get(y_r(24)).Get_size() + y_r(30);
					now++;
					//
					Fonts.Get(y_r(24)).Get_handle().DrawString_RIGHT(xp_t - int(Sel_X(now) * (float)y_r(32)), yp_t, "戦闘に戻る", (select == now) ? GetColor(0, 255, 0) : GetColor(0, 164, 0));
					easing_set(&Sel_X(now), (select == now) ? 1.f : 0.f, 0.9f);
					yp_t += Fonts.Get(y_r(24)).Get_size() + y_r(30);
					now++;
					//
					Fonts.Get(y_r(24)).Get_handle().DrawString_RIGHT(xp_t - int(Sel_X(now) * (float)y_r(32)), yp_t, "強制帰還", (select == now) ? GetColor(0, 255, 0) : GetColor(0, 164, 0));
					easing_set(&Sel_X(now), (select == now) ? 1.f : 0.f, 0.9f);
					yp_t += Fonts.Get(y_r(24)).Get_size() + y_r(30);
					now++;
					//
				}
			}
			else {
				for (auto& x : sel_x) {
					x = -1.f*((float)y_r(100)) / (float)y_r(32);
				}
			}
			//
		}
		//
	};
	//ルール
	class RULE_parts {
	private:
		float Ready = 0.f;
		float Ready_border = 0.f;
		float timer = 0.f;
	public:
		const auto& Get_timer(void) const noexcept { return timer; }
		const auto& Get_Ready(void) const noexcept { return Ready; }
		const auto Get_Start(void) const noexcept { return Ready <= 0.f; }
		const auto Get_end(void) const noexcept { return timer <= 0.f; }
		const auto Get_Playing(void) const noexcept { return Get_Start() && !Get_end(); }
		void Set(void) noexcept {
			Ready = 3.0f;
			Ready_border = Ready;
			timer = 1800.f;
		}
		void Update(void) noexcept {
			if (Get_Start()) {
				timer -= 1.f / FPS;
			}
			else {
				if (Ready <= Ready_border) {
					SE.Get(EnumSound::TIMER).Play(0, DX_PLAYTYPE_BACK, TRUE, 255);
					Ready_border--;
				}
				Ready -= 1.f / FPS;
			}
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
	//銃、マガジン共通モデル
	class Models {
	private:
		std::string name;
		std::string path;
		MV1 model;
		MV1 col;
		std::array<frames, 2> magazine_ammo_f;	//マガジン用弾フレーム
		std::array<frames, 3> LEFT_mag_frame;	//左手座標(マガジン保持時)
		std::array<frames, 3> LEFT_mag_frame_ready;	//左手座標(マガジン装着用意時)
		std::array<frames, 2> magazine_f;		//マガジンフレーム
	public:
		auto& Get_name(void) const noexcept { return name; }
		auto& Get_path(void) const noexcept { return path; }
		auto& Get_model(void) const noexcept { return model; }
		const frames& Get_magazine_ammo_f(int i) const noexcept { return magazine_ammo_f[i]; }
		const frames& Get_LEFT_mag_frame(int i) const noexcept { return LEFT_mag_frame[i]; }
		const frames& Get_LEFT_mag_frame_ready(int i) const noexcept { return LEFT_mag_frame_ready[i]; }
		const frames& Get_magazine_f(int i) const noexcept { return magazine_f[i]; }
		void Ready(std::string path_, std::string named) noexcept {
			this->name = named;
			this->path = path_ + named;
			MV1::Load(this->path + "/model.mv1", &this->model, true);
			MV1::Load(this->path + "/col.mv1", &this->col, true);
		}
		int mdata = -1;
		void Set_(std::function<void()> doing) {
			this->model.SetMatrix(MGetIdent());
			for (int i = 0; i < int(this->model.frame_num()); ++i) {
				std::string p = this->model.frame_name(i);
				if (p == std::string("ammo1")) {
					this->magazine_ammo_f[0].Set_World(i, this->model);
				}
				if (p == std::string("ammo2")) {
					this->magazine_ammo_f[1].Set_World(i, this->model);
				}
				if (p == std::string("LEFT_mag")) {
					this->LEFT_mag_frame[0].Set_World(i, this->model);
					this->LEFT_mag_frame[1].Set_Local(i + 1, this->model);
					this->LEFT_mag_frame[2].Set_Local(i + 2, this->model);
				}
				if (p == std::string("LEFT_mag_ready")) {
					this->LEFT_mag_frame_ready[0].Set_World(i, this->model);
					this->LEFT_mag_frame_ready[1].Set_Local(i + 1, this->model);
					this->LEFT_mag_frame_ready[2].Set_Local(i + 2, this->model);
				}
				if (p == std::string("magazine_fall")) {
					this->magazine_f[0].Set_World(i, this->model);
					this->magazine_f[1].Set_Local(i + 1, this->model);
				}
			}
			this->mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			doing();
			FileRead_close(mdata);
		}
	};
	//銃用オーディオ
	class Audios_Gun {
	private:
	public:
		std::string shot_path;
		std::string slide_path;
		int use_shot = -1;
		int use_slide = -1;
		int use_magdown = -1;
		int use_magset = -1;

		void Set(int mdata) {
			use_shot = getparams::_int(mdata);
			use_slide = getparams::_int(mdata);
			use_magdown = getparams::_int(mdata);
			use_magset = getparams::_int(mdata);

			shot_path = "data/Audio/gun/fire/" + std::to_string(use_shot) + ".wav";
			slide_path = "data/Audio/gun/slide/" + std::to_string(use_slide) + ".wav";
		}
		void Duplicate(const Audios_Gun& tgt) {
			this->use_shot = tgt.use_shot;
			this->use_slide = tgt.use_slide;
			this->use_magdown = tgt.use_magdown;
			this->use_magset = tgt.use_magset;
			this->shot_path = tgt.shot_path;
			this->slide_path = tgt.slide_path;
		}
		void Dispose(void) noexcept {
			this->use_shot = -1;
			this->use_slide = -1;
			this->use_magdown = -1;
			this->use_magset = -1;
			this->shot_path = "";
			this->slide_path = "";
		}
	};
	//弾データ
	class Ammos {
	private:
		std::string name;
		std::string path;
		MV1 model;
		//
		MV1 model_full;//未発射
		//
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		int damage = 10;//ダメージ
	public:
		void Set_name(const std::string& value) noexcept { name = value; }

		const MV1& Get_model(void) const noexcept { return model; }

		const auto& Get_model_full(void) const noexcept { return model_full; }
		const float& Get_caliber(void) const noexcept { return caliber; }
		const float& Get_speed(void) const noexcept { return speed; }
		const float& Get_pene(void) const noexcept { return pene; }
		const int& Get_damage(void) const noexcept { return damage; }
		const std::string& Get_name(void) const noexcept { return name; }

		auto& Set_speed(void) noexcept { return speed; }

		void Set_before(std::string path_, std::string named) {
			this->name = named;
			this->path = path_ + named;
			MV1::Load(this->path + "/ammo.mv1", &this->model, true);
			MV1::Load(this->path + "/ammo2.mv1", &this->model_full, true);
		}
		void Set(void) noexcept {
			int mdata = FileRead_open((this->path + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata) * 0.001f;//口径
			this->speed = getparams::_float(mdata);	//弾速
			this->pene = getparams::_float(mdata);	//貫通
			this->damage = getparams::_int(mdata);//ダメージ
			FileRead_close(mdata);
		}

		Ammos(void) noexcept {
			this->caliber = 0.f;
			this->speed = 100.f;//弾速
			this->pene = 10.f;//貫通
			this->damage = 10;//ダメージ
		}
		Ammos(const Ammos& tgt_minimam) {
			this->caliber = tgt_minimam.caliber;
			this->speed = tgt_minimam.speed;
			this->pene = tgt_minimam.pene;
			this->damage = tgt_minimam.damage;
		}
		void operator=(const Ammos& tgt_minimam) {
			this->caliber = tgt_minimam.caliber;
			this->speed = tgt_minimam.speed;
			this->pene = tgt_minimam.pene;
			this->damage = tgt_minimam.damage;
		}
	};
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
			//SetDrawAlphaTest(DX_CMP_GREATER, 128);
			{
				DrawPolygonIndexed3D_UseVertexBuffer(this->VerBuf, this->IndexBuf, this->hits_pic.get(), TRUE);
			}
			//SetDrawAlphaTest(-1, 0);
		}
	};
	//命中根
	class HIT_PASSIVE {
		//雲
		Model_Instance inst;
		bool isUpdate{ true };
	public:
		//初期化
		void Init(void) noexcept {
			inst.Init("data/model/hit/hit.png", "data/model/hit/model.mv1");
		}
		//毎回のリセット
		void Clear(void) noexcept {
			inst.Clear();
		}

		void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			inst.Set(caliber, Position, Normal, Zvec);
			isUpdate = true;
		}
		void Update(void) noexcept {
			if (isUpdate) {
				isUpdate = false;
				inst.Update();
			}
		}
		void Draw(void) noexcept {
			inst.Draw();
		}
	};
	HIT_PASSIVE hit_obj_p;												//静的弾痕
	class HIT_BLOOD_PASSIVE {
		//雲
		Model_Instance inst;
		bool isUpdate{ true };
	public:
		//初期化
		void Init(void) noexcept {
			inst.Init("data/model/hit_blood/hit.png", "data/model/hit_blood/model.mv1");
		}
		//毎回のリセット
		void Clear(void) noexcept {
			inst.Clear();
		}

		void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			inst.Set(caliber, Position, Normal, Zvec);
			isUpdate = true;
		}
		void Update(void) noexcept {
			if (isUpdate) {
				isUpdate = false;
				inst.Update();
			}
		}
		void Draw(void) noexcept {
			inst.Draw();
		}
	};
	HIT_BLOOD_PASSIVE hit_b_obj_p;										//静的血痕
	class HIT_ACTIVE {
		struct Hit {		      /**/
			bool Flag{ false };   /*弾痕フラグ*/
			int use{ 0 };	      /*使用フレーム*/
			MV1 pic;	      /*弾痕モデル*/
			VECTOR_ref pos;	      /*座標*/
			MATRIX_ref mat;	      /**/
		};								      /**/
		std::array<Hit, 24> hit_obj;					      /*弾痕*/
		int hitbuf = 0;		       /*使用弾痕*/
	public:
		void Set(const MV1& hit_pic) {
			for (auto& h : this->hit_obj) {
				h.Flag = false;
				h.pic = hit_pic.Duplicate();
				h.use = 0;
				h.mat = MGetIdent();
				h.pos = VGet(0.f, 0.f, 0.f);
			}
		}
		void Set(const moves& this_move, const  VECTOR_ref& Put_pos, const  VECTOR_ref& Put_normal, const VECTOR_ref& ammo_nomal/*this->move.vec.Norm()*/, const float&caliber, bool isPene) {
			float asize = 200.f*caliber;
			auto y_vec = MATRIX_ref::Vtrans(Put_normal, this_move.mat.Inverse());
			auto z_vec = MATRIX_ref::Vtrans(Put_normal.cross(ammo_nomal).Norm(), this_move.mat.Inverse());
			auto scale = VECTOR_ref::vget(asize / std::abs(ammo_nomal.dot(Put_normal)), asize, asize);

			this->hit_obj[this->hitbuf].use = (isPene) ? 0 : 1;				//弾痕
			this->hit_obj[this->hitbuf].mat = MATRIX_ref::GetScale(scale)* MATRIX_ref::Axis1_YZ(y_vec, z_vec);
			this->hit_obj[this->hitbuf].pos = MATRIX_ref::Vtrans(Put_pos - this_move.pos, this_move.mat.Inverse()) + y_vec * 0.005f;
			this->hit_obj[this->hitbuf].Flag = true;
			++this->hitbuf %= this->hit_obj.size();
		}
		void Update(const moves& this_move) {
			//弾痕
			for (auto& h : this->hit_obj) {
				if (h.Flag) {
					h.pic.SetMatrix(h.mat* this_move.mat*MATRIX_ref::Mtrans(this_move.pos + MATRIX_ref::Vtrans(h.pos, this_move.mat)));
				}
			}
		}
		void Draw() {
			//弾痕
			for (auto& h : this->hit_obj) {
				if (h.Flag) {
					h.pic.DrawFrame(h.use);
				}
			}
		}
		void Dispose() {
			for (auto& h : this->hit_obj) {
				h.Flag = false;
				h.use = 0;
				h.pic.Dispose();
				h.pos = VGet(0, 0, 0);
				h.mat = MGetIdent();
			}
		}
	};
	//パフォーマンス
	class Performance {
	public:
		float recoil = 0.f;
		float weight = 0.f;
		std::string name;
		std::string info;

		void operator+=(const Performance& t) {
			this->recoil += t.recoil;
			this->weight += t.weight;
		}
		void operator-=(const Performance& t) {
			this->recoil -= t.recoil;
			this->weight -= t.weight;
		}
	};
	//
	class BASE_Obj {
	private:
	public:
		size_t id_t = 0;
		Models mod;
		void Set(size_t id_) {
			this->id_t = id_;
		}
	};
	//パーツデータ
	class GUNPARTs :public BASE_Obj {
		EnumGunParts type = EnumGunParts::NONE;
		float recoil_xup = 0.f;							//反動
		float recoil_xdn = 0.f;							//反動
		float recoil_yup = 0.f;							//反動
		float recoil_ydn = 0.f;							//反動
		float reload_time = 1.f;						//再装填時間
		std::vector <EnumSELECTER> select;				//セレクター
		std::vector <EnumSELECT_LAM> select_lam;		//ライト・レーザー
		std::vector<std::string> can_attach;
		Performance per;								//common
		size_t mazzule_type = 0;						//mazzule
		GraphHandle reticle;							//sight
		float zoom = 1.f;								//sight
		float reticle_size = 600.f;						//sight
		float zoom_size = 600.f;						//sight
		size_t stock_type = 0;							//stock
		std::vector<Ammos> ammo;
		Audios_Gun Audio;
		//magazine
		size_t Ammo_cap = 1;
	public:
		void Copy_Ammo(const GUNPARTs& tgt) {
			if (this->ammo.size() < tgt.ammo.size()) {
				this->ammo.resize(tgt.ammo.size());
			}
			for (int i = 0; i < this->ammo.size(); i++) {
				this->ammo[i].Set_name(tgt.ammo[i].Get_name());
			}
		}
		//setter
		void Set_type(EnumGunParts type_t) noexcept { this->type = type_t; }
		auto& Set_Ammo(size_t ID_t) noexcept { return ammo[ID_t]; }
		//getter
		const auto& Get_recoil_xup(void) const noexcept { return  recoil_xup; }
		const auto& Get_recoil_xdn(void) const noexcept { return  recoil_xdn; }
		const auto& Get_recoil_yup(void) const noexcept { return  recoil_yup; }
		const auto& Get_recoil_ydn(void) const noexcept { return  recoil_ydn; }
		const auto& Get_reloadtime(void) const noexcept { return reload_time; }
		const auto Get_selectsize(void)const noexcept { return select.size(); }
		const auto& Get_select(size_t ID_t)const noexcept { return select[ID_t]; }
		const auto& Get_lamtype(size_t ID_t) const noexcept { return this->select_lam[ID_t]; }
		const auto& Get_can_attach(void)const noexcept { return can_attach; }
		const auto& Get_Performance(void) const noexcept { return this->per; }
		const auto& Get_mazzuletype(void) const noexcept { return this->mazzule_type; }
		const auto& Get_reticle(void) const noexcept { return this->reticle; }
		const auto& Get_zoom(void) const noexcept { return this->zoom; }
		const auto& Get_reticle_size(void) const noexcept { return this->reticle_size; }
		const auto& Get_zoom_size(void) const noexcept { return this->zoom_size; }
		const auto& Get_stocktype(void) const noexcept { return this->stock_type; }
		const auto& Set_Ammo(void) const noexcept { return ammo; }
		const auto& Get_Ammo(size_t ID_t) const noexcept { return ammo[ID_t]; }
		const auto& Get_Audio(void) const noexcept { return Audio; }
		const auto& Get_Ammo_Cap()const noexcept { return Ammo_cap; }
		//
		static void Set_Pre(std::vector<GUNPARTs>* data, std::string file_name, EnumGunParts type_t = EnumGunParts::NONE) noexcept {
			data->clear();
			GetFileNames(file_name);
			for (auto& d : data_t) {
				if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					data->resize(data->size() + 1);
					data->back().mod.Ready(file_name, d.cFileName);
					data->back().Set_type(type_t);
				}
			}
		}
		//
		void Set(size_t id_) {
			BASE_Obj::Set(id_);
			//テキスト
			this->mod.Set_([&] {
				//共通データ
				{
					per.name = getparams::get_str(this->mod.mdata);		//名前
					per.info = getparams::get_str(this->mod.mdata);		//説明
				}
				//
				if (this->type == EnumGunParts::MAZZULE) {
					mazzule_type = getparams::_ulong(this->mod.mdata);
				}
				if (this->type == EnumGunParts::STOCK) {
					stock_type = getparams::_ulong(this->mod.mdata);
				}
				if (this->type == EnumGunParts::BASE) {
					//セレクター設定
					while (true) {
						auto p = getparams::_str(this->mod.mdata);
						if (getparams::getright(p.c_str()) == "end") {
							break;
						}
						else if (getparams::getright(p.c_str()) == "semi") {
							this->select.emplace_back(EnumSELECTER::SEMI);	//セミオート=1
						}
						else if (getparams::getright(p.c_str()) == "full") {
							this->select.emplace_back(EnumSELECTER::FULL);	//フルオート=2
						}
						else if (getparams::getright(p.c_str()) == "3b") {
							this->select.emplace_back(EnumSELECTER::_3B);	//3連バースト=3
						}
						else if (getparams::getright(p.c_str()) == "2b") {
							this->select.emplace_back(EnumSELECTER::_2B);	//2連バースト=4
						}
						else {
							this->select.emplace_back(EnumSELECTER::SEMI);
						}
					}
					this->Audio.Set(this->mod.mdata);						//サウンド
					this->recoil_xup = getparams::_float(this->mod.mdata);	//反動
					this->recoil_xdn = getparams::_float(this->mod.mdata);	//反動
					this->recoil_yup = getparams::_float(this->mod.mdata);	//反動
					this->recoil_ydn = getparams::_float(this->mod.mdata);	//反動
					this->reload_time = getparams::_float(this->mod.mdata);	//リロードタイム
					Set_Ammos_data(this->mod.mdata);						//弾データ
				}
				if (this->type == EnumGunParts::MAGAZINE) {
					this->Ammo_cap = getparams::_long(this->mod.mdata);			//弾数
					Set_Ammos_data(this->mod.mdata);						//弾データ
				}
				if (this->type == EnumGunParts::LAM) {
					//レーザーかライトか
					while (true) {
						auto p = getparams::_str(this->mod.mdata);
						if (getparams::getright(p.c_str()) == "end") {
							break;
						}
						else if (getparams::getright(p.c_str()) == "laser") {
							this->select_lam.emplace_back(EnumSELECT_LAM::LASER);	//レーザー
						}
						else if (getparams::getright(p.c_str()) == "light") {
							this->select_lam.emplace_back(EnumSELECT_LAM::LIGHT);	//ライト
						}
						else {
							this->select_lam.emplace_back(EnumSELECT_LAM::LASER);	//レーザー
						}
					}
				}
				if (this->type == EnumGunParts::SIGHT) {
					this->zoom = getparams::_float(this->mod.mdata);
					this->reticle_size = getparams::_float(this->mod.mdata);
					this->zoom_size = getparams::_float(this->mod.mdata);
				}
				//共通データ
				{
					per.recoil = getparams::_float(this->mod.mdata);		//リコイル
					per.weight = getparams::_float(this->mod.mdata);		//重量
					{
						can_attach.clear();
						while (FileRead_eof(this->mod.mdata) == 0) {
							can_attach.emplace_back(getparams::get_str(this->mod.mdata));
						}
					}
				}
			});
			if (this->type == EnumGunParts::SIGHT) {
				SetUseASyncLoadFlag(FALSE);
				this->reticle = GraphHandle::Load(this->mod.Get_path() + "/reticle.png");
				SetUseASyncLoadFlag(FALSE);
			}
			if (this->type == EnumGunParts::BASE) {
				//フレーム
				for (auto& a : this->ammo) {
					a.Set();
				}
			}
		}
		//
		void Set_Ammos_data(int mdata) {
			while (true) {
				auto p = getparams::_str(mdata);
				if (getparams::getright(p.c_str()).find("end") != std::string::npos) {
					break;
				}
				else {
					this->ammo.resize(this->ammo.size() + 1);
					this->ammo.back().Set_before("data/ammo/", p);
				}
			}
		}
		int Select_Chose(EnumSELECTER sel_chose) {
			auto ans = std::find(this->select.begin(), this->select.end(), sel_chose);
			return int((ans != this->select.end()) ? (ans - this->select.begin()) : -1);
		}
		void Set_gun_select(std::vector<MV1::ani*>& gunanime_sel, EnumSELECTER selecting) {
			for (auto& sel : this->select) {
				easing_set(&gunanime_sel[&sel - &this->select[0]]->per, float(int(sel == this->select[(int)selecting])), 0.5f);
			}
		}
	};
	//薬品データ
	class Meds :public BASE_Obj {
	public:
		int repair = 0;
		static void Set_Pre(std::vector<Meds>* data, std::string file_name) noexcept {
			data->clear();
			GetFileNames(file_name);
			for (auto& d : data_t) {
				if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					data->resize(data->size() + 1);
					data->back().mod.Ready(file_name, d.cFileName);
				}
			}
		}
		void Set(size_t id_) {
			BASE_Obj::Set(id_);
			//テキスト
			this->mod.Set_([&] {
				this->repair = getparams::_long(this->mod.mdata);//
			});
		}
	};
	//薬品データ
	class Grenades :public BASE_Obj {
	public:
		float time = 0;
		static void Set_Pre(std::vector<Grenades>* data, std::string file_name) noexcept {
			data->clear();
			GetFileNames(file_name);
			for (auto& d : data_t) {
				if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					data->resize(data->size() + 1);
					data->back().mod.Ready(file_name, d.cFileName);
				}
			}
		}
		void Set(size_t id_) {
			BASE_Obj::Set(id_);
			//テキスト
			this->mod.Set_([&] {
				this->time = getparams::_float(this->mod.mdata);//
			});
		}
	};
	//アイテム
	class Items {
	private:
		//共通
		moves move;
		MV1 obj;
		float del_timer = 0.f;
		size_t id_t = 0;
		//マガジン専用パラメーター
		GUNPARTs* ptr_mag{ nullptr };
		size_t Ammo_cnt{ 30 };						//入る最大数
		//治療キット専用パラメーター
		Meds* ptr_med{ nullptr };
		Meds medkit;
		//治療キット専用パラメーター
		Grenades* ptr_gre{ nullptr };
		Grenades grenades;
	public:
		bool Flag_canlook_player{ true };
		auto& Get_ptr_mag(void) const noexcept { return ptr_mag; }
		auto& Get_ptr_med(void) const noexcept { return ptr_med; }
		const auto& Get_Ammo_Cnt() const noexcept { return Ammo_cnt; }
		auto& Get_pos_(void) const noexcept { return move.pos; }
	private:
		//mag
		void Set_item(GUNPARTs* magdata, const moves& move_, size_t dnm = SIZE_MAX) {
			this->move = move_;
			this->ptr_mag = magdata;
			this->obj = this->ptr_mag->mod.Get_model().Duplicate();
			if (dnm == SIZE_MAX) {
				if (this->ptr_mag != nullptr) {
					this->Ammo_cnt = int(this->ptr_mag->Get_Ammo_Cap());
				}
			}
			else {
				this->Ammo_cnt = dnm;
			}
			this->del_timer = (this->Ammo_cnt == 0) ? 5.f : 20.f;
		}
		//med
		void Set_item(Meds* meddata, const moves& move_) {
			this->move = move_;
			this->ptr_med = meddata;
			this->obj = this->ptr_med->mod.Get_model().Duplicate();
		}
		//med
		void Set_item(Grenades* gredata, const moves& move_) {
			this->move = move_;
			this->ptr_gre = gredata;
			this->obj = this->ptr_gre->mod.Get_model().Duplicate();
			this->del_timer = this->ptr_gre->time;
		}
	public:
		//mag
		Items(size_t id, GUNPARTs* magdata, const moves& move_, size_t dnm = SIZE_MAX) {
			this->id_t = id;
			Set_item(magdata, move_, dnm);
		}
		bool Set_item_(GUNPARTs* magdata, const moves& move_, size_t dnm) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(magdata, move_, dnm);
				return true;
			}
			return false;
		}
		//med
		Items(size_t id, Meds* meddata, const moves& move_) {
			this->id_t = id;
			this->Set_item(meddata, move_);
		}
		bool Set_item_(Meds* meddata, const moves& move_) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(meddata, move_);
				return true;
			}
			return false;
		}
		//gre
		Items(size_t id, Grenades* gredata, const moves& move_) {
			this->id_t = id;
			this->Set_item(gredata, move_);
		}
		bool Set_item_(Grenades* gredata, const moves& move_) {
			if (this->ptr_mag == nullptr && this->ptr_med == nullptr && this->ptr_gre == nullptr) {
				this->Set_item(gredata, move_);
				return true;
			}
			return false;
		}
		//
		void Update(std::vector<std::shared_ptr<Items>>& item, std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> map_col_line) {
			auto old = this->move.pos;
			if (this->ptr_mag != nullptr || this->ptr_med != nullptr || this->ptr_gre != nullptr) {
				this->obj.SetMatrix(this->move.mat * MATRIX_ref::Mtrans(this->move.pos));
				this->move.Update_Physics();
				for (auto& p : item) {
					if ((p->ptr_mag != nullptr || p->ptr_med != nullptr || p->ptr_gre != nullptr) && p.get() != this) {
						if ((p->move.pos - this->move.pos).size() <= 0.35f) {
							p->move.vec.xadd((p->move.pos - this->move.pos).x() * 5.f / FPS);
							p->move.vec.zadd((p->move.pos - this->move.pos).z() * 5.f / FPS);
							this->move.vec.xadd((this->move.pos - p->move.pos).x() * 5.f / FPS);
							this->move.vec.zadd((this->move.pos - p->move.pos).z() * 5.f / FPS);
						}
					}
				}
				if (this->ptr_gre != nullptr) {
					auto ColResGround = map_col_line(old - VECTOR_ref::vget(0, 0.0025f, 0), this->move.pos - VECTOR_ref::vget(0, 0.005f, 0));
					if (ColResGround.HitFlag == TRUE) {
						this->move.HitGround(ColResGround, 0.005f);
						auto fvec = this->move.vec.Norm();
						auto nvec = VECTOR_ref(ColResGround.Normal).Norm();
						this->move.vec = (fvec + nvec * ((fvec * -1.f).dot(nvec) * 2.f)) * (this->move.vec.size() * 0.5f);
					}
				}
				else {
					auto ColResGround = map_col_line(old - VECTOR_ref::vget(0, 0.0025f, 0), this->move.pos - VECTOR_ref::vget(0, 0.005f, 0));
					if (ColResGround.HitFlag == TRUE) {
						this->move.HitGround(ColResGround, 0.005f);
						this->move.vec.clear();
						//easing_set(&this->move.vec, VECTOR_ref::zero(), 0.8f);
					}
				}
				//
			}
			//
			if (this->ptr_mag != nullptr || this->ptr_gre != nullptr) {
				this->del_timer -= 1.f / FPS;
			}
		}
		template<class PLAYER_CHARA>
		void Get_item_2(VECTOR_ref StartPos, VECTOR_ref EndPos, const std::shared_ptr<PLAYER_CHARA>& chara, std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> map_col_line) {
			if (this->Flag_canlook_player) {
				bool zz = false;
				if (this->ptr_mag != nullptr || this->ptr_med != nullptr || this->ptr_gre != nullptr) {
					auto ColResGround = map_col_line(StartPos, EndPos);
					if (ColResGround.HitFlag == TRUE) {
						EndPos = ColResGround.HitPosition;
					}
					zz = (Segment_Point_MinLength(StartPos.get(), EndPos.get(), this->move.pos.get()) <= 0.4f);
				}
				//個別
				if (this->ptr_mag != nullptr) {
					chara->addf_canget_magitem(zz);
					if (zz) {
						chara->Set_canget_mag(this->id_t, this->ptr_mag->mod.Get_name());
						if (chara->getItem_push() && this->Ammo_cnt != 0) {//&& (this->ptr_mag->Get_Ammo(0).Get_name() == this->ptr_mag->Get_Ammo(0).Get_name())
							chara->Set_sort_f(false);
							chara->Set_Gun_().Get_gun_stat_now()->magazine_plus(this);
							if (chara->Set_Gun_().Get_mag_in().size() == 1) {
								chara->Set_reloadf(true);
							}
							this->Detach_item();
						}
					}
				}
				//
				if (this->ptr_med != nullptr) {
					chara->addf_canget_meditem(zz);
					if (zz) {
						chara->Set_canget_med(this->id_t, this->ptr_med->mod.Get_name());
						if (chara->getItem_push()) {
							chara->Damage.AddHP(this->ptr_med->repair);
							this->Detach_item();
						}
					}
				}
			}
		}
		void Check_CameraViewClip(bool use_occlusion, std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> map_col_line) noexcept {
			this->Flag_canlook_player = true;
			auto ttt = this->move.pos;
			if (CheckCameraViewClip_Box((ttt + VECTOR_ref::vget(-0.3f, 0, -0.3f)).get(), (ttt + VECTOR_ref::vget(0.3f, 0.3f, 0.3f)).get())) {
				this->Flag_canlook_player = false;
				return;
			}
			if (use_occlusion) {
				if (map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.3f, 0)).HitFlag == TRUE &&
					map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.0f, 0)).HitFlag == TRUE) {
					this->Flag_canlook_player = false;
					return;
				}
			}
		}

		void Draw_item(void) noexcept {
			if (this->Flag_canlook_player) {
				if (this->ptr_mag != nullptr || this->ptr_med != nullptr || this->ptr_gre != nullptr) {
					this->obj.DrawModel();
				}
			}
		}
		void Detach_item(void) noexcept {
			this->ptr_mag = nullptr;
			this->ptr_med = nullptr;
			this->ptr_gre = nullptr;
			this->obj.Dispose();
		}
		bool Detach_mag(void) noexcept {
			if (this->ptr_mag != nullptr && this->del_timer <= 0.f) {
				this->Detach_item();
				return true;
			}
			return false;
		}
		template<class PLAYER_CHARA>
		bool Detach_gre(std::shared_ptr<PLAYER_CHARA>& killer, std::vector<std::shared_ptr<PLAYER_CHARA>>& chara, std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> map_col_line) noexcept {
			if (this->ptr_gre != nullptr && this->del_timer <= 0.f) {
				//effect
				killer->Set_Effect(Effect::ef_greexp, this->move.pos, VECTOR_ref::front(), 0.1f / 0.1f);
				//
				SE.Get(EnumSound::Explosion).Play_3D(0, this->move.pos, 100.f, 255);
				//グレ爆破
				this->Detach_item();
				for (auto& tgt : chara) {
					tgt->calc_gredamage(killer);
				}
				return true;
			}
			return false;
		}
	};
	//戦車砲データ
	class gun_frame {
		std::vector<std::string> useammo;//現状未使用

		int Ammo_Cap = 0;
		std::string name;
		std::vector<Ammos> Spec;	/**/
		float load_time = 0.f;
		std::array<frames, 3> frame;
		int sound_use = 0;
	public:
		const auto& Get_Ammo_Cap()const noexcept { return Ammo_Cap; }
		const auto& Get_name()const noexcept { return name; }
		const auto& Get_Spec()const noexcept { return Spec; }
		const auto& Get_load_time()const noexcept { return load_time; }
		const auto& Get_frame(size_t ID_t)const noexcept { return frame[ID_t]; }
		const auto& Get_sound()const noexcept { return sound_use; }
		//const auto& Get_()const noexcept { return; }

		gun_frame(void) noexcept {
			frame[0].first = -1;
			frame[1].first = -1;
			frame[2].first = -1;
		}

		void Set(const MV1& obj, int i) {
			this->frame[0].Set_World(i, obj);
			this->frame[2].first = -1;
			this->frame[1].first = -1;
			auto p2 = obj.frame_parent(this->frame[0].first);
			if (p2 >= 0) {
				this->frame[0].second -= obj.frame(int(p2)); //親がいる時引いとく
			}
			if (obj.frame_child_num(this->frame[0].first) <= 0) {
				return;
			}
			int child_num = (int)obj.frame_child(this->frame[0].first, 0);
			if (obj.frame_name(child_num).find("仰角", 0) != std::string::npos) {
				this->frame[1].Set_Local(child_num, obj);
			}
			if (this->frame[1].first == -1) {
				return;
			}
			if (obj.frame_child_num(this->frame[1].first) <= 0) {
				return;
			}
			{
				this->frame[2].Set_Local((int)obj.frame_child(this->frame[1].first, 0), obj);
			}
		}

		void Set_Performance(int mdata, const std::string& stt) {
			this->name = getparams::getright(stt);
			this->load_time = getparams::_float(mdata);
			this->Ammo_Cap = getparams::_int(mdata);
			this->sound_use = getparams::_int(mdata);
			this->Spec.resize(this->Spec.size() + 1);
			this->Spec.back().Set_before("data/ammo/", getparams::_str(mdata));
			while (true) {
				auto stp = getparams::get_str(mdata);
				if (stp.find("useammo" + std::to_string(this->useammo.size())) == std::string::npos) {
					break;
				}
				this->useammo.emplace_back(getparams::getright(stp));
			}
		}

		void Set_Ammos_after(void) noexcept {
			for (auto& a : this->Spec) {
				a.Set();
			}
		}
	};
	//
	class b2Pats {
		b2FixtureDef fixtureDef;			/*動的ボディフィクスチャを定義します*/
		std::unique_ptr<b2Body> body;		/**/
		b2Fixture* playerfix{ nullptr };	/**/
	public:
		VECTOR_ref pos;/*仮*/

		void Set(b2Body* body_ptr, b2Shape* dynamicBox) {
			fixtureDef.shape = dynamicBox;								/**/
			fixtureDef.density = 1.0f;									/*ボックス密度をゼロ以外に設定すると、動的になる*/
			fixtureDef.friction = 0.3f;									/*デフォルトの摩擦をオーバーライド*/
			this->body.reset(body_ptr);									/**/
			this->playerfix = this->body->CreateFixture(&fixtureDef);	/*シェイプをボディに追加*/
		}

		void SetLinearVelocity(const b2Vec2& position) {
			this->body->SetLinearVelocity(position);
		}

		void Update(const VECTOR_ref& add, float yradadd) {
			this->body->SetLinearVelocity(b2Vec2(add.x(), add.z()));
			this->body->SetAngularVelocity(yradadd);
		}


		void Dispose(void) noexcept {
			if (this->playerfix != nullptr) {
				delete this->playerfix->GetUserData();
				this->playerfix->SetUserData(nullptr);
			}
		}

		const auto Get(void) noexcept { return body.get(); }
		const auto Pos(void) noexcept { return body->GetPosition(); }
		const auto Rad(void) noexcept { return body->GetAngle(); }
		const float Speed(void) noexcept { return std::hypot(this->body->GetLinearVelocity().x, this->body->GetLinearVelocity().y); }

		void SetTransform(const b2Vec2& position, float32 angle) {
			body->SetTransform(position, angle);
		}
	};
	//戦車データ
	class Vehcs {
		std::string name;									/**/
		MV1 obj, col;										/**/
		VECTOR_ref minpos, maxpos;							/**/
		std::vector<gun_frame> gunframe;					/**/
		std::vector<frames> wheelframe;						/**/
		std::vector<frames> wheelframe_nospring;			/*誘導輪回転*/
		int HP = 0;											/**/
		std::vector<std::pair<int, float>> armer_mesh;		/*装甲ID*/
		std::vector<int> space_mesh;						/*装甲ID*/
		std::vector<int> module_mesh;						/*装甲ID*/
		bool isfloat{ false };								/*浮くかどうか*/
		float down_in_water = 0.f;							/*沈む判定箇所*/
		float front_speed_limit = 0.f;						/*前進速度(km/h)*/
		float back_speed_limit = 0.f;						/*後退速度(km/h)*/
		float body_rad_limit = 0.f;							/*旋回速度(度/秒)*/
		float turret_rad_limit = 0.f;						/*砲塔駆動速度(度/秒)*/
		frames fps_view;									//コックピット
		GraphHandle ui_pic;									//シルエット
		int pic_x, pic_y;									//サイズ
		std::array<int, 4> square{ 0 };						//車輛の四辺
		std::array<std::vector<frames>, 2> b2upsideframe;	/*履帯上*/
		std::array<std::vector<frames>, 2> b2downsideframe;	/*履帯*/


		void copy(const Vehcs& t) {
			this->wheelframe.clear();
			for (auto& p : t.wheelframe) {
				this->wheelframe.resize(this->wheelframe.size() + 1);
				this->wheelframe.back() = p;
			}
			this->wheelframe_nospring.clear();
			for (auto& p : t.wheelframe_nospring) {
				this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
				this->wheelframe_nospring.back() = p;
			}
			this->name = t.name;
			this->minpos = t.minpos;
			this->maxpos = t.maxpos;
			this->gunframe = t.gunframe;
			this->HP = t.HP;
			this->armer_mesh = t.armer_mesh;
			this->space_mesh = t.space_mesh;
			this->module_mesh = t.module_mesh;
			this->isfloat = t.isfloat;
			this->down_in_water = t.down_in_water;
			this->front_speed_limit = t.front_speed_limit;
			this->back_speed_limit = t.back_speed_limit;
			this->body_rad_limit = t.body_rad_limit;
			this->turret_rad_limit = t.turret_rad_limit;
			this->square = t.square;
			this->b2upsideframe = t.b2upsideframe;
			this->b2downsideframe = t.b2downsideframe;
			this->fps_view = t.fps_view;

			this->ui_pic = t.ui_pic.Duplicate();
			this->pic_x = t.pic_x;
			this->pic_y = t.pic_y;
		}
	public:
		int Reload_ID = -1;
		//
		const auto& Get_name()const noexcept { return name; }//未使用

		const auto& Get_obj()const noexcept { return obj; }
		const auto& Get_col()const noexcept { return col; }
		const auto& Get_minpos()const noexcept { return minpos; }
		const auto& Get_maxpos()const noexcept { return maxpos; }
		const auto& Get_gunframe(size_t ID_t)const noexcept { return gunframe[ID_t]; }
		const auto& Get_gunframe()const noexcept { return gunframe; }
		const auto& Get_wheelframe()const noexcept { return wheelframe; }
		const auto& Get_wheelframe_nospring()const noexcept { return wheelframe_nospring; }
		const auto& Get_HP()const noexcept { return HP; }
		const auto& Get_armer_mesh()const noexcept { return armer_mesh; }
		const auto& Get_space_mesh()const noexcept { return space_mesh; }
		const auto& Get_module_mesh()const noexcept { return module_mesh; }
		const auto& Get_isfloat()const noexcept { return isfloat; }
		const auto& Get_down_in_water()const noexcept { return down_in_water; }
		const auto& Get_front_speed_limit()const noexcept { return front_speed_limit; }
		const auto& Get_back_speed_limit()const noexcept { return back_speed_limit; }
		const auto& Get_body_rad_limit()const noexcept { return body_rad_limit; }
		const auto& Get_turret_rad_limit()const noexcept { return turret_rad_limit; }
		const auto& Get_fps_view()const noexcept { return fps_view; }
		const auto& Get_ui_pic()const noexcept { return ui_pic; }
		const auto& Get_pic_x()const noexcept { return pic_x; }
		const auto& Get_pic_y()const noexcept { return pic_y; }
		const auto& Get_square(size_t ID_t)const noexcept { return square[ID_t]; }
		const auto& Get_square()const noexcept { return square; }
		const auto& Get_b2upsideframe(size_t ID_t)const noexcept { return b2upsideframe[ID_t]; }
		const auto& Get_b2upsideframe()const noexcept { return b2upsideframe; }
		const auto& Get_b2downsideframe()const noexcept { return b2downsideframe; }
		//コンストラクタ
		Vehcs(void) noexcept { }
		Vehcs(const Vehcs& t) { this->copy(t); }
		void operator=(const Vehcs& t) { this->copy(t); }
		//事前読み込み
		static void Set_Pre(std::vector<Vehcs>* veh_, const char* name) {
			GetFileNames(name);
			for (auto& d : data_t) {
				if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					veh_->resize(veh_->size() + 1);
					veh_->back().name = d.cFileName;
				}
			}
			for (auto& t : *veh_) {
				MV1::Load(std::string(name) + t.name + "/model.mv1", &t.obj, true);
				MV1::Load(std::string(name) + t.name + "/col.mv1", &t.col, true);
				t.ui_pic = GraphHandle::Load(std::string(name) + t.name + "/pic.png");
			}
		}

		const int GetSide(bool isLeft, bool isFront) const noexcept {
			int ans = 0;
			float tmp = 0.f;
			for (auto& f : this->wheelframe) {
				if ((isLeft ? 1.f : -1.f) * f.second.x() >= 0) {
					ans = f.first;
					tmp = f.second.z();
					break;
				}
			}
			for (auto& f : this->wheelframe) {
				if (ans != f.first) {
					if ((isLeft ? 1.f : -1.f) * f.second.x() >= 0) {
						if (isFront) {
							if (tmp > f.second.z()) {
								ans = f.first;
								tmp = f.second.z();
							}
						}
						else {
							if (tmp < f.second.z()) {
								ans = f.first;
								tmp = f.second.z();
							}
						}
					}
				}
			}
			return ans;
		}
		//メイン読み込み
		void Set(void) noexcept {
			using namespace std::literals;
			//αテスト
			this->obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
			GetGraphSize(this->ui_pic.get(), &this->pic_x, &this->pic_y);
			//固有
			this->down_in_water = 0.f;
			for (int i = 0; i < this->obj.mesh_num(); i++) {
				auto p = this->obj.mesh_maxpos(i).y();
				if (this->down_in_water < p) {
					this->down_in_water = p;
				}
			}
			this->down_in_water /= 2.f;
			for (int i = 0; i < this->obj.frame_num(); i++) {
				std::string p = this->obj.frame_name(i);
				if (p.find("転輪", 0) != std::string::npos) {
					this->wheelframe.resize(this->wheelframe.size() + 1);
					this->wheelframe.back().Set_World(i, this->obj);
				}
				else if ((p.find("輪", 0) != std::string::npos) && (p.find("転輪", 0) == std::string::npos)) {
					this->wheelframe_nospring.resize(this->wheelframe_nospring.size() + 1);
					this->wheelframe_nospring.back().Set_World(i, this->obj);
				}
				else if (p.find("旋回", 0) != std::string::npos) {
					this->gunframe.resize(this->gunframe.size() + 1);
					this->gunframe.back().Set(this->obj, i);
				}
				else if (p == "min") {
					this->minpos = this->obj.frame(i);
				}
				else if (p == "max") {
					this->maxpos = this->obj.frame(i);
				}
				else if (p.find("２D物理", 0) != std::string::npos || p.find("2D物理", 0) != std::string::npos) { //2D物理
					this->b2upsideframe[0].clear();
					this->b2upsideframe[1].clear();
					for (int z = 0; z < this->obj.frame_child_num(i); z++) {
						if (this->obj.frame(i + 1 + z).x() > 0) {
							this->b2upsideframe[0].resize(this->b2upsideframe[0].size() + 1);
							this->b2upsideframe[0].back().Set_World(i + 1 + z, this->obj);
						}
						else {
							this->b2upsideframe[1].resize(this->b2upsideframe[1].size() + 1);
							this->b2upsideframe[1].back().Set_World(i + 1 + z, this->obj);
						}
					}
					std::sort(this->b2upsideframe[0].begin(), this->b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //ソート
					std::sort(this->b2upsideframe[1].begin(), this->b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.second.z() < y.second.z(); }); //ソート
				}
				else if (p.find("履帯設置部", 0) != std::string::npos) { //2D物理
					this->b2downsideframe[0].clear();
					this->b2downsideframe[1].clear();
					for (int z = 0; z < this->obj.frame_child_num(i); z++) {
						if (this->obj.frame(i + 1 + z).x() > 0) {
							this->b2downsideframe[0].resize(this->b2downsideframe[0].size() + 1);
							this->b2downsideframe[0].back().Set_World(i + 1 + z, this->obj);
						}
						else {
							this->b2downsideframe[1].resize(this->b2downsideframe[1].size() + 1);
							this->b2downsideframe[1].back().Set_World(i + 1 + z, this->obj);
						}
					}
				}
			}
			//4隅確定
			this->square[0] = GetSide(true, false);			//2		左後部0
			this->square[1] = GetSide(true, true);			//10	左前部1
			this->square[2] = GetSide(false, false);		//3		右後部2
			this->square[3] = GetSide(false, true);			//11	右前部3
			//装甲
			for (int i = 0; i < this->col.mesh_num(); i++) {
				std::string p = this->col.material_name(i);
				if (p.find("armer", 0) != std::string::npos) {
					this->armer_mesh.emplace_back(i, std::stof(getparams::getright(p.c_str())));//装甲
				}
				else if (p.find("space", 0) != std::string::npos) {
					this->space_mesh.emplace_back(i);//空間装甲
				}
				else {
					this->module_mesh.emplace_back(i);//モジュール
				}
			}
			//data
			{
				int mdata = FileRead_open(("data/tank/" + this->name + "/data.txt").c_str(), FALSE);
				this->isfloat = getparams::_bool(mdata);
				this->front_speed_limit = getparams::_float(mdata);
				this->back_speed_limit = getparams::_float(mdata);
				this->body_rad_limit = getparams::_float(mdata);
				this->turret_rad_limit = deg2rad(getparams::_float(mdata));
				this->HP = getparams::_int(mdata);
				auto stt = getparams::get_str(mdata);
				for (auto& g : this->gunframe) { g.Set_Performance(mdata, stt); }
				Reload_ID = getparams::_int(mdata);//サウンド
				FileRead_close(mdata);
				for (auto& g : this->gunframe) { g.Set_Ammos_after(); }
			}
		}
	};
	//パーツデータ管理
	class GunPartsControl {
	private:
		std::vector<GUNPARTs> magazine_data;		//GUNデータ
		std::vector<GUNPARTs> mazzule_data;			//GUNデータ
		std::vector<GUNPARTs> grip_data;			//GUNデータ
		std::vector<GUNPARTs> uperhandguard_data;	//GUNデータ
		std::vector<GUNPARTs> underhandguard_data;	//GUNデータ
		std::vector<GUNPARTs> stock_data;			//GUNデータ
		std::vector<GUNPARTs> dustcover_data;		//GUNデータ
		std::vector<GUNPARTs> sight_data;			//GUNデータ
		std::vector<GUNPARTs> foregrip_data;		//GUNデータ
		std::vector<GUNPARTs> lam_data;				//GUNデータ
		std::vector<GUNPARTs> mount_base_data;		//GUNデータ
		std::vector<GUNPARTs> mount_data;			//GUNデータ
		std::vector<GUNPARTs> gun_data;				//GUNデータ
	public:
		std::vector<GUNPARTs>* Get_Parts_Data_Ptr(EnumGunParts type_sel) noexcept {
			switch (type_sel) {
			case EnumGunParts::MAGAZINE:
				return &this->magazine_data;
			case EnumGunParts::MAZZULE:
				return &this->mazzule_data;
			case EnumGunParts::GRIP:
				return &this->grip_data;
			case EnumGunParts::UPER_HANDGUARD:
				return &this->uperhandguard_data;
			case EnumGunParts::UNDER_HANDGUARD:
				return &this->underhandguard_data;
			case EnumGunParts::DUSTCOVER:
				return &this->dustcover_data;
			case EnumGunParts::STOCK:
				return &this->stock_data;
			case EnumGunParts::LAM:
				return &this->lam_data;
			case EnumGunParts::MOUNT_BASE:
				return &this->mount_base_data;
			case EnumGunParts::MOUNT:
				return &this->mount_data;
			case EnumGunParts::SIGHT:
				return &this->sight_data;
			case EnumGunParts::FOREGRIP:
				return &this->foregrip_data;
			case EnumGunParts::GUN:
				return &this->gun_data;
			default:
				/*
				NONE,
				BASE,
				*/
				return nullptr;
			}
		}
		std::vector<GUNPARTs>& Get_Parts_Data(EnumGunParts type_sel) noexcept {
			auto ptr = Get_Parts_Data_Ptr(type_sel);
			return *ptr;
		}
		GunPartsControl(void) noexcept {
			GUNPARTs::Set_Pre(&this->grip_data, "data/Guns/parts/grip/");
			GUNPARTs::Set_Pre(&this->uperhandguard_data, "data/Guns/parts/uper_handguard/");
			GUNPARTs::Set_Pre(&this->underhandguard_data, "data/Guns/parts/under_handguard/");
			GUNPARTs::Set_Pre(&this->mount_base_data, "data/Guns/parts/mount_base/");
			GUNPARTs::Set_Pre(&this->mount_data, "data/Guns/parts/mount/");
			GUNPARTs::Set_Pre(&this->dustcover_data, "data/Guns/parts/dustcover/");
			GUNPARTs::Set_Pre(&this->foregrip_data, "data/Guns/parts/foregrip/");
			GUNPARTs::Set_Pre(&this->mazzule_data, "data/Guns/parts/mazzule/", EnumGunParts::MAZZULE);
			GUNPARTs::Set_Pre(&this->stock_data, "data/Guns/parts/stock/", EnumGunParts::STOCK);
			GUNPARTs::Set_Pre(&this->sight_data, "data/Guns/parts/sight/", EnumGunParts::SIGHT);
			GUNPARTs::Set_Pre(&this->lam_data, "data/Guns/parts/lam/", EnumGunParts::LAM);
			GUNPARTs::Set_Pre(&this->gun_data, "data/Guns/gun/", EnumGunParts::BASE);					//GUNデータ
			GUNPARTs::Set_Pre(&this->magazine_data, "data/Guns/mag/", EnumGunParts::MAGAZINE);		//MAGデータ
		}

		void Set(void) noexcept {
			for (auto& g : this->mazzule_data) { g.Set(&g - &this->mazzule_data.front()); }
			for (auto& g : this->grip_data) { g.Set(&g - &this->grip_data.front()); }
			for (auto& g : this->uperhandguard_data) { g.Set(&g - &this->uperhandguard_data.front()); }
			for (auto& g : this->underhandguard_data) { g.Set(&g - &this->underhandguard_data.front()); }
			for (auto& g : this->mount_base_data) { g.Set(&g - &this->mount_base_data.front()); }
			for (auto& g : this->mount_data) { g.Set(&g - &this->mount_data.front()); }
			for (auto& g : this->stock_data) { g.Set(&g - &this->stock_data.front()); }
			for (auto& g : this->dustcover_data) { g.Set(&g - &this->dustcover_data.front()); }
			for (auto& g : this->lam_data) { g.Set(&g - &this->lam_data.front()); }
			for (auto& g : this->foregrip_data) { g.Set(&g - &this->foregrip_data.front()); }
			for (auto& g : this->sight_data) { g.Set(&g - &this->sight_data.front()); }
			for (auto& g : this->gun_data) { g.Set(&g - &this->gun_data.front()); }							//GUNデータ2
			for (auto& g : this->magazine_data) { g.Set(&g - &this->magazine_data.front()); }				//MAGデータ2
		}
	};
	//オーディオ管理
	class Audio_Control {
		static void AddSE_By_File(EnumSound ID_t, size_t buffersize = 1, std::string path_t = "") {
			GetFileNames(path_t);
			for (auto& d : data_t) {
				if (std::string(d.cFileName).find(".wav") != std::string::npos) {
					SE.Add(ID_t, buffersize, path_t + d.cFileName);
				}
			}
		}
	public:
		Audio_Control(void) noexcept {
			//環境音
			SE.Add(EnumSound::MAP0_ENVI, 1, "data/Audio/envi.wav", false);
			//キャラ用オーディオ
			SE.Add(EnumSound::Sort_MAG, 2, "data/Audio/chara/sort.wav");
			SE.Add(EnumSound::Cate_Load, 2, "data/Audio/chara/load.wav");
			SE.Add(EnumSound::Foot_Sound, 3, "data/Audio/chara/foot_sand.wav");
			SE.Add(EnumSound::Explosion, 2, "data/Audio/chara/explosion.wav");
			//ボイス
			VOICE.Add(EnumSound::Voice_Damage, 2, "data/Audio/voice/damage.wav");
			VOICE.Add(EnumSound::Voice_Death, 2, "data/Audio/voice/death.wav");
			VOICE.Add(EnumSound::Voice_Breath, 3, "data/Audio/voice/breath.wav");
			VOICE.Add(EnumSound::Voice_Breath_Run, 3, "data/Audio/voice/breath_run.wav");
			//銃用オーディオ
			SE.Add(EnumSound::Cate_Down, 2, "data/Audio/gun/cate/case_2.wav");
			SE.Add(EnumSound::Assemble, 2, "data/Audio/gun/assemble.wav");
			SE.Add(EnumSound::Trigger, 2, "data/Audio/gun/trigger.wav");
			AddSE_By_File(EnumSound::MAG_Down, 2, "data/Audio/gun/mag_down/");
			AddSE_By_File(EnumSound::MAG_Set, 2, "data/Audio/gun/mag_set/");
			AddSE_By_File(EnumSound::Shot, 5, "data/Audio/gun/fire/");
			AddSE_By_File(EnumSound::Slide, 2, "data/Audio/gun/slide/");
			//戦車用オーディオ
			AddSE_By_File(EnumSound::Tank_Damage, 2, "data/Audio/tank/damage/");
			AddSE_By_File(EnumSound::Tank_Shot, 4, "data/Audio/tank/fire/");
			AddSE_By_File(EnumSound::Tank_Reload, 2, "data/Audio/tank/reload/hand/");
			AddSE_By_File(EnumSound::Tank_Ricochet, 2, "data/Audio/tank/ricochet/");
			SE.Add(EnumSound::Tank_engine, 3, "data/Audio/tank/engine.wav");
			//UI用オーディオ
			SE.Add(EnumSound::CANCEL, 2, "data/Audio/UI/cancel.wav", false);
			SE.Add(EnumSound::CURSOR, 2, "data/Audio/UI/cursor.wav", false);
			SE.Add(EnumSound::TIMER, 2, "data/Audio/UI/timer.wav", false);
			//
		}
	};
};