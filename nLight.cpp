#include "nLight.h"

AnLight::AnLight()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pcLight		= NULL;
	}	// AnLight

void AnLight :: BeginPlay ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Starts gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnLight::BeginPlay"));

	// Base behaviour
	AnElement::BeginPlay();

	// Necessary ?
//	SetMobility(EComponentMobility::Movable);

	// Defaults
//	SetLightColor(FLinearColor(1.0f,1.0f,1.0f,1.0f));
//	SetIntensity(fIntense);
	}	// BeginPlay

void AnLight :: EndPlay ( const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Ends gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnLight::EndPlay"));

	// Base behaviour
	AnElement::EndPlay(rsn);
	}	// EndPlay

void AnLight :: onValue (	const WCHAR *pwRoot, 
									const WCHAR *pwLoc,
									const ADTVALUE &v )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		nSpaceClientCB
	//
	//	PURPOSE
	//		-	Called when a listened location receives a value.
	//
	//	PARAMETERS
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	v is the value
	//
	//	RETURN VALUE
	//		true if there is main game loop to be scheduled.
	//
	////////////////////////////////////////////////////////////////////////
	
	// Debug
//	UE_LOG(LogTemp, Log, TEXT("AnLight::onValue:%s:%s"), pwRoot, pwLoc );

	// Base behaviour
	AnElement::onValue(pwRoot,pwLoc,v);

	// Color
	if (!WCASECMP(pwLoc,L"Element/Color/OnFire/Value"))
		{
		FLinearColor	clr	(	((iColor>>16)&0xff)/255.0,
										((iColor>>8)&0xff)/255.0,
										((iColor>>0)&0xff)/255.0,
										((iColor>>24)&0xff)/255.0 );

		// Update
		if (pcLight != NULL)
			pcLight->SetLightColor(clr);
		}	// if

	// Type
	else if (!WCASECMP(pwLoc,L"Type/OnFire/Value"))
		{
		adtString strType(v);

		// Allow dynamic changes of light type ?
		if (pcLight == NULL && strType.length() > 0)
			{
			// Create the appropriate light type
			if (!WCASECMP(strType,L"Point"))
				pcLight				= NewObject<UPointLightComponent>
											(this,UPointLightComponent::StaticClass());

			// Valid light type ?
			if (pcLight != NULL)
				{
				FLinearColor	clr	(	((iColor>>16)&0xff)/255.0,
												((iColor>>8)&0xff)/255.0,
												((iColor>>0)&0xff)/255.0,
												((iColor>>24)&0xff)/255.0 );

				// Activate component
				pcLight->RegisterComponent();

				//	Load defaults into new component
				pcLight->SetLightColor(clr);
				pcLight->SetIntensity(fIntense);

				// Root component has changed
				SetRootComponent(pcLight);
				rootUpdate();
				pcLight->SetVisibility(true,true);
				}	// if
			}	// if

		}	// if

	// Intensity
	else if (!WCASECMP(pwLoc,L"Intensity/OnFire/Value"))
		{
		fIntense = adtFloat(v);

		// Update
		if (pcLight != NULL)
			pcLight->SetIntensity(fIntense);
		}	// if

	}	// onValue
