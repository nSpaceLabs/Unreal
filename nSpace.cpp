////////////////////////////////////////////////////////////////////
//
//									nSpace.cpp
//
//				Implementation of the main nSpace actor object
//
////////////////////////////////////////////////////////////////////

#include "nSpace.h"
#include "nLoc.h"
//#include "nElement.h"

// Globals
// Temporary hack until it is determined why Unreal engine in production
// mode creates two actors.
static AnSpace *pThisActor = NULL;

AnSpace::AnSpace()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pCli			= NULL;
	pDctLoc		= NULL;
	bNewLoc		= false;

	// String references
	strRefActor = L"Actor";
//	strRenLoc	= L"/State/Interface/Dict/RenderList/Dict/";
	strRenLoc	= L"/State/Render/3D/State/Default/";
	strLevel		= L"";
	bLevel		= false;

	// Worker thread
	pThrd			= NULL;
	pTick			= NULL;
	pMnQ			= NULL;
	pMnIt			= NULL;
	pWrkQ			= NULL;
	pWrkIt		= NULL;
	pStQ			= NULL;
	pStIt			= NULL;
	evWork.init();

	// Set this actor to call Tick() every frame.  
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	}	// AnSpace

AnSpace::~AnSpace()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Destructor for the object.
	//
	////////////////////////////////////////////////////////////////////////
	_RELEASE(pTick);
	_RELEASE(pThrd);
	_RELEASE(pMnIt);
	_RELEASE(pMnQ);
	_RELEASE(pStIt);
	_RELEASE(pStQ);
	_RELEASE(pWrkIt);
	_RELEASE(pWrkQ);
	_RELEASE(pDctLoc);
	if (pCli != NULL)
		{
		pCli->close();
		delete pCli;
		}	// if

	}	// ~AnSpace

HRESULT AnSpace :: addMain ( InTick *pTick )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add object to main game loop for ticking.
	//
	//	PARAMETERS
	//		-	pTick is the tickable object.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	lprintf ( LOG_INFO, L"pTick %p lTick %ld", pTick, (U64)pTick );

	// State check
	if (!bWork || pMnQ == NULL)
		return S_OK;

	return pMnQ->write ( adtLong((U64)pTick) );
	}	// addMain

HRESULT AnSpace :: addStore ( const WCHAR *pwLoc, const ADTVALUE &v, 
													const WCHAR *pwLocRen )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add a 'store' request to store queue.
	//
	//	PARAMETERS
	//		-	pwLoc is the store location
	//		-	v is the store value
	//		-	pwLocRen is the root render location
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLoc(pwLoc);

	// State check
	if (!bWork || pStQ == NULL)
		return S_OK;

	// State check
	CCLTRYE ( strLoc.length() > 0, E_INVALIDARG );

	// Prepend the render location for relative locations
	if (hr == S_OK && *pwLoc != '/' && pwLocRen != NULL)
		hr = strLoc.prepend ( pwLocRen );

	// Add to queue and signal thread
	dbgprintf ( L"AnSpace::addStore:%s\r\n", (LPCWSTR)strLoc );
	CCLTRY ( pStQ->write ( strLoc ) );
	CCLTRY ( pStQ->write ( v ) );
	CCLOK  ( evWork.signal(); )

	return hr;
	}	// addStore

HRESULT AnSpace :: addWork ( InTick *pTick )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add object to worker thread for ticking.
	//
	//	PARAMETERS
	//		-	pTick is the tickable object.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr = S_OK;

	// State check
	if (!bWork || pWrkQ == NULL)
		return S_OK;

	// Add to queue and signal thread
	CCLTRY ( pWrkQ->write ( adtLong((U64)pTick) ) );
	CCLOK  ( evWork.signal(); )

	return hr;
	}	// addWork

void AnSpace::BeginPlay()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when the game starts or when spawned.
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT hr = S_OK;

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnSpace::BeginPlay"));

	// Default behaviour
	Super::BeginPlay();

	// TEMPORARY
	if (pThisActor != NULL)
		return;
	pThisActor = this;

	// Create worker object
	CCLTRYE ( (pTick = new AnSpacet(this)) != NULL, E_OUTOFMEMORY );
	CCLOK   ( pTick->AddRef(); )
	CCLTRY  ( pTick->construct() );

	// Create worker thread, no need to wait for startup
	CCLOK (bWork = true;)
	CCLTRY(COCREATE(L"Sys.Thread", IID_IThread, &pThrd ));
	CCLTRY(pThrd->threadStart ( pTick, 0 ));

	}	// BeginPlay

