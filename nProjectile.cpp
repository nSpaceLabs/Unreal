#include "nProjectile.h"

UnProjectile::UnProjectile()
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
	bVel		= false;
	bLoc		= false;
	PrimaryComponentTick.bCanEverTick = true;

	}	// UnProjectile

void UnProjectile :: InitializeComponent ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Starts gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("UnProjectile::InitializeComponent"));

	// Base behaviour
	UnElement::InitializeComponent();

	// Create component of the appropriate type
	pcProj	= NewObject<UProjectileMovementComponent>
					(this,UProjectileMovementComponent::StaticClass());

	pcProj->bAutoActivate= true;
	pcProj->bAutoUpdateTickRegistration = true;
	pcProj->Activate();

	// Place component in world
	pcProj->RegisterComponent();

	}	// InitializeComponent

void UnProjectile :: UninitializeComponent ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Ends gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("UnProjectile::UninitializeComponent"));

	// Clean up
	pcProj						= NULL;

	// Base behaviour
	UnElement::UninitializeComponent();
	}	// UninitializeComponent

bool UnProjectile :: mainTick ( float fD )
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

	// Valid component ?
	if (pcProj == NULL)
		return bWrk;

	// Location
	if (bLoc)
		{
		IDictionary	*pDct		= NULL;
		nElement		*pElem	= NULL;
		adtString	strFull;
		adtValue		vL;
		adtIUnknown	unkV;

		UE_LOG(LogTemp, Warning, TEXT("UnProjectile::mainTick:Location"));

		// Generate full path to target
		CCLTRY(adtValue::copy(pParent->strLoc,strFull));
		CCLTRY(strFull.append(strLocAt));

		// Retrieve object from render dictionary
		CCLTRY(nspcLoadPath(pParent->pRenLoc->pDctRen,strFull,vL));

		// Path will be a dictionary for the entry
		CCLTRY(_QISAFE((unkV=vL),IID_IDictionary,&pDct));

		// Access the actual 'nElement'
		CCLTRY(pDct->load(strnRefLocn,vL));

		// Cast directly, depends on knowledge of how it is stored in "nLoc"
		CCLTRYE( (pElem = ((nElement *)vL.punk)) != NULL, E_UNEXPECTED );

		// If failed at this point, it will not get fixed
		if (hr != S_OK)
			bLoc = false;

		// Wait for a valid root and a valid main scene component
		if (hr == S_OK && pElem->pRoot != NULL)// && pElem->pRoot->getSceneComponent() != NULL)
			{
			// Assign update target for projection rules
			CCLOK ( pcProj->SetUpdatedComponent(pElem->pRoot); )
//			CCLOK ( pcProj->SetUpdatedComponent(pElem->pRoot->getSceneComponent()); )

pcProj->Activate();

			// Done
			bLoc = false;
			}	// if

		// Clean up
		_RELEASE(pDct);
		}	// if

	// Velocity
	else if (bVel)
		{
		UE_LOG(LogTemp, Warning, TEXT("UnProjectile::mainTick:Velocity"));

		// Update velocity
		pcProj->SetVelocityInLocalSpace(FVector(fX,fY,fZ));
		pcProj->UpdateComponentVelocity();
pcProj->bAutoUpdateTickRegistration = true;
pcProj->Activate(true);
		bVel = false;
		}	// if

	return bWrk;
	}	// mainTick
/*
void UnProjectile::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
	{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
if (pcProj != NULL)
	pcProj->TickComponent(DeltaTime,TickType,ThisTickFunction);
	}
*/

bool UnProjectile :: onReceive (	nElement *pElem,
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
	bool	bX		= false;
	bool	bY		= false;
	bool	bZ		= false;

	// Base behaviour
	bSch = UnElement::onReceive(pElem,pwRoot,pwLoc,v);

	// Target location
	if (!WCASECMP(pwLoc,L"Location/OnFire/Value"))
		{
		UE_LOG(LogTemp, Warning, TEXT("UnProjectile::Location"));

		// Update
		adtValue::toString(v,strLocAt);
		bLoc	= true;
		bSch	= true;
		}	// if

	// Velocity
	else if (	((bX = (WCASECMP(pwLoc,L"Velocity/A1/OnFire/Value") == 0)) == true) ||
					((bY = (WCASECMP(pwLoc,L"Velocity/A2/OnFire/Value") == 0)) == true) ||
					((bZ = (WCASECMP(pwLoc,L"Velocity/A3/OnFire/Value") == 0)) == true) )
		{
		UE_LOG(LogTemp, Warning, TEXT("UnProjectile::Velocity"));

		// Shape Type update
		if			(bX)	fX = v;
		else if	(bY)	fY = v;
		else if	(bZ)	fZ = v;
		bVel	= true;
		bSch	= true;
		}	// if

	return bSch;
	}	// onReceive

