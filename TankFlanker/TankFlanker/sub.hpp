#pragma once
#include"Header.hpp"

//Box2D�g��
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
		return GetNowHiPerformanceCount() * (int)(GameSpeed*1000.f)/1000;
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

	//�t�@�C������
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
	//�t�H���g�v�[��
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
	//�G�t�F�N�g���\�[�X
	class EffectControl {
		LONGLONG Update_effect_was = 0;					//�G�t�F�N�g�̃A�b�v�f�[�g�^�C�~���O�^�C�}�[
	public:
		bool Update_effect_f{ true };					//�G�t�F�N�g�̃A�b�v�f�[�g�^�C�~���O�t���O
		std::vector<EffekseerEffectHandle> effsorce;	/*�G�t�F�N�g���\�[�X*/

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
			effsorce.back() = EffekseerEffectHandle::load("data/effect/gndsmk.efk");								//��ԗp�G�t�F�N�g
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
	//���C�g
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
	//�G�t�F�N�g���p�R���g���[��
	class Effect_UseControl {
		std::array<EffectS, int(Effect::effects)> effcs;	/*�G�t�F�N�g*/
	public:
		void Set_Effect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept { this->effcs[(int)ef_].Set(pos_t, nomal_t, scale); }
		void Stop_Effect(Effect ef_) noexcept { this->effcs[(int)ef_].Stop(); }
		void SetSpeed_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Speed(value); }
		void SetScale_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Scale(value); }
		//�G�t�F�N�g�̍X�V
		void Update_Effect(void) noexcept {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs.front();
				if (index != (int)Effect::ef_smoke) {
					t.put(effectControl.effsorce[index]);
				}
			}
		}
		/*�����*/
		void Dispose_Effect(void) noexcept {
			for (auto& t : this->effcs) { t.handle.Dispose(); }
		}
	};
	//�V�F�[�_�[
	class shaders {
	public:
		class shader_Vertex {
		public:
			VERTEX3DSHADER Screen_vertex[6] = { 0.0f };

			// ���_�f�[�^�̏���
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
			this->vshandle = LoadVertexShader(("shader/" + vs).c_str());	// ���_�V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
			this->pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->pscbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->pshandle = LoadPixelShader(("shader/" + ps).c_str());		// �s�N�Z���V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
		}
		void Set_dispsize(int dispx, int dispy) {
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->pscbhandle);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			dispsize->u = float(dispx);
			dispsize->v = float(dispy);
			UpdateShaderConstantBuffer(this->pscbhandle);								// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->pscbhandle, DX_SHADERTYPE_PIXEL, 2);		// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^2�ɃZ�b�g
		}
		void Set_param(float param1, float param2, float param3, float param4) {
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->pscbhandle2);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->pscbhandle2);							// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->pscbhandle2, DX_SHADERTYPE_PIXEL, 3);		// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^3�ɃZ�b�g
		}
		void Draw_lamda(std::function<void()> doing) {
			SetUseVertexShader(this->vshandle);		// �g�p���钸�_�V�F�[�_�[���Z�b�g
			SetUsePixelShader(this->pshandle);		// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
			MV1SetUseOrigShader(TRUE);
			doing();
			MV1SetUseOrigShader(FALSE);
			SetUseVertexShader(-1);					// �g�p���钸�_�V�F�[�_�[���Z�b�g
			SetUsePixelShader(-1);					// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
		}
		void Draw(shader_Vertex& Screen_vertex) {
			Draw_lamda([&] {DrawPolygon3DToShader(Screen_vertex.Screen_vertex, 2); });
		}
	};
};