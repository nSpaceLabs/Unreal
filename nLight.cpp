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
	strType		= L"";
	bType			= false;
	fIntense		= 5000.0f;
	bIntense		= false;
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

bool AnLight :: onValue (	const WCHAR *pwRoot, 
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
	bool	bSch	= false;

	// Debug
//	UE_LOG(LogTemp, Log, TEXT("AnLight::onValue:%s:%s"), pwRoot, pwLoc );

	// Base behaviour
	bSch = AnElement::onValue(pwRoot,pwLoc,v);

	// Color
	if (!WCASECMP(pwLoc,L"Element/Color/OnFire/Value"))
		{
		// Color update
		iColor= adtInt(v);
		bColor= true;
		bSch	= true;
		}	// if

	// Type
	else if (!WCASECMP(pwLoc,L"Type/OnFire/Value"))
		{
		// Light type update
		adtValue::copy ( adtString(v), strType );
 		strType.at();
		bType	= true;
		bSch	= true;
		}	// if

	// Intensity
	else if (!WCASECMP(pwLoc,L"Intensity/OnFire/Value"))
		{
		// Shape Type update
		fIntense = adtFloat(v);
		bIntense	= true;
		bSch		= true;
		}	// if

	return bSch;
	}	// onValue

bool AnLight :: tickMain ( float fD )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Execute work for main game thread.
	//
	//	PARAMETERS
	//		-	fD is the amount of elapsed time since last game loop tick.
	//
	//	RETURN VALUE
	//		true if work is still needed
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr		= S_OK;
	bool		bWrk = false;

	// Default behaviour
	bWrk = AnElement::tickMain(fD);

	// Color
	if (bColor)
		{
		FLinearColor	clr	(	((iColor>>16)&0xff)/255.0,
										((iColor>>8)&0xff)/255.0,
										((iColor>>0)&0xff)/255.0,
										((iColor>>24)&0xff)/255.0 );

		// Update
		if (pcLight != NULL)
			pcLight->SetLightColor(clr);
		bColor = false;
		}	// if

	// Intensity
	if (bIntense)
		{
		// Update
		if (pcLight != NULL)
			pcLight->SetIntensity(fIntense);
		bIntense = false;
		}	// if

	// Allow to change type on the fly ?
	if (bType)
		{
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
				// Activate component
				pcLight->RegisterComponent();

				//	Load defaults into new component
				bColor	= true;
				bIntense	= true;

				// Root component has changed
				SetRootComponent(pcLight);
				rootUpdate();
				pcLight->SetVisibility(true,true);
				}	// if
			}	// if

		// Complete
		bType = false;
		}	// if

	return bWrk;
	}	// tickMain
