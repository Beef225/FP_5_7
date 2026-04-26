// Copyright JG

#pragma once

#if WITH_EDITOR

#include "IDetailCustomization.h"

class FFP_ItemActorDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

#endif // WITH_EDITOR
