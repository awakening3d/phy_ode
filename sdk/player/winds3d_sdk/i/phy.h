
// winds3d physics component interface define

#ifndef _winds3d_phy_h_
#define _winds3d_phy_h_

#include "..\com\component.h"

class IW3D_PHY : public IComponent
{
public:
	virtual int StepSimulation(float fTimeD) = 0;
};


#endif
