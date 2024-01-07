#pragma once
#include	"Header.hpp"
//
namespace FPS_n2 {
	//����
	class InputControl {
	private:
		float			m_AddxRad{ 0.f };
		float			m_AddyRad{ 0.f };
		float			m_xRad{ 0.f };
		float			m_yRad{ 0.f };
		unsigned int	m_Flags{ 0 };
	public:
		void			SetInput(
			float pAddxRad, float pAddyRad,
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,

			bool pRunPress,
			bool pQPress,
			bool pEPress,

			bool pRightPress,
			bool pLeftPress,
			bool pUpPress,
			bool pDownPress,

			bool pAction1,
			bool pAction2,
			bool pAction3,
			bool pAction4,
			bool pAction5,
			bool pAction6
		) {
			this->m_AddxRad = pAddxRad;
			this->m_AddyRad = pAddyRad;
			this->m_Flags = 0;
			if (pGoFrontPress) { this->m_Flags |= (1 << 0); }
			if (pGoBackPress) { this->m_Flags |= (1 << 1); }
			if (pGoLeftPress) { this->m_Flags |= (1 << 2); }
			if (pGoRightPress) { this->m_Flags |= (1 << 3); }
			if (pRunPress) { this->m_Flags |= (1 << 4); }
			if (pQPress) { this->m_Flags |= (1 << 5); }
			if (pEPress) { this->m_Flags |= (1 << 6); }
			if (pRightPress) { this->m_Flags |= (1 << 7); }
			if (pLeftPress) { this->m_Flags |= (1 << 8); }
			if (pUpPress) { this->m_Flags |= (1 << 9); }
			if (pDownPress) { this->m_Flags |= (1 << 10); }
			if (pAction1) { this->m_Flags |= (1 << 11); }
			if (pAction2) { this->m_Flags |= (1 << 12); }
			if (pAction3) { this->m_Flags |= (1 << 13); }
			if (pAction4) { this->m_Flags |= (1 << 14); }
			if (pAction5) { this->m_Flags |= (1 << 15); }
			if (pAction6) { this->m_Flags |= (1 << 16); }
		}
		void			SetRadBuf(float pxRad, float pyRad) {
			this->m_xRad = pxRad;
			this->m_yRad = pyRad;
		}
		void			SetRadBuf(const VECTOR_ref& pRad) {
			this->m_xRad = pRad.x();
			this->m_yRad = pRad.y();
		}
		void			SetKeyInput(unsigned int pFlags) { this->m_Flags = pFlags; }
		const auto&		GetKeyInput(void) const noexcept { return this->m_Flags; }


		const auto&		GetAddxRad(void) const noexcept { return this->m_AddxRad; }
		const auto&		GetAddyRad(void) const noexcept { return this->m_AddyRad; }
		const auto&		GetxRad(void) const noexcept { return this->m_xRad; }
		const auto&		GetyRad(void) const noexcept { return this->m_yRad; }
		const auto	GetGoFrontPress(void) const noexcept { return (this->m_Flags & (1 << 0)) != 0; }
		const auto	GetGoBackPress(void) const noexcept { return (this->m_Flags & (1 << 1)) != 0; }
		const auto	GetGoLeftPress(void) const noexcept { return (this->m_Flags & (1 << 2)) != 0; }
		const auto	GetGoRightPress(void) const noexcept { return (this->m_Flags & (1 << 3)) != 0; }
		const auto	GetRunPress(void) const noexcept { return (this->m_Flags & (1 << 4)) != 0; }
		const auto	GetQPress(void) const noexcept { return (this->m_Flags & (1 << 5)) != 0; }
		const auto	GetEPress(void) const noexcept { return (this->m_Flags & (1 << 6)) != 0; }
		const auto	GetRightPress(void) const noexcept { return (this->m_Flags & (1 << 7)) != 0; }
		const auto	GetLeftPress(void) const noexcept { return (this->m_Flags & (1 << 8)) != 0; }
		const auto	GetUpPress(void) const noexcept { return (this->m_Flags & (1 << 9)) != 0; }
		const auto	GetDownPress(void) const noexcept { return (this->m_Flags & (1 << 10)) != 0; }
		const auto	GetAction1(void) const noexcept { return (this->m_Flags & (1 << 11)) != 0; }
		const auto	GetAction2(void) const noexcept { return (this->m_Flags & (1 << 12)) != 0; }
		const auto	GetAction3(void) const noexcept { return (this->m_Flags & (1 << 13)) != 0; }
		const auto	GetAction4(void) const noexcept { return (this->m_Flags & (1 << 14)) != 0; }
		const auto	GetAction5(void) const noexcept { return (this->m_Flags & (1 << 15)) != 0; }
		const auto	GetAction6(void) const noexcept { return (this->m_Flags & (1 << 16)) != 0; }

