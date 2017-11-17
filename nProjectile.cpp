#include "nProjectile.h"

AnProjectile::AnProjectile()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pcProj	= NULL;
	fX			= 0.0f;
	fY			= 0.0f;
	fZ			= 0.0f;
	}	// AnProjectile

void AnProjectile::BeginPlay()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when the game starts or when spawned.
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnProjectile::BeginPlay"));

	// Default behaviour
	Super::BeginPlay();

	// Create component of the appropriate type
	pcProj	= NewObject<UProjectileMovementComponent>
					(this,UProjectileMovementComponent::StaticClass());

	pcProj->bAutoActivate= true;
	pcProj->bAutoUpdateTickRegistration = true;
	pcProj->Activate();

	// Defaults
	pcProj->ProjectileGravityScale	= 0;
	pcProj->bShouldBounce				= true;
	pcProj->Bounciness					= 1.0f;
	pcProj->bRotationFollowsVelocity	= true;
	pcProj->InitialSpeed					= 0;

	// Place component in world
	pcProj->RegisterComponent();

	}	// BeginPlay

void AnProjectile :: EndPlay ( const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Ends gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnProjectile::EndPlay"));

	// Clean up
	pcProj						= NULL;

	// Base behaviour
	Super::EndPlay(rsn);
	}	// EndPlay

void AnProjectile :: onValue (	const WCHAR *pwRoot, 
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
	////////////////////////////////////////////////////////////////////////
	bool	bX		= false;
	bool	bY		= false;
	bool	bZ		= false;

	// Base behaviour
	AnElement::onValue(pwRoot,pwLoc,v);

	// Target location
	if (!WCASECMP(pwLoc,L"Location/OnFire/Value"))
		{
		HRESULT		hr				= S_OK;
		IDictionary	*pDct			= NULL;
		nElementRef	*pElemRef	= NULL;
		adtString	strLocAt(v);
		adtString	strFull;
		adtValue		vL;
		adtIUnknown	unkV;

		UE_LOG(LogTemp, Warning, TEXT("AnProjectile::Location"));

		// Generate path to target
		CCLTRY(adtValue::copy(strLoc,strFull));
		CCLTRY(strFull.append(strLocAt));

		// Retrieve object from render dictionary
		CCLTRY(nspcLoadPath(pLoc->pDctRen,strFull,vL));

		// Path will be a dictionary for the entry
		CCLTRY(_QISAFE((unkV=vL),IID_IDictionary,&pDct));

		// Access the actual element reference
		CCLTRY(pDct->load(adtString(L"Element"),vL));

		// Cast directly, depends on knowledge of how it is stored in "nLoc"
		CCLTRYE( (pElemRef = ((nElementRef *)vL.punk)) != NULL, E_UNEXPECTED );

		// Wait for a valid root and a valid main scene component
		if (hr == S_OK && pElemRef->pElem->GetRootComponent() != NULL && pcProj != NULL)
			{
			// Assign update target for projection rules
			CCLOK ( pcProj->SetUpdatedComponent(pElemRef->pElem->GetRootComponent()); )

pcProj->Activate();
			}	// if

		// Clean up
//		_RELEASE(pDct);
		}	// if

	// Velocity
	else if (	((bX = (WCASECMP(pwLoc,L"Velocity/A1/OnFire/Value") == 0)) == true) ||
					((bY = (WCASECMP(pwLoc,L"Velocity/A2/OnFire/Value") == 0)) == true) ||
					((bZ = (WCASECMP(pwLoc,L"Velocity/A3/OnFire/Value") == 0)) == true) )
		{
		UE_LOG(LogTemp, Warning, TEXT("AnProjectile::Velocity"));

		// Shape Type update
		if			(bX)	fX = v;
		else if	(bY)	fY = v;
		else if	(bZ)	fZ = v;

		// Update velocity
//		lprintf ( LOG_INFO, L"Velocity : %g %g %g", (float)fX, (float)fY, (float)fZ );

		// How much of this is necessary ?
//		pcProj->v
		pcProj->SetVelocityInLocalSpace(FVector(fX,fY,fZ));
		pcProj->UpdateComponentVelocity();
pcProj->bAutoUpdateTickRegistration = true;
pcProj->Activate(true);
		}	// if

	}	// onReceive