void AnSpace::EndPlay(const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when play ends for this actor.
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnSpace::EndPlay"));

	// TEMPORARY
	if (pThisActor == this)
		{

		// Shutdown worker thread
		if (pThrd != NULL)
			{
			pThrd->threadStop(30000);
			_RELEASE(pThrd);
			}	// if

		pThisActor = NULL;
		}	// if

	// Base behaviour
	Super::EndPlay(rsn);
	}	// EndPlay

HRESULT AnSpace :: onValue (	const WCHAR *pwRoot, 
										const WCHAR *pwLoc,
										const ADTVALUE &vV )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Process a received value.  It is assumed the string values
	//			have already been validated.
	//
	//	PARAMETERS
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	vV contains the value
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr			= S_OK;
	adtInt		iIdx;
//	int			idx;

	// Debug
//	adtString	strV;
//	adtValue::toString(vV,strV);
//	CCLOK ( lprintf ( LOG_INFO, L"%s:%s:%s\r\n", pwRoot, pwLoc, (LPCWSTR)strV ); )
//	UE_LOG(LogTemp, Warning, TEXT("AnSpace::onValue:%s:%s:%s"), pwRoot, pwLoc, (LPCWSTR)strV );

	// Check for render location indices, ignore reserved names
	if (	!WCASENCMP(pwLoc,L"Locations/Dict/",15) &&
			pwLoc[15] != '_' && (iIdx = adtString(&pwLoc[15])) >= 1)
		{
		IDictionary		*pDct	= NULL;
		int				len	= 0;
		adtIUnknown		unkV(vV);
		adtString		strL;
		adtValue			vL;

		// Descriptor
		CCLTRY(_QISAFE(unkV,IID_IDictionary,&pDct));

		// Is a render location specified ?
		CCLTRY ( pDct->load ( adtString(L"Location"), vL ) );
		CCLTRYE( (len = (strL = vL).length()) > 0, E_UNEXPECTED );

		// Local usage requires trailing slash
		if (hr == S_OK && strL[len-1] != '/')
			hr = strL.append ( L"/" );
/*
		// Does location handler at index already exist ?
		if (hr == S_OK && pDctLoc->load ( iIdx, vL ) == S_OK)
			{
			IDictionary *pDsc		= NULL;
			nElement		*pElem	= NULL;
			adtIUnknown	unkV(vL);

			// Access descriptor and root element for existing renderer
			CCLTRY  ( _QISAFE(unkV,IID_IDictionary,&pDsc) );
			CCLTRY  ( pDsc->load ( adtString(L"Element"), vL ) );
			CCLTRYE ( (pElem = (nElement *)(IUnknown *)(unkV=vL)) != NULL, E_UNEXPECTED );
			CCLTRYE ( pElem->pRenLoc != NULL, E_UNEXPECTED );
			_RELEASE(pDsc);

			// Location changed ?
//			if (hr == S_OK && WCASECMP(pElem->pRenLoc->strLocRen,strL))
//				{
				// Shutdown the location
				pElem->pRenLoc->setRoot(NULL);

				// Remove from world
				pElem->pRenLoc->Destroy();

				// Remove from render dictionary
				pDctLoc->remove ( iIdx );

				// A new render location will be created below.
//				}	// if

			// Location same
//			else if (hr == S_OK && pElem->pRoot != NULL)
//				{
//				FVector	fTrans;

				// Render location object exists and location is the same, just update the position.

				// Fill in coordinate.  See comments in 'nElement' for assignment
//				if (hr == S_OK && pDct->load ( adtString(L"X"), vL ) == S_OK)
//					fTrans.Y = 25*adtDouble(vL);
//				if (hr == S_OK && pDct->load ( adtString(L"Y"), vL ) == S_OK)
//					fTrans.Z = 25*adtDouble(vL);
//				if (hr == S_OK && pDct->load ( adtString(L"Z"), vL ) == S_OK)
//					fTrans.X = 25*adtDouble(vL);

				// Position has to be updated in main loop so 'recieve' new position
//				dbgprintf ( L"AnSpace::onValue:Move %g,%g,%g\r\n", fTrans.X, fTrans.Y, fTrans.Z );

//				pElem->pRoot->onReceive ( pElem, L"", L"Element/Transform/Translate/A1/OnFire/Value", adtDouble(fTrans.X) );
//				pElem->pRoot->onReceive ( pElem, L"", L"Element/Transform/Translate/A2/OnFire/Value", adtDouble(fTrans.Y) );
//				pElem->pRoot->onReceive ( pElem, L"", L"Element/Transform/Translate/A3/OnFire/Value", adtDouble(fTrans.Z) );

				// Schedule the element for work
//				addMain ( pElem );
//				}	// else if

			}	// else if
*/
		// Need a new location handler at index ?
		if (hr == S_OK && pDctLoc->load ( iIdx, vL ) != S_OK)
			{
			// Update descriptor with location and initialization request
			CCLTRY ( pDct->store ( adtString(L"Initialize"), adtBool(true) ) );
			CCLTRY ( pDct->store ( adtString(L"Location"), strL ) );

			// Store as descriptor for index
			CCLTRY ( pDctLoc->store ( iIdx, adtIUnknown(pDct) ) );

			// Let game loop know to look for new locations
			CCLOK  ( bNewLoc = true; )
			}	// if

		// Clean up
		_RELEASE(pDct);
		}	// if

	// Level selection
	else if (!WCASECMP(pwLoc,L"Level/Element/Default/OnFire/Value"))
		{
		// Cache level selection for work in the game loop
		adtValue::toString(vV,strLevel);
		bLevel = true;
		}	// else if

	// Camera changes
	else if (	((bCamera[0] = (WCASECMP(pwLoc,L"Camera/Rotate/A1/OnFire/Value") == 0)) == true) ||
					((bCamera[1] = (WCASECMP(pwLoc,L"Camera/Rotate/A2/OnFire/Value") == 0)) == true) ||
					((bCamera[2] = (WCASECMP(pwLoc,L"Camera/Rotate/A3/OnFire/Value") == 0)) == true) ||
					((bCamera[3] = (WCASECMP(pwLoc,L"Camera/Rotate/A4/OnFire/Value") == 0)) == true) )
		{
		// Cache new value
		if			(bCamera[0])	fCamera[0] = adtFloat(vV);
		else if	(bCamera[1])	fCamera[1] = adtFloat(vV);
		else if	(bCamera[2])	fCamera[2] = adtFloat(vV);
		else if	(bCamera[3])	fCamera[3] = adtFloat(vV);
		}	// else if

	return hr;
	}	// onValue