		const InputControl operator+(const InputControl& o) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad + o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad + o.m_AddyRad;
			tmp.m_xRad = this->m_xRad + o.m_xRad;
			tmp.m_yRad = this->m_yRad + o.m_yRad;
			tmp.m_Flags = this->m_Flags;

			return tmp;
		}
		const InputControl operator-(const InputControl& o) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad - o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad - o.m_AddyRad;
			tmp.m_xRad = this->m_xRad - o.m_xRad;
			tmp.m_yRad = this->m_yRad - o.m_yRad;
			tmp.m_Flags = this->m_Flags;

			return tmp;
		}
		const InputControl operator*(float per) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad *per;
			tmp.m_AddyRad = this->m_AddyRad *per;
			tmp.m_xRad = this->m_xRad *per;
			tmp.m_yRad = this->m_yRad *per;
			tmp.m_Flags = this->m_Flags;

			return tmp;
		}

	};

	// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_MIN_SIZE		(0.1f * Scale_Rate)		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_ENUM_DEFAULT_SIZE	(1.6f * Scale_Rate)		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			(0.45f * Scale_Rate)		// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			(16)					// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * Scale_Rate)	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
	//�ǔ��胆�j�o�[�T��
	static bool col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos, const std::vector<std::pair<MV1*, int>>& col_obj_t) noexcept {
		if (col_obj_t.size() == 0) { return false; }
		auto MoveVector = *NowPos - OldPos;
		//MoveVector.y(0);
		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
		std::vector<MV1_COLL_RESULT_POLY> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
		for (const auto& objs : col_obj_t) {
			if ((&objs - &col_obj_t.front()) != 0) {
				if (GetMinLenSegmentToPoint(OldPos, OldPos + VECTOR_ref::up(), objs.first->GetMatrix().pos()) >=
					(20.f*Scale_Rate + PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size())) {
					continue;
				}
			}
			auto HitDim = objs.first->CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), objs.second);
			// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
			for (int i = 0; i < HitDim.HitNum; ++i) {
				auto& h_d = HitDim.Dim[i];
				//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{PLAYER_ENUM_MIN_SIZE��荂���|���S���̂ݓ����蔻����s��
				if (
					(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
					&& (h_d.Position[0].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE || h_d.Position[1].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE || h_d.Position[2].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE)
					&& (h_d.Position[0].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[1].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[2].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE)
					) {
					kabe_.emplace_back(h_d);// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
				}
			}
			MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������
		}
		bool HitFlag = false;
		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (kabe_.size() > 0) {
			HitFlag = false;
			for (auto& h_d : kabe_) {
				if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					if (MoveVector.size() >= 0.001f) {	// x����z�������� 0.001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
						// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
						*NowPos = VECTOR_ref(h_d.Normal).cross(MoveVector.cross(h_d.Normal)) + OldPos;
						//NowPos->y(OldPos.y());
						bool j = false;
						for (auto& h_d2 : kabe_) {
							if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {
								j = true;
								break;// �������Ă����烋�[�v���甲����
							}
						}
						if (!j) {
							HitFlag = false;
							break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
						}
					}
					else {
						break;
					}
				}
			}
			if (HitFlag) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
				for (int k = 0; k < PLAYER_HIT_TRYNUM; ++k) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
					bool HitF = false;
					for (auto& h_d : kabe_) {
						if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
							*NowPos += VECTOR_ref(h_d.Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
							//NowPos->y(OldPos.y());
							bool j = false;
							for (auto& h_d2 : kabe_) {
								if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {// �������Ă����烋�[�v�𔲂���
									j = true;
									break;
								}
							}
							if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
								break;
							}
							HitF = true;
						}
					}
					if (!HitF) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
						break;
					}
				}
			}
			kabe_.clear();
		}
		return HitFlag;
	}

	//�_���[�W�C�x���g
	struct DamageEvent {
		PlayerID				ShotID{ 127 };
		PlayerID				DamageID{ 127 };
		Sceneclass::ObjType		CharaType{ Sceneclass::ObjType::Human };
		HitPoint				Damage{ 0 };
		float					rad{ 0.f };
		void SetEvent(PlayerID ShotID_t, PlayerID DamageID_t, Sceneclass::ObjType pCharaType, HitPoint pDamage, float pRad) {
			this->ShotID = ShotID_t;
			this->DamageID = DamageID_t;
			this->CharaType = pCharaType;
			this->Damage = pDamage;
			this->rad = pRad;
		}
	};


	//�C���X�^�V���O
	class Model_Instance {
	private:
		int						m_Count{ 0 };			//��
		std::vector<VERTEX3D>	m_Vertex;				//
		std::vector<DWORD>		m_Index;				//
		int						m_VerBuf{ -1 };			//
		int						m_IndexBuf{ -1 };		//
		MV1						m_obj;					//���f��
		GraphHandle				m_pic;					//�摜�n���h��
		int						m_vnum{ -1 };			//
		int						m_pnum{ -1 };			//
		MV1_REF_POLYGONLIST		m_RefMesh{};			//
		int						m_Mesh{ 0 };			//
	private:
		void			Init_one(void) noexcept {
			MV1RefreshReferenceMesh(this->m_obj.get(), -1, TRUE, FALSE, m_Mesh);				//�Q�Ɨp���b�V���̍X�V
			this->m_RefMesh = MV1GetReferenceMesh(this->m_obj.get(), -1, TRUE, FALSE, m_Mesh);	//�Q�Ɨp���b�V���̎擾
		}
	public:
		//���Z�b�g
		void			Reset(void) noexcept {
			this->m_Count = 0;
			this->m_vnum = 0;
			this->m_pnum = 0;
			this->m_Vertex.clear();								//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			this->m_Vertex.reserve(2000);						//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			this->m_Index.clear();								//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			this->m_Index.reserve(2000);						//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
		}
		void			Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			this->m_Count++;
			Set_start(this->m_Count);
			float asize = 200.f * caliber;
			const auto& y_vec = Normal;
			auto z_vec = y_vec.cross(Zvec).Norm();
			auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
			MATRIX_ref mat = MATRIX_ref::GetScale(scale) * MATRIX_ref::Axis1_YZ(y_vec, z_vec) * MATRIX_ref::Mtrans(Position + y_vec * 0.02f);
			Set_one(mat);
		}
		void			Set_start(int value) noexcept {
			this->m_Count = value;
			int Num = this->m_RefMesh.VertexNum * this->m_Count;
			this->m_Vertex.resize(Num);			//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
			Num = this->m_RefMesh.PolygonNum * 3 * this->m_Count;
			this->m_Index.resize(Num);		//���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��
		}
		void			Set_one(const MATRIX_ref& mat) noexcept {
			this->m_obj.SetMatrix(mat);
			Init_one();
			for (size_t j = 0; j < size_t(this->m_RefMesh.VertexNum); ++j) {
				auto& g = this->m_Vertex[j + this->m_vnum];
				const auto& r = this->m_RefMesh.Vertexs[j];
				g.pos = r.Position;
				g.norm = r.Normal;
				g.dif = r.DiffuseColor;
				g.spc = r.SpecularColor;
				g.u = r.TexCoord[0].u;
				g.v = r.TexCoord[0].v;
				g.su = r.TexCoord[1].u;
				g.sv = r.TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(this->m_RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(this->m_RefMesh.Polygons[j].VIndex); ++k) {
					this->m_Index[j * 3 + k + this->m_pnum] = WORD(this->m_RefMesh.Polygons[j].VIndex[k] + this->m_vnum);
				}
			}
			this->m_vnum += this->m_RefMesh.VertexNum;
			this->m_pnum += this->m_RefMesh.PolygonNum * 3;
		}
	public:
		void			Init(MV1& mv1path, int MeshNum) noexcept {
			SetUseASyncLoadFlag(FALSE);
			this->m_Mesh = MeshNum;
			auto path = MV1GetTextureGraphHandle(mv1path.get(), MV1GetMaterialDifMapTexture(mv1path.get(), MV1GetMeshMaterial(mv1path.get(), m_Mesh)));
			this->m_pic = path;								 //grass
			this->m_obj = mv1path.Duplicate();				//�e��
			Init_one();
		}
		void			Init(std::string pngpath, std::string mv1path, int MeshNum) noexcept {
			SetUseASyncLoadFlag(FALSE);
			this->m_Mesh = MeshNum;
			this->m_pic = GraphHandle::Load(pngpath);		 //grass
			MV1::Load(mv1path, &this->m_obj, DX_LOADMODEL_PHYSICS_DISABLE);				//�e��
			Init_one();
		}
		void			Execute(void) noexcept {
			this->m_VerBuf = CreateVertexBuffer((int)this->m_Vertex.size(), DX_VERTEX_TYPE_NORMAL_3D);
			this->m_IndexBuf = CreateIndexBuffer((int)this->m_Index.size(), DX_INDEX_TYPE_32BIT);
			SetVertexBufferData(0, this->m_Vertex.data(), (int)this->m_Vertex.size(), this->m_VerBuf);
			SetIndexBufferData(0, this->m_Index.data(), (int)this->m_Index.size(), this->m_IndexBuf);
		}
		void			Draw(void) noexcept {
			//SetDrawAlphaTest(DX_CMP_GREATER, 128);
			DrawPolygonIndexed3D_UseVertexBuffer(this->m_VerBuf, this->m_IndexBuf, this->m_pic.get(), TRUE);
			//SetDrawAlphaTest(-1, 0);
		}
		void			Dispose(void) noexcept {
			this->m_Vertex.clear();
			this->m_Index.clear();
			this->m_obj.Dispose();
			this->m_pic.Dispose();
		}
	};
	//������
	class HIT_PASSIVE {
	private:
		Model_Instance	m_inst;
		bool			m_IsUpdate{ true };
	public:
		//������
		void			Init(void) noexcept {
			this->m_inst.Init("data/m_obj/hit/hit.png", "data/m_obj/hit/m_obj.mv1", -1);
		}
		//����̃��Z�b�g
		void			Clear(void) noexcept {
			this->m_inst.Reset();
		}

		void			Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			this->m_inst.Set(caliber, Position, Normal, Zvec);
			this->m_IsUpdate = true;
		}
		void			Execute(void) noexcept {
			if (this->m_IsUpdate) {
				this->m_IsUpdate = false;
				this->m_inst.Execute();
			}
		}
		void			Draw(void) noexcept {
			this->m_inst.Draw();
		}
	};

	static VECTOR_ref GetScreenPos(const VECTOR_ref&campos, const VECTOR_ref&camvec, const VECTOR_ref&camup, float fov, const VECTOR_ref&worldpos) noexcept {
		int ScrX = y_r(1920);
		int ScrY = y_r(1080);
		// �r���[�s��Ǝˉe�s��̎擾
		MATRIX mat_view;					// �r���[�s��
		VECTOR vec_from = campos.get();		// �J�����̈ʒu
		VECTOR vec_lookat = camvec.get();   // �J�����̒����_
		VECTOR vec_up = camup.get();        // �J�����̏����
		CreateLookAtMatrix(&mat_view, &vec_from, &vec_lookat, &vec_up);
		SetCameraNearFar(0.f, 100.f*Scale_Rate);
		SetupCamera_Perspective(fov);
		MATRIX proj = GetCameraProjectionMatrix();
		// �r���[�|�[�g�s��i�X�N���[���s��j�̍쐬
		float w = (float)ScrX / 2.0f;
		float h = (float)ScrY / 2.0f;
		MATRIX viewport = {
			w , 0 , 0 , 0 ,
			0 ,-h , 0 , 0 ,
			0 , 0 , 1 , 0 ,
			w , h , 0 , 1
		};
		VECTOR screenPos, tmp = worldpos.get();
		// �r���[�ϊ��ƃv���W�F�N�V�����ϊ�
		tmp = VTransform(tmp, mat_view);
		tmp = VTransform(tmp, proj);
		// z�Ŋ�����-1~1�͈̔͂Ɏ��߂�
		tmp.x /= tmp.z; tmp.y /= tmp.z; tmp.z /= tmp.z;
		// �X�N���[���ϊ�
		screenPos = VTransform(tmp, viewport);
		screenPos.z = -1.f;
		if ((camvec - campos).dot(worldpos - campos) > 0.f) {
			screenPos.z = 0.5f;
		}
		return screenPos;
	}


	typedef std::pair<std::string, int> SaveParam;
	class SaveDataClass : public SingletonBase<SaveDataClass> {
	private:
		friend class SingletonBase<SaveDataClass>;
	private:
		std::vector<SaveParam> m_data;
	private:
		SaveDataClass() {
			Load();
		}
		~SaveDataClass() {}
	public:
		SaveParam* GetData(std::string_view Name) noexcept {
			for (auto& d : m_data) {
				if (d.first == Name) {
					return &d;
				}
			}
			return nullptr;
		}
	public:
		void SetParam(std::string_view Name, int value) noexcept {
			auto* Data = GetData(Name);
			if (Data) {
				Data->second = value;
			}
			else {
				m_data.emplace_back(std::make_pair((std::string)Name, value));
			}
		}
		auto GetParam(std::string_view Name) noexcept {
			auto* Data = GetData(Name);
			if (Data) {
				return Data->second;
			}
			return -1;
		}
	public:
		void Save() noexcept {
			std::ofstream outputfile("data/Plane/bokuzyo.ok");
			for (auto& d : m_data) {
				outputfile << d.first + "=" + std::to_string(d.second) + "\n";
			}
			outputfile.close();
		}
		void Load() noexcept {

			m_data.clear();

			std::ifstream inputputfile("data/Plane/bokuzyo.ok");
			std::string line;
			while (std::getline(inputputfile, line)) {
				auto Start = line.find("=");
				if (Start != std::string::npos) {
					m_data.emplace_back(std::make_pair(line.substr(0, Start), std::stoi(line.substr(Start + 1))));
				}
			}
			inputputfile.close();

		}
	};

	template <typename... Args>
	static void DrawFetteString(int xp1, int yp1, float per, bool IsSelect, const std::string& String, Args&&... args) noexcept {
		auto* Fonts = FontPool::Instance();
		auto Red = GetColor(255, 0, 0);
		auto Red75 = GetColor(192, 0, 0);
		auto White = GetColor(255, 255, 255);
		auto Gray75 = GetColor(128, 128, 128);
		auto Gray = GetColor(64, 64, 64);

		Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r((int)((float)(48 * 3 / 2 * 3 / 4) * per)), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40.f*per), yp1 + y_r(20.f*per), IsSelect ? Red75 : Gray75, Gray, String, args...);
		Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r((int)((float)(48 * 2 * 3 / 4) * per)), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? Red : White, Gray, String, args...);
	}

	class Pendulum2D {
		float	m_PendulumLength = 10.f;
		float	m_PendulumMass = 2.f;
		float	m_drag_coeff = 2.02f;

		float	m_rad = deg2rad(12.f);
		float	m_vel = 0.f;
	public:
		void Init(float Length, float N, float rad) {
			m_PendulumLength = Length;
			m_PendulumMass = N;
			m_rad = rad;
			m_vel = 0.f;
		}

		void Execute() {
			m_vel += (-9.8f / this->m_PendulumLength * std::sin(m_rad) - this->m_drag_coeff / this->m_PendulumMass * this->m_vel) / 60.f;
			m_rad += this->m_vel / 60.f;
		}

		const auto GetRad() const noexcept { return this->m_rad; }

		void AddRad(float value) noexcept { this->m_rad += value; }
	};

	namespace Sceneclass {
	};






	//
	static float GameSpeed = 1.0f;

	static const auto GetMocroSec(void) noexcept {
		return GetNowHiPerformanceCount() * (int)(GameSpeed*1000.f) / 1000;
	}

	template <class T>
	static void easing_set_SetSpeed(T* first, const T& aim, const float& ratio) noexcept {
		if (ratio == 0.f) {
			*first = aim;
		}
		else {
			*first = *first + (aim - *first) * (1.f - std::powf(ratio, 60.f / FPS * GameSpeed));
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
		//�X�N���v�g�ǂݍ��ݏ���
		void LoadScript(std::string_view func_t) noexcept {
			m_Args.clear();
			m_Func = func_t;
			{
				// //���폜
				size_t sls = m_Func.find("//");
				if (sls != std::string::npos) { m_Func = m_Func.substr(0, sls); }
				//����Ȃ��v�f��r��
				Sub_Func(m_Func, '{');
				Sub_Func(m_Func, '}');
				Sub_Func(m_Func, ' ');
				Sub_Func(m_Func, '\t');
				Sub_Func(m_Func, ';');
				Sub_Func(m_Func, '\"');
			}
			//()��,�ň͂�ꂽ��������������擾
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
		//����
		bool SetArgs(std::vector<std::string>* NAMES) noexcept {
			//�ϐ��o�^
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
			//�ϐ��ϊ�����
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
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
						break;
					case 1:
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
						break;
					case 2:
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
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
	//�ڍ�
	class CutinfoDetail {
	public:
		CutInfoClass Blur;
		int animsel = -1;
		bool isloop{ false };
		float animspeed{ 0.f };
		float animspeed_Dist{ -1.f };
		float animspeed_Per{ -1.f };


		MATRIX_ref mat_p;
		//matrix�p���
		VECTOR_ref pos_p;
		float Yrad1_p;
		float Yrad2_p;
		bool usemat{ false };
		float startframe{ 0.f };
		float PhysicsSpeed_{ -1.f };
		float OpacityRate{ -1.f };
		float OpacityRate_Dist{ -1.f };
		float OpacityRate_Per{ -1.f };
	};
	class CutAttachDetail {
	public:
		VECTOR_ref poscam;
	};
	//���f���R���g���[��
	class ModelControl {
	public:
		class Model {
			float b_run{ 0.f };
			float b_runrange{ 0.f };
			std::vector<std::pair<int, std::string>> FrameNum;
			moves move;
			int Anim_Sel = 0;
			int DrawCount;
			MATRIX_ref mat_p;			//matrix�p���
			VECTOR_ref pos_p;
			float Yrad1_p;
			float Zrad1_p;
			float Yrad2_p;
			int prevID = -1;
			float AnimChange{ 0.f };
		public:
			bool isBGModel{ false };
			bool ShadowDrawActive{ true };//�e���������ނ�
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
			std::vector<CutinfoDetail> CutDetail;//�J�b�g�̏ڍ�
			bool isDraw{ false };
			bool isEndLoad{ false };
			bool isBigModel{ false };
			bool isFarPhysics{ false };
			bool isOutFrustum{ false };
			bool canUpdate{ true };
			float animspd;
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
			const auto GetFrameMat(std::string_view FrameName) const noexcept {
				for (auto& F : FrameNum) {
					if (F.second == FrameName) {
						return obj.GetFrameLocalWorldMatrix(F.first);
					}
				}
				return MATRIX_ref::zero();
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
					//this->obj.get_anime(ID).time = this->obj.get_anime(prevID).time;
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
				if (this->animspd_Per < 1.f) {
					easing_set_SetSpeed(&this->animspd, this->animspd_Dist, this->animspd_Per);
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
							//�ŏ��̃A�j���[�V��������
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
							if (inf.animspeed_Dist >= 0.f) {
								m.animspd_Dist = inf.animspeed_Dist;
							}
							if (inf.animspeed_Per >= 0.f) {
								m.animspd_Per = inf.animspeed_Per;
							}
						}
						//�A�j���[�V��������
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
					m.AddFrame("�㔼�g");
					m.AddFrame("�㔼�g2");
					m.AddFrame("��");
					m.AddFrame("��");
					m.AddFrame("�E��");
					m.AddFrame("����");
					m.AddFrame("�E�l�w�Q");
					m.AddFrame("�E�Ђ�");
					m.AddFrame("���l�w�Q");
					m.AddFrame("�E�ܐ�");
					m.AddFrame("���ܐ�");

					m.AddFrame("����߰");
					m.AddFrame("�O��]");
					m.AddFrame("�e����");

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
			//�J������far���K���͈͂̐ݒ�̏ꍇ
			if (far_ - 1.f < camfar&& camfar < far_ + 1.f) {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					m.isOutFrustum = false;
					m.isBigModel = false;
					auto center = m.GetFrame("�㔼�g");
					if (center != VECTOR_ref::zero()) {
						//center += (center - GetCameraPosition()).Norm()*-7.f;


						if (CheckCameraViewClip_Box(
							(center - VECTOR_ref::vget(-10.f, -10.f, -10.f)).get(),
							(center - VECTOR_ref::vget(10.f, 10.f, 10.f)).get()
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
			std::vector<CutinfoDetail> CutDetail;//�I���ɂ���J�b�g
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
						//�u���[����
						inf.Blur.Update_(Counter);
						m.isblur = inf.Blur.GetSwitch();

						if (inf.OpacityRate_Dist >= 0.f) {
							if (inf.OpacityRate_Per == 0) {
								if (isFirstLoop && m.Cutinfo.isFirstCut) {
									//�ŏ��̃A�j���[�V��������
									m.Alpha.Set(inf.OpacityRate_Dist, inf.OpacityRate_Per);
								}
							}
							//0�ȊO�͂��[����
							else  if (inf.OpacityRate_Per >= 0.f) {
								m.Alpha.Set(inf.OpacityRate_Dist, inf.OpacityRate_Per);
							}
						}
						//�A�j���[�V��������
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
	public:
		void SetUse(bool value) noexcept { Use = value; }
		const auto& GetIsUse(void) const noexcept { return Use; }
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
				this->Frame = "�㔼�g";
			}
			else if (frame == "UPPER_2") {
				this->Frame = "�㔼�g2";
			}
			else if (frame == "NECK") {
				this->Frame = "��";
			}
			else if (frame == "HEAD") {
				this->Frame = "��";
			}

			else if (frame == "LEFT_EYE") {
				this->Frame = "�E��";
			}
			else if (frame == "RIGHT_EYE") {
				this->Frame = "����";
			}
			else if (frame == "RIGHT_FINGER") {
				this->Frame = "�E�l�w�Q";
			}
			else if (frame == "LEFT_FINGER") {
				this->Frame = "���l�w�Q";
			}
			else if (frame == "RIGHTLEG") {
				this->Frame = "�E�Ђ�";
			}
			else if (frame == "RIGHTFOOT") {
				this->Frame = "�E�ܐ�";
			}
			else if (frame == "LEFTFOOT") {
				this->Frame = "���ܐ�";
			}

			else if (frame == "CANOPY") {
				this->Frame = "����߰";
			}
			else if (frame == "FRONTWHEEL") {
				this->Frame = "�O��]";
			}
			else if (frame == "MOVE") {
				this->Frame = "�ړ�";
			}
			else if (frame == "MUZZLE") {
				this->Frame = "�e����";
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
		int bright[3]{ -1,-1,-1 };
		int fog[3]{ -1,-1,-1 };
		float fog_range[2]{ -1.f,-1.f };
		std::vector<ForcusControl> Forcus;
	public:
		//Getter
		const auto& GetTimeLimit(void) const noexcept { return TimeLimit; }
		void SetTimeLimit(LONGLONG value) noexcept { TimeLimit = value; }
		//
		Cut_Info_First(void) noexcept {
			Aim_camera.SetCamPos(VECTOR_ref::vget(0, 10, -30), VECTOR_ref::vget(0, 10, 0), VECTOR_ref::up());
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
					VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
					this->Aim_camera.GetCamVec(),
					this->Aim_camera.GetCamUp()
				);
				return true;
			}
			//Camvec
			else if (func.find("SetCamvec") != std::string::npos) {
				this->Aim_camera.SetCamPos(
					this->Aim_camera.GetCamPos(),
					VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
					this->Aim_camera.GetCamUp()
				);
				return true;
			}
			else if (func.find("SetCamForcus") != std::string::npos) {
				this->Forcus.resize(this->Forcus.size() + 1);
				this->Forcus.back().Set(args[0], std::stol(args[1]), args[2], VECTOR_ref::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5])));
			}
			//CamUp
			else if (func.find("SetCamup") != std::string::npos) {
				this->Aim_camera.SetCamPos(
					this->Aim_camera.GetCamPos(),
					this->Aim_camera.GetCamVec(),
					VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]))
				);
				return true;
			}
			//Else
			else if (func.find("SetCamInfo") != std::string::npos) {
				this->Aim_camera.SetCamInfo(deg2rad(std::stof(args[0])), std::stof(args[1]), std::stof(args[2]));
				this->cam_per = std::stof(args[3]);
			}
			//���������Ń��Z�b�g����
			else if (func.find("SetNextResetPhysics") != std::string::npos) {
				this->IsResetPhysics = true;
			}
			//�O�̃J�b�g�J�������g�p
			else if (func.find("SetPrevCamInfo") != std::string::npos) {
				this->UsePrevAim = true;
			}
			//�ǂ̋����ŕ`�悷�邩���Z�b�g
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
		VECTOR_ref m_RandcamupSet;
		float m_RandcamvecPer;
		VECTOR_ref m_RandcamvecSet;
		float m_RandcamposPer;
		VECTOR_ref m_RandcamposSet;
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
			//�J�����̃A�b�v�f�[�g
			if (func.find("SetUpdateEnable") != std::string::npos) {
				this->isUseNotFirst = true;
			}
			//camvec
			else if (func.find("SetUpdateCamvec") != std::string::npos) {
				this->camvec_per = std::stof(args[0]);
				if (args.size() > 1) {
					auto pos_t = this->CameraNotFirst.GetCamPos();
					auto vec_t = this->CameraNotFirst.GetCamVec();
					auto up_t = this->CameraNotFirst.GetCamUp();
					this->CameraNotFirst.SetCamPos(
						pos_t,
						VECTOR_ref::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])),
						up_t);
				}
			}
			else if (func.find("SetUpdateCamForcus") != std::string::npos) {
				this->SetForce(std::stof(args[0]), args[1], std::stoi(args[2]), args[3], VECTOR_ref::vget(std::stof(args[4]), std::stof(args[5]), std::stof(args[6])));
			}
			else if (func.find("SetVectorUpdateCamvec") != std::string::npos) {
				auto pos_t = this->CameraNotFirst_Vec.GetCamPos();
				auto vec_t = this->CameraNotFirst_Vec.GetCamVec();
				auto up_t = this->CameraNotFirst_Vec.GetCamUp();
				this->CameraNotFirst_Vec.SetCamPos(
					pos_t,
					VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
					up_t);
			}
			//campos
			else if (func.find("SetUpdateCampos") != std::string::npos) {
				this->campos_per = std::stof(args[0]);
				if (args.size() > 1) {
					auto pos_t = this->CameraNotFirst.GetCamPos();
					auto vec_t = this->CameraNotFirst.GetCamVec();
					auto up_t = this->CameraNotFirst.GetCamUp();
					this->CameraNotFirst.SetCamPos(
						VECTOR_ref::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])),
						vec_t,
						up_t);
				}
			}
			else if (func.find("SetVectorUpdateCampos") != std::string::npos) {
				auto pos_t = this->CameraNotFirst_Vec.GetCamPos();
				auto vec_t = this->CameraNotFirst_Vec.GetCamVec();
				auto up_t = this->CameraNotFirst_Vec.GetCamUp();
				this->CameraNotFirst_Vec.SetCamPos(
					VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
					vec_t,
					up_t);
			}
			//camup
			else if (func.find("SetUpdateCamup") != std::string::npos) {
				this->camup_per = std::stof(args[0]);
				if (args.size() > 1) {
					auto pos_t = this->CameraNotFirst.GetCamPos();
					auto vec_t = this->CameraNotFirst.GetCamVec();
					auto up_t = this->CameraNotFirst.GetCamUp();
					this->CameraNotFirst.SetCamPos(
						pos_t,
						vec_t,
						VECTOR_ref::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])));
				}
			}
			else if (func.find("SetVectorUpdateCamup") != std::string::npos) {
				auto pos_t = this->CameraNotFirst_Vec.GetCamPos();
				auto vec_t = this->CameraNotFirst_Vec.GetCamVec();
				auto up_t = this->CameraNotFirst_Vec.GetCamUp();
				this->CameraNotFirst_Vec.SetCamPos(
					pos_t,
					vec_t,
					VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])));
			}
			//fov
			else if (func.find("SetUpdateCamfov") != std::string::npos) {
				this->fov_per = std::stof(args[0]);
				auto near_t = this->CameraNotFirst.GetCamNear();
				auto far_t = this->CameraNotFirst.GetCamFar();
				this->CameraNotFirst.SetCamInfo(deg2rad(std::stof(args[1])), near_t, far_t);
			}
			//easing
			else if (func.find("SetUpdatePer") != std::string::npos) {
				this->NotFirst_per = std::stof(args[0]);
			}
			//�O�̃J�����A�b�v�f�[�g���g�p
			else if (func.find("SetPrevCamUpdate") != std::string::npos) {
				this->IsUsePrevBuf = true;
			}
			//�A�b�v�f�[�g�J�����Ƀ����_�����w��
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
#if 1
				for (auto&f : Forcus) {
					if (f.GetIsUse()) {
						vec += f.GetForce(models);
						isforcus = true;
					}
				}
				if (isforcus) {
					auto pos_t = this->CameraNotFirst.GetCamPos();
					auto vec_t = this->CameraNotFirst.GetCamVec();
					auto up_t = this->CameraNotFirst.GetCamUp();
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
					auto pos_t = Camera.Aim_camera.GetCamPos();
					auto vec_t = Camera.Aim_camera.GetCamVec();
					auto up_t = Camera.Aim_camera.GetCamUp();
					auto fov_t = Camera.Aim_camera.GetCamFov();
					auto near_t = Camera.Aim_camera.GetCamNear();
					auto far_t = Camera.Aim_camera.GetCamFar();

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
	//kusa
	class Grass {
		//�C���X�^�V���O
		class Model_Instance {
		public:
			int hitss = 0;					/*hits�̐�*/
			std::vector<VERTEX3D> hitsver;	/*hits*/
			std::vector<DWORD> hitsind;	    /*hits*/
			int VerBuf = -1, IndexBuf = -1;	/*hits*/
			MV1 hits;						/*hits���f��*/
			GraphHandle hits_pic;			/*�摜�n���h��*/
			int IndexNum = -1, VerNum = -1;	/*hits*/
			int vnum = -1, pnum = -1;		/*hits*/
			MV1_REF_POLYGONLIST RefMesh{};	/*hits*/
		public:
			//������
			void Init(std::string pngpath, std::string mv1path) noexcept {
				SetUseASyncLoadFlag(FALSE);
				this->hits_pic = GraphHandle::Load(pngpath);		 /*grass*/
				MV1::Load(mv1path, &this->hits, false);	//�e��
				Init_one();
			}
			void Init_one(void) noexcept {
				MV1RefreshReferenceMesh(this->hits.get(), -1, TRUE);			/*�Q�Ɨp���b�V���̍X�V*/
				this->RefMesh = MV1GetReferenceMesh(this->hits.get(), -1, TRUE);	/*�Q�Ɨp���b�V���̎擾*/
			}
			//����̃��Z�b�g
			void Clear(void) noexcept {
				this->hitss = 0;
				this->vnum = 0;
				this->pnum = 0;
				this->hitsver.clear();								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
				this->hitsind.clear();								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
				this->hitsver.reserve(2000);							/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
				this->hitsind.reserve(2000);							/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
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
				this->IndexNum = this->RefMesh.PolygonNum * 3 * this->hitss;				/*�C���f�b�N�X�̐����擾*/
				this->VerNum = this->RefMesh.VertexNum * this->hitss;						/*���_�̐����擾*/
				this->hitsver.resize(this->VerNum);									/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
				this->hitsind.resize(this->IndexNum);								/*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
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
			/*���E�O���ۂ��𔻒f*/
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
		const int grasss = 50 * 50;						/*grass�̐�*/
		std::array<grass_t, grassDiv>grass__;
		std::array<VECTOR_ref, grassDiv>grassPosMin;
		std::array<VECTOR_ref, grassDiv>grassPosMax;
		int grasss2 = 30 * 30;							/*grass�̐�*/
		std::array<grass_t, grassDiv>grass2__;
		int grasss3 = 12 * 12;							/*grass�̐�*/
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
		//y�����ɑ��삷��O��
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
				//�|�W�V��������
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
		void Draw(Camera3DInfo camera_buf) noexcept {
			SetFogEnable(TRUE);
			SetFogStartEnd(camera_buf.GetCamNear()*3.f, camera_buf.GetCamFar()*3.f);
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
