#pragma once

#include "Matrix3x4.h"
#include "Vector3.h"
#include "Hax_CSGO_Manager.h"

struct Hitbox_t
{
	int		m_iBone;
	Vector3	m_vMin;
	Vector3	m_vMax;

	void Setup(int iBone, Vector3 vMin, Vector3 vMax)
	{
		m_iBone = iBone;
		m_vMin	= std::move(vMin);
		m_vMax	= std::move(vMax);
	}
};
extern Hitbox_t Hitbox[21];


class CHitbox
{
	public:
		void OnSetup(const std::shared_ptr<CCSGOManager> & csgoManager)
		{
			Hitbox[0].Setup(BONE_PELVIS, Vector3(-6.42f, -5.7459f, -6.8587f), Vector3(4.5796f, 4.5796f, 6.8373f)); // Torso
			Hitbox[1].Setup(BONE_L_THIGH, Vector3(1.819f, -3.959f, -2.14f), Vector3(22.149002f, 3.424f, 4.5796f));
			Hitbox[2].Setup(BONE_L_CALF, Vector3(2.0758f, -3.21f, -2.1507f), Vector3(19.26f, 2.675f, 3.0495f));
			Hitbox[3].Setup(BONE_L_FOOT, Vector3(1.8725f, -2.675f, -2.4075f), Vector3(5.6175f, 9.694201f, 2.4075f));
			Hitbox[4].Setup(BONE_R_THIGH, Vector3(1.819f, -3.7557f, -4.5796f), Vector3(22.149002f, 3.424f, 2.14f));
			Hitbox[5].Setup(BONE_R_CALF, Vector3(2.0758f, -3.21f, -2.8462f), Vector3(19.26f, 2.675f, 2.247f));
			Hitbox[6].Setup(BONE_R_FOOT, Vector3(1.8725f, -2.675f, -2.4075f), Vector3(5.6175f, 9.694201, 2.4075f));
			Hitbox[7].Setup(BONE_SPINE2, Vector3(-4.28f, -4.5796f, -6.3879f), Vector3(3.21f, 5.885f, 6.2809f)); // Torso
			Hitbox[8].Setup(BONE_SPINE3, Vector3(-4.28f, -5.029f, -6.0883f), Vector3(3.21f, 5.885f, 5.9813f)); // Torso
			Hitbox[9].Setup(BONE_SPINE4, Vector3(-4.28f, -5.35f, -5.885f), Vector3(2.9211f, 5.1467f, 5.885f)); // Chest
			Hitbox[10].Setup(BONE_NECK, Vector3(0.3317f, -3.0174f, -2.4503f), Vector3(3.4026f, 2.4182f, 2.354f)); // Chest
			Hitbox[11].Setup(BONE_HEAD, Vector3(-2.7713f, -2.8783f, -3.103f), Vector3(6.955f, 3.5203f, 3.0067f));
			Hitbox[12].Setup(BONE_L_UPPER_ARM, Vector3(-2.675f, -3.21f, -2.14f), Vector3(12.84f, 3.21f, 2.14f));
			Hitbox[13].Setup(BONE_L_FOREARM, Vector3(-0.f, -2.14f, -2.14f), Vector3(9.63f, 2.14f, 2.14f));
			Hitbox[14].Setup(BONE_L_HAND, Vector3(-1.7227f, -1.2198f, -1.3803f), Vector3(4.4726f, 1.2198f, 1.3803f));
			Hitbox[15].Setup(BONE_R_UPPER_ARM, Vector3(-2.675f, -3.21f, -2.14f), Vector3(12.84f, 3.21f, 2.14f));
			Hitbox[16].Setup(BONE_R_FOREARM, Vector3(-0.f, -2.14f, -2.14f), Vector3(9.63f, 2.14f, 2.14f));
			Hitbox[17].Setup(BONE_R_HAND, Vector3(-1.7227f, -1.2198f, -1.3803f), Vector3(4.4726f, 1.2198f, 1.3803f));
			Hitbox[18].Setup(BONE_L_CLAVICLE, Vector3(-0.f, -3.21f, -5.35f), Vector3(7.49f, 4.28f, 3.21f)); // Chest
			Hitbox[19].Setup(BONE_R_CLAVICLE, Vector3(-0.f, -3.21f, -3.21f), Vector3(7.49f, 4.28f, 5.35f)); // Chest
			Hitbox[21].Setup(BONE_SPINE4, Vector3(-0.2996f, -6.0027f, -4.9969f), Vector3(5.4998f, 2.5038f, 5.1039f));
		}

		void GetHitBoxVectors(Hitbox_t* hitBox, Vector3* hitBoxVectors, Matrix3x4 boneMatrix)
		{
			/*
		      .7-------4
			.'  |    .'|
			6------5'  |
			|   |  |   |
			|  ,0--|---3
			|.'    | .'
			1------2'
			*/
			Vector3 points[8] =
			{
				Vector3(hitBox->m_vMin.m_flXPos, hitBox->m_vMin.m_flYPos, hitBox->m_vMin.m_flZPos),
				Vector3(hitBox->m_vMin.m_flXPos, hitBox->m_vMax.m_flYPos, hitBox->m_vMin.m_flZPos),
				Vector3(hitBox->m_vMax.m_flXPos, hitBox->m_vMax.m_flYPos, hitBox->m_vMin.m_flZPos),
				Vector3(hitBox->m_vMax.m_flXPos, hitBox->m_vMin.m_flYPos, hitBox->m_vMin.m_flZPos),
				Vector3(hitBox->m_vMax.m_flXPos, hitBox->m_vMax.m_flYPos, hitBox->m_vMax.m_flZPos),
				Vector3(hitBox->m_vMin.m_flXPos, hitBox->m_vMax.m_flYPos, hitBox->m_vMax.m_flZPos),
				Vector3(hitBox->m_vMin.m_flXPos, hitBox->m_vMin.m_flYPos, hitBox->m_vMax.m_flZPos),
				Vector3(hitBox->m_vMax.m_flXPos, hitBox->m_vMin.m_flYPos, hitBox->m_vMax.m_flZPos)
			};

			for (int index = 0; index < 8; ++index)
			{
				// scale down the hitbox size a tiny bit (default is a little too big)
				points[index].m_flXPos *= 0.9f;
				points[index].m_flYPos *= 0.9f;
				points[index].m_flZPos *= 0.9f;

				// transform the vector
				m_cCSGOManager->GetEngineHelper()->VectorTransform(points[index], boneMatrix, hitBoxVectors[index]);
			}
		}

	private:
		std::shared_ptr <CCSGOManager>		m_cCSGOManager;
};