void AnSpace::Tick( float DeltaTime )
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
	HRESULT	hr = S_OK;

	// Base behaviour
	Super::Tick( DeltaTime );

	// Execute any scheduled work
	if (hr == S_OK && pMnQ != NULL && pMnQ->isEmpty() != S_OK)
		{
		U32			sz	= 0;
		adtValue		vV;

		// One pass through
		CCLTRY ( pMnQ->size ( &sz ) );
		for (U32 i = 0;i < sz;++i)
			{
			// Object for ticking
			CCLTRY ( pMnIt->read ( vV ) );

			// Tick work
			if (hr == S_OK)
				{
				// Requeue if still needs more work
				if (((InTick *)(vV.vlong))->tickMain(DeltaTime))
					pMnQ->write ( vV );
				}	// if

			// Move to next vlaue
			pMnIt->next();
			}	// for

		}	// if

	// Has at least one render location been added ?
	if (bNewLoc)
		{
		IIt			*pIt	= NULL;
		adtValue		vL;
		adtIUnknown	unkV;

		// No new locations
		bNewLoc = false;

		// Spawn a render location actor for every uninitialized location
		CCLTRY(pDctLoc->keys(&pIt));
		while (hr == S_OK && pIt->read(vL) == S_OK)
			{
			IDictionary *pDct	= NULL;
			adtInt		iIdx(vL);

			// Check if index needs a location handler initialized
			if (	pDctLoc->load ( iIdx, vL ) == S_OK			&&				// Index present ?
					(IUnknown *)(NULL) != (unkV=vL)				&&				// Valid descriptor ?
					_QI(unkV,IID_IDictionary,&pDct) == S_OK	&&				// Dictionary ?
					pDct->load(adtString(L"Initialize"),vL) == S_OK)		// Need to be initialized ?
				{
				AnLoc *pLoc = NULL;

				// Location of visual
				CCLTRY ( pDct->load ( adtString(L"Location"), vL ) );

				// Spawn an nSpace render location actor
				CCLTRYE ( (pLoc = Cast<AnLoc>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
								this,AnLoc::StaticClass(),FTransform()))) != NULL, E_UNEXPECTED );
				CCLOK ( pLoc->init(this,adtString(vL),iIdx); )
				CCLOK ( UGameplayStatics::FinishSpawningActor(pLoc,FTransform()); )

				// Initialized
				pDct->remove(adtString(L"Initialize"));
				}	// if

			// Clean up
			_RELEASE(pDct);
			pIt->next();
			}	// while

		// Clean up
		_RELEASE(pIt);

		// No need to error out of whole loop
		hr = S_OK;
		}	// while

	// Level change ?
	if (bLevel)
		{
		char	*pcLevel	= NULL;

		// Convert to ASCII for API
		if (strLevel.length() > 0 && strLevel.toAscii(&pcLevel) == S_OK)
			{
			// Is the level name changing ?
			if (UGameplayStatics::GetCurrentLevelName(GetWorld()).
					Compare(pcLevel) != 0)
				{
				UE_LOG(LogTemp, Warning, TEXT("Tick::Level change:%s"), (LPCWSTR)strLevel);

				// Request level change
				UGameplayStatics::OpenLevel(GetWorld(),pcLevel);
				}	// if

			// Clean up
			_FREETASKMEM(pcLevel);
			}	// if

		// Request complete
		bLevel = false;
		}	// if

	// Camera update ?  NOTE: Currently using the knowledge that all
	// four coordinates are updated at the same time, change this ?
