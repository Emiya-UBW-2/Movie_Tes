#include "ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			ObjectBaseClass::LoadModel(PHYSICS_SETUP TYPE, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
			this->m_IsBaseModel = true;
			this->m_objActive = true;
			this->m_PHYSICS_SETUP = TYPE;
			this->m_FilePath = filepath;
			this->m_ObjFileName = objfilename;
			this->m_ColFileName = colfilename;
			auto Load = [&](MV1* obj, std::string Path, std::string NameAdd, int PHYSICS_TYPE) {
				FILEINFO FileInfo;
				if (FileRead_findFirst((Path + NameAdd + ".mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
					//MV1::Load(Path + ".pmx", obj, PHYSICS_TYPE);
					MV1::Load((Path + NameAdd + ".mv1").c_str(), obj, PHYSICS_TYPE);
				}
				else if (FileRead_findFirst((Path + ".pmx").c_str(), &FileInfo) != (DWORD_PTR)-1) {
					MV1::Load(Path + ".pmx", obj, PHYSICS_TYPE);
				}
			};
			//model
			switch (this->m_PHYSICS_SETUP) {
			case PHYSICS_SETUP::DISABLE:
				Load(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_DISABLE", DX_LOADMODEL_PHYSICS_DISABLE);
				break;
			case PHYSICS_SETUP::LOADCALC:
				Load(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_LOADCALC", DX_LOADMODEL_PHYSICS_LOADCALC);
				break;
			case PHYSICS_SETUP::REALTIME:
				Load(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_REALTIME", DX_LOADMODEL_PHYSICS_REALTIME);
				break;
			default:
				break;
			}
			//col
			Load(&this->m_col, this->m_FilePath + this->m_ColFileName, "", DX_LOADMODEL_PHYSICS_DISABLE);
			if (this->m_col.IsActive()) {
				for (int i = 0; i < this->m_col.mesh_num(); ++i) { this->m_col.SetupCollInfo(8, 8, 8, -1, i); }
			}
			//フレーム
			{
				switch (this->m_objType) {
				case ObjType::Human:
					this->m_Frames.resize((int)CharaFrame::Max);
					break;
				default:
					this->m_Frames.clear();
					break;
				}
				for (auto& f : this->m_Frames) {
					f.first = -1;
				}
				int count = 0;
				for (int frameNum = 0; frameNum < this->GetObj().frame_num(); frameNum++) {
					std::string FName = this->GetObj().frame_name(frameNum);
					std::string CName = "";
					switch (this->m_objType) {
					case ObjType::Human:
						CName = CharaFrameName[count];
						break;
					default:
						break;
					}
					bool compare = (FName == CName);
					//if (!compare) { compare = (FName.find(CName) != std::string::npos); }
					if (compare) {
						//そのフレームを登録
						this->m_Frames[count].first = frameNum;
						this->m_Frames[count].second = MATRIX_ref::Mtrans(this->GetObj().GetFrameLocalMatrix(this->m_Frames[count].first).pos());
					}
					else if (frameNum < this->GetObj().frame_num() - 1) {
						continue;//飛ばす
					}
					count++;
					frameNum = 0;
					if (count >= (int)this->m_Frames.size()) {
						break;
					}
				}
			}
			//シェイプ
			{
				switch (this->m_objType) {
				case ObjType::Human:
					this->m_Shapes.resize((int)CharaShape::Max);
					for (int j = 1; j < (int)CharaShape::Max; j++) {
						auto s = MV1SearchShape(this->GetObj().get(), CharaShapeName[j]);
						if (s >= 0) {
							this->m_Shapes[j].first = s;
							this->m_Shapes[j].second = 0.f;
						}
					}
					break;
				default:
					this->m_Shapes.clear();
					break;
				}
			}
		}
		void			ObjectBaseClass::CopyModel(const std::shared_ptr<ObjectBaseClass>& pBase) noexcept {
			this->m_IsBaseModel = false;
			this->m_objActive = true;
			this->m_PHYSICS_SETUP = pBase->m_PHYSICS_SETUP;
			this->m_FilePath = pBase->m_FilePath;
			this->m_ObjFileName = pBase->m_ObjFileName;
			this->m_ColFileName = pBase->m_ColFileName;
			//model
			this->m_obj = pBase->m_obj.Duplicate();
			//col
			if (pBase->m_col.IsActive()) {
				this->m_col = pBase->m_col.Duplicate();
			}
			if (this->m_col.IsActive()) {
				for (int i = 0; i < this->m_col.mesh_num(); ++i) { this->m_col.SetupCollInfo(8, 8, 8, -1, i); }
			}
			//フレーム
			this->m_Frames.resize(pBase->m_Frames.size());
			for (auto& f : this->m_Frames) {
				auto index = &f - &this->m_Frames.front();
				f.first = pBase->m_Frames.at(index).first;
				if (f.first != -1) {
					f.second = pBase->m_Frames.at(index).second;
				}
			}
			//シェイプ
			this->m_Shapes.resize(pBase->m_Shapes.size());
			for (auto& f : this->m_Shapes) {
				auto index = &f - &this->m_Shapes.front();
				f.first = pBase->m_Shapes.at(index).first;
				if (f.first != -1) {
					f.second = pBase->m_Shapes.at(index).second;
				}
			}
		}
		void			ObjectBaseClass::SaveModel(bool UseToonWhenCreateFile) noexcept {
			if (this->m_IsBaseModel) {
				auto Save = [&](MV1* obj, std::string Path, std::string NameAdd, int PHYSICS_TYPE) {
					FILEINFO FileInfo;
					if (
						!(FileRead_findFirst((Path + NameAdd + ".mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) &&
						(FileRead_findFirst((Path + ".pmx").c_str(), &FileInfo) != (DWORD_PTR)-1)
						) {
						MV1SetLoadModelUsePhysicsMode(PHYSICS_TYPE);
						if (!UseToonWhenCreateFile) {
							MV1SetMaterialTypeAll(obj->get(), DX_MATERIAL_TYPE_NORMAL);
						}
						MV1SaveModelToMV1File(obj->get(), (Path + NameAdd + ".mv1").c_str());
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					}
				};
				//model
				switch (this->m_PHYSICS_SETUP) {
				case PHYSICS_SETUP::DISABLE:
					Save(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_DISABLE", DX_LOADMODEL_PHYSICS_DISABLE);
					break;
				case PHYSICS_SETUP::LOADCALC:
					Save(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_LOADCALC", DX_LOADMODEL_PHYSICS_LOADCALC);
					break;
				case PHYSICS_SETUP::REALTIME:
					Save(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_REALTIME", DX_LOADMODEL_PHYSICS_REALTIME);
					break;
				default:
					break;
				}
				//col
				Save(&this->m_col, this->m_FilePath + this->m_ColFileName, "", DX_LOADMODEL_PHYSICS_DISABLE);
			}
		}
		//
		void			ObjectBaseClass::Init(void) noexcept {
			this->m_IsActive = true;
			this->m_IsResetPhysics = true;
			this->m_IsFirstLoop = true;
			this->m_IsDraw = false;
		}
		//
		void			ObjectBaseClass::ExecuteCommon(void) noexcept {
			if (this->m_IsFirstLoop) {
				this->m_PrevMat = this->GetObj().GetMatrix();
			}
			//シェイプ更新
			switch (this->m_objType) {
			case ObjType::Human:
				for (int j = 1; j < (int)CharaShape::Max; j++) {
					MV1SetShapeRate(this->GetObj().get(), m_Shapes[j].first, (1.f - this->m_Shapes[0].second)*this->m_Shapes[j].second);
				}
				break;
			default:
				break;
			}
			//物理更新
			if (this->m_PHYSICS_SETUP == PHYSICS_SETUP::REALTIME) {
				if (this->m_IsResetPhysics) {
					this->m_IsResetPhysics = false;
					this->GetObj().PhysicsResetState();
				}
				else {
					auto NowMat = this->GetObj().GetMatrix();
					int Max = 2;
					for (int i = 0; i < Max; i++) {
						this->GetObj().SetMatrix(
							Lerp_Matrix(this->m_PrevMat.GetRot(), NowMat.GetRot(), (float)(i + 1) / (float)Max)
							* MATRIX_ref::Mtrans(Lerp(this->m_PrevMat.pos(), NowMat.pos(), (float)(i + 1) / (float)Max)));
						this->GetObj().PhysicsCalculation(1000.0f / FPS * 60.f);
					}
				}
			}
			//最初のループ終わり
			this->m_IsFirstLoop = false;
		}

		void			ObjectBaseClass::DrawShadow(void) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				this->GetObj().DrawModel();
			}
		}
		void			ObjectBaseClass::CheckDraw(void) noexcept {
			this->m_IsDraw = false;
			this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetCameraPosition()).size();
			if (CheckCameraViewClip_Box(
				(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-1.f*Scale_Rate, -0.f*Scale_Rate, -1.f*Scale_Rate)).get(),
				(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(1.f*Scale_Rate, 1.f*Scale_Rate, 1.f*Scale_Rate)).get()) == FALSE
				) {
				this->m_IsDraw |= true;
			}
		}
		void			ObjectBaseClass::Draw(bool isDrawSemiTrans) noexcept {
			if (this->m_IsActive && this->m_IsDraw) {
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-1.f*Scale_Rate, -0.f*Scale_Rate, -1.f*Scale_Rate)).get(),
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(1.f*Scale_Rate, 1.f*Scale_Rate, 1.f*Scale_Rate)).get()) == FALSE
					) {
					for (int i = 0; i < this->GetObj().mesh_num(); i++) {
						if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
							this->GetObj().DrawMesh(i);
						}
					}
				}
			}
		}
		//
		void			ObjectBaseClass::Dispose(void) noexcept {
			this->GetObj().Dispose();
			this->m_col.Dispose();
		}
	};
};
