#include "nLight.h"

UnLight::UnLight()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pcLight	= NULL;
	fIntense	= 5000.0f;
	bIntense	= false;
	}	// UnLight

void UnLight :: InitializeComponent ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Starts gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("UnLight::InitializeComponent"));

	// Base behaviour
	UnElement::InitializeComponent();

	// Create light component of the appropriate type
	// TODO: More light types
	pcLight				= NewObject<UPointLightComponent>
								(this,UPointLightComponent::StaticClass());
	pcLight->bVisible	= false;
	pcLight->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);

	// Necessary ?
	pcLight->SetMobility(EComponentMobility::Movable);

	// Defaults
	pcLight->SetLightColor(FLinearColor(1.0f,1.0f,1.0f,1.0f));
	pcLight->SetIntensity(fIntense);

	// Place component in world
	pcLight->RegisterComponent();

	}	// InitializeComponent

void UnLight :: UninitializeComponent ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Ends gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("UnLight::UninitializeComponent"));

	// Shape
	if (pcLight != NULL)
		{
		pcLight->bVisible			= false;
		pcLight->bHiddenInGame	= true;
		pcLight						= NULL;
		}	// if

	// Base behaviour
	UnElement::UninitializeComponent();
	}	// UninitializeComponent

bool UnLight :: mainTick ( float fD )
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
	bWrk = UnElement::mainTick(fD);

	// Color
	if (bColor && pcLight != NULL)
		{
		FLinearColor	clr	(	((iColor>>16)&0xff)/255.0,
										((iColor>>8)&0xff)/255.0,
										((iColor>>0)&0xff)/255.0,
										((iColor>>24)&0xff)/255.0 );

		// Update
		pcLight->SetLightColor(clr);
		bColor = false;
		}	// if

	// Intensity
	if (bIntense && pcLight != NULL)
		{
		// Update
		pcLight->SetIntensity(fIntense);
		bIntense = false;
		}	// if

	// Allow to change type on the fly ?

	return bWrk;
	}	// mainTick

bool UnLight :: onReceive (	nElement *pElem,
											const WCHAR *pwRoot, 
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
	//		-	pElem is the nSpace element
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	v is the value
	//
	//	RETURN VALUE
	//		true if there is main game loop to be scheduled.
	//
	////////////////////////////////////////////////////////////////////////
	bool	bSch	= false;

	// Base behaviour
	bSch = UnElement::onReceive(pElem,pwRoot,pwLoc,v);

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
		// Shape Type update
		adtValue::copy ( adtString(v), strType );
 		strType.at();
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
	}	// onReceive

