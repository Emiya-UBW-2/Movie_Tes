#pragma once
#include"Header.hpp"

// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			0.4f	// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			1.8f	// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			16		// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��

namespace FPS_n2 {
	class MAPclass {
	public:
		static const int grassDiv = 1;//6;

		class Map;
		class MiniMap;

		static VECTOR_ref Get_Chunk(int x_, int z_, int x_t, int z_t, bool ismini) {
			int pos_x = 0, pos_z = 0;
			if (ismini) {
				pos_x = 100 * x_ / grassDiv - 50;
				pos_z = 100 * z_ / grassDiv - 50;
			}
			else {
				pos_x = 100 * (x_ + 1) / grassDiv - 50;
				pos_z = 100 * (z_ + 1) / grassDiv - 50;
			}
			return VECTOR_ref::vget((float)(x_t * pos_x) / 100.0f, 0.f, (float)(z_t * pos_z) / 100.0f);
		}

		class Map : public std::enable_shared_from_this<Map> {
		private:
			std::string path;
			MV1 map, map_col, mapcol_tank;		//�n��
			MV1 sky;							//��
			std::vector<VECTOR_ref> way_point;
			std::vector<VECTOR_ref> lean_point_q;
			std::vector<VECTOR_ref> lean_point_e;
			std::vector<VECTOR_ref> spawn_point;
			int grass_pos = -1;
			GraphHandle minmap;
			int x_size = 0;
			int y_size = 0;
			//���z
			GraphHandle sun_pic;				/*�摜�n���h��*/
			VECTOR_ref sun_pos;
			//kusa
			int grasss = 2000;					/*grass�̐�*/

			shaders shader;						/*�V�F�[�_�[*/
			shaders Depth;						/*�V�F�[�_�[*/

			class grass_t {
			private:
				std::shared_ptr<Map> MAPPTs{ nullptr };
				bool canlook = true;

			public:
				Model_Instance inst;
			public:

				void Init(int total, const std::shared_ptr<Map>& MAPPTs_t) {
					MAPPTs = MAPPTs_t;

					this->inst.Init("data/model/grass/grass.png", "data/model/grass/model.mv1");

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

					MAPPTs.reset();
					this->inst.hitsver.clear();
					this->inst.hitsind.clear();

					this->inst.hits.Dispose();
					this->inst.hits_pic.Dispose();
				}

				/*���E�O���ۂ��𔻒f*/
				void Check_CameraViewClip(int x_, int z_, bool use_occlusion) noexcept {
					VECTOR_ref min = Get_Chunk(x_, z_, int(MAPPTs->x_max - MAPPTs->x_min), int(MAPPTs->z_max - MAPPTs->z_min), true);
					VECTOR_ref max = Get_Chunk(x_, z_, int(MAPPTs->x_max - MAPPTs->x_min), int(MAPPTs->z_max - MAPPTs->z_min), false);
					min.y(-5.f);
					max.y(5.f);

					this->canlook = true;

					float range = std::hypotf((MAPPTs->x_max - MAPPTs->x_min) / grassDiv, (MAPPTs->z_max - MAPPTs->z_min) / grassDiv);

					if ((min - GetCameraPosition()).size() > range && (max - GetCameraPosition()).size() > range) {
						this->canlook = false;
						return;
					}
					if (CheckCameraViewClip_Box(min.get(), max.get())) {
						this->canlook = false;
						return;
					}
					if (use_occlusion) {
						auto ttt = (max - min) * 0.5f + min;
						ttt.y(0.f);
						if (MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 1.8f, 0)).HitFlag == TRUE &&
							MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::vget(0, 0.f, 0)).HitFlag == TRUE) {
							this->canlook = false;
							return;
						}
					}
				}