//	else if (bCamera[0] || bCamera[1] || bCamera[2] || bCamera[3])
	else if (bCamera[3])
		{
		static bool			bFirst = true;
		static FRotator	rCtlr0;
		static FRotator	rCamera0;

		// Controller for the game
		APlayerController
		*pCtlr = UGameplayStatics::GetPlayerController(GetWorld(),0);

		// Update orientation
		if (pCtlr != NULL)
			{
			FRotator 
			rCamera = FRotator(FQuat(fCamera[0],fCamera[1],fCamera[2],fCamera[3]));

			// Use start orientation as a baseline
			if (bFirst)
				{
				// Store initial position of player
				rCtlr0	= pCtlr->GetControlRotation();

				// Store initial offset for camera
				rCamera0	= rCamera;

				// Initial capture done
				bFirst	= false;
				}	// if

			// Offset of baseline
			rCamera -= rCamera0;

			// Compute new offset player controller
			FRotator
			rCtlr		= rCtlr0;
			rCtlr		+= rCamera;

			// New orientation
			pCtlr->SetControlRotation(rCtlr);
			}	// if
		}	// else if

	}	// Tick

//
// AnSpacet
//

AnSpacet :: AnSpacet ( AnSpace *_pThis )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	//	PARAMETERS
	//		-	_pR is the parent object
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	pThis	= _pThis;
	}	// AnSpacet

HRESULT AnSpacet :: onReceive (	const WCHAR *pwRoot, 
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
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	return pThis->onValue ( pwRoot, pwLoc, v );
	}	// onReceive

HRESULT AnSpacet :: tick ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		ITickable
	//
	//	PURPOSE
	//		-	Perform one 'tick's worth of work.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr	= S_OK;

	// Wait for work
	CCLTRYE ( pThis->evWork.wait ( -1 ), ERROR_TIMEOUT );

	// Execute scheduled work	
	while (	hr == S_OK		&& 
				pThis->bWork && 
				pThis->pWrkQ->isEmpty() != S_OK)
		{
		adtValue		vV;

		// Object for ticking
		CCLTRY ( pThis->pWrkIt->read ( vV ) );

		// Tick work
		CCLOK ( ((InTick *)(vV.vlong))->tickWork(); )

		// Move to next vlaue
		pThis->pWrkIt->next();
		}	// while

	// Execute scheduled stores
	while (	hr == S_OK		&& 
				pThis->bWork && 
				pThis->pStQ->isEmpty() != S_OK)
		{
		adtValue		vLoc,vV;

		// Load location and value
		if (	pThis->pStIt->read ( vLoc ) == S_OK &&
				adtValue::type(vLoc) == VTYPE_STR )
			{
			// Load value
			pThis->pStIt->next();
			if (pThis->pStIt->read ( vV ) == S_OK )
				pThis->pCli->store ( vLoc.pstr, vV );
			}	// if

		// Move to next vlaue
		pThis->pStIt->next();
		}	// while

	// Keep running ?
	CCLTRYE ( pThis->bWork == true, S_FALSE );

	return hr;
	}	// tick

HRESULT AnSpacet :: tickAbort ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		ITickable
	//
	//	PURPOSE
	//		-	Notifies the object that 'ticking' should abort.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	pThis->bWork = false;
	pThis->evWork.signal();
	return S_OK;
	}	// tickAbort

