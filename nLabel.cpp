#include "nLabel.h"

AnLabel::AnLabel()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object
	//
	////////////////////////////////////////////////////////////////////////
	pfXs		= NULL;
	iXs		= 0;
	iCaret	= -2;
	iCaretP	= -2;
	pcLbl		= NULL;
	pcCrt		= NULL;
	pcBox		= NULL;
	}	// AnLabel

AnLabel::~AnLabel()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Destructor for the object
	//
	////////////////////////////////////////////////////////////////////////
	if (pfXs != NULL)
		delete[] pfXs;
	}	// ~AnLabel

void AnLabel::BeginPlay()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when the game starts or when spawned.
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr = S_OK;
	float		fSclMax;
	FVector	fV;

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnSpace::BeginPlay"));

	// Default behaviour
	Super::BeginPlay();

	// Create label sub-component
	pcLbl								= NewObject<UTextRenderComponent>(this,UTextRenderComponent::StaticClass());
	pcLbl->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	pcLbl->VerticalAlignment	= EVerticalTextAligment::EVRTA_TextCenter;
	pcLbl->bVisible				= true;				// Debug
	pcLbl->bHiddenInGame			= false;
	SetRootComponent(pcLbl);
//	pcLbl->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	pcLbl->RegisterComponent();

	// Compute local scaling necessary to fit one average letter inside
	// a unit cube.
	pcLbl->SetText ( FText::FromString("X") );
	fV			= pcLbl->GetTextLocalSize();
	fSclMax	= 0;
	if (fV.X > fSclMax)
		fSclMax = fV.X;
	if (fV.Y > fSclMax)
		fSclMax = fV.Y;
	if (fV.Z > fSclMax)
		fSclMax = fV.Z;

	// Scale to unit cube centered at origin
	fSclLcl.X = +0.5/fSclMax;
	fSclLcl.Y = fSclLcl.X;
	fSclLcl.Z = fSclLcl.X;

	// Initial scaling and rotation
	pcLbl->SetRelativeTransform ( FTransform(FRotator(-90,90,0),FVector(0,0,0),fSclLcl) );

	pcLbl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	pcLbl->SetCollisionResponseToChannel ( ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block );

	// Debug
//	pcLbl->SetText ( L"<>" );

	//
	// Input
	//

	// Box component for collision detection
	pcBox	= NewObject<UBoxComponent>(this,UBoxComponent::StaticClass());
	pcBox->bHiddenInGame = false;						// Debug
	pcBox->bHiddenInGame = true;
	pcBox->AttachToComponent(pcLbl, FAttachmentTransformRules::KeepWorldTransform);
	pcBox->RegisterComponent();

	// Collision detection
	pcBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	pcBox->SetCollisionResponseToChannel ( ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block );

	//
	// Caret
	//

	// Caret component 
	pcCrt					= NewObject<UTextRenderComponent>(this,UTextRenderComponent::StaticClass());
	pcCrt->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	pcCrt->VerticalAlignment	= EVerticalTextAligment::EVRTA_TextCenter;
	pcCrt->bVisible				= false;
	pcCrt->bHiddenInGame			= true;
	pcCrt->AttachToComponent(pcLbl, FAttachmentTransformRules::KeepWorldTransform);
	pcCrt->RegisterComponent();
	pcCrt->SetText ( FText::FromString("^") );
	pcCrt->SetTextRenderColor ( FColor ( 0xff404040 ) );
	}	// BeginPlay

void AnLabel :: EndPlay ( const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Ends gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnLabel::EndPlay"));

	// Unregistering component causes a breakpoint in engine.
	// Already taken care of ?

	// Caret
	if (pcCrt != NULL)
		{
//		pcCrt->DetachFromParent();
//		pcCrt->UninitializeComponent();
		pcCrt->bVisible		= false;
		pcCrt->bHiddenInGame	= true;
		pcCrt						= NULL;
		}	// if

	// Bounding box
	if (pcBox != NULL)
		{
//		pcBox->DetachFromParent();
//		pcBox->UninitializeComponent();
		pcBox->bVisible		= false;
		pcBox->bHiddenInGame	= true;
		pcBox						= NULL;
		}	// if

	// Text
	if (pcLbl != NULL)
		{
//		pcLbl->DetachFromParent();
//		pcLbl->UninitializeComponent();
		pcLbl->bVisible		= false;
		pcLbl->bHiddenInGame	= true;
		pcLbl						= NULL;
		}	// if

	// Base behaviour
	Super::EndPlay(rsn);
	}	// EndPlay

