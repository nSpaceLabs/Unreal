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
	pcUp		= NULL;
	fX			= 0.0f;
	fY			= 0.0f;
	fZ			= 0.0f;
	fAccum	= 0.0f;
	fUpdate	= 0.0f;
	tPrev.SetIdentity();
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
	pcProj->bRotationFollowsVelocity	= false;
	pcProj->InitialSpeed					= 0;

	// It does not seem like 'bounciness' belongs in the same
	// visual state as 'projectile' ??
	pcProj->bShouldBounce				= false;
	pcProj->Bounciness					= 0.0f;

	// Also, gravity handled externally ?
	pcProj->ProjectileGravityScale	= 0;

	// For debug
//	pcProj->bRotationFollowsVelocity	= true;
//	pcProj->bShouldBounce				= true;
//	pcProj->Bounciness					= 1.0f;

	// Place component in world
	pcProj->RegisterComponent();

	// Other setup
	fAccum	= 0.0f;
	fUpdate	= 0.0f;

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
		if (hr == S_OK && (pcUp = pElemRef->pElem->GetRootComponent()) != NULL && pcProj != NULL)
			{
			// Assign update target for projection rules
			CCLOK ( pcProj->SetUpdatedComponent(pcUp); )

			// Snapshot latest transform
//			CCLOK ( tPrev = pcUp->GetRelativeTransform(); )
pcProj->Activate();
			}	// if

		// Clean up
//		_RELEASE(pDct);
		}	// if

	// Transform update
	else if (!WCASECMP(pwLoc,L"Update/OnFire/Value"))
		{
		adtInt iUp(v);

		// Reset timing
		fUpdate	= (iUp != 0) ? iUp/1000.0f : 0.0f;
		fAccum	= 0.0f;
		}	// else if

	// Velocity
	else if (	((bX = (WCASECMP(pwLoc,L"Velocity/A1/OnFire/Value") == 0)) == true) ||
					((bY = (WCASECMP(pwLoc,L"Velocity/A2/OnFire/Value") == 0)) == true) ||
					((bZ = (WCASECMP(pwLoc,L"Velocity/A3/OnFire/Value") == 0)) == true) )
		{
//		UE_LOG(LogTemp, Warning, TEXT("AnProjectile::Velocity"));

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

		// The Unreal projectile component unlinks from the target component any
		// time there is a hit event, re-assign each change of velocity so ensure
		// it keeps working.
		if (pcUp != NULL)
			pcProj->SetUpdatedComponent(pcUp);
		}	// if

	}	// onReceive

void AnProjectile :: Tick( float DeltaTime )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called every frame
	//
	//	PARAMETERS
	//		-	DeltaTime is amount of elapased time.
	//
	////////////////////////////////////////////////////////////////////////

	// Base behaviour
	Super::Tick( DeltaTime );

	// Time to update ?
	fAccum += DeltaTime;
	if (fUpdate > 0.0f && fAccum > fUpdate)
		{
		// Update any changes in the transform
//		UE_LOG(LogTemp, Warning, TEXT("Update!") );

		// Valid update component ?
		if (pcUp != NULL)
			{
			// Current transform
			FTransform
			tNow = pcUp->GetRelativeTransform();

			// Compare each set of components

			// Translation
			FVector
			fvNow		= tNow.GetTranslation();
			FVector
			fvPrev	= tPrev.GetTranslation();
			if (fvNow.X != fvPrev.X)
				updateValue ( L"Transform/Translate/A1/Fire", fvNow.X );
			if (fvNow.Y != fvPrev.Y)
				updateValue ( L"Transform/Translate/A2/Fire", fvNow.Y );
			if (fvNow.Z != fvPrev.Z)
				updateValue ( L"Transform/Translate/A3/Fire", fvNow.Z );

			// Scaling
			fvNow		= tNow.GetScale3D();
			fvPrev	= tPrev.GetScale3D();
			if (fvNow.X != fvPrev.X)
				updateValue ( L"Transform/Scale/A1/Fire", fvNow.X );
			if (fvNow.Y != fvPrev.Y)
				updateValue ( L"Transform/Scale/A2/Fire", fvNow.Y );
			if (fvNow.Z != fvPrev.Z)
				updateValue ( L"Transform/Scale/A3/Fire", fvNow.Z );

			// Rotation
			FQuat
			fqNow		= tNow.GetRotation();
			FQuat
			fqPrev	= tPrev.GetRotation();
			if (fqNow.X != fqPrev.X)
				updateValue ( L"Transform/Rotate/A1/Fire", fqNow.X );
			if (fqNow.Y != fqPrev.Y)
				updateValue ( L"Transform/Rotate/A2/Fire", fqNow.Y );
			if (fqNow.Z != fqPrev.Z)
				updateValue ( L"Transform/Rotate/A3/Fire", fqNow.Z );
			if (fqNow.W != fqPrev.W)
				updateValue ( L"Transform/Rotate/A4/Fire", fqNow.Z );

			// Latest transform
			tPrev = tNow;
			}	// if

		// Accumulated time
		fAccum = (fAccum-fUpdate);
		}	// if

	}	// Tick

void AnProjectile :: updateValue ( const WCHAR *pwVal, float fV )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Update(store) a value into the element.
	//
	//	PARAMETERS
	//		-	pwVal is the relative location of the value
	//		-	fV is the value to use
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLocUp;

	// Generate location to value using element location
	CCLTRY ( adtValue::copy ( strLoc, strLocUp ) );
	CCLTRY ( strLocUp.append ( pwVal ) );

	// Store into path
	CCLTRY ( pLoc->addStore ( strLocUp, adtFloat(fV) ) );

	}	// updateValue