HRESULT AnSpacet :: tickBegin ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		ITickable
	//
	//	PURPOSE
	//		-	Notifies the object that it should get ready to 'tick'.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr			= S_OK;
//	nElement	*pElem	= NULL;

	// Initialize COM for thread
	CCLTRYE ( CoInitializeEx ( NULL, COINIT_MULTITHREADED ) == S_OK,
					GetLastError() );

	// Create dictionary for render locations
	CCLTRY ( COCREATE ( L"Adt.Dictionary", IID_IDictionary, &pThis->pDctLoc ) );

	//
	// Work queues
	//

	// Create queue for scheduling work from main game loop
	CCLTRY ( COCREATE ( L"Adt.Queue", IID_IList, &pThis->pMnQ ) );
	CCLTRY ( pThis->pMnQ->iterate ( &pThis->pMnIt ) );

	// Create queue and event for scheduling work from worker thread
	CCLTRY ( COCREATE ( L"Adt.Queue", IID_IList, &pThis->pWrkQ ) );
	CCLTRY ( pThis->pWrkQ->iterate ( &pThis->pWrkIt ) );

	// Create queue for scheduling stores from worker thread
	CCLTRY ( COCREATE ( L"Adt.Queue", IID_IList, &pThis->pStQ ) );
	CCLTRY ( pThis->pStQ->iterate ( &pThis->pStIt ) );

	//
	// Client
	//

	// Create client
	CCLTRYE((pThis->pCli = new nSpaceClient()) != NULL, E_OUTOFMEMORY);

	// Open default namespace 
	CCLTRY(pThis->pCli->open(L"{ Namespace Default }", true, NULL));

	// Listen to the default render locations which contains desired visuals to be rendered
	CCLTRY ( pThis->pCli->listen ( pThis->strRenLoc, true, this ) );

	// Debug
//	UE_LOG(LogTemp, Warning, TEXT("AnSpace::tickBegin"));
	
//	if (hr != S_OK)
//		MessageBox ( NULL, L"AnSpaceLoc::tickBegin", L"Error!", MB_OK );

	return hr;
	}	// tickBegin

HRESULT AnSpacet :: tickEnd ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		ITickable
	//
	//	PURPOSE
	//		-	Notifies the object that 'ticking' is to stop.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr		= S_OK;
	IIt		*pIt	= NULL;

	// Stop listening to render location
	if (pThis->pCli != NULL)
		pThis->pCli->listen ( pThis->strRenLoc, false );

	// Shutdown root element render locations
	if (pThis->pDctLoc != NULL && pThis->pDctLoc->iterate ( &pIt ) == S_OK)
		{
		adtValue		vN;
/*
		// Clear render locations
		while (pIt->read ( vN ) == S_OK)
			{
			IDictionary		*pDct		= NULL;
			nElement		*pElem	= NULL;
			adtIUnknown		unkV;

			// Obtain descriptor and element
			if (	(IUnknown *)(NULL) != (unkV=vN)							&&
					_QI(unkV,IID_IDictionary,&pDct) == S_OK				&&
					pDct->load ( adtString(L"Element"), vN ) == S_OK	&&
					(IUnknown *)(NULL) != (unkV=vN)							&&
					(pElem	= (nElement *)(unkV.punk))->pRenLoc != NULL)
				pElem->pRenLoc->setRoot(NULL);

			// Next entry
			_RELEASE(pDct);
			pIt->next();
			}	// while
*/
		// Clean up
		_RELEASE(pIt);
		}	// if

	// Clear queues
	if (pThis->pMnQ != NULL)
		pThis->pMnQ->clear();
	if (pThis->pWrkQ != NULL)
		pThis->pWrkQ->clear();
	if (pThis->pStQ != NULL)
		pThis->pStQ->clear();

	// Clean up
	_RELEASE(pThis->pMnIt);
	_RELEASE(pThis->pMnQ);
	_RELEASE(pThis->pStIt);
	_RELEASE(pThis->pStQ);
	_RELEASE(pThis->pWrkIt);
	_RELEASE(pThis->pWrkQ);

	// Clean up
	_RELEASE(pThis->pDctLoc);
	if (pThis->pCli != NULL)
		{
		// Close link
		pThis->pCli->close();

		// Clean up
		delete pThis->pCli;
		pThis->pCli	= NULL;
		}	// if

	// Clean up
	CoUninitialize();

	return hr;
	}	// tickEnd

