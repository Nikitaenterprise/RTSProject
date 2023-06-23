#pragma once

#include "UObject/Interface.h"
#include "ResourceInterface.generated.h"

UINTERFACE()
class UResourceInterface : public UInterface
{
	GENERATED_BODY()
};

class RTSPROJECT_API IResourceInterface
{
	GENERATED_BODY()
	
public:

	//virtual EResourceType GetResourceType() const; 
};