void AnLabel :: measure ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Measure the current string assigned to the label
	//
	////////////////////////////////////////////////////////////////////////
	FString	str;
	int		i;

	// Previous state
	if (pfXs != NULL)
		{
		delete[] pfXs;
		pfXs = NULL;
		iXs = 0;
		}	// if

	// Current text for label
	str	= pcLbl->Text.ToString();

	// Allocate room for values
	if ((iXs = str.Len()) >= 0)
		pfXs = new float[++iXs];

	// Obtain the beginning X position of each character by measuring the string
	// one character at a time.
	for (i = 0;i < iXs;++i)
		{
		// Obtain current string
		FString strSub = str.Left(i);

		// Compute how wide the substring is
		int32 sz = pcLbl->Font->GetStringSize(*strSub);

		// Assign new position
		pfXs[i] = sz;
		}	// for

	}	// measure

void AnLabel :: onValue (	const WCHAR *pwRoot, 
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

	// Debug
//	UE_LOG(LogTemp, Log, TEXT("AnLabel::onValue:%s:%s"), pwRoot, pwLoc );

	// Base behaviour
	AnElement::onValue(pwRoot,pwLoc,v);

	// Color
	if (!WCASECMP(pwLoc,L"Element/Color/OnFire/Value"))
		{
		pcLbl->SetTextRenderColor ( FColor ( iColor ) );
		}	// if

	// Text for label
	else if (!WCASECMP(pwLoc,L"Interface/Element/Default/OnFire/Value") )
		{
		adtString strLbl(v);

		// Debug
//		dbgprintf ( L"AnLabel::onReceive:%s:%s:%s\r\n",	
//						pwLoc, (LPCWSTR)pParent->strLoc, (LPCWSTR)adtString(v) );

		// Label ?
		if (strLbl.length() > 0)
			{
			// Use new text
			pcLbl->SetText ( FText::FromString((LPCWSTR)strLbl) );

			// Debug
	//		if (!WCASECMP(strHorz,L"Right"))
	//			dbgprintf ( L"Hi\r\n" );

			// Scale box to just cover the bounds
			// Full width of text, box scaling is half that
			// Ensure text is covered by slightly increasing Z
			// Must use 'local' coordinate system that the text render component uses.
			FVector 
			fSz = pcLbl->GetTextLocalSize();
			fSz.X = 1.1;
			fSz.Y /= 2;
			fSz.Z /= 2;

			// Take into account alignment
			FVector fOff(0,0,0);
			fOff = pcBox->GetRelativeTransform().GetLocation();
			if (!WCASECMP(strHorz,L"Left"))
				fOff.Y = -fSz.Y;
			else if (!WCASECMP(strHorz,L"Right"))
				fOff.Y = fSz.Y;

			// Update bounding box
			pcBox->SetBoxExtent ( fSz, true );
			pcBox->SetRelativeLocation(fOff);

			// Measure the current label
			measure();

			// Caret needs updating
			if (iCaret == -2)
				iCaret	= iCaretP;
			}	// if

		}	// if

	// Alignment
	else if (!WCASECMP(pwLoc,L"AlignHorz/OnFire/Value"))
		{
		adtValue::toString(v,strHorz);
		pcLbl->SetHorizontalAlignment ( 
			(!WCASECMP(strHorz,L"Left"))	?	EHorizTextAligment::EHTA_Left :
			(!WCASECMP(strHorz,L"Right")) ?	EHorizTextAligment::EHTA_Right :
														EHorizTextAligment::EHTA_Center );
		}	// if
	else if (!WCASECMP(pwLoc,L"AlignVert/OnFire/Value"))
		{
		adtValue::toString(v,strVert);

		// No 'SetVerticalAlignment' ?  Does this work ?
		pcLbl->VerticalAlignment	=
			(!WCASECMP(strVert,L"Top"))		?	EVerticalTextAligment::EVRTA_TextTop :
			(!WCASECMP(strVert,L"Bottom"))	?	EVerticalTextAligment::EVRTA_TextBottom	:
															EVerticalTextAligment::EVRTA_TextCenter;
		}	// if

	// Caret position
	else if (!WCASECMP(pwLoc,L"Caret/OnFire/Value"))
		{
		iCaret = adtInt(v);

		// Caret
		if (iCaret != -2)
			{
			// Set position and visibility of caret
			if (iCaret >= 0)
				{
				// Position caret at index
				if (iCaret < iXs)
					{
					// Extent of box surrounding text and receving input
					FVector
					szBox = pcBox->GetUnscaledBoxExtent();

					// Current location 
					FVector 
					fLoc = pcCrt->GetRelativeTransform().GetLocation();

					// Compute new location
					fLoc.Y = szBox.Y - pfXs[iCaret];
					fLoc.Z = -szBox.Z;

					// Move into position using text coordinate system
					pcCrt->SetRelativeLocation ( fLoc );

					// Ensure visible
					pcCrt->bHiddenInGame = false;
					pcCrt->SetVisibility(true,true);
					}	// if
				}	// if

			// -1 means no caret
			else
				{
				pcCrt->bHiddenInGame = true;
				pcCrt->SetVisibility(false,true);
				}	// else

			// Updated
			iCaretP	= iCaret;
			iCaret	= -2;
			}	// if

		}	// if

	}	// onValue

