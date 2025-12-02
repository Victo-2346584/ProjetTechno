#include "ClayPigeon.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// Constructeur - initialise tous les composants
AClayPigeon::AClayPigeon()
{
    // Active Tick() pour permettre la rotation
    PrimaryActorTick.bCanEverTick = true;

    // === CRÉATION DU MESH (modèle 3D) ===
    PigeonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PigeonMesh"));
    RootComponent = PigeonMesh;

    // Configure les collisions pour détecter les impacts
    PigeonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PigeonMesh->SetCollisionResponseToAllChannels(ECR_Block);
    PigeonMesh->SetSimulatePhysics(false); // On utilise ProjectileMovement au lieu de physique pure

    // === CRÉATION DU COMPOSANT DE MOUVEMENT ===
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = PigeonMesh;
    ProjectileMovement->InitialSpeed = 0.f; // Sera défini dans Launch()
    ProjectileMovement->MaxSpeed = 5000.f;
    ProjectileMovement->bRotationFollowsVelocity = false; // On gère la rotation nous-mêmes
    ProjectileMovement->bShouldBounce = false; // Pas de rebond
    ProjectileMovement->ProjectileGravityScale = 1.0f;

    // === VALEURS PAR DÉFAUT ===
    LaunchSpeed = 3000.f; // ~108 km/h
    GravityScale = 1.0f;
    RotationSpeed = FRotator(180.f, 360.f, 90.f); // Rotation réaliste dans les airs
    LifeSpan = 10.f; // 10 secondes avant disparition
    ScoreValue = 10;

    bHasBeenHit = false;
    TimeAlive = 0.f;
}

// Appelé quand le jeu commence
void AClayPigeon::BeginPlay()
{
    Super::BeginPlay();

    // Configure la durée de vie automatique
    SetLifeSpan(LifeSpan);
}

// Appelé à chaque frame pour gérer la rotation
void AClayPigeon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TimeAlive += DeltaTime;

    // Applique la rotation continue pour un effet réaliste
    if (!bHasBeenHit)
    {
        ApplyRotation(DeltaTime);
    }
}

// Lance le pigeon avec une vitesse et direction
void AClayPigeon::Launch(FVector LaunchVelocity, FVector LaunchDirection)
{
    if (ProjectileMovement)
    {
        // Normalise la direction et applique la vitesse
        FVector Direction = LaunchDirection.GetSafeNormal();
        FVector Velocity = Direction * LaunchSpeed;

        // Si une vélocité personnalisée est fournie, on l'utilise
        if (!LaunchVelocity.IsNearlyZero())
        {
            Velocity = LaunchVelocity;
        }

        // Configure le mouvement
        ProjectileMovement->Velocity = Velocity;
        ProjectileMovement->ProjectileGravityScale = GravityScale;

        // Log pour déboguer
        UE_LOG(LogTemp, Warning, TEXT("Clay Pigeon launched with velocity: %s"), *Velocity.ToString());
    }
}

// Appelé quand le pigeon est touché par un tir
void AClayPigeon::OnHit()
{
    // Évite de compter plusieurs fois le même pigeon
    if (bHasBeenHit)
        return;

    bHasBeenHit = true;

    // Log pour déboguer
    UE_LOG(LogTemp, Warning, TEXT("Clay Pigeon HIT! Score: %d"), ScoreValue);

    // TODO: Ajouter le score au GameMode ici
    // Exemple: Cast<AYourGameMode>(GetWorld()->GetAuthGameMode())->AddScore(ScoreValue);

    // Joue les effets de destruction
    if (DestructionParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            DestructionParticles,
            GetActorLocation(),
            GetActorRotation(),
            FVector(1.0f), // Taille normale
            true // Auto-destroy
        );
    }

    if (DestructionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            DestructionSound,
            GetActorLocation(),
            1.0f, // Volume
            1.0f  // Pitch
        );
    }

    // Détruit le pigeon après un court délai
    DestroyPigeon();
}

// Applique la rotation réaliste du pigeon
void AClayPigeon::ApplyRotation(float DeltaTime)
{
    // Calcule la rotation pour cette frame
    FRotator CurrentRotation = GetActorRotation();
    FRotator DeltaRotation = RotationSpeed * DeltaTime;
    FRotator NewRotation = CurrentRotation + DeltaRotation;

    SetActorRotation(NewRotation);
}

// Détruit le pigeon proprement
void AClayPigeon::DestroyPigeon()
{
    // Désactive les collisions immédiatement
    PigeonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Cache le mesh
    PigeonMesh->SetVisibility(false);

    // Détruit l'acteur après 0.1 seconde (pour laisser les effets se jouer)
    SetLifeSpan(0.1f);
}