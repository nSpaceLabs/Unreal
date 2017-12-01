#include "nAudio.h"

AnAudio::AnAudio()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	vAudio.Set(0,0,0);
	strAudio = L"";
	}	// AnAudio

void AnAudio::BeginPlay()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when the game starts or when spawned.
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnAudio::BeginPlay"));

	// Default behaviour
	Super::BeginPlay();
	}	// BeginPlay

void AnAudio :: onValue (	const WCHAR *pwRoot, 
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

	// Source position of audio
	if (	((bX = (WCASECMP(pwLoc,L"Position/A1/OnFire/Value") == 0)) == true) ||
			((bY = (WCASECMP(pwLoc,L"Position/A2/OnFire/Value") == 0)) == true) ||
			((bZ = (WCASECMP(pwLoc,L"Position/A3/OnFire/Value") == 0)) == true) )
		{
		// Cache position
		if			(bX)	vAudio.X = adtFloat(v);
		else if	(bY)	vAudio.Y = adtFloat(v);
		else if	(bZ)	vAudio.Z = adtFloat(v);
		}	// else if

	// Id/resource string
	else if (WCASECMP(pwLoc,L"Id/OnFire/Value") == 0)
		{
		// Cache Id
		adtValue::toString(v,strAudio);
		}	// else if

	// Play sound
	else if (WCASECMP(pwLoc,L"Play/OnFire/Value") == 0)
		{
		char			*pc	= NULL;

		// To ASCII for Unreal
		if (strAudio.length() > 0 && strAudio.toAscii(&pc) == S_OK)
			{
			// Load specified assest
			FStringAssetReference	path(pc);
			UObject						*pObj;

			// Try and load the sound cue
			if (	(pObj = path.TryLoad()) != NULL			&&
					pObj != NULL									&&
					pObj->IsA(USoundBase::StaticClass()) )
				{
				USoundBase	*pS = Cast<USoundBase>(pObj);

				// Play it at cached location
				UGameplayStatics::PlaySoundAtLocation(GetWorld(),pS,vAudio);
				}	// if

			// Clean up
			_FREEMEM(pc);
			}	// if

		}	// else if

	}	// onReceive