				void Draw(void) noexcept {
					if (this->canlook) {
						this->inst.Draw();
					}
				}
			};

		public:
			bool isDispose = false;

			std::shared_ptr<b2World> world;

			GraphHandle DepthScreen;		// �[�x���擾����X�N���[��

			float x_max = 0.f;
			float z_max = 0.f;
			float x_min = 0.f;
			float z_min = 0.f;
			std::array<std::array<grass_t, grassDiv>, grassDiv>grass__;
			std::vector<std::shared_ptr<Items>> item;					//�A�C�e��

			struct wallPats {
				b2Pats b2;
				std::array<VECTOR_ref, 2> pos;
			};

			std::vector<wallPats> wall;					//�ǂ��Z�b�g
			int dispx = deskx;
			int dispy = desky;
		public:
			const std::vector<VECTOR_ref>& Get_waypoint(void) const noexcept { return way_point; }
			const std::vector<VECTOR_ref>& Get_leanpoint_q(void) const noexcept { return lean_point_q; }
			const std::vector<VECTOR_ref>& Get_leanpoint_e(void) const noexcept { return lean_point_e; }
			const std::vector<VECTOR_ref>& Get_spawn_point(void) const noexcept { return spawn_point; }
			GraphHandle& Get_minmap(void) noexcept { return minmap; }
			int& Get_x_size(void) noexcept { return x_size; }
			int& Get_y_size(void) noexcept { return y_size; }
			Map(int grass_level, int dispx_t, int dispy_t) noexcept {
				dispx = dispx_t;
				dispy = dispy_t;
				switch (grass_level) {
				case 0:
					grasss = 0;
					break;
				case 1:
					grasss = 1000 / (grassDiv * grassDiv);
					break;
				case 2:
					grasss = 2000 / (grassDiv * grassDiv);
					break;
				case 3:
					grasss = 4000 / (grassDiv * grassDiv);
					break;
				case 4:
					grasss = 8000 / (grassDiv * grassDiv);
					break;
				default:
					grasss = 0;
					break;
				}

				this->world = std::make_shared<b2World>(b2Vec2(0.0f, 0.0f)); /* ���̂�ێ�����уV�~�����[�g���郏�[���h�I�u�W�F�N�g���\�z*/
			}
			~Map(void) noexcept {
				Dispose();
			}
			void Ready_map(std::string dir) noexcept {
				this->path = dir + "/";
				MV1::Load(this->path + "model.mv1", &map, true);					//map
				MV1::Load(this->path + "col.mv1", &map_col, true);					//map�R���W����
				MV1::Load(this->path + "col_tank.mv1", &mapcol_tank, true);			//map�R���W����
				MV1::Load(this->path + "sky/model.mv1", &sky, true);				//��

				SetUseASyncLoadFlag(TRUE);
				minmap = GraphHandle::Load(this->path + "minimap.png");				/*minimap*/
				this->sun_pic = GraphHandle::Load("data/sun.png");					/*sun*/
				SetUseASyncLoadFlag(FALSE);

				// �[�x��`�悷��e�N�X�`���̍쐬( �P�`�����l�����������_�P�U�r�b�g�e�N�X�`�� )
				{
					SetCreateDrawValidGraphChannelNum(1);
					SetDrawValidFloatTypeGraphCreateFlag(TRUE);
					SetCreateGraphChannelBitDepth(16);
					DepthScreen = GraphHandle::Make(dispx, dispy, false);
					SetCreateDrawValidGraphChannelNum(0);
					SetDrawValidFloatTypeGraphCreateFlag(FALSE);
					SetCreateGraphChannelBitDepth(0);
				}
				shader.Init("NormalMesh_PointLightVS.vso", "NormalMesh_PointLightPS.pso");
				Depth.Init("DepthVS.vso", "DepthPS.pso");
			}
			void Start(void) noexcept {
				isDispose = true;
				//map.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
				VECTOR_ref size;
				{
					auto sizetmp = map_col.mesh_maxpos(0) - map_col.mesh_minpos(0);
					if (size.x() < sizetmp.x()) { size.x(sizetmp.x()); }
					if (size.y() < sizetmp.y()) { size.y(sizetmp.y()); }
					if (size.z() < sizetmp.z()) { size.z(sizetmp.z()); }
				}
				map_col.SetupCollInfo(std::max(int(size.x() / 5.f), 1), std::max(int(size.y() / 5.f), 1), std::max(int(size.z() / 5.f), 1), 0, 0);
				{
					MV1SetupReferenceMesh(map_col.get(), 0, FALSE);
					auto p = MV1GetReferenceMesh(map_col.get(), 0, FALSE);
					for (int i = 0; i < p.PolygonNum; ++i) {
						if (p.Polygons[i].MaterialIndex >= 1 && p.Polygons[i].MaterialIndex <= 4) {
							//
							auto points = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
							MV1_COLL_RESULT_POLY colres = map_col_line(points + VECTOR_ref::vget(0, 10.f, 0.f), points + VECTOR_ref::vget(0, -10.f, 0.f));
							if (colres.HitFlag == TRUE) { points = colres.HitPosition; }
							//
							switch (p.Polygons[i].MaterialIndex) {
							case 1:
							{
								way_point.emplace_back(points);
								break;
							}
							case 2:
							{
								lean_point_e.emplace_back(points);
								break;
							}
							case 3:
							{
								lean_point_q.emplace_back(points);
								break;
							}
							case 4:
							{
								spawn_point.emplace_back(points);
								break;
							}
							default: break;
							}
						}
					}
				}
				{
					MV1SetupReferenceMesh(mapcol_tank.get(), 0, FALSE);
					MV1_REF_POLYGONLIST p = MV1GetReferenceMesh(mapcol_tank.get(), 0, FALSE);

					for (int i = 0; i < p.PolygonNum; i++) {
						//��
						{
							wall.resize(wall.size() + 1);
							wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
							wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
							if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
								wall.pop_back();
							}

							wall.resize(wall.size() + 1);
							wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
							wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
							if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
								wall.pop_back();
							}

							wall.resize(wall.size() + 1);
							wall.back().pos[0] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
							wall.back().pos[1] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
							if (b2DistanceSquared(b2Vec2(wall.back().pos[0].x(), wall.back().pos[0].z()), b2Vec2(wall.back().pos[1].x(), wall.back().pos[1].z())) <= 0.005f * 0.005f) {
								wall.pop_back();
							}
						}
					}

					for (auto& w : wall) {
						std::array<b2Vec2, 2> vs;
						vs[0].Set(w.pos[0].x(), w.pos[0].z());
						vs[1].Set(w.pos[1].x(), w.pos[1].z());
						b2ChainShape chain;		// This a chain shape with isolated vertices
						chain.CreateChain(&vs[0], 2);
						b2FixtureDef fixtureDef;			       /*���I�{�f�B�t�B�N�X�`�����`���܂�*/
						fixtureDef.shape = &chain;			       /**/
						fixtureDef.density = 1.0f;			       /*�{�b�N�X���x���[���ȊO�ɐݒ肷��ƁA���I�ɂȂ�܂�*/
						fixtureDef.friction = 0.3f;			       /*�f�t�H���g�̖��C���I�[�o�[���C�h���܂�*/
						b2BodyDef bodyDef;				       /*�_�C�i�~�b�N�{�f�B���`���܂��B���̈ʒu��ݒ肵�A�{�f�B�t�@�N�g�����Ăяo���܂�*/
						bodyDef.type = b2_staticBody;			       /**/
						bodyDef.position.Set(0, 0);			       /**/
						bodyDef.angle = 0.f;				       /**/
						w.b2.Set(this->world->CreateBody(&bodyDef), &chain);
					}
				}
				SetFogStartEnd(40.0f - 15.f, 40.f);
				SetFogColor(12, 6, 0);
				item.clear();
				/*minimap*/
				minmap.GetSize(&x_size, &y_size);
				/*grass_*/
				if (grasss != 0) {
					int xs = 0, ys = 0;
					GetSoftImageSize(grass_pos, &xs, &ys);
					x_max = map_col.mesh_maxpos(0).x();
					z_max = map_col.mesh_maxpos(0).z();
					x_min = map_col.mesh_minpos(0).x();
					z_min = map_col.mesh_minpos(0).z();

					for (int x_ = 0; x_ < grassDiv; x_++) {
						for (int z_ = 0; z_ < grassDiv; z_++) {
							auto& tgt_g = grass__[x_][z_];
							tgt_g.Init(grasss, shared_from_this());
							for (int i = 0; i < grasss; ++i) {
								VECTOR_ref min = Get_Chunk(x_, z_, int(x_max - x_min), int(z_max - z_min), true);
								VECTOR_ref max = Get_Chunk(x_, z_, int(x_max - x_min), int(z_max - z_min), false);
								float x_t = (float)(GetRand(int(max.x() - min.x()) * 100) + int(min.x()) * 100) / 100.0f;
								float z_t = (float)(GetRand(int(max.z() - min.z()) * 100) + int(min.z()) * 100) / 100.0f;
								int _r_;
								int cnt = 0;
								while (true) {
									GetPixelSoftImage(grass_pos, int((x_t - x_min) / (x_max - x_min) * float(xs)), int((z_t - z_min) / (z_max - z_min) * float(ys)), &_r_, nullptr, nullptr, nullptr);
									if (_r_ <= 128) {
										break;
									}
									else {
										x_t = (float)(GetRand(int(max.x() - min.x()) * 100) + int(min.x()) * 100) / 100.0f;
										z_t = (float)(GetRand(int(max.z() - min.z()) * 100) + int(min.z()) * 100) / 100.0f;
									}
									cnt++;
									if (cnt >= 10) {
										break;
									}
								}
								auto tmpvect2 = VECTOR_ref::vget(x_t, -5.f, z_t);
								auto tmpvect = VECTOR_ref::vget(x_t, 5.f, z_t);
								map_col_nearest(tmpvect2, &tmpvect);
								//
								tgt_g.inst.hits.SetMatrix((MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(VECTOR_ref::vget(1.f, float(100 - 20 + GetRand(20 * 2)) / 100.f, 1.f))) * MATRIX_ref::Mtrans(tmpvect));
								//
								tgt_g.Set_one();
							}
							tgt_g.put();
						}
					}
					//
					DeleteSoftImage(grass_pos);
				}
			}
			void Start_Ray(const VECTOR_ref& ray) noexcept {
				this->sun_pos = ray.Norm() * -1500.f;
			}
			void Set(void) noexcept {
				SE.Get(EnumSound::MAP0_ENVI).Play(0, DX_PLAYTYPE_LOOP, TRUE);
			}
			void Dispose(void) noexcept {
				if (isDispose) {
					isDispose = false;
					for (auto& i : item) { i->Detach_item(); }
					item.clear();
					map.Dispose();		   //map
					map_col.Dispose();		   //map�R���W����
					mapcol_tank.Dispose();
					for (auto& w : wall) { w.b2.Dispose(); }
					wall.clear();
					sky.Dispose();	 //��
					way_point.clear();
					lean_point_q.clear();
					lean_point_e.clear();
					spawn_point.clear();
					minmap.Dispose();
					this->sun_pic.Dispose();
					if (grasss != 0) {
						for (int x_ = 0; x_ < grassDiv; x_++) {
							for (int z_ = 0; z_ < grassDiv; z_++) {
								auto& tgt_g = grass__[x_][z_];
								tgt_g.Dispose();
							}
						}
					}
				}
			}
			auto& map_col_get(void) const noexcept { return map_col; }
			const MV1_COLL_RESULT_POLY map_col_line(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos) const noexcept {
				return map_col.CollCheck_Line(StartPos, EndPos, 0, 0);
			}
			//�֐��I�u�W�F��
			std::function<MV1_COLL_RESULT_POLY(const VECTOR_ref&, const VECTOR_ref&)> Map_col_line = [&](const VECTOR_ref& StartPos, const VECTOR_ref& EndPos) {return map_col_line(StartPos, EndPos); };
			//
			bool map_col_nearest(const VECTOR_ref& StartPos, VECTOR_ref* EndPos) const noexcept {
				bool ans = false;
				MV1_COLL_RESULT_POLY colres;
				while (true) {
					colres = this->map_col_line(StartPos, *EndPos);
					if (colres.HitFlag == TRUE) {
						ans = true;
						if (*EndPos == colres.HitPosition) {
							break;
						}
						*EndPos = colres.HitPosition;
					}
					else {
						break;
					}
				}
				return ans;
			}
			//�}�b�v�ǔ���
			void map_col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos) noexcept {
				col_wall(OldPos, NowPos, this->map_col);
			}
			//�ǔ��胆�j�o�[�T��
			static void col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos, MV1& col_obj_t) noexcept {
				auto MoveVector = *NowPos - OldPos;
				// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
				auto HitDim = col_obj_t.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
				std::vector<MV1_COLL_RESULT_POLY*> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
				// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
				for (int i = 0; i < HitDim.HitNum; ++i) {
					auto& h_d = HitDim.Dim[i];
					//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{0.1f��荂���|���S���̂ݓ����蔻����s��
					if (
						(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
						&& (h_d.Position[0].y > OldPos.y() + 0.1f || h_d.Position[1].y > OldPos.y() + 0.1f || h_d.Position[2].y > OldPos.y() + 0.1f)
						&& (h_d.Position[0].y < OldPos.y() + 1.6f || h_d.Position[1].y < OldPos.y() + 1.6f || h_d.Position[2].y < OldPos.y() + 1.6f)
						) {
						kabe_.emplace_back(&h_d);// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
					}
				}
				// �ǃ|���S���Ƃ̓����蔻�菈��
				if (kabe_.size() > 0) {
					bool HitFlag = false;
					for (auto& KeyBind : kabe_) {
						if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
							HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
							if (MoveVector.size() >= 0.0001f) {	// x����y�������� 0.0001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
								// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
								*NowPos = VECTOR_ref(KeyBind->Normal).cross(MoveVector.cross(KeyBind->Normal)) + OldPos;
								bool j = false;
								for (auto& b_ : kabe_) {
									if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
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
					//*
					if (
						HitFlag
						) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
						for (int k = 0; k < PLAYER_HIT_TRYNUM; ++k) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
							bool HitF = false;
							for (auto& KeyBind : kabe_) {
								if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
									*NowPos += VECTOR_ref(KeyBind->Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
									bool j = false;
									for (auto& b_ : kabe_) {
										if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// �������Ă����烋�[�v�𔲂���
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
					//*/
					kabe_.clear();
				}
				MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������
			}
			//��`��
			void sky_Draw(void) noexcept {
				{
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					{
						sky.DrawModel();
						DrawBillboard3D(this->sun_pos.get(), 0.5f, 0.5f, 200.f, 0.f, this->sun_pic.get(), TRUE);
					}
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
				}
				return;
			}
			void grass_Draw(void) noexcept {
				if (grasss != 0) {
					SetFogStartEnd(0.0f, 500.f);
					SetFogColor(192, 192, 192);

					SetDrawAlphaTest(DX_CMP_GREATER, 128);
					//SetUseLighting(FALSE);
					{
						for (int x_ = 0; x_ < grassDiv; x_++) {
							for (int z_ = 0; z_ < grassDiv; z_++) {
								auto& tgt_g = grass__[x_][z_];
								tgt_g.Draw();
							}
						}
					}
					//SetUseLighting(TRUE);
					SetDrawAlphaTest(-1, 0);
				}
			}
			void Shadow_Draw_Far(void) noexcept {
				for (int i = 0; i < 3; ++i) {
					map.DrawMesh(i);
				}
				grass_Draw();
			}
			void item_Draw(void) noexcept {
				for (auto& i : item) { i->Draw_item(); }
			}
			void main_Draw(void) noexcept {
				for (int i = 0; i < 3; ++i) {
					map.DrawMesh(i);
				}

				shader.Draw_lamda(
					[&] {
					SetUseTextureToShader(1, DepthScreen.get());
					map.DrawMesh(3);
				}
				);

				grass_Draw();
				item_Draw();
			}
			//�[�x�l�`��
			void DepthDraw(bool ischangescreen) {
				int handle = GetDrawScreen();
				DepthScreen.SetDraw_Screen(GetCameraPosition(), GetCameraTarget(), GetCameraUpVector(), GetCameraFov(), GetCameraNear(), GetCameraFar());
				{
					Depth.Draw_lamda(
						[&] {
						SetUseTextureToShader(0, -1);
						map.DrawMesh(0);
					}
					);
				}
				if (ischangescreen) {
					GraphHandle::SetDraw_Screen(handle, GetCameraPosition(), GetCameraTarget(), GetCameraUpVector(), GetCameraFov(), GetCameraNear(), GetCameraFar());
				}
			}
			//
			template<class PLAYER_CHARA>
			void Get_item(VECTOR_ref StartPos, VECTOR_ref EndPos, const std::shared_ptr<PLAYER_CHARA>& chara) noexcept {
				chara->ReSet_canget_item();
				for (auto& g : this->item) { g->Get_item_2(StartPos, EndPos, chara, shared_from_this()->Map_col_line); }
			}
			//
			int Get_next_waypoint(std::vector<int> wayp_pre, VECTOR_ref poss, VECTOR_ref zvec = VECTOR_ref::zero()) {
				int now = -1;
				auto tmp = VECTOR_ref::vget(0, 100.f, 0);
				for (auto& w : way_point) {
					auto id = &w - &way_point[0];
					bool tt = true;
					for (auto& ww : wayp_pre) {
						if (id == ww) {
							tt = false;
						}
					}
					if (tt) {
						if (tmp.size() >= (w - poss).size()) {
							auto colres = map_col_line(w + VECTOR_ref::vget(0, 0.5f, 0), poss + VECTOR_ref::vget(0, 0.5f, 0));
							if (!(colres.HitFlag == TRUE) && (zvec == VECTOR_ref::zero() || zvec.Norm().dot((w - poss).Norm()) < 0.f)) {
								tmp = (w - poss);
								now = int(id);
							}
						}
					}
				}
				return now;
			}

			void Check_CameraViewClip(bool use_occlusion) {
				for (int x_ = 0; x_ < grassDiv; x_++) {
					for (int z_ = 0; z_ < grassDiv; z_++) {
						this->grass__[x_][z_].Check_CameraViewClip(x_, z_, use_occlusion);
					}
				}
				DepthDraw(false);
			}
		};
		class MiniMap {
		private:
			bool usemasks = false;

			int x_size = 0;
			int y_size = 0;
			GraphHandle UI_player;			//�`��X�N���[��
			GraphHandle UI_minimap;			//�`��X�N���[��
			float xcam = 1.f;
			int MaskHandle = -1;
			bool loadmasks = true;
			//
			std::shared_ptr<Map> MAPPTs;
		private:
			void Set_pos_chara(int* xp, int* yp, const VECTOR_ref& chara_pos, float& radp) noexcept {
				auto x_2 = chara_pos.x() / MAPPTs->map_col_get().mesh_maxpos(0).x() * (float)(MAPPTs->Get_x_size() / 2) * xcam;
				auto y_2 = -chara_pos.z() / MAPPTs->map_col_get().mesh_maxpos(0).z() * (float)(MAPPTs->Get_y_size() / 2) * xcam;

				*xp = int(x_2 * cos(radp) - y_2 * sin(radp));
				*yp = int(y_2 * cos(radp) + x_2 * sin(radp));
			}
		public:
			MiniMap(std::shared_ptr<Map> MAPPTs_t) noexcept {
				SetUseASyncLoadFlag(FALSE);
				if (usemasks) {
					CreateMaskScreen();
					MaskHandle = LoadMask("data/UI/testMask.bmp");
					GetMaskSize(&x_size, &y_size, MaskHandle);
					loadmasks = true;
				}
				else {
					x_size = 300;
					y_size = 300;
					loadmasks = false;
				}
				UI_minimap = GraphHandle::Make(x_size, y_size, true);
				UI_player = GraphHandle::Load("data/UI/player.bmp");
				MAPPTs = MAPPTs_t;
			}
			template<class PLAYER_CHARA>
			void UI_Draw(std::vector<std::shared_ptr<PLAYER_CHARA>>& chara, const std::shared_ptr<PLAYER_CHARA>& mine) noexcept {
				UI_minimap.SetDraw_Screen(true);
				{
					DrawBox(0, 0, x_size, y_size, GetColor(0, 128, 0), TRUE);
					int xp = 0, yp = 0;
					float radp = 0.f;
					{
						easing_set(&xcam, 1.f + mine->Get_pseed_per() * 0.2f, 0.9f);
						radp = -mine->Get_yrad();
						int xpos = 0, ypos = 0;
						Set_pos_chara(&xpos, &ypos, mine->Get_Pos_Map(), radp);
						xp = x_size / 2 - xpos;
						yp = y_size / 2 - ypos;
					}

					MAPPTs->Get_minmap().DrawRotaGraph(xp, yp, xcam, radp, true);
					for (auto& c : chara) {
						int xpos = 0, ypos = 0;
						Set_pos_chara(&xpos, &ypos, c->Get_Pos_Map(), radp);
						int xt = xp + xpos;
						int yt = yp + ypos;

						UI_player.DrawRotaGraph(xt, yt, xcam, radp + c->Get_yrad(), true);
					}
				}
			}
			void Draw(void) noexcept {
				int xpos = y_r(64);
				int ypos = y_r(64);
				int xpos2 = xpos + y_r(x_size);
				int ypos2 = ypos + y_r(y_size);

				if (usemasks) {
					if (loadmasks) {
						FillMaskScreen(0);
						DrawMask(xpos, ypos, MaskHandle, DX_MASKTRANS_BLACK);
						loadmasks = false;
					}
					SetUseMaskScreenFlag(TRUE);
				}
				UI_minimap.DrawExtendGraph(xpos, ypos, xpos2, ypos2, true);
				if (usemasks) {
					SetUseMaskScreenFlag(FALSE);
				}
			}
		};
	};
};