/*
void AnLabel :: onRay ( IDictionary *pDct, const FVector &vLoc,
									const FVector &vDir )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called for a ray event.
	//
	//	PARAMETERS
	//		-	pDct contains and will receive event information
	//		-	vLoc is the ray intersection location
	//		-	vDir is the ray direction
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	FVector		szBox;
	FString		str;
	float			fX,fY,fAt;
	int			i,iAt;

	// Compute the floating point position of the button event inside
	// the current string.  
//	dbgprintf ( L"AnLabel::onInput:%g,%g,%g\r\n", vAt.X, vAt.Y, vAt.Z );

	// Extent of box surrounding text and receving input
	szBox = pcBox->GetUnscaledBoxExtent();

	// Calculate position in each dimension of event
	// Text is rotated, etc so use appropriate coordinates
	fX = (szBox.Y - vLoc.Y);
	fY = (vLoc.Z + szBox.Z);

//	dbgprintf ( L"AnLabel::onInput:Box:%g,%g:%g,%g: (%g,%g,%g)\r\n", 
//						vLoc.Y, vLoc.Z, fX, fY, szBox.X, szBox.Y, szBox.Z );

	// TODO: Better way to do this ?
	// Measure string one letter at a time until the event location
	//	intersects a letter.
	iAt	= 0;
	fAt	= 0;
	for (i = 1;i < iXs;++i)
		{
		// Obtain current string
//		FString strSub = str.Left(i);

		// Compute how wide the substring is
//		int32 sz = pcLbl->Font->GetStringSize(*strSub);

		// Bigger than remaining size ?
		if (pfXs[i] >= fX)
			{
			// Match is one index before
			iAt = (i > 0) ? i-1 : 0;

			// Compute percent into next letter for floating point index
			fAt = (fX-pfXs[i-1])/(pfXs[i]-pfXs[i-1]);
			break;
			}	// if

		// Previous size
//		szp = sz;
		}	// for

	// Past end of string
	if (i >= iXs)
		iAt	= iXs;

	// Store position of event withing string in dictionary
	dbgprintf ( L"AnLabel::onInput:At:%g\r\n", (iAt+fAt) );
	CCLTRY ( pDct->store ( adtString(L"Index"), adtFloat((iAt+fAt)) ) );

	// Default behaviour
	CCLOK ( UnElement::onRay(pDct,vLoc,vDir); )
	}	// onRay
*/
